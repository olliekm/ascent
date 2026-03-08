#include "server.h"
#include "protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>


void server_run(int listen_fd, int n_workers, int n, float lr, int max_rounds, Model model) {
    int *client_fds = malloc(n_workers * sizeof(int));
    int connected = 0;

    while (connected < n_workers) {
        int client_fd = accept(listen_fd, NULL, NULL);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        client_fds[connected] = client_fd;
        connected++;
        fprintf(stderr, "Worker %d connected, %d remaining open slots", connected, n_workers - connected);
    }

    fprintf(stderr, "All workers connected. Starting training.\n");
    float *weights = malloc(n * sizeof(float));
    float *grad_buf = malloc(n * sizeof(float));
    float *grad_accum = malloc(n * sizeof(float));

    for (int i = 0; i < n; i++) {
        weights[i] = 0.0f;
    }

    for (int round = 0; round < max_rounds; round++) {
        for (int i = 0; i < n; i++) {
            grad_accum[i] = 0.0f;
        }

        for (int i = 0; i < n_workers; i++) {
            write_floats(client_fds[i], weights, n);
        }

        int received = 0;
        fd_set read_fds;
        int *got_gradient = calloc(n_workers, sizeof(int));

        while (received < n_workers) {
            FD_ZERO(&read_fds);

            int maxfd = -1;
            for (int i = 0; i < n_workers; i++) {
                if (!got_gradient[i]) {
                    FD_SET(client_fds[i], &read_fds);
                    if (client_fds[i] > maxfd) {
                        maxfd = client_fds[i];
                    }
                }
            }

            int ready = select(maxfd + 1, &read_fds, NULL, NULL, NULL);
            if (ready < 0) {
                perror("select");
                break;
            }

            for (int i = 0; i < n_workers; i++) {
                if (!got_gradient[i] && FD_ISSET(client_fds[i], &read_fds)) {
                    int ret = read_floats(client_fds[i], grad_buf, n);
                    if (ret < 0) {
                        fprintf(stderr, "Worker %d disconnected\n", i);
                        close(client_fds[i]);
                        client_fds[i] = -1;
                    } else {
                        for (int j = 0; j < n; j++) {
                            grad_accum[j] += grad_buf[j];
                        }
                    }
                    got_gradient[i] = 1;
                    received++;
                }
            }
        }

        free(got_gradient);
        for (int i = 0; i < n; i++) {
            weights[i] -= lr * (grad_accum[i]/n_workers);
        } 

        fprintf(stderr, "Round %d/%d complete\n", round + 1, max_rounds);
    }

    for (int i = 0; i < n_workers; i++) {
        if (client_fds[i] >= 0) {
            close(client_fds[i]);
        }
    }

    free(client_fds);
    free(weights);
    free(grad_buf);
    free(grad_accum);

    fprintf(stderr, "Training complete\n");
}

