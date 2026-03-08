#include "protocol.h"
#include <unistd.h>

int write_floats(int fd, const float *data, int n) {
    size_t total = n * sizeof(float);
    size_t written = 0;
    
    const char *buf = (const char * )data;

    while (written < total) {
        ssize_t ret = write(fd, buf + written, total - written);
        if (ret <= 0) {
            return -1;
        }
        written += ret;
    }

    return 0;
}

int read_floats(int fd, float *data, int n) {
    size_t total = n * sizeof(float);
    size_t has_read = 0;

    while (has_read < total) {
        ssize_t ret = read(fd, (char *)data + has_read, total - has_read);
        if (ret <= 0) {
            return -1;
        }
        has_read += ret;
    }

    return 0;
}