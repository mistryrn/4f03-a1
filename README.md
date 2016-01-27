##4F03 Assignment 1
This project is also available through [GitHub](https://github.com/mistryrn/4f03). The URL will be private until the submission deadline. 

###Authors  
  
| Name           | Student Number | Email                | Website                                     |
|:---------------|:---------------|:---------------------|:--------------------------------------------|
| Rakesh Mistry  | 1221428        |mistryrn@mcmaster.ca  | [rakeshmistry.ca](http://rakeshmistry.ca/)  |
| Jamie Counsell | 1054209        | mistryrn@mcmaster.ca | [jamiecounsell.me](http://rakeshmistry.ca/) |

###Description
This C program provides two methods of image filtering to reduce noise in `ppm` formatted images. The two filter options are:

- [Mean Filter](http://subsurfwiki.org/wiki/Mean_filter)  
- [Median Filter](https://en.wikipedia.org/wiki/Median_filter)


###Dependencies
- `openmpi`

###Installation
- Install the dependencies
- Clone the repository

###Operation
To run the program, first run make:

```
$ make clean
... [truncated]
$ make
mpicc -g -O2 -Wall -Wno-unused-variable   -c -o main.o main.c
mpicc -g -O2 -Wall -Wno-unused-variable   -c -o readwriteppm.o readwriteppm.c
mpicc -g -O2 -Wall -Wno-unused-variable   -c -o processimage.o processimage.c
mpicc -o ppmf main.o readwriteppm.o processimage.o 
```
Then run the program as follows:

```
$ mpirun -np p ./ppmf input.ppm output.ppm N F
```
where:

• **p** is the number of processes  
• **input.ppm** is the name of the input file  
• **output.ppm** is the name of the output file  
• **N** specifies the size of the window, that is N × N window, where N is an odd integer ≥ 3  
• **F** is the type of filter. `A` for *mean filter*, and `M` for *median filter* 

###Work Breakdown
The work was broken down as follows:

- Jamie wrote the mean filter
- Rakesh wrote the median filter
- Both authors wrote the rest of `main.c` and `processimage.c` together, in person

###Notes
- Each process allocates and receives only the chunk of the image that it requires. Process 0 allocates and stores the entire source image, as it needs it to write back at the end of the filtering. Each other process uses only the pixels required to compute its ration of the original problem.
- This current implementation will suffer if an image loaded is larger than available memory. It is assumed that this is not the case for this assignment.

###Results
The following are results from McMaster's MPIHost server.

<img src="http://jamiecounsell.me/static/img/mpi/res1.jpg" width=400>


<img src="http://jamiecounsell.me/static/img/mpi/res2.jpg" width=400>