#include <sys/time.h>
#include <string.h>
#include <unistd.h>     /* getopt() */

#include "images.h"
#include "phods.h"

#define FILENAME "mot_vect.txt"

static void usage(char *argv0) 
{
	char *help =
		"Usage: %s [switches]\n"
		"       block_size          : size of blocks during frame estimation\n"
        "                             Block size must divide fully N and M...\n"
        "       -d                  : enable debug mode\n"
		"       -h                  : print this help information\n";
	fprintf(stderr, help, argv0);
	exit(-1);
}

int B = 16;     /* Initialize the default block size... */

/* Time and option variables */
int debug = 0;  
long int t;
long opt;
struct timeval t0,t1;


int main(int argc, char **argv)
{
    /* Check for -h help messages requested when executing main.c */
    if(argc == 2 && strcmp(argv[1], "-h") == 0)
        usage(argv[0]);

    if(argc != 3)
        printf("usage: Using default block size B=%i...\n", B);
    else
    {
        B = atoi(argv[2]);

        /* N,M are known standards for QCIF formatted frames 
        Make sure that block_size can fully divide N and M  */
        if((N % B != 0) || (M % B != 0))
        {
            printf("Error: Block size provided doesn't fully divide N or M...\n");
            return 1;
        }

        printf("Running with block size B=%i...\n", B);
    }

    /* Local initialization of motion vectors, due to varying block_size */
    int motion_vectors_x[N/B][M/B], motion_vectors_y[N/B][M/B];
    
    /* File descriptor for saving execution times, after each benchmark */
    FILE *fd = fopen("times.txt", "a");
    if (fd == NULL) 
    {
        fprintf(stderr, "Unable to open/locate the file.\n");
        return 1; 
    }

    if (argc > 1 && strcmp(argv[1], "-phodsO0") == 0)
    {
        /* Without optimization, running PHODS algorithm */
        gettimeofday(&t0, NULL);    
        phods_motion_estimation(curr, prev, motion_vectors_x, motion_vectors_y);
        gettimeofday(&t1, NULL);
        t=((t1.tv_sec*1000000)+(t1.tv_usec))-((t0.tv_sec*1000000)+(t0.tv_usec));
        fprintf(fd, "Without -O3: To finish motion estimation for %ix%i image and block size: %i requires %ld usec\n", N, M, B, t);
    }
    else if (argc > 1 && strcmp(argv[1], "-phodsO1") == 0)
    {
        /* With optimizations... */
        gettimeofday(&t0, NULL);    
        phods_motion_estimation_opt(curr, prev, motion_vectors_x, motion_vectors_y);
        gettimeofday(&t1, NULL);
        t=((t1.tv_sec*1000000)+(t1.tv_usec))-((t0.tv_sec*1000000)+(t0.tv_usec));
	    fprintf(fd, "With -O1: To finish motion estimation for %ix%i image and block size: %i requires %ld usec\n", N, M, B, t);
    }
    else if (argc > 1 && strcmp(argv[1], "-phodsO2") == 0)
    {
        /* With optimizations... */
        gettimeofday(&t0, NULL);    
        phods_motion_estimation_opt_reuse(curr, prev, motion_vectors_x, motion_vectors_y);
        gettimeofday(&t1, NULL);
        t=((t1.tv_sec*1000000)+(t1.tv_usec))-((t0.tv_sec*1000000)+(t0.tv_usec));
	    fprintf(fd, "With -O2: To finish motion estimation for %ix%i image and block size: %i requires %ld usec\n", N, M, B, t);
    }
    else
    {
        /* In case the user doesn't provide any arguments */
        printf("Error: Requires to choose if application should be running with/without optimizations...\n");
        return 1;
    }

    print_info(FILENAME, motion_vectors_x, motion_vectors_y);
    fclose(fd);

    return 0;
}
