
#include "a1.h"
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void meanFilter(int width, int height, RGB *image, int window, int start, int end);
// void medianFilter();

void processImage(int width, int height, RGB *image, int argc, char** argv)
{
  // Initialize variables
  int verbose = 1;
  int i, my_rank, p, n, my_range[2];
  double a, b;

  // initialize MPI
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
  if (my_rank == 0 && verbose) {
    printf("Window size:     %dx%d\nFilter Type:     %s\n", window, window, filter);
    printf("Pixels in image: %d\nPixels/process:  %.0f\nTotal processes: %d\n", size, h, p);
    printf("----------------------\n");
  }


  // Determine lower and upper values for pixel range
  my_range[0] = size/p*my_rank;

  // ensure last process gets all the pixels up to the end
  if (my_rank == p - 1) {
    my_range[1] = size - 1;
  } else {
    my_range[1] = my_range[0] + size/p;
  }

  // Run mean filtering on image
  meanFilter(width, height, image, window, my_range[0], my_range[1]);

  if (my_rank != 0) {
    printf("Process %d/%d sending (%d - %d)...\n", my_rank, p, my_range[0], size/p);
    MPI_Send(image + my_range[0], size/p, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
    printf("Process %d/%d sent.\n", my_rank, p);
  } else {
    for (int i=1; i < p; i ++) {
      printf("Process 0 receiving process %d\n", i);
      MPI_Recv((image + (size/p * i)), size/p, MPI_CHAR, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
      printf("Process %d/%d message received.\n", i, p);
    }
  }

}

void meanFilter(int width, int height, RGB *image, int window, int start, int end){
  int thing = (window - 1)/2;
  int topleft, current;
  double sum;
  RGB *current_pixel;
  // For each pixel in this worker's quota
  for (int pc = start; pc < end; pc ++) {
    RGB *pixel = image + pc;
    topleft = pc - (thing * width) - thing;
    sum = 0;
    int count = 1;
    //printf("sum: %0.1f   count: %d\n", sum, count);

    // For each row in window
    for (int i=0; i < window; i ++) {

      // For each column in row
      for (int j=0; j < window; j ++) {
        current = topleft + i * width - thing + j + 1;

        // If current pixel is outside range of window and image
        if (current < 0 || current > width * height - 1 || (current % width) > (pc % width) + thing) {
          //printf("pc: %d    out of range: %d\n", pc, current);

        // If current pixel is in range of window and image
        } else {
          current_pixel = image + current;
          //printf("looking at: %d    r value:     %d\n", current, current_pixel->r);
          sum = sum + (current_pixel->r);
          count = count + 1;
        }
      }
    }
    //printf("pc: %d    total:     %f\n", pc, sum);
    // new pixel red value is average of ^
    pixel->r = sum/count;
  }
}
