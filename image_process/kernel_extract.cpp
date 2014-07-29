/*
 * algo_contour.cpp
 *
 *  Created on: Jan 22, 2010
 *      Author: tkcheng
 */

/* function in this file
 * void KernelExtract3X3(IplImage* iplImg, unsigned char *pix_data, int target_pix_x, int target_pix_y)
 * void SetPixel(IplImage* iplImg, unsigned char pix_data, int target_pix_x, int target_pix_y)
 *
 */
//#define DBUG_MESG

#define CV_NO_BACKWARD_COMPATIBILITY

#include "cv.h"
#include "highgui.h"

#ifndef EXPORT_MODULE
#include <iostream>
#include <cstdio>
#endif

#define KERNEL_SIZE			"3X3\n"
/*
 * PARAM IN: iplImg : image to extract data
 * 			: target_pix_x : x-location of target pixel
 * 			: target_pix_y : y-location of target pixel
 * PARAM OUT: *pix_data	   : output 3X3 kernal data around the target pixel as below
 * FUNCTION : extract 3X3 pixel value from iplImg and store in pix_data
 *
 *     |-------------|-------------|-------------|
 *     | pix_data[0] | pix_data[1] | pix_data[2] |
 *     |-------------|-------------|-------------|
 *     | pix_data[3] | pix_data[4] | pix_data[5] |
 *     |-------------|-------------|-------------|
 *     | pix_data[6] | pix_data[7] | pix_data[8] |
 *     |-------------|-------------|-------------|
 */
void KernelExtract3X3(IplImage* iplImg, unsigned char *pix_data, int target_pix_x, int target_pix_y)
{
	int pos_x,pos_y;
	unsigned char *pixel_ref;

	pos_x = target_pix_x;
	pos_y = target_pix_y;

#ifdef DBUG_MESG
	printf("Enter kernel_extract with size");
	printf(KERNEL_SIZE);
#endif
//	for(pos_y=1;pos_y<iplImg->height-1;pos_y++)		//exclude filter on first and last row
	{
//		for(pos_x=1;pos_x<iplImg->width-1;pos_x++)	//exclude filter on first pixel and last pixel
		{
			//get top left pixel location as starting location
			pixel_ref = (unsigned char*)&iplImg->imageData[(pos_x-1)*iplImg->nChannels+(pos_y-1)*iplImg->widthStep];

			//get top left pixel value
			pix_data[0] = *(pixel_ref+(iplImg->nChannels*0));

			//get top pixel value
		    pix_data[1] = *(pixel_ref+(iplImg->nChannels*1));

			//get top right pixel value
		    pix_data[2] = *(pixel_ref+(iplImg->nChannels*2));

			//get left pixel value
		    pix_data[3] = *(pixel_ref+(iplImg->widthStep*1)+(iplImg->nChannels*0));

			//get center/target pixel value
		    pix_data[4] = *(pixel_ref+(iplImg->widthStep*1)+(iplImg->nChannels*1));

			//get right pixel value
		    pix_data[5] = *(pixel_ref+(iplImg->widthStep*1)+(iplImg->nChannels*2));

			//get bottom left pixel value
		    pix_data[6] = *(pixel_ref+(iplImg->widthStep*2)+(iplImg->nChannels*0));

			//get bottom pixel value
			pix_data[7] = *(pixel_ref+(iplImg->widthStep*2)+(iplImg->nChannels*1));

			//get bottom right pixel value
			pix_data[8] = *(pixel_ref+(iplImg->widthStep*2)+(iplImg->nChannels*2));
		}
	}
}

/*
 * PARAM IN : pix_data : data to set into image
 * 			: target_pix_x : x-location of target pixel
 * 			: target_pix_y : y-location of target pixel
 * PARAM OUT: iplImg : set data into this image
 * FUNCTION : Set pixel value within the image
 */
void SetPixel(IplImage* iplImg, unsigned char pix_data, int target_pix_x, int target_pix_y)
{
	unsigned char *pixel_ref;

	//get center/target pixel value
	pixel_ref = (unsigned char*)&iplImg->imageData[(target_pix_x)*iplImg->nChannels+(target_pix_y)*iplImg->widthStep];

    *pixel_ref = pix_data;
}
