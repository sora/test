#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define FNAME "/dev/pmem"
#define ADDR  0x100000

int main (void)
{
	unsigned char buf[0xF];
	int fd, n;

	fd = open(FNAME, O_RDONLY);
	lseek(fd, 0x100000, SEEK_SET);
	n = read(fd, buf, 1);
	if (n != 1) {
		printf("err: n=%d\n", n);
		return 1;
	}

	printf("val: %02X\n", buf[0]);

	close(fd);
	return 0;
}

