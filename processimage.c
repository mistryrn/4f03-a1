#include "a1.h"
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

void processImage(int width, int height, RGB *image, int argc, char** argv)
{
  // Initialize variables
  int i, my_rank, p, n, my_range[2];
  double a, b;

  // Initialize MPI
  MPI_Status status;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  int source = 0, dest = 0, tag = 0;

  // Get total size of image
  int size = width*height;

  // Get number of pixels per process
  double h = size/p;

  // Get parameters from command line
  int window = atoi(argv[3]);
  char *filter = argv[4];

  // Print header
  if (my_rank == 0) {
    printf("Window size:     %dx%d\nFilter Type:     %s\n", window, window, filter);
    printf("Pixels in image: %d\nPixels/process:  %.0f\nTotal processes: %d\n", size, h, p);
    printf("----------------------\n");
  }

  /* create a type for struct RGB */
  const int nitems=3;
  int          blocklengths[3] = {sizeof(char),sizeof(char),sizeof(char)};
  MPI_Datatype types[3] = {MPI_CHAR, MPI_CHAR, MPI_CHAR};
  MPI_Datatype mpi_rgb_type;
  MPI_Aint     offsets[3];
  offsets[0] = offsetof(RGB, r);
  offsets[1] = offsetof(RGB, g);
  offsets[2] = offsetof(RGB, b);
  MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_rgb_type);
  MPI_Type_commit(&mpi_rgb_type);
  /* ------ */

  MPI_Barrier(MPI_COMM_WORLD);
  // Determine lower and upper values for pixel range
  my_range[0] = h*my_rank;

  // ensure last process gets all the pixels up to the end
  if (my_rank == p - 1) {
    my_range[1] = size;
  } else {
    my_range[1] = my_range[0] + h;
  }

  // Run filtering on image
  printf("Process %d crunching pixels %d - %d...\n", my_rank, my_range[0], my_range[1]);

  if ( *filter == 'A') { // mean filter
    meanFilter(width, height, image, window, my_range[0], my_range[1], my_rank);

  } else if ( *filter == 'M' ) { // median filter
    medianFilter(width, height, image, window, my_range[0], my_range[1], my_rank);

  } else { // Invalid input for filter type
    if (my_rank == 0){
      printf("Error: Invalid filter specified. Please use either 'A' for Mean, or 'M' for Median.\n");
    }
  }
  if (my_rank != 0) {
    // Send this rank's image chunk to process zero
    MPI_Send(image + (size/p) * my_rank, size/p, mpi_rgb_type, dest, tag, MPI_COMM_WORLD);
  } else {
    for (i=1; i < p; i ++) {
      // Wait to receive data from other processes
      MPI_Recv(image + size/p * i, size + h, mpi_rgb_type, i, tag, MPI_COMM_WORLD, &status);
    }
  }
}
