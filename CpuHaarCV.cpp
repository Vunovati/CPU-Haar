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

	if (img->nChannels = 1)
	{
		for (int i = 0; i < (img->width * img->height); i++)
		{
			out_image[i] = (unsigned char)img->imageData[i] / 255.f; // pretvori svaki element iz chara u float (vrijednosti 0..1)
		}
	}
	else
	{
	  for (int i = 0; i < (img->width * img->height * 4); i++)
		{
			out_image[i * 3 + 0] = (unsigned char)img->imageData[i * 4 + 0] / 255.f; // |B|G|R|prazno|B|G|R|prazno|B|G|R|prazno|
			out_image[i * 3 + 1] = (unsigned char)img->imageData[i * 4 + 1] / 255.f;
			out_image[i * 3 + 2] = (unsigned char)img->imageData[i * 4 + 2] / 255.f;
		}
	
	}


	return( out_image );
}

char *float2CVdata(float *image, int nPixels, int channels)
{
	char* out_char = new char[nPixels * 4];
		
	if (channels = 1)
	{
		for (int i = 0; i<(nPixels); i++)
		{
			out_char[i] = (char)floor(image[i] * 255.f); // prebaci nazad u char
		}
	}
	else
	{
		for (int i = 0; i < (nPixels * 4); i++)
		{
			out_char[i * 4 + 0] = (char)floor(image[i * 3 + 0] * 255.f);
			out_char[i * 4 + 0] = (char)floor(image[i * 3 + 0] * 255.f);
			out_char[i * 4 + 0] = (char)floor(image[i * 3 + 0] * 255.f);		
		}	
	}

	return( out_char );
}

int main(int argc, char *argv[])
{
	//IplImage* img = cvLoadImage("testnaslika.jpg", CV_LOAD_IMAGE_UNCHANGED); 
	IplImage* img = cvLoadImage("testnaslika.jpg", CV_LOAD_IMAGE_GRAYSCALE);   
	int height,width,step,channels, i;


	// get the image data
	height    = img->height;
	width     = img->width;
	step      = img->widthStep;
	channels  = img->nChannels;
	/*data      = (uchar *)img->imageData;*/
	printf("Processing a %dx%d image with %d channels\n",height,width,channels); 

	float* cpu_image = new float[width * height * 4];

	float* filtered_image = new float[width * height * 4];
	float* filtered_blue = new float[width * height * 4];
	float* filtered_green = new float[width * height * 4];
	float* filtered_red = new float[width * height * 4];


	char* buff = new char[(width) * (height) * 4];
	IplImage* out_image = cvCreateImage(cvSize(width, height), img->depth, channels);

	cpu_image = CV2float(img);


	if (channels = 1)
	{
		filtered_image = haarStep(cpu_image, height, width); // pozivamo funkciju za haar
	}
	else 
	{
		filtered_blue = haarStepBGR(cpu_image, height, width, 0); // haar na plavom kanalu
		filtered_green = haarStepBGR(cpu_image, height, width, 1); // haar na zelenom kanalu
		filtered_red = haarStepBGR(cpu_image, height, width, 2); // haar na crvenom kanalu

		for (i = 0; i< height * width; i++)
		{
			filtered_image[i * 3 + 0] = filtered_blue[i];
			filtered_image[i * 3 + 1] = filtered_green[i];
			filtered_image[i * 3 + 2] = filtered_red[i];
		}
	}

	buff = float2CVdata(filtered_image, width*height, channels);

	out_image->imageData = buff; // napunimo podatke za sliku podatcima iz buffera


	// show the image
	cvShowImage("Haar", out_image );
	cvShowImage("Original", img );

	// wait for a key
	cvWaitKey(0);

	// oslobodi memoriju za buffere
	delete[] cpu_image;
	delete[] buff;
	delete[] filtered_image;
	delete[] filtered_blue;
	delete[] filtered_green;
	delete[] filtered_red;

	// release the image
	cvReleaseImage(&img );
	cvReleaseImage(&out_image);
	return 0;
}