/* MPI Matrix Vector Multiplication                         				  */
/* Gilang Anggara                                           				  */
/* 13214076                                                 				  */
/* Desember 2017                                           					  */
/* whirlpool27@gmail.com                                   					  */
/* Serial program author : Yudi Gondokaryono, November 11, 2002 			  */
/******************************************************************************/
/* Purpose : Imgage Filtering Program with OpenMP                             */
/******************************************************************************/
/* Variable Definitions (Main Function):                                      */
/* Variable Name          Type     Description                                */
/* pict[256][256]         int      image array                                */
/* new_pict[256][256]     int      new image array                            */
/* r                      int      # of row in the image                      */
/* c                      int      # of column in the image                   */
/* flt[3][3]              double   filter array                               */
/******************************************************************************/
/* Source Code:                                                               */

/* Include all library we need                                                */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <omp.h>

/* Declare all constant                                                       */
#define IM_SIZE 256 // maximum image size
#define FILTER_SIZE 3 // filter size

/* Declare all function prototype                                             */
void read_pict(int pict[][IM_SIZE], int *r, int *c);
void image_filter(int pict[][IM_SIZE], int r, int c, double filter[][FILTER_SIZE],
				  int new_pict[][IM_SIZE]);
void write_pict(int pict[][IM_SIZE], int r, int c);

/* Begin the Main Function                                                    */
int main( void )
{
	int width, height; // actual image size
	int pict[IM_SIZE][IM_SIZE];
	int new_pict[IM_SIZE][IM_SIZE];
	double flt[FILTER_SIZE][FILTER_SIZE]={{-1.25, 0, -1.25},
										  {0, 10, 0},
									   	  {-1.25, 0, -1.25}};
	
	read_pict(pict, &height, &width);
	image_filter(pict, height, width, flt, new_pict);
	write_pict(new_pict, height, width);

	return(0);
/* End Main Function                                                          */
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
void read_pict(int pict[][IM_SIZE], int *r, int *c)
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
			fscanf(in, "%d", &pict[i][j]);
	fclose(in);

	return;
/* End read_pict function                                                     */
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
void image_filter(int pict[][IM_SIZE], int r, int c, double filter[][FILTER_SIZE],
				  int new_pict[][IM_SIZE])
{
	double coeff = 0;
	double sum;
	int i, j, m, n;
	
/*  copy edges                                                                */
	#pragma omp parallel for shared(r, new_pict, pict, c) private (i)
	for(i = 0; i < r; i++)
	{
		new_pict[i][0] = pict[i][0];
		new_pict[i][c-1] = pict[i][c-1];
	}

	#pragma omp parallel for shared(c, new_pict, pict, r) private(j)
	for(j = 0; j < c; j++)
	{
		new_pict[0][j] = pict[0][j];
		new_pict[r-1][j] = pict[r-1][j];
	}

/*  compute coefficient                                                       */
	#pragma omp parallel for shared(filter) private (i, j) reduction(+:coeff)	
	for(i = 0; i < FILTER_SIZE; i++)
		for(j = 0; j < FILTER_SIZE; j++)
			coeff += filter[i][j];

/*  filter the image                                                          */
	#pragma omp parallel for shared(r, c, pict, filter, new_pict) private (i, j, m, n) reduction(+:sum)
	for(i = 1; i < r-1; i++)
		for(j = 1; j < c-1; j++)
		{
			sum = 0;
			for(m = 0; m < FILTER_SIZE; m++)
				for(n = 0; n < FILTER_SIZE; n++)
					sum += pict[i+(m-1)][j+(n-1)]*filter[m][n];
			new_pict[i][j] = (int)sum;
		} 

	if(coeff != 0)
	{
		#pragma omp parallel for shared(r, c, new_pict, coeff) private(i, j)
		for(i = 1; i < r-1; i++)
			for(j = 1; j < c-1; j++)
				new_pict[i][j] = (int)(new_pict[i][j]/coeff);
	}
	
/*  check for pixel > 255 and pixel < 0                                       */
	#pragma omp parallel for shared(r, c, new_pict) private(i, j)
	for(i = 1; i < r-1; i++)
		for(j = 1; j < c-1; j++)
		{
			if(new_pict[i][j] < 0)
				new_pict[i][j] = 0;
			else if(new_pict[i][j] > 255)
				new_pict[i][j] = 255;
		}
		
	return;
/* End image_filter function                                                  */
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
void write_pict(int pict[][IM_SIZE], int r, int c)
{
	int i, j;
	FILE *out;
	
	out=fopen("omp_new.pgm", "w");

	fprintf(out, "P2\n");
	fprintf(out, "# new.pgm\n");
	fprintf(out, "%d %d\n", r, c);
	fprintf(out, "255\n");
	
	for(i = 0; i < r; i++)
	{
		for(j = 0; j < c; j++)
			fprintf(out, "%5d", pict[i][j]);
		fprintf(out, "\n");
	}
	
	fclose(out);
	return;
/* End write_pict function                                                    */
}

