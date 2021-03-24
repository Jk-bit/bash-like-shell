#include <signal.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

void inthandler(int);

int main(){
    printf("main\n");
    struct sigaction new;
    new.sa_handler = inthandler;
    sigemptyset(&new.sa_mask);
    printf("main");
    sigaction(SIGINT, &new, NULL);
    printf("main>");
    fflush(stdout);
    while(1);
    return 0;
}

void inthandler(int x){
    printf("SIGINT received");
    fflush(stdout);
}

