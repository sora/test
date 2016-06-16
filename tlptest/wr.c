#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define FNAME   "/dev/pmem"
#define ADDR    0xc0004000
#define VAL     0xAABBCCDD
#define WRSIZE  4

int main (void)
{
	int fd, val = VAL;
	size_t n;

	fd = open(FNAME, O_WRONLY);
	lseek(fd, ADDR, SEEK_SET);
	n = write(fd, &val, WRSIZE);
	if (n != WRSIZE) {
		printf("err: n=%d\n", (int)n);
		return 1;
	}

	close(fd);
	return 0;
}

