#include <stdlib.h>
#include "a1.h"
#include "mpi.h"
#include <time.h>
#include <stdio.h>
#include <assert.h>

int main(int argc, char** argv)
{
  RGB *image = NULL;
  int width = 0, height = 0, max = 0;
  int my_rank, p, i;
  clock_t begin = 0;
  double time_spent;


  MPI_Status status;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &p);

  if (my_rank == 0) {
    image = readPPM(argv[1], &width, &height, &max);
  }
  MPI_Bcast(&width, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&height, 1, MPI_INT, 0, MPI_COMM_WORLD);

  if (my_rank == 0) {
    for (i=1; i<p; i++) {
      MPI_Send(image, width*height*sizeof(RGB), MPI_CHAR, i, 0, MPI_COMM_WORLD);
    }
    begin = clock();
  }
  else {
    RGB *image = (RGB*)malloc(width*height*sizeof(RGB));
    assert(image);

    for(i=0; i < width*height; i++) 
      {
        image[i].r = 128;
        image[i].g = 128;
        image[i].b = 128;
      }
  
    MPI_Recv(image, width*height*sizeof(RGB), MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
  }
  processImage(width, height, image, argc, argv);
  if (my_rank == 0) {
    time_spent = (double)(clock() - begin) / CLOCKS_PER_SEC;
    writePPM(argv[2], width, height, max, image);
    printf("done. processImage took %fs\n", time_spent);
  }
  free(image);

  MPI_Finalize();

  return(0);
}
