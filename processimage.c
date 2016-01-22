
#include "a1.h"
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void meanFilter(int width, int height, RGB *image, int window, int start, int end, int rank);
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
  my_range[0] = h*my_rank;

  // ensure last process gets all the pixels up to the end
  if (my_rank == p - 1) {
    printf("highest ranking process\n");
    my_range[1] = size;
  } else {
    my_range[1] = my_range[0] + h;
  }

  // Run mean filtering on image
  printf("Process %d crunching (%d - %d)...\n", my_rank, my_range[0], my_range[1]);
  meanFilter(width, height, image, window, my_range[0], my_range[1], my_rank);

  if (my_rank != 0) {
    printf("Process %d/%d sending (%d - %d)...\n", my_rank, p-1, my_range[0], my_range[1]);
    MPI_Send(image + size/p * my_rank, size*sizeof(char) + h, MPI_CHAR, 0, tag, MPI_COMM_WORLD);
    printf("Process %d/%d sent.\n", my_rank, p-1);
  } else {
    for (int i=1; i < p; i ++) {
      printf("Process 0 receiving process %d\n", i);
      MPI_Recv(image + size/p * i, size*sizeof(char) + h, MPI_CHAR, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
      printf("Process %d/%d message received.\n", i, p-1);
    }
  }

}

void meanFilter(int width, int height, RGB *image, int window, int start, int end, int rank){
  int thing = (window - 1)/2;
  int topleft, current;
  double sum[3];
  RGB *current_pixel;
  // For each pixel in this worker's quota
  for (int pc = start; pc < end; pc ++) {
    RGB *pixel = image + pc;
    topleft = pc - (thing * width) - thing;
    sum[0] = 0;
    //sum[1] = 0;
    //sum[2] = 0;
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
          sum[0] = sum[0] + (current_pixel->r);
          //sum[1] = sum[1] + (current_pixel->g);
          //sum[2] = sum[2] + (current_pixel->b);

          count = count + 1;
        }
      }
    }
    //printf("pc: %d    total:     %f\n", pc, sum);
    // new pixel rgb value is average of ^
    pixel->r = sum[0]/count;
    pixel->g = sum[1]/count;
    pixel->b = sum[2]/count;
  }
}
