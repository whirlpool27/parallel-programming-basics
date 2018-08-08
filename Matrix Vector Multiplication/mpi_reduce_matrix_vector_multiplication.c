/* MPI Matrix Vector Multiplication                         */
/* Gilang Anggara                                           */
/* 13214076                                                 */
/* Desember 2017                                            */
/* whirlpool27@gmail.com                                    */
/************************************************************/
/* Purpose : calculate matrix and vector multiplication     */
/*           using MPI reduce                               */
/************************************************************/

/* Variable Definitions (Main Function):                    */
/* Variable Name        Type        Description             */
/* rank                 int         processor rank          */
/* size                 int         number of processor     */
/* master               int         master processor(rank 0)*/
/* i, j, k              int         iterating index         */
/* file_vector          FILE *      pointer used for vector */
/*                                  file                    */
/* file_matrix          FILE *      pointer used for matrix */
/*                                  file                    */
/* file_result          FILE *      pointer used for result */
/*                                  vector file             */
/* matrix               int *       matrix pointer for      */
/*                                  matrix array            */
/* vector               int *       pointer for vector arr  */
/* result               int *       pointer for result arr  */
/* local_result         int *       pointer for local result*/
/*                                  in each processor       */
/* start_time           int *       starting time for       */
/*                                  mat-vec multiplication  */
/* finish_time          int *       finish time for mat-vec */
/*                                  multiplication          */


// include all library needed
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


// matrix size, MSIZE x MSIZE with MSIZE vector
#define MSIZE 20

int main(int argc, char *argv[]){
    int rank, size;
    int master = 0;
    int i, j, k;

    FILE *file_vector;
    FILE *file_matrix;
    FILE *file_result;

    int *matrix;
    int *vector;
    int *result;
    int *local_result;

    double start_time, finish_time;

    MPI_Status status;

    // initialize mpi
    MPI_Init(&argc, &argv);

    // get size and rank
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // allocate vector
    vector = malloc(MSIZE*sizeof(int));

    // allocate matrix
    matrix = malloc(MSIZE*MSIZE*sizeof(int));

    // allocate result
    result = calloc(MSIZE, sizeof(int)); 

    // read vector from file
    file_vector = fopen("vector.txt", "r");
        
    for (i = 0; i < MSIZE; i++){
        fscanf(file_vector, "%d", &vector[i]);
    }

    fclose(file_vector);
        
    // read matrix from file
    file_matrix = fopen("matrix.txt", "r");

    k = 0;
    for(i = 0; i < MSIZE; i++){
        for (j = 0; j < MSIZE; j++){
            fscanf(file_matrix, "%d", &matrix[k]);
            k++;                
        }
    }

    fclose(file_matrix);
        
    // Get starting time
    MPI_Barrier(MPI_COMM_WORLD);
    start_time = MPI_Wtime();

    // allocate memory for local result
    local_result = calloc(MSIZE, sizeof(int));

    // calculate result for each parts
    for (i = 0; i < MSIZE; i++){
        for (j = rank*MSIZE/size; j < (rank+1)*MSIZE/size; j++){
            local_result[i] += matrix[i*MSIZE+j]*vector[j];
        }
    }

    // call reduce
    MPI_Reduce(local_result, 
            result, 
            MSIZE,
            MPI_INT,
            MPI_SUM,
            master,
            MPI_COMM_WORLD);
    
    // get finish time
    MPI_Barrier(MPI_COMM_WORLD);
    finish_time = MPI_Wtime();
 
    //print to file
    if(rank == master){
        file_result = fopen("result_mpi_reduce.txt", "w");
        for (i = 0; i < MSIZE; i++){
            fprintf(file_result, "%d\n", result[i]);
        }
        fclose(file_result);

        // print time needed
        printf("Time needed for mpi reduce is %f s\n", finish_time-start_time);
    }
    
    // free allocated memory
    free(local_result);
    free(matrix);
    free(vector);
    free(result);

    // finalize MPI
    MPI_Finalize();

    // return
    return 0;

    // end of program
}
