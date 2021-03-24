#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

void handle_sigint(int sig) 
{ 
    printf("Caught signal %d\n", sig); 
} 
  
int main() 
{ 
    printf("main");
    struct sigaction inthandler;
    inthandler.sa_handler = handle_sigint;
    sigemptyset(&inthandler.sa_mask);

    sigaction(SIGINT, &inthandler, NULL);
    fflush(stdout);
    while (1) ; 
    return 0; 
} 
