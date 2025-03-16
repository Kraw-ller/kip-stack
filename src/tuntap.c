#include "tuntap.h"
#include "utils.h"
#include "syshead.h"

#define CLEAR(x) memset(&(x), 0, sizeof(x))

static int tun_fd;

int tun_alloc(char *dev)
{
    struct ifreq ifr;
    int fd, err;

    if ((tun_fd = open("/dev/net/tun", O_RDWR)) < 0)
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

    if ((err = ioctl(tun_fd, TUNSETIFF, (void *)&ifr)) < 0)
    {
        printf("ERR: Could not ioctl tun: %s\n", strerror(errno));
        // close(tun_fd);
        return err;
    }

    strcpy(dev, ifr.ifr_name);
    return tun_fd;
}

ssize_t tun_read(char *buf, int BUFLEN)
{
    int fd;
    if (tun_fd < 0)
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
        tun_fd = fd;
    }

    ssize_t nread;

    nread = read(tun_fd, buf, BUFLEN);
    printf("nread : %d\n", nread);
    if (nread > 0)
        printf("recived %d bytes\n", nread);
    else if (nread < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
    {
        perror("Error reading from TAP device");
        return nread;
    }
    printf("no data available, retrying...\n");
    usleep(100000);

    return nread;
}

ssize_t tun_write(char *buf, int BUFLEN)
{
    int fd;
    if (tun_fd < 0)
    {

        char dev[sizeof(((struct ifreq *)0)->ifr_name)] = "";
        fd = tun_alloc(dev);

        printf("fd : %i\ndev: %s\n", fd, dev);

        if (fd < 0)
        {
            printf("Error: Could not allocate TAP device\n");
            return -1;
        }
        tun_fd = fd;
    }

    // fcntl(fd, F_SETFL, O_NONBLOCK);

    return write(tun_fd, buf, BUFLEN);
}

static int set_if_route(char *dev, char *cidr)
{
    return run_cmd("ip route add dev %s %s", dev, cidr);
}

static int set_if_up(char *dev)
{
    return run_cmd("ip link set dev %s up", dev);
}

void tun_init(char *dev)
{
    tun_fd = tun_alloc(dev);

    if (set_if_up(dev) != 0)
    {
        printf("ERROR when setting up if\n");
    }

    if (set_if_route(dev, "10.0.0.0/24") != 0)
    {
        printf("ERROR when setting route for if\n");
    }
}