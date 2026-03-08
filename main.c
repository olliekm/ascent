#include "server.h"
#include "worker.h"
#include "protocol.h"
#include "model.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    // Creata cli commands to configure this
    int n_workers = 3;
    int n_features = 3;
    float lr = 0.01;
    int max_rounds = 100;

    Model model = logistic_regression_model();

    int server_write_fds[n_workers];
    int server_read_fds[n_workers];

    for (int i = 0; i < n_workers; i++) {
        int to_worker[2];
        int from_worker[2];

        pipe(to_worker);
        pipe(from_worker);

        pid_t pid = fork();

        if (pid == 0) {
            // Child
            close(to_worker[1]);
            close(from_worker[0]);
            
            float *X;
            int *y;
            char filename[64];
            snprintf(filename, sizeof(filename), "data/shard_%d.csv", i);
            int m = load_csv(filename, &X, &y, n_features);
            worker_loop(to_worker[0], from_worker[1], X, y, m, n_features, model);
            
            free(X);
            free(y);
            exit(0);
        } else {
            // Parent / Server
            close(to_worker[0]);
            close(from_worker[1]);

            server_write_fds[i] = to_worker[1];
            server_read_fds[i] = from_worker[0];

        }
    }


    server_loop(server_write_fds, server_read_fds, n_workers, n_features, lr, max_rounds, model);
    
    for (int i = 0; i < n_workers; i++) {
        wait(NULL);
    }

    return 0;
}