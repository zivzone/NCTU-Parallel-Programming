#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#ifndef W
#define W 20                                    // Width
#endif
#define INT_MAX 2147483647    // maximum (signed) int value
int main(int argc, char **argv) {
  // set r = rank, the current cpu's rank in the all cpu. Start from zero
  // set s = size, how many number of all cpu
  /* set st = status, represents the status of the received message
     The structure listed below:
        typedef struct _MPI_Status {
          int count;
          int cancelled;
          int MPI_SOURCE;
          int MPI_TAG;
          int MPI_ERROR;
        } MPI_Status, *PMPI_Status;
  */

  //Set MPI Status
  MPI_Status st;

  // MPI tutorial, you can see https://mpitutorial.com/tutorials/mpi-hello-world/
  // Initialize the MPI environment
	MPI_Init(NULL, NULL);
  // Get the rank of the process
  int r;
	MPI_Comm_rank(MPI_COMM_WORLD, &r);
  // Get the number of processes
  int s;
	MPI_Comm_size(MPI_COMM_WORLD, &s);
	
  
  
  
  int L = atoi(argv[1]);                        // Length
  int iteration = atoi(argv[2]);                // Iteration
  srand(atoi(argv[3]));                         // Seed
  float d = (float) random() / RAND_MAX * 0.2;  // Diffusivity
  int *temp = malloc(L*W*sizeof(int));          // Current temperature
  int *next = malloc(L*W*sizeof(int));          // Next time step

  for (int i = 0; i < L; i++) {
    for (int j = 0; j < W; j++) {
      temp[i*W+j] = random()>>3;
    }
  }
  int count = 0, balance = 0;
  // Seperate all task into s part to let s cpus to do it respectly.
  int current_start = L * r / s;
  int current_finish = L * ( r + 1 ) / s;
  int T;
  while (iteration--) {     // Compute with up, left, right, down points
    balance = 0;
    count++;
    for (int i = current_start; i < current_finish; i++) {
      for (int j = 0; j < W; j++) {
        float t = temp[i*W+j] / d;
        t += temp[i*W+j] * -4;
        t += temp[(i - 1 <  0 ? 0 : i - 1) * W + j];
        t += temp[(i + 1 >= L ? i : i + 1)*W+j];
        t += temp[i*W+(j - 1 <  0 ? 0 : j - 1)];
        t += temp[i*W+(j + 1 >= W ? j : j + 1)];
        t *= d;
        next[i*W+j] = t ;
        if (next[i*W+j] != temp[i*W+j]) {
          balance = 1;
        }
      }
    }
    /*
    MPI_Send:
      Performs a standard mode send operation and returns when the send buffer can be safely reused.
      Format : 
      int MPIAPI MPI_Send(
          _In_opt_ void         *buf,
          int          count,
          MPI_Datatype datatype,
          int          dest,
          int          tag,
          MPI_Comm     comm
      );
      We use MPI_INT as MPI_Datatype

    MPI_Recv:
      Performs a receive operation and does not return until a matching message is received.
      Format:
      int MPIAPI MPI_Recv(
          _In_opt_ void         *buf,
          int          count,
          MPI_Datatype datatype,
          int          source,
          int          tag,
          MPI_Comm     comm,
          _Out_    MPI_Status   *status
      );
      We use MPI_INT as MPI_Datatype

    */

    // Initiates barrier synchronization across all members of a group.
    MPI_Barrier(MPI_COMM_WORLD);
	  if(r > 0){
      // If current cpu is not the host cpu , send result back to the lower one
	    int des = r - 1;
      int tag = 0;
      MPI_Send(&next[(current_start*W)], W, MPI_INT, des, tag, MPI_COMM_WORLD);
	  }
	  if(r < s-1){
      // If current cpu is not the last cpu , send result back to the upper one
	    int sor = r + 1;
      int tag = 0;
      MPI_Recv(&next[(current_finish*W)], W, MPI_INT, sor, tag, MPI_COMM_WORLD, &st);
	  }
	  if(r < s-1){
      // If current cpu is not the last cpu , send result back to the upper one
	    int des = r + 1;
      int tag = 0;
      MPI_Send(&next[((current_finish-1)*W)], W, MPI_INT, des, tag, MPI_COMM_WORLD);
	  }
	  if(r > 0){
      int sor = r - 1;
      int tag = 0;
	    MPI_Recv(&next[((current_start-1)*W)], W, MPI_INT, sor, tag, MPI_COMM_WORLD, &st);
	  }

    // Initiates barrier synchronization across all members of a group.
	  MPI_Barrier(MPI_COMM_WORLD);

    // Combines values from all processes and distributes the result back to all processes.
	  MPI_Allreduce(&balance, &T, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    if (T == 0) {
      break;
    }
    int *tmp = temp;
    temp = next;
    next = tmp;
  }
  int min = INT_MAX;

  for (int i = current_start; i < current_finish; i++) {
    for (int j = 0; j < W; j++) {
      if (temp[i*W+j] < min) {
        min = temp[i*W+j];
      }
    }
  }
  int result;
  // Initiates barrier synchronization across all members of a group.
	MPI_Barrier(MPI_COMM_WORLD);

  // Combines values from all processes and distributes the result back to all processes.
	MPI_Allreduce(&min, &result, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
	// If every sub process finish and back to host
  if(r == 0){
    printf("Size: %d*%d, Iteration: %d, Min Temp: %d\n", L, W, count, result);
  }
  // Finalize the MPI environment.
	MPI_Finalize();
  return 0;
}
