#include "syshead.h"
#include "tuntap.h"
#include "utils.h"
#include "ethernet.h"
#include "arp.h"
#include "netdev.h"

#define BUFLEN 100
#define CLEAR(x) memset(&(x), 0, sizeof(x))

#define NETDEV_IP "10.0.0.4"
#define NETDEV_MAC "00:0c:29:6d:50:25"

int main()
{
    char buf[BUFLEN];
    char *dev = calloc(10, 1);
    struct netdev netdev;
    struct eth_hdr *ethhdr;

    CLEAR(buf);
    netdev_init(&netdev, NETDEV_IP, NETDEV_MAC);
    arp_init();
    tun_init(dev);

    while(1)
    {
        ssize_t nread = tun_read(buf, sizeof(buf));
        if (nread > 0)
        {
       
            printf("ethernet frame read : %s\n", buf);
            ethhdr = (struct eth_hdr*) buf;
            recv_arp(&netdev, ethhdr);
        }
        else
        printf("no frames read");
    }
    return 0;
}


// we had to run sudo sysctl -w net.ipv4.conf.tap0.arp_accept=1 to make the system add us to the arp cache
