#include <sys/types.h>
#include <sys/wait.h>
#include <stdint.h>

typedef struct node{
    pid_t pid;  
    int job_no;
    struct node *next;  
}node;

typedef node* linked_list;

void llist_init(linked_list *head);

int llist_insert(linked_list *head, pid_t pid);

pid_t llist_remove(linked_list *head, int job_no, int pid);


