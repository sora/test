#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define FNAME "/dev/pmem"
#define ADDR  0x100000
#define VAL 0xAA

int main (void)
{
	unsigned char buf[0xF];
	int fd, val = VAL;
	size_t n;

	fd = open(FNAME, O_WRONLY);
	lseek(fd, 0x100000, SEEK_SET);
	n = write(fd, &val, 1);
	if (n != 1) {
		printf("err: n=%d\n", (int)n);
		return 1;
	}

	printf("val: %02X\n", buf[0]);

	close(fd);
	return 0;
}

