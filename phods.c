#include "phods.h"

void phods_motion_estimation(const uint8_t current[N][M], const uint8_t previous[N][M],
                             int vectors_x[N/B][M/B], int vectors_y[N/B][M/B])
{
    int x, y, i, j, k, l, p1, p2, q2, distx, disty, S, min1, min2, bestx = 0, besty = 0;

    distx = 0;
    disty = 0;

    /* Initialize the vector motion matrices */
    for (i = 0; i < N / B; i++)
    {
        for (j = 0; j < M / B; j++)
        {
            vectors_x[i][j] = 0;
            vectors_y[i][j] = 0;
        }
    }

    /* For all blocks in the current frame */
    for (x = 0; x < N / B; x++)
    {
        for (y = 0; y < M / B; y++)
        {
            S = 4;

            while (S > 0)
            {
                min1 = 255 * B * B;
                min2 = 255 * B * B;

                /*For all candidate blocks in X dimension*/
                for (i = -S; i < S + 1; i += S)
                {
                    distx = 0;

                    /* For all pixels in the block */
                    for (k = 0; k < B; k++)
                    {
                        for (l = 0; l < B; l++)
                        {
                            p1 = current[B * x + k][B * y + l];

                            if ((B * x + vectors_x[x][y] + i + k) < 0 ||
                                (B * x + vectors_x[x][y] + i + k) > (N - 1) ||
                                (B * y + vectors_y[x][y] + l) < 0 ||
                                (B * y + vectors_y[x][y] + l) > (M - 1))
                            {
                                p2 = 0;
                            }
                            else
                            {
                                p2 = previous[B * x + vectors_x[x][y] + i + k][B * y + vectors_y[x][y] + l];
                            }

                            if (p1 - p2 > 0)
                            {
                                distx += p1 - p2;
                            }
                            else
                            {
                                distx += p2 - p1;
                            }
                        }
                    }

                    if (distx < min1)
                    {
                        min1 = distx;
                        bestx = i;
                    }
                }

                /* For all candidate blocks in Y dimension */
                for (i = -S; i < S + 1; i += S)
                {
                    disty = 0;

                    /*For all pixels in the block*/
                    for (k = 0; k < B; k++)
                    {
                        for (l = 0; l < B; l++)
                        {
                            p1 = current[B * x + k][B * y + l];

                            if ((B * x + vectors_x[x][y] + k) < 0 ||
                                (B * x + vectors_x[x][y] + k) > (N - 1) ||
                                (B * y + vectors_y[x][y] + i + l) < 0 ||
                                (B * y + vectors_y[x][y] + i + l) > (M - 1))
                            {
                                q2 = 0;
                            }
                            else
                            {
                                q2 = previous[B * x + vectors_x[x][y] + k][B * y + vectors_y[x][y] + i + l];
                            }

                            if (p1 - q2 > 0)
                            {
                                disty += p1 - q2;
                            }
                            else
                            {
                                disty += q2 - p1;
                            }
                        }
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

void print_info(char *filename, int motion_vectors_x[N/B][M/B], int motion_vectors_y[N/B][M/B])
{
    printf("Saving motion vectors for the 2 frames in the file provided: %s...\n", filename);
    // remove(filename);
    FILE *file = fopen(filename, "w");

    printf("Size of motion_vectors is: %dx%d\n", N/B, M/B);
    
    // Check if the file was opened successfully and write the arrays into the file
    if (file != NULL) 
    {
        for (int i = 0; i < N/B; i++) 
        {
            for (int j = 0; j < M/B; j++) 
            {
                if(j == M/B-1)
                {
                    fprintf(file, "%d\n", motion_vectors_x[i][j]);
                    break;
                }
                fprintf(file, "%d ", motion_vectors_x[i][j]);
            }
        }

        fprintf(file, "\n\n");
        for (int i = 0; i < N/B; i++) 
        {
            for (int j = 0; j < M/B; j++) 
            {
                if(j == M/B-1)
                {
                    fprintf(file, "%d\n", motion_vectors_y[i][j]);
                    break;
                }
                fprintf(file, "%d ", motion_vectors_y[i][j]);
            }
        }

        // Close the file
        fclose(file);
    } else {
        // Handle the case where the file couldn't be opened
        printf("Error opening the file for writing...\n");
    }
}


