#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define N 144    /* Frame dimension for QCIF format */
#define M 176    /* Frame dimension for QCIF format */

extern int B;    /* Block size provided from userspace */

#define p 7      /* Search space. Restricted in a [-p,p] region around the original location of the block */

extern int block_size;

void phods_motion_estimation(const uint8_t current[N][M], const uint8_t previous[N][M],
                             int vectors_x[N/B][M/B], int vectors_y[N/B][M/B]);

void print_info(char *filename, int motion_vectors_x[N/B][M/B], int motion_vectors_y[N/B][M/B]);

/* Optimized version of PHODS algorithm for motion estimation */
void phods_motion_estimation_opt(const uint8_t current[N][M], const uint8_t previous[N][M],
                             int vectors_x[N/B][M/B], int vectors_y[N/B][M/B]);

void phods_motion_estimation_opt_reuse(const uint8_t current[N][M], const uint8_t previous[N][M],
                             int vectors_x[N/B][M/B], int vectors_y[N/B][M/B]);

                             