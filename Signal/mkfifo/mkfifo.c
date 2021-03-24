#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
int main(int argc, char *argv[]) {
	int fd, fd2;
	char ch[1600];	

	if(mkfifo(argv[1], S_IRUSR | S_IWUSR) != 0)  {
		perror("mkfifo failed");
		return errno;
	}
	printf("fifo opened\n");
	fd = open(argv[1], O_WRONLY, S_IRUSR | S_IWUSR);
	if(fd == -1) {
		perror("open:");
		exit(errno);
	}
	printf("file opened\n");
	write(fd, "Hello", 6);
}
