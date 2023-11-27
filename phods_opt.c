#include "phods.h"

/* Improved optimized PHODS with loop merging and data reuse */
void phods_motion_estimation_opt_reuse(const uint8_t current[N][M], const uint8_t previous[N][M],
                             int vectors_x[N/B][M/B], int vectors_y[N/B][M/B])
{
    int x, y, i, j, k, l, p1, p2, q2, distx, disty, S, min1, min2, bestx = 0, besty = 0;
    int vect_xaxis[5], vect_yaxis[5], temp[3], temp_new[4];

    /* Smaller Local copies of current/previous image frame, for cache usage optimization */
    int current_line[M][B], block[B][B];
    int previous_line[B+2*p][M], rw[B+2*p][B+2*p];

    /* For all blocks in the current frame */
    for (x = 0; x < N / B; x++)
    {
        vect_xaxis[3] = B*x;
        vect_xaxis[5] = vect_xaxis[3] - p;

        if(x==N/B - 1)
            temp[2] = 1;

        /* Copy data from current to buffer current_line */
        for (j = 0; j < M; j++) 
            for (i = 0; i < B; i++)
                current_line[i][j] = current[vect_xaxis[3] + i][j];
        
        for (i = 0; i < 2 * p + B; i++)
        {
            temp[0] = i - p;    /* Don't calculate continuously constant per j variables */
            temp[1] = i + B;
            vect_xaxis[4] = vect_xaxis[3] + i - p;

            for (j = 0; j < M; j++)
            {
                if (x == 0)
                {
                    if (temp[0] < 0)
                        previous_line[i][j] = 0;
                    else
                        previous_line[i][j] = previous[temp[0]][j]; /* Copy from previous array */
                }
                else
                {
                    if (temp[0] < p)
                        previous_line[i][j] = previous_line[temp[1]][j]; /* Reuse from the same array*/
                    else
                    {
                        if (temp[2] && temp[0] > B)
                            previous_line[i][j] = 0;
                        else
                            previous_line[i][j] = previous[vect_xaxis[4]][j];
                    }
                }
            }
        }

        for (y = 0; y < M / B; y++)
        {
            /* Initialize the vector motion matrices 
            Better for data cache locality for loops below... */
            vectors_x[x][y] = 0;
            vectors_y[x][y] = 0;

            S = 4;

            vect_yaxis[3] = B*y;
            vect_yaxis[4] = vect_yaxis[3] - p;

            /* Copy data from current to buffer block */
            for (k = 0; k < B; k++)
                for (l = 0; l < B; l++)
                    block[k][l] = current_line[k][vect_yaxis[3] + l];

            /* Copy data from previous_line to RW */
            for (k = 0; k < B + 2 * p; k++) 
            {
                vect_xaxis[6] = vect_xaxis[5] + k;
                for (l = 0; l < B + 2 * p; l++)
                {
                    if (vect_xaxis[6] < 0 || vect_xaxis[6]  > (N - 1) || (vect_yaxis[4] + l) < 0 || (vect_yaxis[4] + l) > (M - 1))
                        rw[k][l] = 0;
                    else
                    {
                        if (l > 2 * p - 1 || y == 0)
                            rw[k][l] = previous_line[k - p][vect_yaxis[4] + l];
                        /* Copy from previous_line array */
                        else
                            rw[k][l] = rw[k][l + B]; /* Reuse from the same array*/
                    }
                }
            }

            while (S > 0)
            {
                min1 = 255 * B * B;
                min2 = min1;  

                /* For all candidate blocks in X dimension and Y dimension: Same for loops into running blocks BxB */
                for (i = -S; i < S + 1; i += S)
                {
                    distx = 0;
                    disty = 0;

                    /* For all pixels in the block */
                    for (k = 0; k < B; k++)
                    {
                        vect_xaxis[0] = B*x + k;
                        vect_xaxis[1] = vect_xaxis[0] + vectors_x[x][y] + i; 
                        vect_xaxis[2] = vect_xaxis[1] - i; 
                        
                        temp_new[0] = vect_xaxis[1] - B*x + p;
                        temp_new[2] = temp_new[0] - i;

                        for (l = 0; l < B; l++)
                        {
                            vect_yaxis[0] = vect_yaxis[3] + l;
                            vect_yaxis[1] = vect_yaxis[0] + vectors_y[x][y] + i; 
                            vect_yaxis[2] = vect_yaxis[1] - i; 
                            temp_new[1] = vect_yaxis[2] - vect_yaxis[3] + p;
                            temp_new[3] = temp_new[1] + i;

                            /* Read the data from buffer current_line */
                            p1 = block[k][l]; 

                            /* For X-dimension vectors */
                            if (vect_xaxis[1] < 0 || vect_xaxis[1] > (N - 1) || vect_yaxis[2] < 0 || vect_yaxis[2] > (M - 1))
                                p2 = 0;
                            else
                                p2 = rw[temp_new[0]][temp_new[1]]; /* Read from RW */

                            if (p1 - p2 > 0)
                                distx += p1 - p2;
                            else
                                distx += p2 - p1;

                            if(i == 0)
                                disty = distx;
                            else
                            {
                                /* For Y-dimension vectors */
                                if (vect_xaxis[2] < 0 || vect_xaxis[2] > (N - 1) ||vect_yaxis[1] < 0 ||vect_yaxis[1] > (M - 1))
                                    q2 = 0;
                                else
                                    q2 = rw[temp_new[2]][temp_new[3]]; /* Read from RW */ 

                            
                                if (p1 - q2 > 0)
                                    disty += p1 - q2;
                                else
                                    disty += q2 - p1;
                            }
                        }
                    }

                    if (distx < min1)
                    {
                        min1 = distx;
                        bestx = i;
                    }
                    
                    if (disty < min2)
                    {
                        min2 = disty;
                        besty = i;
                    }
                }

                S = S / 2;
                vectors_x[x][y] += bestx;
                vectors_y[x][y] += besty;
            }
        }
    }
}

/* Optimized PHODS with loop merging */
void phods_motion_estimation_opt(const uint8_t current[N][M], const uint8_t previous[N][M], 
                                int vectors_x[N/B][M/B], int vectors_y[N/B][M/B])
{
    int x, y, i, k, l, p1, p2, q2, distx, disty, S, min1, min2, bestx = 0, besty = 0;
    int vect_xaxis[3], vect_yaxis[3];

    /* For all blocks in the current frame */
    for (x = 0; x < N / B; x++)
    {
        for (y = 0; y < M / B; y++)
        {
            /* Initialize the vector motion matrices
            Better for data cache locality for loops below... */
            vectors_x[x][y] = 0;
            vectors_y[x][y] = 0;

            S = 4;

            while (S > 0)
            {
                min1 = 255 * B * B;
                min2 = min1; 

                /* For all candidate blocks in X dimension and Y dimension: Same for loops into running blocks BxB */
                for (i = -S; i < S + 1; i += S)
                {
                    distx = 0;
                    disty = 0;

                    /* For all pixels in the block */
                    for (k = 0; k < B; k++)
                    {
                        vect_xaxis[0] = B * x + k;
                        vect_xaxis[1] = vect_xaxis[0] + vectors_x[x][y] + i;
                        vect_xaxis[2] = vect_xaxis[1] - i;

                        for (l = 0; l < B; l++)
                        {
                            vect_yaxis[0] = B * y + l;
                            vect_yaxis[1] = vect_yaxis[0] + vectors_y[x][y] + i;
                            vect_yaxis[2] = vect_yaxis[1] - i;

                            p1 = current[vect_xaxis[0]][vect_yaxis[0]];

                            /* For X-dimension vectors */
                            if (vect_xaxis[1] < 0 || vect_xaxis[1] > (N - 1) || vect_yaxis[2] < 0 || vect_yaxis[2] > (M - 1))
                                p2 = 0;
                            else
                                p2 = previous[vect_xaxis[1]][vect_yaxis[2]];

                            if (p1 - p2 > 0)
                                distx += p1 - p2;
                            else
                                distx += p2 - p1;

                            if (i == 0)
                                disty = distx;
                            else
                            {
                                /* For Y-dimension vectors */
                                if (vect_xaxis[2] < 0 || vect_xaxis[2] > (N - 1) || vect_yaxis[1] < 0 || vect_yaxis[1] > (M - 1))
                                    q2 = 0;
                                else
                                    q2 = previous[vect_xaxis[2]][vect_yaxis[1]];

                                if (p1 - q2 > 0)
                                    disty += p1 - q2;
                                else
                                    disty += q2 - p1;
                            }
                        }
                    }

                    if (distx < min1)
                    {
                        min1 = distx;
                        bestx = i;
                    }

                    if (disty < min2)
                    {
                        min2 = disty;
                        besty = i;
                    }
                }

                S = S / 2;
                vectors_x[x][y] += bestx;
                vectors_y[x][y] += besty;
            }
        }
    }
}
