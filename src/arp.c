#include "arp.h"

#define CLEAR(x) memset(&(x), 0, sizeof(x))

bool merge_flag;

static struct arp_cache_entry arp_cache[ARP_CACHE_LEN];

void arp_init()
{
    memset(arp_cache, 0, ARP_CACHE_LEN * sizeof(struct arp_cache_entry));
}

bool check_translation_table(struct arp_hdr *hdr)
{
    struct arp_cache_entry *entry;
    struct arp_ipv4 *data = (struct arp_ipv4 *)hdr->data;
    uint16_t hwtype = hdr->hw_type;

    for (int i = 0; i < ARP_CACHE_LEN; i++)
    {
        entry = &arp_cache[i];
        if (entry->state == ARP_FREE)
            continue;

        if (entry->hwtype == hdr->hw_type && entry->sip == data->sip)
        {
            memcpy(entry->smac, data->smac, 6);
            return true;
        }
    }
    return false;
}

bool insert_translation_table(struct arp_hdr *hdr)
{
    struct arp_cache_entry *entry = NULL;
    struct arp_ipv4 *data = (struct arp_ipv4 *)hdr->data;
    uint16_t hwtype = hdr->hw_type;

    for (int i = 0; i < ARP_CACHE_LEN; i++)
    {
        entry = &arp_cache[i];
        if (entry->state == ARP_FREE)
        {
            entry->state = ARP_RESOLVED;
            entry->hwtype = hdr->hw_type;
            entry->sip = data->sip;
            memcpy(entry->smac, data->smac, sizeof(data->smac));
            return true;
        }
    }
    return false;
}

void arp_reply(struct netdev *netdev, struct eth_hdr *hdr)
{
    struct arp_hdr *arphdr;
    struct arp_ipv4 *arpdata;
    arphdr = (struct arp_hdr *)hdr->payload;
    arpdata = (struct arp_ipv4 *)arphdr->data;

    arphdr->hw_type = htons(arphdr->hw_type);
    arphdr->protype = htons(arphdr->protype);
    arphdr->opcode = htons(arphdr->opcode);

    int len = sizeof(struct arp_hdr) + sizeof(struct arp_ipv4);

    netdev_transmit(netdev, hdr, ETH_P_ARP, len, arpdata->dmac);
}

void recv_arp(struct netdev *netdev, struct eth_hdr *hdr)
{

    struct arp_hdr *arphdr;
    struct arp_ipv4 *arpdata;

    arphdr = (struct arp_hdr *)hdr->payload;
    arphdr->hw_type = ntohs(arphdr->hw_type);
    arphdr->protype = ntohs(arphdr->protype);
    arphdr->opcode = ntohs(arphdr->opcode);

    if (arphdr->hw_type != ARP_ETHERNET)
    {
        printf("unsupported HW type.\n");
        return;
    }
    // optionally check the HW length here

    if (arphdr->protype != ARP_IPV4)
    {
        printf("unsupported protocol.\n");
        return;
    }
    // optionally check the protocol length here

    arpdata = (struct arp_ipv4 *)arphdr->data;

    if (arpdata == NULL)
    {
        printf("[ERR] arpdata is NULL!\n");
        return;
    }

    merge_flag = false;
    merge_flag = check_translation_table(arphdr);

    if (netdev->addr != arpdata->dip)
        printf("we are not the target protocol address.\n");

    if (!merge_flag)
        if (!insert_translation_table(arphdr))
            printf("[ERR] No free space in ARP translation table!\n");

    if (arphdr->opcode == ARP_REQUEST)
    {
        memcpy(arpdata->dmac, arpdata->smac, 6); 
        arpdata->dip = arpdata->sip;             
        memcpy(arpdata->smac, netdev->hwaddr, 6); 
        arpdata->sip = netdev->addr;
        arphdr->opcode = ARP_REPLY;
        arp_reply(netdev, hdr);
        // handle errors
    }
}
