/* Gauss Elimination                                        */
/* Gilang Anggara                                           */
/* 13214076                                                 */
/* Desember 2017                                            */
/* whirlpool27@gmail.com                                    */
/************************************************************/
/* Purpose : calculate unknown from gaussian elimination    */
/************************************************************/
/* Variable Definitions (Main Function):                    */
/* Variable Name        Type        Description             */
/* i, j, k              int         iterate index           */
/* N                    int         size of matrix          */
/* fp                   FILE *      file pointer for data   */
/* a                    double *    obe matrix              */
/* b                    double *    obe vector              */
/* x                    double *    result for the unknowns */

// include libraries needed
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// define basic math function
#define mat_elem(a, y, x, n) (a + (y*n+x))

// function declaration
void print_matrix(
  double *mat, 
  int size); // print matrix

void swap_row(
  double *a, 
  double *b, 
  int r1, 
  int r2, 
  int n); // swap matrix row

void gauss_eliminate(
  double *a, 
  double *b, 
  double *x, 
  int n); // gaussian elimination

// main program
int main(){
  // variable declaration
  int i, j, k;
  int N;

  FILE *fp;

  double *a;
  double *b;
  double *x;

  // open file
  fp = fopen("gauss.txt", "r");
  if(fp == NULL){
    printf("Failed at opening file.");
    exit(1);
  }

  // read matrix size
  fscanf(fp, "%d", &N);

  // allocate memory
  a = malloc(N*N*sizeof(double));
  b = malloc(N*sizeof(double));
  x = malloc(N*sizeof(double));

  // read matrix a
  for(i = 0; i < N; i++){
    for(j = 0; j < N; j++){
      fscanf(fp, "%lf", &a[i*N+j]);
    }
  }

  // read matrix b
  for(i = 0; i < N; i++){
    fscanf(fp, "%lf", &b[i]);
  }

  // close file
  fclose(fp);

  // eliminate matrix
  gauss_eliminate(a, b, x, N);

  // print result
  fp = fopen("result.txt", "w"); // open file
  if (fp == NULL){
    printf("Opening result file has failed.");
    exit(1);
  }
  for(i = 0; i < N; i++){ // print
    fprintf(fp, "%lf\n", x[i]);
  }
  fclose(fp); // close file

  return 0;
  // end of main program
}

// Function Definition
void print_matrix(double *mat, int size){
  int i,j;

  for (i = 0; i < size; i++){
    for (j = 0; j < size; j++){
      printf("%lf ", mat[i*size+j]);
    }
    printf("\n");
  }
  printf("\n");
}

void swap_row(double *a, double *b, int r1, int r2, int n)
/* Author : rosettacode.org */
{
	double tmp, *p1, *p2;
	int i;
 
	if (r1 == r2) return;
	for (i = 0; i < n; i++) {
		p1 = mat_elem(a, r1, i, n);
		p2 = mat_elem(a, r2, i, n);
		tmp = *p1, *p1 = *p2, *p2 = tmp;
	}
	tmp = b[r1], b[r1] = b[r2], b[r2] = tmp;
}

void gauss_eliminate(double *a, double *b, double *x, int n)
/* Author : rosettacode.org */
{
#define A(y, x) (*mat_elem(a, y, x, n))
	int i, j, col, row, max_row,dia;
	double max, tmp;
 
	for (dia = 0; dia < n; dia++) {
		max_row = dia, max = A(dia, dia);
 
		for (row = dia + 1; row < n; row++)
			if ((tmp = fabs(A(row, dia))) > max)
				max_row = row, max = tmp;
 
		swap_row(a, b, dia, max_row, n);
 
		for (row = dia + 1; row < n; row++) {
			tmp = A(row, dia) / A(dia, dia);
			for (col = dia+1; col < n; col++)
				A(row, col) -= tmp * A(dia, col);
			A(row, dia) = 0;
			b[row] -= tmp * b[dia];
		}
	}
	for (row = n - 1; row >= 0; row--) {
		tmp = b[row];
		for (j = n - 1; j > row; j--)
			tmp -= x[j] * A(row, j);
		x[row] = tmp / A(row, row);
	}
#undef A
}