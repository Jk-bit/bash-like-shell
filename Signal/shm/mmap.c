#include<stdio.h>
#include<sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
int main(int argc, char *argv[]) {
	int fd, i;
	//int pgsize = sysconf(_SC_PAGE_SIZE);
	char *data;
	fd = open(argv[1], O_RDONLY);
	if(fd == -1) {
		perror("open failed");
		exit(EBADF);
	}
	//printf("page size = %d\n", pgsize);
	data = mmap(NULL, 4096, PROT_READ, MAP_SHARED, fd, 0);
	if(data == NULL)
		return ENOMEM;
	for(i = 0; i < 100; i++)
		putchar(data[i]);
	munmap(data, 1024);
	return 0;
}
