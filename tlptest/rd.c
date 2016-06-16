#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define FNAME   "/dev/pmem"
#define ADDR    0xc0004000
#define RDSIZE  4

int main (void)
{
	unsigned char buf[0xF];
	int fd, n;

	fd = open(FNAME, O_RDONLY);
	lseek(fd, ADDR, SEEK_SET);
	n = read(fd, buf, RDSIZE);
	if (n != RDSIZE) {
		printf("err: n=%d\n", n);
		return 1;
	}

	printf("val: %02X %02X %02X %02X\n", buf[0], buf[1], buf[2], buf[3]);

	close(fd);
	return 0;
}

