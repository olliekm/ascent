#include "server.h"
#include "protocol.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void server_loop(int *write_fds, int *read_fds, int n_workers, int n, float lr, int max_rounds, Model model) {
    float *weights = malloc(sizeof(float) * n);
    for (int i = 0; i < n; i++) {
        weights[i] = 0.0f;
    }

    float *grad_buf = malloc(sizeof(float) * n);
    
    for (int round = 0; round < max_rounds; round++) {
        float *grad_accum = malloc(sizeof(float) * n); 
        for (int j = 0; j < n; j++) {
            grad_accum[j] = 0.0f;
        }

        for (int i = 0; i < n_workers; i++) {
            write_floats(write_fds[i], weights, n);
        }
    
        for (int i = 0; i < n_workers; i++) {
            read_floats(read_fds[i], grad_buf, n);
            for (int j = 0; j < n; j++) {
                grad_accum[j] += grad_buf[j];
            } 
        }
    
        for (int i = 0; i < n; i++) {
            weights[i] -= lr * (grad_accum[i] / n_workers);
        }
        
        free(grad_accum);
        fprintf(stderr, "Completed %d/%d rounds\n", round, max_rounds);
    }

    free(grad_buf);
    free(weights);
    for (int i = 0; i < n_workers; i++) {
        close(write_fds[i]);
        close(read_fds[i]);
    }

}