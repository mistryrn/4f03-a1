#ifndef INCLUDED_A1_H
#define INCLUDED_A1_H

typedef struct {
  unsigned char r,g,b;
} RGB;

RGB * readPPM(char* file, int* width, int* height, int* max);
void writePPM(char* file, int width, int height, int max, const RGB *image);

void meanFilter(int size, int width, RGB *image, int window, int start, int end, int rank);
void medianFilter(int width, int height, RGB *image, int window, int start, int end, int rank);

int getSize(int rank, int width, int height, int window, int p);
int getStart(int rank, int width, int height, int window, int p);

void processImage(int width, int height, RGB *image, int argc, char** argv);

#endif
