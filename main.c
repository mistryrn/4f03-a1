#include <stdlib.h>
#include "a1.h"
#include "mpi.h"

int main(int argc, char** argv)
{
  RGB *image = NULL;
  int width = 0, height = 0, max = 0;
  int my_rank, p;

  MPI_Status status;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &p);

  image = readPPM(argv[1], &width, &height, &max);
  
  processImage(width, height, image, argc, argv);
  
  free(image);
  if (my_rank == 0) {
    writePPM(argv[2], width, height, max, image);
  }

  return(0);
}
