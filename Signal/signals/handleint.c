#include <signal.h>
#include <stdio.h>
#include <unistd.h>
void inthandler(int x) {
	printf("SIGINT received\n");
}
int main() {
	printf("main");
	struct sigaction new, old;
	new.sa_handler = inthandler;	
	sigemptyset(&new.sa_mask);
	sigaction(SIGINT, &new, NULL);
	while(1) 
		sleep(1);
}
