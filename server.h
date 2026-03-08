#ifndef SERVER_H
#define SERVER_H

#include "model.h"

void server_run(int listen_fd, int n_workers, int n, float lr, int max_rounds, Model model);

#endif