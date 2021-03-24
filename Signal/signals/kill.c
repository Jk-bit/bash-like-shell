#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
int main(int argc, char *argv[]) {
	int pid, signo;
	pid = atoi(argv[1]);
	signo = atoi(argv[2]);
	kill(pid, signo);
	return 0;
}

