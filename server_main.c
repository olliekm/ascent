#include "server.h"
#include "model.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 9000

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <n_workers> <n_features> <max_rounds>\n", argv[0]);
        exit(1);
    }

    int n_workers = atoi(argv[1]);
    int n_features = atoi(argv[2]);
    int max_rounds = atoi(argv[3]);
    float lr = 0.01;

    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        exit(1);
    }

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY; 
    
    if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(listen_fd, 5) < 0) {
        perror("listen");
        exit(1);
    }

    fprintf(stderr, "Server listening on port %d and waiting for %d workers\n", PORT, n_workers);

    Model model = logistic_regression_model();
    server_run(listen_fd, n_workers, n_features, lr, max_rounds, model);

    close(listen_fd);
    return 0;

}


