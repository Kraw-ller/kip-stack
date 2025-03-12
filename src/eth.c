#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <linux/if.h>
#include <linux/if_ether.h>

struct eth_hdr
{
    unsigned char dmac[6]; // destination mac
    unsigned char smac[6]; // source mac
    u_int16_t ethertype;   // if < 1536 : length of payload else : type of payload (IPV4, ARP)
    // tags 
    unsigned char payload[];
    // crc
} __attribute__((packed));

int handle_frame(int fd, struct eth_hdr hdr);