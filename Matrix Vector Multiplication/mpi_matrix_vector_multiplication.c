/* MPI Matrix Vector Multiplication                         */
/* Gilang Anggara                                           */
/* 13214076                                                 */
/* Desember 2017                                            */
/* whirlpool27@gmail.com                                    */
/************************************************************/
/* Purpose : calculate matrix and vector multiplication     */
/*           using MPI Send & Receive                       */
/************************************************************/

/* Variable Definitions (Main Function):                    */
/* Variable Name        Type        Description             */
/* fp                   FILE *      file pointer for matirx */
/*                                  and vector              */
/* matrix               int *       pointer for matrix made */
/* vector               int *       pointer for vector made */
/* result               int *       result of multiplication*/
/* i, j, k              int         index used to iterate   */
/* rank                 int         processes rank          */
/* size                 int         number of processes     */
/* master               int         rank of process 0       */
/* row_length           int         length of matrix row    */
/* received answer      int         answer received from    */
/*                                  each rank               */
/* start_time           double      starting time           */
/* finish_time          double      finish time             */
/* status               MPI_status  status of recevie       */



#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MSIZE 20
#define DEBUG 1

int main(int argc, char *argv[]){
    int rank, size;
    int master = 0;
    int i, j, k;
   
    FILE *fp;

    int *matrix;
    int *vector;
    int *result;

    double start_time, finish_time;

    MPI_Status status;
    
    //initialize mpi
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Master
    if (rank == master){
        // allocate vector
        vector = malloc(MSIZE*sizeof(int));
        // allocate matrix
        matrix = malloc(MSIZE*MSIZE*sizeof(int));

        // read vector
        fp = fopen("vector.txt", "r");

        for (i = 0; i < MSIZE; i++){
            fscanf(fp, "%d", &vector[i]);
        }
        fclose(fp);

        // read matrix
        fp = fopen("matrix.txt", "r");

        for(i = 0; i < MSIZE; i++){
            for (j = 0; j < MSIZE; j++){
                fscanf(fp, "%d", &matrix[i*MSIZE+j]);
            }
        }
        fclose(fp);
        
        // start counting time
        start_time = MPI_Wtime();

         // send vector to other rank        
        for (i = 1; i < size; i++){
            // MPI_TAG for Vector = MSZIE + i
            MPI_Send(vector, MSIZE, MPI_INT, i, MSIZE+i, MPI_COMM_WORLD);
        }
        
        // send matrix
        for (i = 1; i < size; i++){
            MPI_Send(matrix, MSIZE*MSIZE, MPI_INT, i, i, MPI_COMM_WORLD);
        }

        
        // allocate memory for result
        result = calloc(MSIZE, sizeof(int));

        // calculate result part in master
        for (i = 0; i < MSIZE/size; i++){
            for (j = 0; j < MSIZE; j++){
                result[i] += matrix[i*MSIZE+j]*vector[j];
            }
        }
        
        // receive answer
        int sender_num = 0;
        int row_count;
        int *buffer;
        while(sender_num < size-1){
            buffer = malloc((MSIZE/size+1)*sizeof(int));
            MPI_Recv(buffer, MSIZE/size+1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_INT, &row_count);
            j = 0;
            for (i = status.MPI_SOURCE*MSIZE/size; i < status.MPI_SOURCE*MSIZE/size+row_count; i++){
                result[i] = buffer[j];
                j++;
            }
            sender_num++;
        }

        // stop counting time after finish calculating
        finish_time = MPI_Wtime();

        //print to file
        fp = fopen("result_mpi_send_receive.txt", "w");
        for (i = 0; i < MSIZE; i++){
            fprintf(fp, "%d\n", result[i]);
        }
        fclose(fp);

        // free allocated memory
        free(vector);
        free(matrix);
        free(result);
        free(buffer);

        // print time needed
        printf("Time needed for mpi send receive is %f s\n", finish_time-start_time);
    }

    // Slave
    else {
        //receive vector
        vector = malloc(MSIZE*sizeof(int));
        MPI_Recv(vector, MSIZE, MPI_INT, master, MSIZE+rank, MPI_COMM_WORLD, &status);

        //receive matrix
        matrix = malloc(MSIZE*MSIZE*sizeof(int));
        MPI_Recv(matrix, MSIZE*MSIZE, MPI_INT, master, rank, MPI_COMM_WORLD, &status);

        //calculate answer
        int row_count = 0;
        for (i = rank*MSIZE/size; i < (rank+1)*MSIZE/size;i++){
            row_count++;
        }

        // allocate memory for result
        int *result_part = (int *) calloc(row_count,sizeof(int)); 
        
        // calculate result
        k = 0;
        for (i = rank*MSIZE/size; i < (rank+1)*MSIZE/size; i++){
            for (j = 0; j < MSIZE; j++){
                result_part[k] += matrix[i*MSIZE+j]*vector[j];
            }
            k++;
        }

        //send answer
        MPI_Send(result_part, row_count, MPI_INT, master, MSIZE*2+rank, MPI_COMM_WORLD);

        //free allocated memory
        free(matrix);
        free(vector);
        free(result_part);
        
    }


    // finalize mpi
    MPI_Finalize();

    return 0;

    // end of program
}
