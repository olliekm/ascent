#ifndef SERVER_H
#define SERVER_H

#include "model.h"

void server_loop(int *write_fds, int *read_fds, int n_workers, int n, float lr, int max_rounds, Model model);

#endif