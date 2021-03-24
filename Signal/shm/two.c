/*
 * shm-client - client program to demonstrate shared memory.
 */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define SHMSZ	 27

int main()
{
	int shmid;
	key_t key;
	char *shm, *s;

	/*
	 * We need to get the segment named
	 * "5679", created by the server.
	 */
	key = 5679;

	/*
	 * Locate the segment.
	 */
	if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
		perror("shmget");
		exit(1);
	}

	/*
	 * Now we attach the segment to our data space.
	 */
	if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
		perror("shmat");
		exit(1);
	}

	/*
	 * Now read what the server put in the memory.
	 */
	printf("Reading data from shared memory\n");
	for (s = shm; *s != 0; s++)
		putchar(*s);
	putchar('\n');

	sleep(5);
	/*
	 * Finally, change the first character of the 
	 * segment to '*', indicating we have read 
	 * the segment.
	 */
	printf("Writing *\n");
	*shm = '*';

	exit(0);
}

