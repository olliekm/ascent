#include "worker.h"
#include "model.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <shard_file> <n_features>\n", argv[0]);
        exit(1);
    }

    const char *shard_file = argv[1];
    int n_features = atoi(argv[2]);

    float *X;
    int *y;
    int m = load_csv(shard_file, &X, &y, n_features);
    if (m < 0) {
        fprintf(stderr, "Failed to load %s\n", shard_file);
        exit(1);
    }
    fprintf(stderr, "Loaded %d samples from %s\n", m, shard_file);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        exit(1);
    }
    fprintf(stderr, "Connnected to server\n");

    Model model = logistic_regression_model();
    worker_loop(sockfd, sockfd, X, y, m, n_features, model);

    close(sockfd);
    free(X);
    free(y);

    return 0;
}