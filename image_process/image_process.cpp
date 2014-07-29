/*
 * image_process.cpp
 *
 *  Created on: Jan 29, 2010
 *      Author: tkcheng
 */
#include "stdio.h"
#include "cv.h"
#include "highgui.h"

/*  **** Function in this file ****
 * void Image_Threshold(IplImage* image_src, IplImage* image_dst)
 * void Image_SobelXY(IplImage* iplImg_Src, IplImage* image_dst)
 */
////////////////////////////FUNCTION PROTOTYPE/////////////////////////////
extern void SetPixel(IplImage* iplImg, unsigned char pix_data, int target_pix_x, int target_pix_y);
extern void KernelExtract3X3(IplImage* iplImg, unsigned char *pix_data, int target_pix_x, int target_pix_y);

extern unsigned char Filter_SearchMedian(unsigned char * data, unsigned char size);
extern unsigned char Filter_SearchMax(unsigned char * data, unsigned char size);
extern unsigned char Filter_SearchMin(unsigned char * data, unsigned char size);
extern unsigned char Filter_SearchAverage(unsigned char * data, unsigned char size);

unsigned char Filter_Sobel_3X3(unsigned char * data, unsigned char axis, unsigned char direction, unsigned char scharr);
#define USE_SOBEL					0
#define USE_SCHARR					1
#define DETECT_HIGH2LOW_EDGE		1
#define DETECT_LOW2HIGH_EDGE		0
#define X_DIRECTION					1
#define Y_DIRECTION					0
unsigned char Filter_Threshold(unsigned char data, unsigned char threshold);
signed int Filter_Laplacian_3X3(unsigned char * data, unsigned char apply, unsigned char diagonal, unsigned char scale);

int DFT(int dir,long m,IplImage* DFT_src, IplImage* DFT_dst);

//util.h
void print_propertis(IplImage* iplImg);
////////////////////////////END:FUNCTION PROTOTYPE///////////////////////////////
void trackbarHandlerdummy(int pos)
{
}

/*
 * PARAM IN : image_src : grayscale source image for data process
 * PARAM OUT: image_dst : after process, write the result here
 * FUNCTION : Set pixel to white color if below threshold
 * 			  Set pixel to black color if above or equal to threshold
 */
void Image_Threshold(IplImage* image_src, IplImage* image_dst)
{
	int pos_y,pos_x;
    unsigned char pix_value;
    static unsigned char run_once=1;
    static int trackbar_Threshold=25;
    static int trackbar_Threshold_Invert=0;
	int maxVal=255;

    if(run_once)
    {
        cvNamedWindow( "Threshold", 1 );
    	run_once = 0;
    	cvCreateTrackbar("Threshold1","Threshold",&trackbar_Threshold,maxVal,trackbarHandlerdummy);
    	cvCreateTrackbar("Invert1","Threshold",&trackbar_Threshold_Invert,1,trackbarHandlerdummy);
    }
	for(pos_y=1;pos_y<image_src->height-1;pos_y++)		//exclude filter on first and last row
	{
		for(pos_x=1;pos_x<image_src->width-1;pos_x++)	//exclude filter on first pixel and last pixel
		{
			pix_value = image_src->imageData[(pos_x)*image_src->nChannels+(pos_y)*image_src->widthStep];
			pix_value  = Filter_Threshold(pix_value , trackbar_Threshold);
			if(trackbar_Threshold_Invert)
			{
				if(pix_value)
					pix_value=0;
				else	
					pix_value=0xff;
			}

			SetPixel(image_dst, pix_value, pos_x, pos_y);
		}
	}
    cvShowImage( "Threshold", image_dst );
}

#define NUM_ELEMENT			9
#define ENABLE				1
#define DISABLE				0
#define HIGHLOW				0
#define LOWHIGH				1
#define HIGHLOW_LOWHIGH		2
/*
 * PARAM IN : iplImg_Src : grayscale source image for data process
 * PARAM OUT: image_dst : after process, write the result here
 * FUNCTION : Perform Sobel filter for gradient detection
 * 			: 3.7.3 - Use of First Derivatives for Enhancement - The Gradient
 */
void Image_SobelXY(IplImage* iplImg_Src, IplImage* image_dst)
{
	int pos_y,pos_x;
    unsigned int final_pix;
	unsigned char kernel_pix_data[NUM_ELEMENT];
    static unsigned char run_once=1;
    static int trackbar_x_enable_disable=ENABLE;
    static int trackbar_x_highlow=HIGHLOW_LOWHIGH;
    static int trackbar_y_enable_disable=ENABLE;
    static int trackbar_y_highlow=HIGHLOW_LOWHIGH;
    static int trackbar_filter_type=USE_SOBEL;

    if(run_once)
    {
    	printf("First Derivative for Gradient Detection - Sobel Filter\n");

    	cvNamedWindow( "SobelXY", 1 );
    	cvNamedWindow( "SobelXY Control Panel", 1 );
    	run_once = 0;
    	cvCreateTrackbar("Sobel/Scharr","SobelXY Control Panel",&trackbar_filter_type,1,trackbarHandlerdummy);
    	cvCreateTrackbar("X-Enable","SobelXY Control Panel",&trackbar_x_enable_disable,1,trackbarHandlerdummy);
    	cvCreateTrackbar("X-H2L,L2H","SobelXY Control Panel",&trackbar_x_highlow,2,trackbarHandlerdummy);
    	cvCreateTrackbar("Y-Enable","SobelXY Control Panel",&trackbar_y_enable_disable,1,trackbarHandlerdummy);
    	cvCreateTrackbar("Y-H2L,L2H","SobelXY Control Panel",&trackbar_y_highlow,2,trackbarHandlerdummy);

    	printf("Sobel Help\n");
    	printf("Sobel/Scharr -- 0 = Use Sobel filter\n");
    	printf("			 -- 1 = Use Scharr filter\n");
    	printf("X-Enable -- 0 = Disable Sobel on X-axis\n");
    	printf("            1 = Enable Sobel on X-axis\n");
    	printf("X-H2L,L2H-- 0 = X-axis high brightness -> low brightness edge detection\n");
    	printf("            1 = X-axis low brightness -> high brightness edge detection\n");
    	printf("            2 = X-axis low -> high and high -> lowbrightness edge detection\n");
    	printf("Y axis - similar to above except is apply on Y-axis instead of X-axis\n");
    }

	for(pos_y=1;pos_y<iplImg_Src->height-1;pos_y++)		//exclude filter on first and last row
	{
		for(pos_x=1;pos_x<iplImg_Src->width-1;pos_x++)	//exclude filter on first pixel and last pixel
		{
			final_pix=0;
			//extract pixel information from image
			KernelExtract3X3(iplImg_Src, kernel_pix_data, pos_x, pos_y);
			//process data
			if(trackbar_x_enable_disable == ENABLE)
			{
				if(trackbar_x_highlow == HIGHLOW_LOWHIGH || trackbar_x_highlow == HIGHLOW)
					final_pix = Filter_Sobel_3X3(kernel_pix_data,X_DIRECTION,DETECT_HIGH2LOW_EDGE, trackbar_filter_type);
				if(trackbar_x_highlow == HIGHLOW_LOWHIGH || trackbar_x_highlow == LOWHIGH)
					final_pix += Filter_Sobel_3X3(kernel_pix_data,X_DIRECTION,DETECT_LOW2HIGH_EDGE, trackbar_filter_type);
			}

			if(trackbar_y_enable_disable == ENABLE)
			{
				if(trackbar_y_highlow == HIGHLOW_LOWHIGH || trackbar_y_highlow == HIGHLOW)
					final_pix += Filter_Sobel_3X3(kernel_pix_data,Y_DIRECTION,DETECT_HIGH2LOW_EDGE, trackbar_filter_type);
				if(trackbar_y_highlow == HIGHLOW_LOWHIGH || trackbar_y_highlow == LOWHIGH)
					final_pix += Filter_Sobel_3X3(kernel_pix_data,Y_DIRECTION,DETECT_LOW2HIGH_EDGE, trackbar_filter_type);
			}

			if(final_pix>255)
				final_pix = 255;
			SetPixel(image_dst, final_pix, pos_x, pos_y);
		}
	}
    cvShowImage( "SobelXY", image_dst );
}


#define EXCLUDE_DIAGONAL		0
#define INCLUDE_DIAGONAL		1
#define NOT_APPLY_TO_IMG		0
#define APPLY_TO_IMG			1
#define SCALING_20				20
/*
 * PARAM IN : iplImg_Src : grayscale source image for data process
 * PARAM OUT: image_dst : after process, write the result here
 * FUNCTION : Perform Laplacian filter for gradient detection
 * 			: 3.7.3 - Use of Second Derivatives for Enhancement - Sharpen Image
 */
void Image_Laplacian(IplImage* iplImg_Src, IplImage* image_dst)
{
	int pos_y,pos_x;
    signed int final_pix;
	unsigned char kernel_pix_data[NUM_ELEMENT];
    static unsigned char run_once=1;
    static int trackbar_diagonal=INCLUDE_DIAGONAL;
    static int trackbar_apply=APPLY_TO_IMG;
    static int trackbar_scale=SCALING_20;
    IplImage* Laplace_Scale;

    Laplace_Scale = cvCreateImage(cvGetSize(iplImg_Src), IPL_DEPTH_8S, 1);

    if(run_once)
    {
    	printf("Second Derivative for Edge Detection - Laplacian Filter\n");

    	cvNamedWindow( "Laplace_Scale", 1 );

    	cvNamedWindow( "Laplacian", 1 );
    	run_once = 0;
    	cvCreateTrackbar("Diagonal","Laplace_Scale",&trackbar_diagonal,1,trackbarHandlerdummy);
    	cvCreateTrackbar("Apply","Laplace_Scale",&trackbar_apply,1,trackbarHandlerdummy);
    	cvCreateTrackbar("Scale","Laplace_Scale",&trackbar_scale,100,trackbarHandlerdummy);


    	printf("Laplacian Help\n");
    	printf("Diagonal -- 0 = Do not include diagonal in Laplacian filter\n");
    	printf("            1 = Include diagonal value in Laplacian filter\n");
    	printf("Apply    -- 0 = Do not apply final filter into image\n");
    	printf("            1 = Add filter result back to original image\n");
    }

    for(pos_y=1;pos_y<iplImg_Src->height-1;pos_y++)		//exclude filter on first and last row
	{
		for(pos_x=1;pos_x<iplImg_Src->width-1;pos_x++)	//exclude filter on first pixel and last pixel
		{
			final_pix=0;
			//extract pixel information from image
			KernelExtract3X3(iplImg_Src, kernel_pix_data, pos_x, pos_y);
			//process data
			if(trackbar_apply)
			{
				if(trackbar_diagonal)
					final_pix = Filter_Laplacian_3X3(kernel_pix_data, APPLY_TO_IMG, INCLUDE_DIAGONAL,trackbar_scale);
				else
					final_pix = Filter_Laplacian_3X3(kernel_pix_data, APPLY_TO_IMG, EXCLUDE_DIAGONAL,trackbar_scale);

				//scale info into 8bit signed
				final_pix /= (sizeof(signed int)/sizeof(signed char));

				//scale data from 16bit-signed to 8bit signed
				if(final_pix>255)
					final_pix = 255;
				else if(final_pix <0)
					final_pix = 0;
			}
			else
			{
				if(trackbar_diagonal)
					final_pix = Filter_Laplacian_3X3(kernel_pix_data, NOT_APPLY_TO_IMG, INCLUDE_DIAGONAL,trackbar_scale);
				else
					final_pix = Filter_Laplacian_3X3(kernel_pix_data, NOT_APPLY_TO_IMG, EXCLUDE_DIAGONAL,trackbar_scale);
			}

			Laplace_Scale->imageData[(pos_x)*Laplace_Scale->nChannels+(pos_y)*Laplace_Scale->widthStep] = final_pix;

			if(trackbar_apply)
			{
				if(trackbar_diagonal)
					final_pix = Filter_Laplacian_3X3(kernel_pix_data, APPLY_TO_IMG, INCLUDE_DIAGONAL,trackbar_scale);
				else
					final_pix = Filter_Laplacian_3X3(kernel_pix_data, APPLY_TO_IMG, EXCLUDE_DIAGONAL,trackbar_scale);
			}
			else
			{
				if(trackbar_diagonal)
					final_pix = Filter_Laplacian_3X3(kernel_pix_data, NOT_APPLY_TO_IMG, INCLUDE_DIAGONAL,trackbar_scale);
				else
					final_pix = Filter_Laplacian_3X3(kernel_pix_data, NOT_APPLY_TO_IMG, EXCLUDE_DIAGONAL,trackbar_scale);

				final_pix += kernel_pix_data[4];
			}

			if(final_pix>255)
				final_pix = 255;
			else if(final_pix <0)
				final_pix = 0;

			SetPixel(image_dst, final_pix, pos_x, pos_y);

		}
	}
    cvShowImage( "Laplacian", image_dst );
    cvShowImage( "Laplace_Scale", Laplace_Scale );
    cvReleaseImage(&Laplace_Scale);
}

/*
 * PARAM IN : iplImg_Src : grayscale source image for data process
 * PARAM OUT: image_dst : after process, write the result here
 * FUNCTION : Search for Maximum/Minimum/Median/Average filtering
 */
void Image_MaxMinMedianAvg(IplImage* iplImg_Src, IplImage* image_dst)
{
	int pos_y,pos_x;
    signed int final_pix;
	unsigned char kernel_pix_data[NUM_ELEMENT];
    static unsigned char run_once=1;
    static int trackbar_min=0;
    static int trackbar_max=0;
    static int trackbar_median=0;
    static int trackbar_average=1;

    if(run_once)
    {
    	cvNamedWindow( "Filter", 1 );

    	run_once = 0;
    	cvCreateTrackbar("Min","Filter",&trackbar_min,1,trackbarHandlerdummy);
    	cvCreateTrackbar("Max","Filter",&trackbar_max,1,trackbarHandlerdummy);
    	cvCreateTrackbar("Median","Filter",&trackbar_median,1,trackbarHandlerdummy);
    	cvCreateTrackbar("Average","Filter",&trackbar_average,1,trackbarHandlerdummy);


    	printf("Basic Filter Help\n");
    	printf("Minimum  -- 0 = Do nothing\n");
    	printf("            1 = Search for minimum value along 3X3 kernel\n");
    	printf("Maximum  -- 0 = Do nothing\n");
    	printf("            1 = Search for maximum value along 3X3 kernel\n");
    	printf("Median   -- 0 = Do nothing\n");
    	printf("            1 = Search for median value along 3X3 kernel\n");
    	printf("Average  -- 0 = Do nothing\n");
    	printf("            1 = Search for average value along 3X3 kernel\n");
    }

    for(pos_y=1;pos_y<iplImg_Src->height-1;pos_y++)		//exclude filter on first and last row
	{
		for(pos_x=1;pos_x<iplImg_Src->width-1;pos_x++)	//exclude filter on first pixel and last pixel
		{
			final_pix=0;
			//extract pixel information from image
			KernelExtract3X3(iplImg_Src, kernel_pix_data, pos_x, pos_y);
			//process data
			if(trackbar_min == 1)
				final_pix = Filter_SearchMin(kernel_pix_data, NUM_ELEMENT);
			else if(trackbar_max == 1)
				final_pix = Filter_SearchMax(kernel_pix_data, NUM_ELEMENT);
			else if(trackbar_median == 1)
				final_pix = Filter_SearchMedian(kernel_pix_data, NUM_ELEMENT);
			else if(trackbar_average == 1)
				final_pix = Filter_SearchAverage(kernel_pix_data, NUM_ELEMENT);
			else
				return;

			SetPixel(image_dst, final_pix, pos_x, pos_y);

		}
	}
    cvShowImage( "MinMaxMedianAverage", image_dst );
}
void Image_DFT(IplImage* iplImg_Src)
{
//	int pos_y,pos_x;
    static unsigned char run_once=1;
    static int trackbar_direction=1;
//    static int trackbar_apply=APPLY_TO_IMG;
//    static int trackbar_scale=SCALING_20;
    IplImage* DFT_dst;

    DFT_dst = cvCreateImage(cvGetSize(iplImg_Src), IPL_DEPTH_8U, 1);

    if(run_once)
    {
//   	printf("Second Derivative for Edge Detection - Laplacian Filter\n");

//    	cvNamedWindow( "Laplace_Scale", 1 );

    	cvNamedWindow( "DFT", 1 );
    	run_once = 0;
    	cvCreateTrackbar("Direction","DFT",&trackbar_direction,1,trackbarHandlerdummy);
//    	cvCreateTrackbar("Apply","Laplace_Scale",&trackbar_apply,1,trackbarHandlerdummy);
//    	cvCreateTrackbar("Scale","Laplace_Scale",&trackbar_scale,100,trackbarHandlerdummy);


//    	printf("Laplacian Help\n");
//    	printf("Diagonal -- 0 = Do not include diagonal in Laplacian filter\n");
//    	printf("            1 = Include diagonal value in Laplacian filter\n");
//    	printf("Apply    -- 0 = Do not apply final filter into image\n");
//    	printf("            1 = Add filter result back to original image\n");
    }
    print_propertis(iplImg_Src);
//    for(pos_y=1;pos_y<iplImg_Src->height-1;pos_y++)		//exclude filter on first and last row
	{
//		for(pos_x=1;pos_x<iplImg_Src->width-1;pos_x++)	//exclude filter on first pixel and last pixel
		{
			//extract pixel information from image
//			KernelExtract3X3(iplImg_Src, kernel_pix_data, pos_x, pos_y);
			//process data
			DFT(1,(iplImg_Src->width)*(iplImg_Src->height),iplImg_Src,DFT_dst);
//			DFT_dst->imageData[(pos_x)*DFT_dst->nChannels+(pos_y)*DFT_dst->widthStep] = iplImg_Src->imageData[(pos_x)*iplImg_Src->nChannels+(pos_y)*iplImg_Src->widthStep];

//			SetPixel(image_dst, final_pix, pos_x, pos_y);

		}
	}
    cvShowImage( "fft result", DFT_dst );
    cvReleaseImage(&DFT_dst);
}
