#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdbool.h>
#include "linked_list.h"
#define PATHS 10
#define NO_OF_ARG 1000
#define WORD_SIZE 100

char **parse(char **, int );

void pipe_handling(char **, int );

void redirect_handling(char **, int);

char *clean_string(char *);

void signal_init(void);

void signal_child();

void child_int_handler(int);

void int_handler(int);

int fork_ret;

bool backgroundproc = 0;

bool ischildprocess = 0;

bool fg_proc = 0;

bool bg_proc = 0;

void stp_handler(int);

bool isbackgroundproc(char**);

int isfgbg(char *);

int isbg(char *);

linked_list stopped_jobs;
int stopped_jobs_counter = 0;

