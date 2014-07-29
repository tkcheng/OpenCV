/*
 * fft.cpp
 *
 *  Created on: Feb 11, 2010
 *      Author: tkcheng
 */
#include "stdio.h"
#include "cv.h"
#include "highgui.h"

#include "math.h"
#include "type.h"

/*
 * m = total number of sample
 * dir = inverse DFT or forward DFT
 * x1 =
 * y1 =
 */
int DFT(int dir,long m,IplImage* DFT_src, IplImage* DFT_dst)
{
	int pos_x,pos_y;
	long i,k;
	double arg;
	double cosarg,sinarg,temp;
	double *x1=NULL,*x2=NULL,*y2=NULL;

	x1 = (double*)malloc(m*sizeof(double));
	x2 = (double*)malloc(m*sizeof(double));
	y2 = (double*)malloc(m*sizeof(double));

	if (x1 == NULL || x2 == NULL || y2 == NULL)
      return(FALSE);

/*
//testing, copy src to dst
	for(pos_y=0;pos_y< (DFT_src->height);pos_y++)
	{
		for(pos_x=0;pos_x< (DFT_src->width);pos_x++)
		{
			DFT_dst->imageData[(pos_x)*DFT_dst->nChannels+(pos_y)*DFT_dst->widthStep] = DFT_src->imageData[(pos_x)*DFT_src->nChannels+(pos_y)*DFT_src->widthStep];
		}
	}
*/

/*
 * testing : src -> x1/double -> dst
 */	//convert frorm 8U type to double

	for(pos_y=0;pos_y< (DFT_src->height);pos_y++)
	{
		for(pos_x=0;pos_x< (DFT_src->width);pos_x++)
		{
			x1[pos_y*DFT_src->width + pos_x] = DFT_src->imageData[(pos_x)*DFT_src->nChannels+(pos_y)*DFT_src->widthStep];
		}
    }
/*
	for(pos_y=0;pos_y< (DFT_src->height);pos_y++)
	{
		for(pos_x=0;pos_x< (DFT_src->width);pos_x++)
		{
			DFT_dst->imageData[(pos_x)*DFT_dst->nChannels+(pos_y)*DFT_dst->widthStep] = x2[pos_y*DFT_src->width + pos_x];
		}
    }
*/

	printf("m=%d",m);
	printf("start\n");

	for (i=0;i<m;i++)
	{
    	x2[i] = 0;
    	y2[i] = 0;
    	arg = dir * 2.0 * 3.141592654 * (double)i / (double)m;
    	for (k=0;k<m;k++)
    	{
        	cosarg = cos(k * arg);
        	sinarg = sin(k * arg);
//        	x2[i] += (x1[k] * cosarg - y1[k] * sinarg);
//        	y2[i] += (x1[k] * sinarg + y1[k] * cosarg);
//        	x2[i] += (x1[k] * sinarg + x1[k] * cosarg);
        	x2[i] += (x1[k] * cosarg - x1[k] * sinarg);
        	//search for magnitude of the complex number
//        	temp = (x1[k] * sinarg)*(x1[k] * sinarg) + (x1[k] * cosarg)*(x1[k] * cosarg);
//        	x2[i] += sqrt(temp);

    	}
    	if(i%1000==0)
    		printf("%d\n",i);
   }

	printf("completed\n");

   /* Copy the data back */
   if (dir == 1)
   {
//      for (i=0;i<m;i++)
//      {
//         x1[i] = x2[i] / (double)m;
//         y1[i] = y2[i] / (double)m;
//      }

		for(pos_y=0;pos_y< (DFT_src->height);pos_y++)
		{
			for(pos_x=0;pos_x< (DFT_src->width);pos_x++)
			{
				DFT_dst->imageData[(pos_x)*DFT_dst->nChannels+(pos_y)*DFT_dst->widthStep] = x2[pos_y*DFT_src->width + pos_x]/(double)m;
			}
	    }

   }
   else
   {
//      for (i=0;i<m;i++)
//      {
//         x1[i] = x2[i];
//       y1[i] = y2[i];
//      }
		for(pos_y=0;pos_y< (DFT_src->height);pos_y++)
		{
			for(pos_x=0;pos_x< (DFT_src->width);pos_x++)
			{
				DFT_dst->imageData[(pos_x)*DFT_dst->nChannels+(pos_y)*DFT_dst->widthStep] = x2[pos_y*DFT_src->width + pos_x];
			}
	    }
   }

   free(x2);
   free(y2);
   return(TRUE);
}
