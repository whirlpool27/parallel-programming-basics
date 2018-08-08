/* Hello World MPI                                          */
/* Gilang Anggara                                           */
/* 13214076                                                 */
/* Desember 2017                                            */
/************************************************************/
/* Purpose : Print "Hello World" with more than 1 processing*/
/*           unit                                           */
/************************************************************/

/* Variable Definitions (Main Function):                    */
/* Variable Name        Type        Description             */
/* world_size           Integer     number of processes     */
/* world_rank           Integer     processes rank          */
/* processor_name       Char[]      name of processor_name  */
/* name_len             Integer     length of processor name*/


/* Source Code:                                             */

/* Include all library needed                               */
// standard input output library
#include <stdio.h>

// mpi library
#include <mpi.h>

/* Main Program                                             */
int main(int argc, char **argv){
    int world_size;
    int world_rank;

    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;

    //Initialize MPI
    MPI_Init(&argc, &argv);

    // Get the number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get processes rank
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank)

    // Get processor name
    MPI_Get_processor_name(processor_name, &name_len);

    // Print hello world
    printf("Hello world from processor %s, rank %d,"
           " out of %d processors\n", 
           processor_name, world_rank, world_size);
    
    // Finalize MPI
    MPI_Finalize();

    //end of mpi hello world
}