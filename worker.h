#ifndef WORKER_H
#define WORKER_H

# include "model.h"

int load_csv(const char *filname, float **X, int **y, int n_features);
void worker_loop(int read_fd, int write_fd, const float *X, const int *y, int m, int n, Model model);

#endif