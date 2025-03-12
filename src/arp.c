#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <linux/if.h>
#include <linux/if_ether.h>

struct arp_hdr 
{
    u_int16_t hw_type; // link layer type used (ethernet)
    u_int16_t protype; // protocol type (IPV4)
    unsigned char hwsize;
    unsigned char prosize;
    u_int16_t opcode; // type of ARP message
    unsigned char data[];
} __attribute__((packed));

struct arp_ipv4
{
    unsigned char smac[6];
    u_int32_t sip;
    unsigned char dmac[6];
    u_int32_t dip;
} __attribute__((packed));