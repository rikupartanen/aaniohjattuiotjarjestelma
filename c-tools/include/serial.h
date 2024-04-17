#ifndef SERIAL_H__
#define SERIAL_H__
#include <stdint.h>
#include "layer.h"

int open_port(const char *port);
void close_port(int fd);
int send_data(int fd, void *tx_buff, size_t len);
int write_weights(int, struct layer*);

#endif /* SERIAL_H__ */
