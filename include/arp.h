#ifndef ARP_H
#define ARP_H
#include "syshead.h"
#include "ethernet.h"
#include "netdev.h"

#define ARP_ETHERNET 0x0001
#define ARP_IPV4 0x0800
#define ARP_REQUEST 0x0001
#define ARP_REPLY 0x0002

#define ARP_CACHE_LEN 32
#define ARP_FREE 0
#define ARP_WAITING 1
#define ARP_RESOLVED 2

struct arp_hdr
{
    uint16_t hw_type; // link layer type used (ethernet)
    uint16_t protype; // protocol type (IPV4)
    unsigned char hwsize;
    unsigned char prosize;
    uint16_t opcode; // type of ARP message
    unsigned char data[];
} __attribute__((packed));

struct arp_ipv4
{
    unsigned char smac[6];
    uint32_t sip;
    unsigned char dmac[6];
    uint32_t dip;
} __attribute__((packed));

struct arp_cache_entry
{
    uint16_t hwtype;
    uint32_t sip;
    unsigned char smac[6];
    unsigned int state; // free , waiting , resolved
};

void arp_init();
bool check_translation_table(struct arp_hdr *hdr);
bool insert_translation_table(struct arp_hdr *hdr);
void arp_reply(struct netdev *netdev, struct eth_hdr *hdr);
void recv_arp(struct netdev *netdev, struct eth_hdr *hdr);


#endif