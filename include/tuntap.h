#ifndef TUNTAP_H
#define TUNTAP_H

#include "syshead.h"
#include "utils.h"

int tun_alloc(char *dev);
ssize_t tun_read(char *buf, int BUFLEN);
ssize_t tun_write(char *buf, int BUFLEN);
void tun_init(char *dev);
static int set_if_up(char *dev);
static int set_if_route(char *dev, char *cidr);
#endif