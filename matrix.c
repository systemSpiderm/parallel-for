#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "parallel_for.h"

// 函数参数
typedef struct {
    double* A, *B, *C;
    int M, N, K;
} MatrixArg;

// 矩阵乘法函数（循环代码块）
void* multiply_functor(void* matrix_arg, int row);

// 从lower到upper的随机浮点数生成器
double get_random_double(double lower, double upper);

// 随机初始化矩阵
void initialize_matrix(double *matrix, int M, int N);

int main(int argc, char* argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Please enter thread_count, M, N and K\n");
        exit(1);
    }

    int thread_count = atoi(argv[1]);
    int M = atoi(argv[2]);
    int N = atoi(argv[3]);
    int K = atoi(argv[4]);

    if (M <= 0 || N <= 0 || K <= 0) {
        printf("Matrix dimensions must be positive integers.\n");
        return 1;
    }

    double *A, *B, *C;
    A = (double *)malloc(M * N * sizeof(double));
    B = (double *)malloc(N * K * sizeof(double));
    C = (double *)malloc(M * K * sizeof(double));

    // 初始化矩阵
    memset(C, 0, M * K * sizeof(double));
    initialize_matrix(A, M, N);
    initialize_matrix(B, N, K);

    // 创建矩阵参数arg
    MatrixArg* matrix_arg = (MatrixArg*)malloc(sizeof(MatrixArg));
    *matrix_arg = (MatrixArg){A, B, C, M, N, K};

    // 记录开始时间
    struct timespec start, end;
    //clock_gettime(CLOCK_MONOTONIC, &start);

    // 执行并行矩阵乘法
    parallel_for(0, M, 1, multiply_functor, matrix_arg, thread_count);

    // 记录结束时间
    //clock_gettime(CLOCK_MONOTONIC, &end);

    // 计算运行时间（秒）
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Running Time: %.6f seconds\n", elapsed_time);

    free(A);
    free(B);
    free(C);
    free(matrix_arg);
    return 0;
}

double get_random_double(double lower, double upper) {
    int random_int = rand();
    return lower + (double)(random_int / (RAND_MAX + 1.0)) * (upper - lower);
}

void initialize_matrix(double *matrix, int M, int N) {
    for (int i = 0; i < M * N; i++) {
        matrix[i] = get_random_double(10000.0, 1e100);
    }
}

void* multiply_functor(void* matrix_arg, int row) {
    MatrixArg* args = (MatrixArg*)matrix_arg;
    double *A = args->A, *B = args->B, *C = args->C;
    int M = args->M, N = args->N, K = args->K;

    for (int k = 0; k < N; k++) {
        for (int j = 0; j < K; j++) {
            C[row * K + j] += A[row * N + k] * B[k * K + j];
        }
    }
    return NULL;
}
