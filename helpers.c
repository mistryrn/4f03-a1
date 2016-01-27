

// Get start position of chunk for process
int getStart(int rank, int width, int height, int window, int p) {
    int total = width * height;
    int ration = total/p;

    int start = ration * rank - ((window - 1)/2 * width);

    if (start < 0) {
        start = 0;
    }
    return start;
}

// Get size of chunk for process
int getSize(int rank, int width, int height, int window, int p) {
    int total = width * height;
    int ration = total/p;
    int size = ration + (width * (window-1));
    int start = getStart(rank, width, height, window, p);
    if (rank >= p-1 || size > total) {
        size = total - start;
    } else if (start == 0) {
        size = size - ((window-1)/2 * width);
    }
    return size;
}
