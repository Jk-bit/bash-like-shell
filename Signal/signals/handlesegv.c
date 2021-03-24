#include <signal.h>
#include <stdio.h>
#include <unistd.h>
void seghandler(int x) {
	printf("SIGSEGV received\n");
}
int main() {
	printf("Hi");
	struct sigaction new, old;
	int *p = 1234;
	new.sa_handler = seghandler;	
	sigemptyset(&new.sa_mask);
	sigaction(SIGSEGV, &new, NULL);
	*p = 100;
	
}
