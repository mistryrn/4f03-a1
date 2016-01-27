#include "a1.h"
#include "mpi.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

int main(int argc, char** argv)
{
  // Initialize variables
  RGB *image = NULL;
  int width = 0, height = 0, max = 0;
  int my_rank, p, i, start, size;
  clock_t begin = 0;
  double time_spent;
  int window = atoi(argv[3]);

  // Initialize MPI
  MPI_Status status;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &p);

  if (my_rank == 0) {
    // Rank 0 read image from disk
    image = readPPM(argv[1], &width, &height, &max);
  } 

  // Broadcast width and height to other processes
  MPI_Bcast(&width, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&height, 1, MPI_INT, 0, MPI_COMM_WORLD);

  if (my_rank == 0) {
    begin = clock();
    for (i=1; i<p; i++) {
      start = getStart(i, width, height, window, p);
      size = getSize(i, width, height, window, p);
      MPI_Send(image +start, size*sizeof(RGB), MPI_CHAR, i, 0, MPI_COMM_WORLD);
    }
  } else {
    start = getStart(my_rank, width, height, window, p);
    size = getSize(my_rank, width, height, window, p);
    image = (RGB*)malloc(size*sizeof(RGB)); 
    assert(image);    
    MPI_Recv(image, size*sizeof(RGB), MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
  }

  processImage(width, height, image, argc, argv);

  if (my_rank == 0) {
    time_spent = (double)(clock() - begin) / CLOCKS_PER_SEC;
    writePPM(argv[2], width, height, max, image);
    printf("done. processImage took %fs\n", time_spent);
  }

  // Cleanup
  free(image);
  MPI_Finalize();

  return(0);
}
