#ifndef PROTOCOL_H
#define PROTOCOL_H

int write_floats(int fd, const float *data, int n);
int read_floats(int fd, float *data, int n);

#endif