#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
int main(int argc, char *argv[]) {
	int fd, fd2;
	char string[1600];	

	fd = open(argv[1], O_RDONLY);
	if(fd == -1) {
		perror("open:");
		exit(errno);
	}
	read(fd, string, 6);
	printf("%s\n", string);
}
