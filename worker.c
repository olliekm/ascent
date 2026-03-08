#include "worker.h"
#include "protocol.h"
#include "model.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_SIZE 1024


int load_csv(const char *filename, float **X, int **y, int n_features) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("fopen");
        return -1;
    }
    char line[MAX_LINE_SIZE];
    int lines = 0;
    while (fgets(line, sizeof(line), fp) != NULL) {
        lines++;
    }

    rewind(fp);
    
    *X = malloc(lines * n_features * sizeof(float));
    *y = malloc(lines * sizeof(int));

    int row = 0;
    while (fgets(line, sizeof(line), fp) !=  NULL) {
        char *token = strtok(line, ",");
        int col = 0;
        while (token != NULL) {
            if (col < n_features) {
                (*X)[row * n_features + col] = strtof(token, NULL);
            } else {
                (*y)[row] = atoi(token);
            }
            col++;
            token = strtok(NULL, ",");
        }

        row++;
    }
    fclose(fp);
    return lines;
}

void worker_loop(int read_fd, int write_fd, const float *X, const int *y, int m, int n, Model model) {
    float *w_buf = malloc(n * sizeof(float));
    float *grad_buf = malloc(n * sizeof(float));
    while (1) {
        int ret = read_floats(read_fd, w_buf, n);
        if (ret == -1) {
            break;
        }
        model.gradient(grad_buf, w_buf, X, y, m, n);
        write_floats(write_fd, grad_buf, n);
    }
    free(grad_buf);
    free(w_buf);
}

