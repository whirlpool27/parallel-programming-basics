#include "cuda_runtime.h"
#include "device_launch_parameters.h"

/* Include all library needed */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/* Declare all constant */
#define IM_SIZE 256 // maximum image size
#define FILTER_SIZE 3 // filter size

void read_pict(int pict[][IM_SIZE], int *r, int *c);

void write_pict(int pict[][IM_SIZE], int r, int c);

cudaError_t image_filter_with_cuda(
    int *pict, 
    int r, 
    int c, 
    double *filter,
    int *new_pict);

__global__ void image_filter(
    int *pict, 
    int r, 
    int c, 
    double *filter,
    int *new_pict);

__global__ void copyEdges(
	int *pict, 
	int *new_pict, 
	int r, 
	int c);

__global__ void computeCoefficient(
	int *filter, 
	int coeff);

__global__ void filterImage (
	int *pict, 
	int *new_pict, 
	int *filter, 
	int coeff);

__global__ void imageSum(
	int sum, 
	int *pict, 
	int *new_pict, 
	int i, 
	int j,
	int **filter);



int main(){
    int width, height; // actual image size
	int *pict;
	int *new_pict;
	double *flt;

	pict = malloc(IM_SIZE*IM_SIZE*sizeof(int));
	new_pict = malloc(IM_SIZE*IM_SIZE*sizeof(int));
	flt = malloc(FILTER_SIZE*FILTER_SIZE*sizeof(double));
	flt ={-1.25, 0, -1.25,
			0, 10, 0,
		-1.25, 0, -1.25};
	
	read_pict(pict, &height, &width);
	
	cudaError_t cudaStatus = image_filter_with_cuda(pict, height, width, flt, new_pict);

	write_pict(new_pict, height, width);

	return(0);
}


/* Begin read_pict function                                                   */
/******************************************************************************/
/* Purpose : This function reads the image from original.pgm                  */
/******************************************************************************/
/* Variable Definitions                                                       */
/* Variable Name          Type     Description                                */
/* pict[][]               int      array address                              */
/* r                      int *    # of rows pointer                          */
/* c                      int *    # of column pointer                        */
/* max                    int      maximum pixel value                        */
/* i                      int      loop counter                               */
/* j                      int      loop counter                               */
/* line[200]              char     string                                     */
/* in                     FILE *   input file pointer                         */
/******************************************************************************/
/* Source Code:                                                               */
void read_pict(int *pict, int *r, int *c)
{
	int i, j, max;
	FILE *in;
	char line[200];
	
	in=fopen("original.pgm", "r");	// f16 image
	if(in == NULL)
	{
		printf("Error reading original.pgm\n");
		exit(1);
	}
	
	fgets(line, 199, in); // get PGM Type
	
	line[strlen(line)-1]='\0';  // get rid of '\n'
	
	if(strcmp(line, "P2") != 0)
	{
		printf("Cannot process %s PGM format, only P2 type\n", line);
		exit(2);
	}
	
	fgets(line, 199, in); // get comment
		
	fscanf(in, "%d %d", c, r); // get size width x height
	
	fscanf(in, "%d", &max); // get maximum pixel value

	for(i = 0; i < *r; i++)
		for(j = 0; j < *c; j++)
			fscanf(in, "%d", &pict[i*IM_SIZE+j]);
	fclose(in);

	return;
/* End read_pict function                                                     */
}	

/* Begin write_pict function                                                  */
/******************************************************************************/
/* Purpose : This function write the filtered image to new.pgm                */
/******************************************************************************/
/* Variable Definitions                                                       */
/* Variable Name          Type     Description                                */
/* pict[][]               int      array address                              */
/* new_pict[][]           int      array address                              */
/* r                      int      # of rows                                  */
/* c                      int      # of column                                */
/* i                      int      loop counter                               */
/* j                      int      loop counter                               */
/* out                    FILE *   output FILE pointer                        */
/******************************************************************************/
/* Source Code:                                                               */
void write_pict(int *pict, int r, int c)
{
	int i, j;
	FILE *out;
	
	out=fopen("new.pgm", "w");

	fprintf(out, "P2\n");
	fprintf(out, "# new.pgm\n");
	fprintf(out, "%d %d\n", r, c);
	fprintf(out, "255\n");
	
	for(i = 0; i < r; i++)
	{
		for(j = 0; j < c; j++)
			fprintf(out, "%5d", pict[i*IM_SIZE+j]);
		fprintf(out, "\n");
	}
	
	fclose(out);
	return;
/* End write_pict function                                                    */
}

cudaError_t image_filter_with_cuda(
    int *pict, 
    int r, 
    int c, 
    double *filter,
    int *new_pict)
{
	int *dev_pict;
	int *dev_new_pict;
	int *dev_filter;

	// choose GPU to run
	cudaStatus = cudaSetDevice(0);
	if (cudaStatus != cudaSuccess){
		fprintf(stderr, "cudaSetDevice failed!\n");
		goto Error;
	}

	// Allocate GPU buffers
	cudaStatus = cudaMalloc((void**)&dev_pict, IM_SIZE*IM_SIZE*sizeof(int));
	if (cudaStatus != cudaSuccess){
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}

	cudaStatus = cudaMalloc((void**)&dev_new_pict, IM_SIZE*IM_SIZE*sizeof(int));
	if (cudaStatus != cudaSuccess){
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}

	cudaStatus = cudaMalloc((void**)&dev_filter, FILTER_SIZE*FILTER_SIZE*sizeof(double));
	if (cudaStatus != cudaSuccess){
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}

	// copy input from host memory to GPU buffers
	cudaStatus = cudaMemcpy(dev_pict, pict, IM_SIZE*IM_SIZE*sizeof(int), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess){
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

	cudaStatus = cudaMemcpy(dev_new_pict, new_pict, IM_SIZE*IM_SIZE*sizeof(int), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess){
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

	cudaStatus = cudaMemcpy(dev_filter, filter, FILTER_SIZE*FILTER_SIZE*sizeof(double), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess){
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

	// launch kernel
	image_filter<<<1, IM_SIZE>>>(pict, r, c, filter, new_pict);

	// check for any launching error
	cudaStatus = cudaGetLastError();
	if (cudaStatus != cudaSuccess){
		fprintf(stderr, "image_filter kernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
		goto Error;
	}

	// synch
	cudaStatus = cudaDeviceSynchronize();
	if(cudaStatus != cudaSuccess){
		fprintf(stderr, "cudaDeviceSynchronize return error code %d after launching image_filter");
		goto Error;
	}

	//copy output from GPU buffer to host memory
	cudaStatus = cudaMemcpy(new_pict, dev_new_pict, IM_SIZE*IM_SIZE*sizeof(int), cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess){
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

Error:
	cudaFree(dev_pict);
	cudaFree(dev_new_pict);
	cudaFree(dev_filter);

	return cudaStatus;
}

/* Begin image_filter function                                                */
/******************************************************************************/
/* Purpose : This function filter the image and create a new image            */
/******************************************************************************/
/* Variable Definitions                                                       */
/* Variable Name          Type     Description                                */
/* pict[][]               int      array address                              */
/* new_pict[][]           int      array address                              */
/* r                      int      # of rows                                  */
/* c                      int      # of column                                */
/* i                      int      loop counter                               */
/* j                      int      loop counter                               */
/* m                      int      loop counter                               */
/* n                      int      loop counter                               */
/* coeff                  double   coefficient                                */
/* sum                    double   sum                                        */
/******************************************************************************/
/* Source Code:                                                               */
__global__ void image_filter(
    int *pict, 
    int r, 
    int c, 
    double *filter,
    int *new_pict)
{
	double coeff = 0;
	double sum;
	int i, j, m, n;
	
	copyEdges <<<1,IM_SIZE>>> (pict, new_pict, r, c);
	computeCoefficient <<<1,FILTER_SIZE>>> (coeff, filter);
	filterImage <<<1, IM_SIZE>>> (pict, new_pict, filter, coeff);

/* End image_filter function                                                  */
}

__global__ void copyEdges(int *pict, int *new_pict, int r, int c){
	int i = threadIdx.x;
	int j = threadIdx.y;

	new_pict[i*IM_SIZE] = pict[i*IM_SIZE];
	new_pict[i*IM_SIZE + c-1] = pict[i*IM_SIZE+c-1];

	new_pict[j] = pict[j];
	new_pict[(r-1)*IM_SIZE+j] = pict[(r-1)*IM_SIZE+j];

	return;
}

__global__ void computeCoefficient(int *filter, int coeff){
	int i = threadIdx.x;
	int j = threadIdx.y;

	coeff += filter[i][j];

	return;
}

__global__ void filterImage (int *pict, int *new_pict, int *filter, int coeff){
	int i = threadIdx.x;
	int j = threadIdx.y;
	int sum = 0;

	imageSum<<<1,FILTER_SIZE>>>(sum, pict, new_pict, i, j, filter);
	
	new_pict[i*IM_SIZE+j] = (int) sum;

	if(coeff != 0){
		new_pict[i*IM_SIZE+j] = (int)(new_pict[i*IM_SIZE+j]/coeff);
	}

	if(new_pict[i*IM_SIZE+j] <0)
		new_pict[i*IM_SIZE+j] = 0;
	else if(new_pict[i*IM_SIZE+j] > 255)
		new_pict[i*IM_SIZE+j] = 255;

	return;
}

__global__ void imageSum(int sum, int *pict, int *new_pict, int i, int j,int *filter){
	int m = threadIdx.x;
	int n = threadIdx.y;

	sum = 0;
	sum += pict[(i+(m-1))*IM_SIZE+j+(n-1)]*filter[m*FILTER_SIZE+n];

	return;
}

