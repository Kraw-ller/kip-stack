#ifndef ETHERNET_H_
#define ETHERNET_H_
#include <linux/if_ether.h>
#include <stdint.h>


struct eth_hdr
{
    unsigned char dmac[6]; // destination mac
    unsigned char smac[6]; // source mac
    uint16_t ethertype;   // if < 1536 : length of payload else : type of payload (IPV4, ARP)
    // tags 
    unsigned char payload[];
    // crc
} __attribute__((packed));


struct eth_hdr* init_eth_hdr(char* buf);

#endif