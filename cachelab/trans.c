/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";


//自己书写的函数，目的是为了是缓存更多的命中
//32*32,m<300
//64*64,m<1300
//61*67,m<2000
void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {
    if (M == 32) {
        int a, b, i;
        int k0, k1, k2, k3, k4, k5, k6, k7;
        for (a = 0; a < 4; ++a) {      //行增加
            for (b = 0; b < 4; ++b) {  //列增加
                for (i = 0; i < 8; ++i) {
                    k0 = A[i + a * 8][b * 8];
                    k1 = A[i + a * 8][1 + b * 8];
                    k2 = A[i + a * 8][2 + b * 8];
                    k3 = A[i + a * 8][3 + b * 8];
                    k4 = A[i + a * 8][4 + b * 8];
                    k5 = A[i + a * 8][5 + b * 8];
                    k6 = A[i + a * 8][6 + b * 8];
                    k7 = A[i + a * 8][7 + b * 8];
                    B[b * 8][i + a * 8] = k0;
                    B[1 + b * 8][i + a * 8] = k1;
                    B[2 + b * 8][i + a * 8] = k2;
                    B[3 + b * 8][i + a * 8] = k3;
                    B[4 + b * 8][i + a * 8] = k4;
                    B[5 + b * 8][i + a * 8] = k5;
                    B[6 + b * 8][i + a * 8] = k6;
                    B[7 + b * 8][i + a * 8] = k7;
                }
            }
        }
    } else if(M==64){
        int a, b, i;
        int k0, k1, k2, k3;
        for (a = 0; a < 16; ++a) {      //行增加
            for (b = 0; b < 16; ++b) {  //列增加
                for (i = 0; i < 4; ++i) {
                    k0 = A[i + a * 4][b * 4];
                    k1 = A[i + a * 4][1 + b * 4];
                    k2 = A[i + a * 4][2 + b * 4];
                    k3 = A[i + a * 4][3 + b * 4];
                    B[b * 4][i + a * 4] = k0;
                    B[1 + b * 4][i + a * 4] = k1;
                    B[2 + b * 4][i + a * 4] = k2;
                    B[3 + b * 4][i + a * 4] = k3;
                }
            }
        }
    } else{
        int a, b, i;
        int temp;
        int step = 17;
        for (a = 0; a < N; a += step) {      //行增加
            for (b = 0; b < M; b += step) {  //列增加
                for (i = 0; i < step && i + a < N; ++i) {
                    for (int j = 0; j < step && j + b < M; ++j) {
                        temp = A[a + i][b + j];
                        B[b + j][a + i] = temp;
                    }
                }
            }
        }
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";

void trans(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions() {
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc);

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N]) {
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

