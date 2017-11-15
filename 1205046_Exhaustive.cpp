/*-------property of the half blood prince-----*/

// Template matching video
//compile g++ Hierarchical.cpp -o Hierarchical `pkg-config opencv --cflags --libs`

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cstdio>
#include <iostream>
using namespace cv;
using namespace std;

double grayscaleval(int r, int g, int b){
    return r;
}

double sq(double val){
    return val*val;
}

double cost(vector< vector <double> > &rfrnc, vector < vector <double> > &test, int posx, int posy){
    double s=0.0;
    for(int i=0; i<rfrnc.size(); ++i){
        for(int j=0;j<rfrnc[i].size(); ++j){
            s=s+sq(rfrnc[i][j]-test[posx+i][posy+j]);
        }
    }
    return s;
}

pair <int,int> bestfit(vector < vector <double> > &rfrnc, vector < vector <double> > &test){
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

int main(){
    VideoCapture cap("input.MOV");
    Mat ref_frame=imread("reference.jpg");
    if(!cap.isOpened() || !ref_frame.data){
        cout<<"oops!\n";
        return -1;
    }
    double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
    double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

    cout << "Frame Size = " << dWidth << "x" << dHeight << endl;
    Size frameSize(static_cast<int>(dWidth), static_cast<int>(dHeight));

    VideoWriter oVideoWriter ("output.avi", CV_FOURCC('P','I','M','1'), 20, frameSize, true); //initialize the VideoWriter object
    if (!oVideoWriter.isOpened()){
        cout << "ERROR: Failed to write the video" << endl;
        return -1;
    }

    vector < vector <double> >ref_mat=convert(ref_frame);

    int cnt=0;
    while(1){
        Mat test_frame;
        bool success=cap.read(test_frame);
        if(!success){ cout<<"end of video\n"; break;}
        cout<<"frame no: "<<++cnt<<"\n";
        if(cnt%10)continue;
        vector < vector <double> >test_mat=convert(test_frame);
        pair<int,int>bestpos=bestfit(ref_mat,test_mat);
        //cout<<"best match at pixel : "<<bestpos.second<<" "<<bestpos.first<<"\n";
        fillrectangle(test_frame,bestpos.first,bestpos.second,ref_frame);
        oVideoWriter.write(test_frame);
        if(waitKey(10)==27){
            cout<<"esc pressed\n";
            break;
        }
    }
    return 0;
}
