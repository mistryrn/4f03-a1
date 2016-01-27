#include "a1.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

float median(int n, int x[]);
float median2(int n, int arr[]);

void swap(int* a, int* b);

void medianFilter(int size, int width, RGB *image, int window, int start, int end, int rank){
  int thing = (window - 1)/2;
  int windowsq = window*window;
  int topleft, current;
  int pc, i, j, cpypx;

  // Storage for the R, G, and B values in the window
  int rvalues[windowsq];
  int gvalues[windowsq];
  int bvalues[windowsq];

  RGB *unmodified = (RGB*)malloc(size*sizeof(RGB));
  RGB *current_pixel;
  RGB *copydestpixel;
  RGB *copysrcpixel;
  RGB *pixel;


  // Deep copy unmodified <- img
  for (i=0; i < size; i++) {
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

    // Reset R, G, and B values to 0's
    memset(rvalues, 0, sizeof(int)*windowsq);
    memset(gvalues, 0, sizeof(int)*windowsq);
    memset(bvalues, 0, sizeof(int)*windowsq);

    int count = 1;

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
          rvalues[count - 1] = current_pixel->r;
          gvalues[count - 1] = current_pixel->g;
          bvalues[count - 1] = current_pixel->b;

          count = count + 1;
        }
      }
    }

    pixel->r = median2(count, rvalues);
    pixel->g = median2(count, gvalues);
    pixel->b = median2(count, bvalues);
  }
}

// Code courtesy of https://en.wikiversity.org/wiki/C_Source_Code/Find_the_median_and_mean
// Great for testing, but slow.
float median(int n, int x[]) {
  float temp;
  int i, j;
  // the following two loops sort the array x in ascending order
  for (i = 0; i < n-1; i++) {
    for (j = i+1; j < n; j++) {
      if (x[j] < x[i]) {
        // swap elements
        temp = x[i];
        x[i] = x[j];
        x[j] = temp;
      }
    }
  }

  if (n%2 == 0) {
    // if there is an even number of elements, return the mean of the two elements in the middle
    return ((x[n/2] + x[n/2 - 1]) / 2.0);
  } else {
    // else return the element in the middle
    return x[n/2];
  }
}

void swap(int* a, int* b){
    int t = *a;
    *a = *b;
    *b = t;
}

// Code courtesy of http://stackoverflow.com/questions/1961173/median-function-in-c-math-library
// Much faster than median(). 
float median2(int n, int arr[])
{
    int low, high ;
    int median;
    int middle, ll, hh;
    low = 0 ; high = n-1 ; median = (low + high) / 2;
    for (;;) {
        if (high <= low) /* One element only */
            return arr[median] ;
        if (high == low + 1) { /* Two elements only */
            if (arr[low] > arr[high])
                swap(arr+low, arr+high) ;
                return arr[median] ;
        }
        /* Find median of low, middle and high items; swap into position low */
        middle = (low + high) / 2;
        if (arr[middle] > arr[high]){
            swap(arr + middle, arr +high);
        }
        if (arr[low] > arr[high]){
          swap(arr+low, arr+high);
        }
        if (arr[middle] > arr[low]){
            swap(arr+middle, arr+low);
        }
        
        /* Swap low item (now in position middle) into position (low+1) */
        swap(arr+middle, arr+low+1) ;
        /* Nibble from each end towards middle, swapping items when stuck */
        ll = low + 1;
        hh = high;

        for (;;) {
            do ll++; while (arr[low] > arr[ll]) ;
            do hh--; while (arr[hh] > arr[low]) ;
            if (hh < ll)
                break;
            swap(arr+ll, arr+hh) ;
        }
        
        /* Swap middle item (in position low) back into correct position */
        swap(arr+low, arr+hh) ;
        /* Re-set active partition */
        if (hh <= median)
            low = ll;
        if (hh >= median)
            high = hh - 1;
    }
    return arr[median];
}
