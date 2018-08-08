/* Serial Matrix Vector Multiplication                      */
/* Gilang Anggara                                           */
/* 13214076                                                 */
/* Desember 2017                                            */
/* whirlpool27@gmail.com                                    */
/************************************************************/
/* Purpose : calculate matrix and vector multiplication     */
/************************************************************/

/* Variable Definitions (Main Function):                    */
/* Variable Name        Type        Description             */
/* fp                   FILE *      file pointer for matirx */
/*                                  and vector              */
/* matrix               int *       pointer for matrix made */
/* vector               int *       pointer for vector made */
/* answer               int *       result of multiplication*/
/* i, j, k              int         index used to iterate   */
/* start_time           clock_t     starting time of        */
/*                                  calculation             */
/* finish_time          clock_t     finish time of          */
/*                                  calculation             */

/* Source Code:                                             */
/* include all library needed                               */
// standard input output
#include <stdio.h>

// standard library for file processing
#include <stdlib.h>

// library for time
#include <time.h>

//maximum matrix size
#define MSIZE 20

/* Main Program                                             */
int main(){
    FILE *fp;
    
    int *matrix;
    int *vector;
    int *answer;
    int i, j, k;

    clock_t start_time, finish_time;

    // read matrix
    matrix = malloc(sizeof(int)*MSIZE*MSIZE);
    fp = fopen("matrix.txt", "r");
    k = 0;
    for(i = 0; i < MSIZE; i++){
        for (j = 0; j < MSIZE; j++){
            fscanf(fp, "%d", &matrix[k]);
            k++;
        }
    }
    fclose(fp);

    // read vector
    vector = malloc(sizeof(int)*MSIZE);
    fp = fopen("vector.txt", "r");
    for (i = 0; i < MSIZE; i++){
        fscanf(fp, "%d", &vector[i]);
    }
    fclose(fp);

    // start counting time
    start_time = clock();

    // calculate result
    answer = calloc(MSIZE, sizeof(int));
    
    for (i = 0; i < MSIZE; i++){
        for (j = 0; j < MSIZE; j++){
            answer[i] += matrix[i*MSIZE+j]*vector[j];
        }
    }
   
    // stop counting time after multiplication
    finish_time = clock();

    // print result
    fp = fopen("result_serial.txt", "w");
    for(i = 0; i < MSIZE; i++){
        fprintf(fp, "%d\n", answer[i]);
    }
    fclose(fp);

    // free allocated memory
    free(matrix);
    free(vector);

    // print time needed
    printf("Time needed for serial program is %f s\n", 
    (double)(finish_time-start_time)/CLOCKS_PER_SEC);

    return 0;
    // end of program
}
