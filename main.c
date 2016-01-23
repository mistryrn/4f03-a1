#include <stdlib.h>
#include "a1.h"
#include "mpi.h"
#include <time.h>
#include <stdio.h>

int main(int argc, char** argv)
{
  RGB *image = NULL;
  int width = 0, height = 0, max = 0;
  int my_rank, p;
  clock_t begin = 0;
  double time_spent;

  MPI_Status status;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &p);

  image = readPPM(argv[1], &width, &height, &max);
  if (my_rank == 0) {
    begin = clock();
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
