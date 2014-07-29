/*
 * pixel_process.cpp
 *
 *  Created on: Jan 28, 2010
 *      Author: tkcheng
 */

/*  **** Function in this file ****
 *  unsigned char Filter_Sobel_3X3(unsigned char * data, unsigned char axis, unsigned char direction)
 *  Filter_Threshold
 *  unsigned char Filter_SearchMax(unsigned char * data, unsigned char size)
 *  unsigned char Filter_SearchMedian(unsigned char * data, unsigned char size)
 *  unsigned char Filter_SearchMin(unsigned char * data, unsigned char size)
 *  unsigned char Filter_SearchAverage(unsigned char * data, unsigned char size)
 */
//#define DEBUG_SOBELX

#include <iostream>
#include <cstdio>

//Laplacian for edge detection
signed int laplacian_matrics_4[]=	{  0, -1,  0,
									  -1,  4, -1,
									   0, -1,  0};
signed int laplacian_matrics_8[]=	{ -1, -1, -1,
									  -1,  8, -1,
									  -1, -1, -1};
signed int laplacian_matrics_5[]=	{  0, -1,  0,
									  -1,  5, -1,
									   0, -1,  0};
signed int laplacian_matrics_9[]=	{ -1, -1, -1,
									  -1,  9, -1,
									  -1, -1, -1};
#define EXCLUDE_DIAGONAL		0
#define INCLUDE_DIAGONAL		1
#define NOT_APPLY_TO_IMG		0
#define APPLY_TO_IMG			1
#define CENTER					4
signed int Filter_Laplacian_3X3(unsigned char * data, unsigned char apply, unsigned char diagonal, unsigned char scale)
{
	unsigned char i;
	signed int value=0;
	signed int temp_val;
	static unsigned char last_scale=0;

	for(i=0;i<9;i++)
	{
		if(apply == APPLY_TO_IMG)
		{
			if(diagonal == INCLUDE_DIAGONAL)
			{
				if(i == CENTER)
				{
					temp_val = data[i]*laplacian_matrics_9[i];
					value += (temp_val*(scale+50))/70;
					last_scale = scale;
				}
				else
					value += data[i]*laplacian_matrics_9[i];
			}
			else
			{
				if(i == CENTER)
				{
					temp_val = data[i]*laplacian_matrics_5[i];
					value += (temp_val*(scale+50))/70;
					last_scale = scale;
				}
				else
					value += data[i]*laplacian_matrics_5[i];
			}
		}
		else
		{
			if(diagonal == INCLUDE_DIAGONAL)
			{
				if(i == CENTER)
				{
					temp_val = data[i]*laplacian_matrics_8[i];
					value += (temp_val*(scale+50))/70;
					last_scale = scale;
				}
				else
					value += data[i]*laplacian_matrics_8[i];
			}
			else
			{
				if(i == CENTER)
				{
					temp_val = data[i]*laplacian_matrics_4[i];
					value += (temp_val*(scale+50))/70;
					last_scale = scale;
				}
				else
					value += data[i]*laplacian_matrics_4[i];
			}
		}
	}
/*
	if(value < 0)
		value = 0;
	if(value > 0xff)
		value = 0xff;
*/
	//image scaling
///	value += 255;
//	value /= 2;

	return value;
}
/*
 * PARAM IN : data : data to be process
 * 			: threshold : threshold value
 * PARAM OUT: return the 1 if input value > threshold, else 0
 * FUNCTION : convert date into binary value
 */
unsigned char Filter_Threshold(unsigned char data, unsigned char threshold)
{
	if(data>=threshold)
		return 0xff;
	else
		return 0;
}

//SOBEL for edge detection

signed char sobel_matricsY_HIGH2LOW[]=	{ 1,  2,  1,
										  0,  0,  0,
										 -1, -2, -1};

signed char sobel_matricsY_LOW2HIGH[]=	{-1, -2, -1,
										  0,  0,  0,
										  1,  2,  1};

signed char sobel_matricsX_HIGH2LOW[]=	{ 1, 0, -1,
										  2, 0, -2,
										  1, 0, -1};

signed char sobel_matricsX_LOW2HIGH[]=	{-1, 0, 1,
										 -2, 0, 2,
										 -1, 0, 1};
//SOBEL Scharr filter
signed char scharr_matricsY_HIGH2LOW[]=	{ 3,  10,  3,
										  0,   0,  0,
										 -3, -10, -3};

signed char scharr_matricsY_LOW2HIGH[]=	{-3, -10, -3,
										  0,   0,  0,
										  3,  10,  3};

signed char scharr_matricsX_HIGH2LOW[]=	{ 3, 0,  -3,
										 10, 0, -10,
										  3, 0,  -3};

signed char scharr_matricsX_LOW2HIGH[]=	{ -3, 0,  3,
										 -10, 0, 10,
										  -3, 0,  3};

#define USE_SOBEL					0
#define USE_SCHARR					1
#define DETECT_HIGH2LOW_EDGE		1
#define DETECT_LOW2HIGH_EDGE		0
#define X_DIRECTION					1
#define Y_DIRECTION					0
/*
 * PARAM IN : data : pointer to data to be process
 * 			: axis : Sobel direction, can be X-axis detection or Y-axis detection
 * 			: direction : Sobel can only detect gradient from Low brightness to High brightness or vice versa
 * 						: for complete detection (low->high and high->), this function should call both with
 * 						: direction =1 and =0 respectively
 * PARAM OUT: value : final data after execute Sobel filter
 * FUNCTION : perform Sobel filtering on input data
 */
unsigned char Filter_Sobel_3X3(unsigned char * data, unsigned char axis, unsigned char direction, unsigned char scharr)
{
	unsigned char i;
	signed int value=0, ref_data;
	signed char *sobel_LUT;

#ifdef DEBUG_SOBELX
	static unsigned char run_once=0;

	if(run_once ==0)
	{
		printf("sobelX input param\n");
		printf("data[0] = %d\n",data[0]);
		printf("data[1] = %d\n",data[1]);
		printf("data[2] = %d\n",data[2]);
		printf("data[3] = %d\n",data[3]);
		printf("data[4] = %d\n",data[4]);
		printf("data[5] = %d\n",data[5]);
		printf("data[6] = %d\n",data[6]);
		printf("data[7] = %d\n",data[7]);
		printf("data[8] = %d\n",data[8]);

	}
#endif
	if(scharr == USE_SCHARR)
	{
		if(axis == Y_DIRECTION)
		{
			if(direction == DETECT_HIGH2LOW_EDGE)
				sobel_LUT = scharr_matricsY_HIGH2LOW;
			else
				sobel_LUT = scharr_matricsY_LOW2HIGH;
		}
		else
		{
			if(direction == DETECT_HIGH2LOW_EDGE)
				sobel_LUT = scharr_matricsX_HIGH2LOW;
			else
				sobel_LUT = scharr_matricsX_LOW2HIGH;
		}
	}
	else
	{
		if(axis == Y_DIRECTION)
		{
			if(direction == DETECT_HIGH2LOW_EDGE)
				sobel_LUT = sobel_matricsY_HIGH2LOW;
			else
				sobel_LUT = sobel_matricsY_LOW2HIGH;
		}
		else
		{
			if(direction == DETECT_HIGH2LOW_EDGE)
				sobel_LUT = sobel_matricsX_HIGH2LOW;
			else
				sobel_LUT = sobel_matricsX_LOW2HIGH;
		}
	}

	for(i=0;i<9;i++)
	{
		ref_data = data[i];
		value += sobel_LUT[i] * ref_data;
#ifdef DEBUG_SOBELX
		if(run_once==0)
		{
			printf("sobel[%d] = %d\n",i,value);
		}
#endif
	}
#ifdef DEBUG_SOBELX
	if(run_once ==0)
	{
		printf("sobelX = %d\n",value);
		run_once =1;
	}
#endif
//	value /=9;
	//limit max and min value
	if(value < 0)
		value = 0;
	if(value > 0xff)
		value = 0xff;
	return (unsigned char)value;
}

/*
 * PARAM IN : data : pointer to data to be process
 * 			: size : total of element to search through
 * PARAM OUT: return the Maximum value within the data range
 * FUNCTION : return the Maximum value within the data range
 */
unsigned char Filter_SearchMax(unsigned char * data, unsigned char size)
{
	unsigned char i,max_value;

	max_value = data[0];
	for(i=1;i<size;i++)
	{
		if(max_value < data[i])
			max_value = data[i];
	}

	return max_value;
}

/*
 * PARAM IN : data : pointer to data to be process
 * 			: size : total of element to search through
 * PARAM OUT: return the Minimum value within the data range
 * FUNCTION : return the Minimum value within the data range
 */
unsigned char Filter_SearchMin(unsigned char * data, unsigned char size)
{
	unsigned char i,min_value;

	min_value = data[0];
	for(i=1;i<size;i++)
	{
		if(min_value > data[i])
			min_value = data[i];
	}

	return min_value;
}

/*
 * PARAM IN : data : pointer to data to be process
 * 			: size : total of element to search through
 * PARAM OUT: return the Average value within the data range
 * FUNCTION : return the Average value within the data range
 */
unsigned char Filter_SearchAverage(unsigned char * data, unsigned char size)
{
	unsigned int val=0;
	unsigned char i;

	for(i=1;i<size;i++)
	{
		val += data[i];
	}

	val /= size;

	return (unsigned char)val;
}

/*
 * PARAM IN : data : pointer to data to be process
 * 			: size : total of element to search through
 * PARAM OUT: return the Median value within the data range
 * FUNCTION : return the Median value within the data range
 */
unsigned char Filter_SearchMedian(unsigned char * data, unsigned char size)
{
	unsigned char i, y, num_higher_value, val;
	unsigned char smallest_val,smallest_loc=0;

	num_higher_value = (size/2) + 1;

#ifdef DBUG_MESG
	printf("num_higher_value  = %d\n",num_higher_value );
#endif
	//assume first num_higher_value is the higher value
	//then search through the rest of the list compare to the first num_higher_valu element
	for(i=num_higher_value;i<size;i++)
	{
		val = data[i];

		//search for the smallest value among 'num_higher_value'
		smallest_val = data[0];
		smallest_loc = 0;
		for(y=1;y<num_higher_value;y++)
		{
			if(smallest_val > data[y])
			{
				smallest_val = data[y];
				smallest_loc = y;
			}

		}

		if(val > smallest_val)
		{
			data[smallest_loc] = val;
		}
	}
#ifdef DBUG_MESG
	printf("data = %d\n",data[0]);
	printf("data = %d\n",data[1]);
	printf("data = %d\n",data[2]);
	printf("data = %d\n",data[3]);
	printf("data = %d\n",data[4]);
#endif

	//search for smallest value in buffer sort_data
	smallest_val = data[0];
	for(i=1;i<num_higher_value;i++)
	{
		if(smallest_val > data[i])
		{
			smallest_val = data[i];
			smallest_loc = i;
		}
	}

	return smallest_val;
}


