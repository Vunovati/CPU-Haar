#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>



float *haarStep(float *cpu_image, int rows, int columns)
{
	float* filtered_image = new float[rows * columns * 4];
	float L12, L34, H12, H34;
	int x,y, xx;

	for (x=0; x<rows/2; x++)
	{
		for(y=0; y<columns/2; y++)
		{
			xx = 2*(x + rows*y); // [0][0] svakog kvadratica (svaki drugi po retcima i stupcima)
			L12 = (cpu_image[xx] + cpu_image[xx+1]); // kvadratic[0][0] + kvadratic[1][0]
			L34 = (cpu_image[xx + rows] + cpu_image[xx + rows + 1]); // kvadratic[0][1] + kvadratic[1][1]
			H12 = (cpu_image[xx] - cpu_image[xx+1]); // kvadratic[0][0] - kvadratic[1][0]
			H34 = (cpu_image[xx + rows] - cpu_image[xx + rows + 1]); // kvadratic[0][1] - kvadratic[1][1]

			filtered_image[x + rows*y] = (L12 + L34)/4; // gornji lijevi kvadrant
			filtered_image[(x + rows/2) + rows*y ] = (L12 - L34)/4; // donji lijevi kvadrant
			filtered_image[x + rows/2 * columns + rows*y] = (H12 + H34)/4; // gornji desni kvadrant
			filtered_image[(x + rows/2) + rows/2 * columns + rows*y] = (H12 - H34)/4; // donji desni kvadrant
		}
	}
	return( filtered_image ); 
}

float *haarStepBGR(float *cpu_image, int rows, int columns, int boja)
{
	float* filtered_image = new float[rows * columns * 4];
	float L12, L34, H12, H34;
	int x,y, xx;

	for (x=0; x<rows/2; x++)
	{
		for(y=0; y<columns/2; y++)
		{
			xx = 3 * (2*(x + rows*y)) + boja; // [0][0] svakog kvadratica (svaki drugi po retcima i stupcima)
			L12 = (cpu_image[xx] + cpu_image[xx+1]); // kvadratic[0][0] + kvadratic[1][0]
			L34 = (cpu_image[xx + rows] + cpu_image[xx + rows + 1]); // kvadratic[0][1] + kvadratic[1][1]
			H12 = (cpu_image[xx] - cpu_image[xx+1]); // kvadratic[0][0] - kvadratic[1][0]
			H34 = (cpu_image[xx + rows] - cpu_image[xx + rows + 1]); // kvadratic[0][1] - kvadratic[1][1]

			filtered_image[3 * (x + rows*y) + boja] = (L12 + L34)/4; // gornji lijevi kvadrant
			filtered_image[3 * ((x + rows/2) + rows*y) + boja] = (L12 - L34)/4; // donji lijevi kvadrant
			filtered_image[3 * (x + rows/2 * columns + rows*y) + boja] = (H12 + H34)/4; // gornji desni kvadrant
			filtered_image[3 * ((x + rows/2) + rows/2 * columns + rows*y) + boja] = (H12 - H34)/4; // donji desni kvadrant
		}
	}
	return( filtered_image ); 
}

float *CV2float(IplImage *img)
{
	float* out_image = new float[img->width * img->height * 4];

	//if (img->nChannels == 1)
	//{
	for (int i = 0; i < (img->width * img->height); i++)
	{
		out_image[i] = (unsigned char)img->imageData[i] / 255.f; // pretvori svaki element iz chara u float (vrijednosti 0..1)
	}
	//}
	//else
	//{
	//  for (int i = 0; i < (img->width * img->height * 4); i++)
	//	{
	//		out_image[i * 3 + 0] = (unsigned char)img->imageData[i * 4 + 0] / 255.f; // |B|G|R|prazno|B|G|R|prazno|B|G|R|prazno|
	//		out_image[i * 3 + 1] = (unsigned char)img->imageData[i * 4 + 1] / 255.f;
	//		out_image[i * 3 + 2] = (unsigned char)img->imageData[i * 4 + 2] / 255.f;
	//	}
	//
	//}


	return( out_image );
}

char *float2CVdata(float *image, int nPixels, int channels)
{
	char* out_char = new char[nPixels * 4];
	//	
	//if (channels = 1)
	//{
	for (int i = 0; i<(nPixels); i++)
	{
		out_char[i] = (char)floor(image[i] * 255.f); // prebaci nazad u char
	}
	//}
	//else
	//{
	//	for (int i = 0; i < (nPixels * 4); i++)
	//	{
	//		out_char[i * 4 + 0] = (char)floor(image[i * 3 + 0] * 255.f);
	//		out_char[i * 4 + 0] = (char)floor(image[i * 3 + 0] * 255.f);
	//		out_char[i * 4 + 0] = (char)floor(image[i * 3 + 0] * 255.f);		
	//	}	
	//}

	return( out_char );
}

int main(int argc, char *argv[])
{

	CvCapture* capture = cvCaptureFromCAM(0); // capture from video device


	if(!cvGrabFrame(capture))
	{
		printf("could not grab a frame\n\7");
		exit(0);
	}


	while(true)
	{
		IplImage* frame = 0;


		if(!cvGrabFrame(capture))
		{
			printf("could not grab a frame\n\7");
			exit(0);
		}

		frame = cvRetrieveFrame(capture); // retrieve the captured frame

		cvSetImageROI(frame, cvRect(0,0,400,400)); // namjesti ROI



		IplImage* img = cvCreateImage(cvGetSize(frame), frame->depth, frame->nChannels);

		cvCopy(frame, img, NULL);

		//IplImage* img = cvLoadImage("testnaslika.jpg", CV_LOAD_IMAGE_UNCHANGED); 
		//IplImage* img = cvLoadImage("testnaslika.jpg", CV_LOAD_IMAGE_GRAYSCALE);   



		int height,width,step,channels;


		// get the image data
		height    = img->height;
		width     = img->width;
		step      = img->widthStep;
		channels  = img->nChannels;
		/*data      = (uchar *)img->imageData;*/
		printf("Processing a %dx%d image with %d channels\n",img->height,img->width,img->nChannels); 

		float* cpu_image = new float[img->width * img->height * 4];
		float* cpu_blue = new float[img->width * img->height * 4];
		float* cpu_green = new float[img->width * img->height * 4];
		float* cpu_red = new float[img->width * img->height * 4];


		float* filtered_image = new float[img->width * img->height * 4];
		float* filtered_blue = new float[img->width * img->height * 4];
		float* filtered_green = new float[img->width * img->height * 4];
		float* filtered_red = new float[img->width * img->height * 4];


		//char* buff = new char[(width) * (img->height) * 4];
		//char* blue_buff = new char[(width) * (img->height)];
		//char* green_buff = new char[(width) * (img->height)];
		//char* red_buff = new char[(width) * (img->height)];
		IplImage* out_image = cvCreateImage(cvSize(img->width, img->height), img->depth, img->nChannels);
		IplImage* blue_image = cvCreateImage(cvSize(img->width, img->height), img->depth, 1);
		IplImage* green_image = cvCreateImage(cvSize(img->width, img->height), img->depth, 1);
		IplImage* red_image = cvCreateImage(cvSize(img->width, img->height), img->depth, 1);



		if (img->nChannels == 1)
		{
			cpu_image = CV2float(img);

			filtered_image = haarStep(cpu_image, img->height, img->width); // pozivamo funkciju za haar
			//buff = float2CVdata(filtered_image, img->width*img->height, img->nChannels);
			out_image->imageData = float2CVdata(filtered_image, img->width*img->height, img->nChannels);
			//out_image->imageData = buff; 


		}
		else 
		{
			cvSplit(img, blue_image, green_image, red_image, NULL);

			cpu_blue = CV2float(blue_image);
			cpu_green = CV2float(green_image);
			cpu_red = CV2float(red_image);


			filtered_blue = haarStep(cpu_blue, img->height, img->width);
			filtered_green = haarStep(cpu_green, img->height, img->width);
			filtered_red = haarStep(cpu_red, img->height, img->width);


			blue_image->imageData = float2CVdata(filtered_blue, img->width*img->height, img->nChannels);
			green_image->imageData = float2CVdata(filtered_green, img->width*img->height, img->nChannels);
			red_image->imageData = float2CVdata(filtered_red, img->width*img->height, img->nChannels);


			/*blue_image->imageData = blue_buff;*/
			//green_image->imageData = green_buff;
			//red_image->imageData = red_buff;

			cvMerge(blue_image, green_image, red_image, NULL, out_image);

			//filtered_blue = haarStepBGR(cpu_image, img->height, img->width, 0); // haar na plavom kanalu
			//filtered_green = haarStepBGR(cpu_image, img->height, img->width, 1); // haar na zelenom kanalu
			//filtered_red = haarStepBGR(cpu_image, img->height, img->width, 2); // haar na crvenom kanalu

			//for (i = 0; i< img->height * width; i++)
			//{
			//	filtered_image[i * 3 + 0] = filtered_blue[i];
			//	filtered_image[i * 3 + 1] = filtered_green[i];
			//	filtered_image[i * 3 + 2] = filtered_red[i];
			//}
		}


		// show the image
		cvShowImage("Haar", out_image );
		cvShowImage("Original", img );

		// wait for a key
		// cvWaitKey(0);

		int c = cvWaitKey(20); // radimo delay

		if((char)c == 27 )
			break;


		// oslobodi memoriju za buffere
		delete[] cpu_image;
		//delete[] buff;
		//delete[] blue_buff;
		//delete[] green_buff;
		//delete[] red_buff;
		delete[] filtered_image;
		delete[] filtered_blue;
		delete[] filtered_green;
		delete[] filtered_red;
		cvReleaseImage(&out_image);
		cvReleaseImage(&blue_image);
		cvReleaseImage(&green_image);
		cvReleaseImage(&red_image);
		// release the image
		cvReleaseImage(&img );
	}



	cvReleaseCapture(&capture); // automatski realocira i image

	return 0;
}