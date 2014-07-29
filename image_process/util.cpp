/*
 * util.cpp
 *
 *  Created on: Feb 11, 2010
 *      Author: tkcheng
 */
#include "stdio.h"
#include "cv.h"
#include "highgui.h"

void print_propertis(IplImage* iplImg)
{
    printf( "*** IplImage Properties ***\n");
    printf( "nSize = %d\n", iplImg->nSize);
    printf( "ID = %d\n", iplImg->ID);
    printf( "nChannels = %d\n", iplImg->nChannels);
    printf( "depth = %d\n", iplImg->depth);
    switch(iplImg->depth){
		case IPL_DEPTH_8U:
			printf("depth = IPL_DEPTH_8U\n");
			break;
		case IPL_DEPTH_8S:
			printf("depth = IPL_DEPTH_8S\n");
			break;
		case IPL_DEPTH_16U:
			printf("depth = IPL_DEPTH_16U\n");
			break;
		case IPL_DEPTH_16S:
			printf("depth = IPL_DEPTH_16S\n");
			break;
		case IPL_DEPTH_32S:
			printf("depth = IPL_DEPTH_32S\n");
			break;
		case IPL_DEPTH_32F:
			printf("depth = IPL_DEPTH_32F\n");
			break;
		case IPL_DEPTH_64F:
			printf("depth = IPL_DEPTH_64F\n");
			break;
		default:
			printf("depth = unknown!!\n");
			break;
    }
    printf( "nChannels = %d\n", iplImg->nChannels);
    printf( "data order = %d\n", iplImg->dataOrder);
    if(iplImg->dataOrder == 1)
    	printf("storage as RRRRGGGGBBBB\n");
    else if(iplImg->dataOrder == 0)
    	printf("storage as RGBRGBRGBRGB\n");
    else
    	printf("storage = unknown !!\n");
    printf( "origin = %d\n", iplImg->origin);
    printf( "align = %d\n", iplImg->align);
    printf( "width = %d\n", iplImg->width);
    printf( "height = %d\n", iplImg->height);
//		printf( "roi= %d\n", iplImg->origin);
    printf( "imageSize = %d\n", iplImg->imageSize);
    printf( "widthStep = %d\n", iplImg->widthStep);
}
