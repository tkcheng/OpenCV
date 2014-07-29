/* This file will have simple example of using opencv component
 *
 */
#define CV_NO_BACKWARD_COMPATIBILITY

//****Input method : file/camera/avi file ****//
//video file : not implement
//#define ENABLE_OPEN_FILE				//read a lena.jpg file from pc
#define FILE_NAME					"../sample/color.jpg"
//#define FILE_NAME					"../sample/lena.jpg"
//#define FILE_NAME					"../sample/stp1fur3.png"//"../stp5.png"//"../son3.png"//"../lena.jpg"
#ifndef ENABLE_OPEN_FILE
	#define ENABLE_WEB_CAMERA			//open build-in camera
	//#define	ENABLE_VIDEO_FILE			//open fish.avi file
	#define VIDEO_FILE_NAME			"../2.mp4"//"fish.avi"
#endif

//**** Input method : mouse ****//
#define ENABLE_MOUSE_DETECTION				//refer to project opencv_basic
//**** Input method : keyboard ****//
#define ENABLE_KEYBOARD_DETECTION			//refer to project opencv_basic
//**** Input method : trackbar/slider ****//
#define ENABLE_SLIDER						//refer to project opencv_basic
//****Image Process : Colorspace conversion ****//
#define ENABLE_IMG_COLORSPACE_CONVERT		//refer to project opencv_basic
//****Image Process : Drawing ****//
#define ENABLE_IMG_DRAW						//refer to project opencv_basic
#ifdef ENABLE_IMG_DRAW
	#define DRAW_RECTANGLE
	#define DRAW_CIRCLE
	#define DRAW_LINE
	#define DRAW_POLYLINE
	#define DRAW_FILL_POLY
	#define DRAW_TEXT
#endif

//****Image Process : Identify white color from RGB image ****//
//#define IMG_DETECT_WHITE	//not working

//****Image Process : Contour ****//
//#define IMG_CONTOUR

//****Image Process : Sobel ****//
#define IMG_SOBEL

//**** Image Process : Laplacian ****//
//#define IMG_LAPLACIAN

//**** Image Process : Erode/Dilation ****//
//#define IMG_ERODE

//**** Image Process : DFT (Fast Fourier Transform)****//
//#define IMG_DFT //not working

#define NUM_ELEMENT		9

#include "cv.h"
#include "highgui.h"

#include <iostream>
#include <cstdio>


////////////////////////////FUNCTION PROTOTYPE/////////////////////////////
extern void trackbarHandlerdummy(int pos);

extern void SetPixel(IplImage* iplImg, unsigned char pix_data, int target_pix_x, int target_pix_y);
extern void KernelExtract3X3(IplImage* iplImg, unsigned char *pix_data, int target_pix_x, int target_pix_y);

extern unsigned char Filter_SearchMedian(unsigned char * data, unsigned char size);
extern unsigned char Filter_SearchMax(unsigned char * data, unsigned char size);
extern unsigned char Filter_SearchMin(unsigned char * data, unsigned char size);
extern unsigned char Filter_SearchAverage(unsigned char * data, unsigned char size);

unsigned char Filter_Sobel_3X3(unsigned char * data, unsigned char axis, unsigned char direction);
#define DETECT_HIGH2LOW_EDGE		1
#define DETECT_LOW2HIGH_EDGE		0
#define X_DIRECTION					1
#define Y_DIRECTION					0
unsigned char Filter_Threshold(unsigned char data, unsigned char threshold);
void Image_Threshold(IplImage* image_src, IplImage* image_dst);
void Image_SobelXY(IplImage* image_src, IplImage* image_dst);
void Image_Laplacian(IplImage* iplImg_Src, IplImage* image_dst);
void Image_MaxMinMedianAvg(IplImage* iplImg_Src, IplImage* image_dst);

void Image_DFT(IplImage* iplImg_Src);
////////////////////////////END:FUNCTION PROTOTYPE///////////////////////////////

#ifdef _EiC
#define WIN32
#endif

using namespace std;
using namespace cv;

String cascadeName =
"../data/haarcascades/haarcascade_frontalface_alt.xml";
String nestedCascadeName =
"../data/haarcascades/haarcascade_eye_tree_eyeglasses.xml";

CvCapture* capture = NULL;
IplImage* ImageSource1;

int main( int argc, const char** argv )
{
    Mat frame, frameCopy;

	printf("running openCV Basic\n");

#ifdef ENABLE_OPEN_FILE
	printf("Load from file ");
	printf("%s\n",FILE_NAME);

	IplImage* iplImg = cvLoadImage( FILE_NAME);
#else
	#ifdef ENABLE_WEB_CAMERA
		printf("Capture from view cam\n");

	    const String scaleOpt = "--scale=";
	    size_t scaleOptLen = scaleOpt.length();
	    const String cascadeOpt = "--cascade=";
	    size_t cascadeOptLen = cascadeOpt.length();
	    const String nestedCascadeOpt = "--nested-cascade";
	    size_t nestedCascadeOptLen = nestedCascadeOpt.length();
	    String inputName;


	    CascadeClassifier cascade, nestedCascade;
	    double scale = 2;

	    for( int i = 1; i < argc; i++ )
	    {
	        if( cascadeOpt.compare( 0, cascadeOptLen, argv[i], cascadeOptLen ) == 0 )
	            cascadeName.assign( argv[i] + cascadeOptLen );
	        else if( nestedCascadeOpt.compare( 0, nestedCascadeOptLen, argv[i], nestedCascadeOptLen ) == 0 )
	        {
	            if( argv[i][nestedCascadeOpt.length()] == '=' )
	                nestedCascadeName.assign( argv[i] + nestedCascadeOpt.length() + 1 );
	            if( !nestedCascade.load( nestedCascadeName ) )
	                cerr << "WARNING: Could not load classifier cascade for nested objects" << endl;
	        }
	        else if( scaleOpt.compare( 0, scaleOptLen, argv[i], scaleOptLen ) == 0 )
	        {
	            if( !sscanf( argv[i] + scaleOpt.length(), "%lf", &scale ) || scale < 1 )
	                scale = 1;
	        }
	        else if( argv[i][0] == '-' )
	        {
	            cerr << "WARNING: Unknown option %s" << argv[i] << endl;
	        }
	        else
	            inputName.assign( argv[i] );
	    }

	    if( !cascade.load( cascadeName ) )
	    {
	        cerr << "ERROR: Could not load classifier cascade" << endl;
	        cerr << "Usage: facedetect [--cascade=\"<cascade_path>\"]\n"
	            "   [--nested-cascade[=\"nested_cascade_path\"]]\n"
	            "   [--scale[=<image scale>\n"
	            "   [filename|camera_index]\n" ;
	        return -1;
	    }

		capture = cvCaptureFromCAM( inputName.empty() ? 0 : inputName.c_str()[0] - '0' );

	#else
		#ifdef ENABLE_VIDEO_FILE
			CvCapture* capture = NULL;
			capture = cvCreateFileCapture( VIDEO_FILE_NAME );
		#else
			!error!Undefined input picture
		#endif
	#endif
#endif

	cvNamedWindow( "original" );

	printf("Press 'q' to quit program\n");

    for(;;)
    {

#ifdef ENABLE_WEB_CAMERA
		IplImage* iplImg = cvQueryFrame( capture );
#else
	#ifdef ENABLE_VIDEO_FILE
		IplImage* iplImg = cvQueryFrame( capture );
	#endif
        if( !iplImg )
        {
        	printf("Invalid Img File\n");
        	break;
        }
#endif

        //show first image without any process
        frame = iplImg;
        if( frame.empty() )
            return 0;
        if( iplImg->origin == IPL_ORIGIN_TL )
            frame.copyTo( frameCopy );
        else
            flip( frame, frameCopy, 0 );
        cv::imshow( "original", frameCopy );

        int key = waitKey (10);
        if( key == 'q' )		//wait key for 10ms, if 0 then wait indefinitely
        {
        	goto _cleanup_;
        }
#ifdef IMG_DFT
        Mat dft_grayscale;

        //convert input picture to grayscale
        cvtColor( iplImg, dft_grayscale, CV_BGR2GRAY );
        cv::imshow( "Convert to Grayscale", dft_grayscale);

        //assign src image type
        //Mat -> IplImage
        IplImage DFT_iplImg_gray = dft_grayscale;
//        IplImage* laplace_iplImg_Src = &laplace_iplImg_gray ;

		//destination image
//    	IplImage* laplacian_image_dst = cvCloneImage(laplace_iplImg_Src);
    	Image_DFT(&DFT_iplImg_gray);

//        IplImage* iplImg_dummy = cvCloneImage(laplacian_image_dst);
//        Image_Threshold(laplacian_image_dst, iplImg_dummy);
#endif
#ifdef IMG_DETECT_WHITE //failed :-)
		{
static unsigned char run_once=0;

if(run_once == 0)
{
	run_once = 1;
            Mat grayscale;
		    Mat img_yuv;

    		IplImage* img_dst = cvCloneImage(iplImg);

			//convert input picture to YUV
//            cvtColor( iplImg, img_yuv, CV_BGR2HLS );
            cvtColor( iplImg, img_yuv, CV_BGR2HSV );

            //assign src image type
            //Mat -> IplImage
            IplImage iplImg_tmp = img_yuv;
			IplImage* iplImg_yuv = &iplImg_tmp;

        IplImage *y  = cvCreateImage(cvGetSize(iplImg_yuv), iplImg_yuv->depth, 1);
        IplImage *cb = cvCreateImage(cvGetSize(iplImg_yuv), iplImg_yuv->depth, 1);
        IplImage *cr = cvCreateImage(cvGetSize(iplImg_yuv), iplImg->depth, 1);
        IplImage *threshold = cvCreateImage(cvGetSize(iplImg_yuv), iplImg->depth, 1);
        IplImage *dst = cvCreateImage(cvGetSize(iplImg_yuv), iplImg->depth, 1);
		
        cvSplit(iplImg_yuv, y, cb, cr, NULL);

		cvMerge(y,cb,cr,NULL,img_dst);

//threshold on component
						     //threshold,  max_val
//		cvThreshold(y, threshold, 25,       255,   CV_THRESH_BINARY_INV);
//		cvShowImage( "-threshold-y-", threshold );
		cvThreshold(y, threshold, 25,       1,   CV_THRESH_BINARY_INV);
//		cvShowImage( "-threshold-y-", threshold );
		cvMul(threshold, cr, dst);
		cvShowImage( "-dst*h-", dst );

//		cvThreshold(cb, threshold, 150,       255,   CV_THRESH_BINARY_INV);
//		cvShowImage( "-threshold-s", threshold );
		cvThreshold(cb, threshold, 150,       1,   CV_THRESH_BINARY_INV);
//		cvShowImage( "-threshold-s-binary", threshold );
		cvMul(dst, threshold, dst);
		cvShowImage( "-dst-", dst );

		cvThreshold(dst, threshold, 50, 255, CV_THRESH_BINARY);
		cvShowImage( "-threshold-dst", threshold );

		cvShowImage( "-H-", y );
		cvShowImage( "-s-", cb );
		cvShowImage( "-B-", cr );

		Mat img_disp;
		cvtColor( img_dst, img_disp, CV_YCrCb2BGR );
		cv::imshow("aaa", img_disp);
}		

		}
#endif	//IMG_DETECT_WHITE
#ifdef IMG_SOBEL
        {
            Mat grayscale;

            //convert input picture to grayscale
            cvtColor(Mat(iplImg), grayscale, CV_BGR2GRAY );
            cv::imshow( "Convert to Grayscale", grayscale);

            //assign src image type
            //Mat -> IplImage
            IplImage iplImg_gray = grayscale;
            IplImage* iplImg_Src = &iplImg_gray;

			//destination image
        	IplImage* image_dst = cvCloneImage(iplImg_Src);
        	Image_SobelXY(iplImg_Src, image_dst);

       	    ImageSource1 = cvCloneImage(image_dst);

            //IplImage* iplImg_dummy = cvCloneImage(image_dst);
            //Image_Threshold(image_dst, iplImg_dummy);
        }
#endif
#ifdef IMG_ERODE
        Mat erode_grayscale;

        //convert input picture to grayscale
        cvtColor( iplImg, erode_grayscale, CV_BGR2GRAY );
        //cv::imshow( "Grayscale", erode_grayscale);

        //convert 'Mat' -> IplImage
        IplImage erode_iplImg_gray = erode_grayscale;
        //IplImage *erode_iplImg_src = &erode_iplImg_gray;
        //lets use sober filter result instead of raw greyscale
        IplImage *erode_iplImg_src = ImageSource1;

    	IplImage* erode_image_dst = cvCloneImage(erode_iplImg_src);
/*
        cvErode(erode_iplImg_src, erode_image_dst);
        cvNamedWindow( "Erode", 1 );
        cvShowImage( "Erode", erode_image_dst );
*/
/*
        cvDilate(erode_iplImg_src, erode_image_dst);
        cvNamedWindow( "Dilation", 1 );
        cvShowImage( "Dilation", erode_image_dst );
*/
        Image_MaxMinMedianAvg(erode_iplImg_src, erode_image_dst);
        cvtColor( iplImg, laplace_grayscale, CV_BGR2GRAY );
        cv::imshow( "Convert to Grayscale", laplace_grayscale);

        //assign src image type
        //Mat -> IplImage
        IplImage laplace_iplImg_gray = laplace_grayscale;
        IplImage* laplace_iplImg_Src = &laplace_iplImg_gray ;

		//destination image
    	IplImage* laplacian_image_dst = cvCloneImage(laplace_iplImg_Src);
    	Image_Laplacian(laplace_iplImg_Src, laplacian_image_dst);

        IplImage* iplImg_dummy = cvCloneImage(laplacian_image_dst);
        Image_Threshold(laplacian_image_dst, iplImg_dummy);
#endif
#ifdef IMG_CONTOUR
        CvMemStorage* storage = cvCreateMemStorage(0);
        CvSeq* contour = 0;
//        static int contour_level = -1;
        static int contour_level =1;
        Mat grayscale;

        cvNamedWindow( "CONTOUR", 1 );
    	cvCreateTrackbar("CONTOUR Level","CONTOUR",&contour_level,10,trackbarHandlerdummy);

        //convert img to grayscale
        cvtColor( frameCopy, grayscale, CV_BGR2GRAY );
//      cv::imshow( "Convert to Grayscale", grayscale);

        //convert 'Mat' -> 'iplImg'
        IplImage iplImg_binary = grayscale;

        //perform threshold to convert grayscale to binary image
        int threshold_val = 40;
        cvThreshold( &iplImg_binary, &iplImg_binary, threshold_val, 255, CV_THRESH_BINARY );

        IplImage* contour_dst = cvCreateImage( cvGetSize(&iplImg_binary), 8, 3 );
//        cvNamedWindow( "Source", 1 );
//        cvShowImage( "Source", &iplImg_binary );

        cvFindContours( &iplImg_binary, storage, &contour, sizeof(CvContour),
                      CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );
        cvZero( contour_dst );

        if(key == '>')
        {
        	contour_level ++;
       		printf("Contour level = %d\n",contour_level);
        }

        if(key == '<')
        {
        	if(contour_level > -1)
        	{
        		contour_level --;
        		printf("Contour level = %d\n",contour_level);
        	}
        	else
        		printf("Contour level decrement exceed limit\n");
        }

        for( ; contour != 0; contour = contour->h_next )
        {
//          CvScalar color = CV_RGB( rand()&255, rand()&255, rand()&255 );
            /* replace CV_FILLED with 1 to see the outlines */
//          cvDrawContours( contour_dst, contour, color, color, 0, CV_FILLED, 8 );

            cvDrawContours( contour_dst, contour, CV_RGB(255,0,0), CV_RGB(0,255,0), contour_level, 2/*CV_FILLED*/, 4 );
        }

        cvNamedWindow( "Components", 1 );
        cvShowImage( "Components", contour_dst );


        cvAdd(iplImg, contour_dst, contour_dst, NULL);
        cvNamedWindow( "Final Results", 1 );
        cvShowImage( "Final Results", contour_dst );

#endif

    }

_cleanup_:

#ifdef ENABLE_WEB_CAMERA
	cvReleaseCapture( &capture );
#endif
    cvDestroyWindow("original");

    return 0;
}

