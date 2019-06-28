#include <stdio.h>
#include <string.h>
#include <mpi.h>

void main(int argc, char*argv[]) {
int name, p, source, dest, tag = 0;
char message[100];
MPI_Status status;
MPI_Init(&argc,&argv);
MPI_Comm_rank(MPI_COMM_WORLD,&name);

}