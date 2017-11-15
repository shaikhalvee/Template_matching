# Template Matching

## Synopsis
Its mainly a project which matches a given image pattern in other videos via image processing.

## Decription
This project maps the reference image,  reads the video file, separates the frames, tracks the reference file in the frame,
marks it with some marker and merges the frames back to a video file name “output.MOV”.
The given output file marks the reference with a red rectangle.

## Algorithm Implementation
The entire job is done for the following methods:
1. Exhaustive search technique
2. 2D Logarithmic search
3. Hierarchical search