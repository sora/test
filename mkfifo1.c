#include <stdio.h>
#include <fcntl.h>
#include <string.h>

/* pipe(2) */
#include <unistd.h>

/* mkfifo(2) */
#include <sys/types.h>
#include <sys/stat.h>

/*
 * push
 */
void push(int fd)
{
	char *data = "hoge\n";
	int ret;

	ret = write(fd, data, strlen(data));
	if (ret < 0) {
		perror("write");
	}
}

/*
 * pop
 */
void pop(int fd)
{
	char buf[0xFF];
	int ret;

	ret = read(fd, buf, 0xFF);
	if (ret < 0) {
		perror("read");
	}

	printf("output: %s", buf);
}

/*
 * main
 */
int main(int argc, char **argv)
{
	int fd, i;
	char *fifo0 = "/tmp/myfifo";

	mkfifo(fifo0, 0666);
	fd = open(fifo0, O_RDWR);

	for (i = 0; i < 10; i++) {
		push(fd);
		pop(fd);
	}

	close(fd);
	unlink(fifo0);

	return 0;
}

