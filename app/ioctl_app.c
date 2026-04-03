#include <fcntl.h>
#include <linux/ioctl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define CHAR_DRIVER_IOCTL_MAGIC 'k'
#define CHAR_DRIVER_IOCTL_CLEAR_MESSAGE _IO(CHAR_DRIVER_IOCTL_MAGIC, 0)

int main(void)
{
    int fd = open("/dev/char_dev", O_RDWR);

    if (fd < 0) {
        perror("open");
        return 1;
    }

    if (ioctl(fd, CHAR_DRIVER_IOCTL_CLEAR_MESSAGE) < 0) {
        perror("ioctl");
        close(fd);
        return 1;
    }

    close(fd);
    return 0;
}
