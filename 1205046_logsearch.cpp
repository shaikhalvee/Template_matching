/*-------property of the half blood prince-----*/

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cstdio>
#include <iostream>
#define ISVALID(cx,cy,m,n,M,N) (((cx)+(m))<(M) && ((cy)+(n))<(N) && (cx)>=0 && (cy)>=0)
using namespace cv;
using namespace std;

vector < vector <double> > ref_mat;
vector < vector <double> >  test_mat;
int Parr[23]={512,256,128,120,111,100,98,72,65,64,50,48,32,27,24,16,12,11,8,4,3,2,1};
int totalframesearch=0;

double grayscaleval(int r, int g, int b){
    return r;
}

double sq(double val){
    return val*val;
}

double cost(vector< vector <double> > &rfrnc, vector < vector <double> > &test, int posx, int posy){
    double s=0.0;
    int tr=test_mat.size(); int rr=ref_mat.size();
    int tc=test_mat[0].size(); int rc=ref_mat[0].size();
    for(int i=0; i<rr; ++i){
        for(int j=0;j<rc; ++j){
            if((posx+i)>=tr || (posy+j)>=tc){s+=1000000000.00;printf("moga\n");}
            else s=s+sq(rfrnc[i][j]-test[posx+i][posy+j]);
        }
    }
    return s;
}

pair <int,int> exhaustivebestfit(vector < vector <double> > &rfrnc, vector < vector <double> > &test){
    double mnn=1000000000000.00;
    pair< int,int >bestpos=make_pair(-1,-1);
    int refrow=rfrnc.size();
    int refcol=rfrnc[0].size();
    int testrow=test.size();
    int testcol=test[0].size();
    for(int i = 0;(i+refrow)<testrow  ; ++i){
        for( int  j = 0; (j+refcol)<testcol; ++j){
            double val=cost(rfrnc,test,i,j);
            if(val<mnn){
                mnn=val;
                bestpos.first=i;
                bestpos.second=j;
            }
        }
    }
    //cout<<mnn<<"\n";
    return bestpos;
}

void fillrectangle(Mat &test_frame, int posi, int posj, Mat &ref_frame){
    int refrow=ref_frame.rows;
    int refcol=ref_frame.cols;

    for(int y = posi; y<(posi+refrow); ++y){
        for(int x= posj; x<(posj+refcol); ++x){
            if(x==posj || x==(posj+refcol-1) || y==posi || y==(posi+refrow-1)){
                Vec3b col=test_frame.at<Vec3b>(Point(x,y));
                col[0]=0;
                col[1]=0;
                col[2]=255;
                test_frame.at<Vec3b>(Point(x,y))=col;
            }
        }
    }
}

vector< vector <double> >convert(Mat &frame){
    vector< vector <double> >mat;
    vector<double>line;
    for(int y=0;y<frame.rows; ++y){
        mat.push_back(line);
        for(int x=0;x<frame.cols;++x){
            Vec3b col=frame.at<Vec3b>(Point(x,y));
            int b=(int)col[0];
            int g=(int)col[1];
            int r=(int)col[2];
            double gray=grayscaleval(r,g,b);
            mat[y].push_back(gray);
        }
    }
    return mat;
}

pair <int,int> logsearch(int p, int cx, int cy, int start, double prevcost){
    int refrow=ref_mat.size(); int refcol=ref_mat[0].size(); int testrow=test_mat.size(); int testcol=test_mat[0].size();
    if(p==0)return make_pair(cx,cy);
    //printf("log search e p %d te\n",p);
    double mnn=100000000000000.00;int mnx=-1;int mny=-1;
    if(!start){mnn=prevcost;mnx=cx;mny=cy;}
    for(int x=-p;x<=p;x+=p){
        for(int y=-p;y<=p;y+=p){
            int curx=cx+x;
            int cury=cy+y;
            //printf("%d %d\n",curx,cury);
            if(x==0 && y==0 && !start)continue;
            if(ISVALID(curx,cury,refrow,refcol,testrow,testcol)){
                double val=cost(ref_mat,test_mat,curx,cury);
                totalframesearch++;
                if(val<mnn){
                    mnn=val; mnx=curx; mny=cury;
                }
            }
        }
    }
    //printf("minx: %d miny: %d\n",mnx,mny);
    return logsearch(p/2,mnx,mny,0,mnn);
}

int main(){
    for(int i=2;i<3;++i){
        int P=Parr[i];
        totalframesearch=0;
        VideoCapture cap("input.MOV");
        Mat ref_frame=imread("reference.jpg");
        if(!cap.isOpened() || !ref_frame.data){
            cout<<"oops!\n";
            return -1;
        }
        double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
        double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

        //cout << "Frame Size = " << dWidth << "x" << dHeight << endl;
        Size frameSize(static_cast<int>(dWidth), static_cast<int>(dHeight));

        VideoWriter oVideoWriter ("output2.avi", CV_FOURCC('P','I','M','1'), 20, frameSize, true); //initialize the VideoWriter object
        if (!oVideoWriter.isOpened()){
            cout << "ERROR: Failed to write the video" << endl;
            return -1;
        }

        ref_mat=convert(ref_frame);

        int cnt=0;
        pair<int,int>bestpos=make_pair(-1,-1);
        while(1){
            Mat test_frame;
            bool success=cap.read(test_frame);
            if(!success){
                //cout<<"end of video\n";
                break;
            }
            ++cnt;
            //printf("frame no: %d\n",cnt);
            //if(cnt%10)continue;
            test_mat=convert(test_frame);
            if(bestpos.first==-1){
                bestpos=exhaustivebestfit(ref_mat,test_mat);
            }
            else{
                pair<int,int>tempos=bestpos;
                bestpos=logsearch(P,tempos.first,tempos.second,1,-1.0);
            }
            //printf("best match at pixel : %d %d\n",bestpos.second,bestpos.first);
            fillrectangle(test_frame,bestpos.first,bestpos.second,ref_frame);
            oVideoWriter.write(test_frame);
            if(waitKey(10)==27){
                cout<<"esc pressed\n";
                break;
            }
        }
        printf("%d\t%.2lf\n",P,(double)totalframesearch/(double)cnt);
    }
    return 0;
}
