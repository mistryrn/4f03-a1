#include "a1.h"
#include <stdlib.h>

void meanFilter(int size, int width, RGB *image, int window, int start, int end, int rank){
  int thing = (window - 1)/2;
  int topleft, current;
  int pc, i, j, cpypx;
  double sum[3];
  RGB *unmodified = (RGB*)malloc(size*sizeof(RGB));
  RGB *current_pixel;
  RGB *copydestpixel;
  RGB *copysrcpixel;
  RGB *pixel;


  // Deep copy to store unmodified values
  for (i=0; i < size; i++) {
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
        if (current < 0 || current > size -1 || (current % width) > (pc % width) + thing) {
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