#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <linux/if_ether.h>

#define CLEAR(x) memset(&(x), 0, sizeof(x))

int tun_alloc(char *dev);
ssize_t tun_read(char *buf, int BUFLEN);
ssize_t tun_write(char *buf, int BUFLEN);

int main()
{
    printf("here\n");
    char buf[2048];
    ssize_t nread = tun_read(buf, sizeof(buf));
    if (nread > 0)
        printf("ethernet frame read : %s\n", buf);
    else
        printf("no frames read");

    return 0;
}

int tun_alloc(char *dev)
{
    struct ifreq ifr;
    int fd, err;

    if ((fd = open("/dev/net/tun", O_RDWR)) < 0)
    {
        printf("Cannot open TUN/TAP dev\n");
        exit(1);
    }

    CLEAR(ifr);

    /* Flags: IFF_TUN   - TUN device (no Ethernet headers)
     *        IFF_TAP   - TAP device
     *
     *        IFF_NO_PI - Do not provide packet information
     */
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    if (*dev)
    {
        strncpy(ifr.ifr_name, dev, IFNAMSIZ);
    }

    if ((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0)
    {
        printf("ERR: Could not ioctl tun: %s\n", strerror(errno));
        close(fd);
        return err;
    }

    strcpy(dev, ifr.ifr_name);
    return fd;
}

ssize_t tun_read(char *buf, int BUFLEN)
{
    char dev[sizeof(((struct ifreq *)0)->ifr_name)] = "";
    int fd = tun_alloc(dev);
    printf("fd : %i\ndev: %s\n", fd, dev);

    if (fd < 0)
    {
        printf("Error: Could not allocate TAP device\n");
        return -1;
    }

    fcntl(fd, F_SETFL, O_NONBLOCK);

    ssize_t nread;

    while (1)
    {
        nread = read(fd, buf, BUFLEN);
        printf("nread : %d\n", nread);
        if (nread > 0) printf("recived %d bytes\n", nread);
        else if (nread < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
        {
            perror("Error reading from TAP device");
            break;
        } 
        printf("no data available, retrying...\n");
        usleep(100000);
    }

    return nread;
}

ssize_t tun_write(char *buf, int BUFLEN)
{
    char dev[sizeof(((struct ifreq *)0)->ifr_name)] = "";
    int fd = tun_alloc(dev);

    printf("fd : %i\ndev: %s\n", fd, dev);

    if (fd < 0)
    {
        printf("Error: Could not allocate TAP device\n");
        return -1;
    }

    // fcntl(fd, F_SETFL, O_NONBLOCK); 

    return write(fd, buf, BUFLEN);
}