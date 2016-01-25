#include "a1.h"
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void meanFilter(int width, int height, RGB *image, int window, int start, int end, int rank);
void medianFilter(int width, int height, RGB *image, int window, int start, int end, int rank);

void processImage(int width, int height, RGB *image, int argc, char** argv)
{
  // Initialize variables
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
  RGB *pixxxlllz = image + width*height-1;
    printf("thing r: %d from rank: %d\n", pixxxlllz->r, my_rank);
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

  // Run mean filtering on image
  printf("Process %d crunching pixels %d - %d...\n", my_rank, my_range[0], my_range[1]);
  if ( *filter == 'A') {
    meanFilter(width, height, image, window, my_range[0], my_range[1], my_rank);
  } else if ( *filter == 'M' ) {
    medianFilter(width, height, image, window, my_range[0], my_range[1], my_rank);
  } else {
    printf("Error: Invalid filter specified. Please use either 'A' for Mean, or 'M' for Median.\n");
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

void meanFilter(int width, int height, RGB *image, int window, int start, int end, int rank){
  int thing = (window - 1)/2;
  int topleft, current;
  int pc, i, j, cpypx;
  double sum[3];
  RGB *unmodified = (RGB*)malloc(width*height*sizeof(RGB));
  RGB *current_pixel;
  RGB *copydestpixel;
  RGB *copysrcpixel;
  RGB *pixel;


  // Deep copy unmodified <- img
  for (i=0; i < width*height; i++) {
    copydestpixel = unmodified + i;
    copysrcpixel = image + i;
    copydestpixel->r = copysrcpixel->r; // SEG FAULT HERE
    copydestpixel->g = copysrcpixel->g;
    copydestpixel->b = copysrcpixel->b;
  }

  // For each pixel in this worker's quota
  for (pc = start; pc < end; pc ++) {
    // Current pixel of interest
    pixel = image + pc;

    // Pixel at top left of window
    topleft = pc - (thing * width) - thing;

    sum[0] = 0; // Red values
    sum[1] = 0; // Green values
    sum[2] = 0; // Blue values
    int count = 1;
    //printf("sum: %0.1f   count: %d\n", sum, count);

    // For each row in window
    for (i=0; i < window; i ++) {
      // For each column in window
      for (j=0; j < window; j ++) {
        // Determine the pixel we're looking at
        current = topleft + i * width - thing + j + 1;

        // If current pixel is outside range of window and image, skip it
        if (current < 0 || current > width * height -1 || (current % width) > (pc % width) + thing) {
          // Do nothing

        // If current pixel is in range of window and image
        } else {
          current_pixel = unmodified + current;
          sum[0] = sum[0] + (current_pixel->r);
          sum[1] = sum[1] + (current_pixel->g);
          sum[2] = sum[2] + (current_pixel->b);

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


// Code courtesy of https://en.wikiversity.org/wiki/C_Source_Code/Find_the_median_and_mean
void median(int n, int s, int x[], float m[]) {
    int temp[3] = {0};
    int i, j;
    // the following two loops sort the array x in ascending order
    for(i=0; i<n-1; i++) {
        for(j=i+1; j<n; j++) {
            if(x[j] < x[i]) {
                // swap elements
                temp[0] = x[i];
                temp[1] = x[i+s];
                temp[2] = x[i+2*s];

                x[i] = x[j];
                x[i+s] = x[j+s];
                x[i+2*s] = x[j+2*s];

                x[j] = temp[0];
                x[j+s] = temp[1];
                x[j+2*s] = temp[2];
            }
        }
    }

    if(n%2==0) {
        // if there is an even number of elements, return mean of the two elements in the middle
        m[0] = (x[n/2] + x[n/2 - 1]) / 2.0;
        m[1] = (x[n/2 + s] + x[n/2 - 1 + s]) / 2.0;
        m[2] = (x[n/2 + 2*s] + x[n/2 - 1 + 2*s]) / 2.0;
    } else {
        // else return the element in the middle
        m[0] = x[n/2];
        m[1] = x[n/2 + s];
        m[2] = x[n/2 + 2*s];
    }
}

void medianFilter(int width, int height, RGB *image, int window, int start, int end, int rank){
  int thing = (window - 1)/2;
  int windowsq = window*window;
  int topleft, current;
  int pc, i, j, cpypx;

  // Stores the RGB values in the window, ie first window^2 values are R's, second are B's, then G's
  int rgbvalues[3*windowsq];
  float medianvalues[3] = {0};

  RGB *unmodified = (RGB*)malloc(width*height*sizeof(RGB));
  RGB *current_pixel;
  RGB *copydestpixel;
  RGB *copysrcpixel;
  RGB *pixel;


  // Deep copy unmodified <- img
  for (i=0; i < width*height; i++) {
    copydestpixel = unmodified + i;
    copysrcpixel = image + i;

    copydestpixel->r = copysrcpixel->r;
    copydestpixel->g = copysrcpixel->g;
    copydestpixel->b = copysrcpixel->b;
  }

  // For each pixel in this worker's quota
  for (pc = start; pc < end; pc ++) {
    // Current pixel of interest
    pixel = image + pc;

    // Pixel at top left of window
    topleft = pc - (thing * width) - thing;

    // Reset rgbvalues and median to 0's
    memset(medianvalues, 0, sizeof(float)*3);
    memset(rgbvalues, 0, sizeof(int)*3*windowsq);

    int count = 1;

    // For each row in window
    for (i=0; i < window; i ++) {
      // For each column in window
      for (j=0; j < window; j ++) {
        // Determine the pixel we're looking at
        current = topleft + i * width - thing + j + 1;

        // If current pixel is outside range of window and image, skip it
        if (current < 0 || current > width * height -1 || (current % width) > (pc % width) + thing) {
          // Do nothing

        // If current pixel is in range of window and image
        } else {
          current_pixel = unmodified + current;
          rgbvalues[count - 1] = current_pixel->r;
          rgbvalues[count - 1 + windowsq] = current_pixel->g;
          rgbvalues[count - 1 + 2*windowsq] = current_pixel->b;

          count = count + 1;
        }
      }
    }

    // new pixel rgb values are median of values in window, sorted by R-value
    median(count, windowsq, rgbvalues, medianvalues);
    pixel->r = medianvalues[0];
    pixel->g = medianvalues[1];
    pixel->b = medianvalues[2];
  }
}
