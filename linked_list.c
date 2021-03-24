#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "linked_list.h"

void llist_init(linked_list *head){
    *head = NULL;
}

int llist_insert(linked_list *head, pid_t pid){
    linked_list temp = *head;
    if(temp == NULL){
	(*head) = (node *)malloc(sizeof(node));
	(*head)->job_no = 1;
	(*head)->pid = pid;
	(*head)->next = NULL;
	return 1;
    }
    while(temp->next != NULL){
	temp = temp->next;
    }
    temp->next = (node *)malloc(sizeof(node));
    temp->next->job_no = (temp->job_no) + 1;
    temp = temp->next;
    temp->pid = pid;
    temp->next = NULL;
    return temp->job_no;
}


/* 
 * @breif removes the node at the given position from tyhe given linked list
 * #param1 : pointer to the head of the linked list
 * @param2 : position to remove the node
 * @retval : -1 if there is no such position 0 on success
 */
pid_t llist_remove(linked_list *head, int job_no, int pid){
    pid_t p;
    linked_list temp = *head;
    if(temp == NULL){
	return 0;
    }
    if(job_no != 0){
	if(temp->job_no == job_no){
	    p = temp->pid;
	    (*head) = temp->next;
	    return p;
	}
	linked_list ptr;
	while(1){
	    if(temp->next == NULL){
		return 0;
	    }
	    if(temp->next->job_no == job_no){
		p = temp->next->pid;
		temp->next = temp->next->next;
		return p;
	    }
	    temp = temp->next;
	}
    }
    else if(pid != 0){
	if(temp->pid == pid){
	    p = temp->pid;
	    (*head) = temp->next;
	    return pid;
	}
	linked_list ptr;
	while(1){
	    if(temp->next == NULL){
		return 0;
	    }
	    if(temp->next->pid == pid){
		p = temp->next->pid;
		temp->next = temp->next->next;
		return p;
	    }
	    temp = temp->next;
	}

    }


}
