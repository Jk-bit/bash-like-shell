
/*  
 *  A small bug is present 
 *	- after stopping several processes when you run fg/bg it works
 *	fine for the first time, but when you try to run it for the second time
 *	it sucks(kills the process)
 *
 */


#include "defs.h"
#include "errno.h"


void handler(int x);

int main(){
    //Setting the path in the environment
    char env[] = "PATH=/bin:/usr/bin:/snap/bin";
    putenv(env);
    int pfd[2];
    size_t n = 0;
    char *cmd;
    char **argv;
    char *try;
    int redirect_flag;
    pipe(pfd);
    signal_init();
    int pid;
    llist_init(&stopped_jobs);
    int debug;
    while(1){
	//TODO print the cwd
	ischildprocess = 0;
	printf("prompt>");
	/* getline in its definition reallocs if the given size is less to the buffer (check man getline) 
	 * the getline reads also the \n from the line so substituing the \n as \0
	 * */
	ssize_t size_of_comm = getline(&try, &n, stdin);
	try[size_of_comm - 1] = '\0';
	if((pid = isfgbg(try)) != 0){
	    /*if(fg_proc){
		waitpid(pid, &debug, WUNTRACED);
	    }
	    fg_proc = 0;
	    ischildprocess = 0;
	    bg_proc = 0;*/
	    continue;
	}
	/*else if(isbg(try)){
	    continue;
	}*/

	backgroundproc = isbackgroundproc(&try);
	/* read(0, try, NO_OF_ARG * WORD_SIZE); */
	if(strchr(try, '|') != NULL){
	    pipe_handling(&try, size_of_comm);
	    continue;
	
	}
	else if(strchr(try, '>') != NULL || strchr(try, '<')){
	    redirect_handling(&try, size_of_comm);
	    continue;
	    //redirect_flag = 1;
	}
	argv = parse(&try, size_of_comm);
	int i = 0;
	char *ptr = argv[i++];
	//call a function which returns the path where the cmd is present
	ischildprocess = 1;
	fork_ret = fork();
	if(fork_ret == 0){
	    signal_child();
	    if(execvp(argv[0], argv) == -1){
		printf("%s : No such command\n", argv[0]);
		kill(getpid(), SIGSTOP);
	    }
	}
	else{
	    //TODO change this to every parent
	    if(backgroundproc){
		backgroundproc = 0;
		continue;
	    }
	    waitid(P_PID, fork_ret, NULL, WEXITED | WSTOPPED);
	}
    } 
    return 0;
}
/* 
 * @breif parse function parses the string as space and tab as the tokens
 * @param try pointer to the string which has the input from the shell
 * @retval vector of pointers having all the tokens and null terminated
 * TODO including the quotes 
 * */
char **parse(char **try, int size_of_comm){
    /* @ptr pointer to the string (command)*/
    char *ptr = *try;
    /* 
     * @word : every word delimeted by " " || "\t" || "|" || "<"
     * */
    char *word = (char *)malloc(sizeof(char)*WORD_SIZE);
    /* 
     * @argv vector of arguments terminated by NULL
     * */
    char **argv = (char **)malloc(sizeof(char)*NO_OF_ARG);
    int index_word = 0, flag, index_ptr = 0, index_argv = 0;
    while(1){
	if(ptr[index_ptr] == '\"'){
	    index_ptr++;
	    while(ptr[index_ptr] != '\"'){
		word[index_word++] = ptr[index_ptr++];
	    }
	    index_ptr++;

	}
	else if(ptr[index_ptr] == ' ' || ptr[index_ptr] == '\t'){
	    if(flag){
		index_ptr++;
		continue;
	    }
	    else{

		word[index_word] = '\0';
		argv[index_argv] = (char *)malloc(sizeof(char)*strlen(word));
		strcpy(argv[index_argv], word);
		//printf("%s in word\n", word);
		index_argv++;
		index_word = 0;
		flag = 1;
	    }
	}
	else if(ptr[index_ptr] == '\0'){
	    break;
	}
	else{
	    word[index_word] = ptr[index_ptr];
	    index_word++;
	    index_ptr++;
	    flag = 0;
	}
    }
    if(index_word){
	word[index_word] = '\0';
	argv[index_argv] = (char *)malloc(sizeof(char)*strlen(word));
	strcpy(argv[index_argv], word);
	index_argv++;
    }
    argv[index_argv++] = NULL;
    return argv;
}
/* @breif handles the piping
 * @param try : pointer to string (command) typed by the user
 * @param index_ptr : index till which the func parse has parsed
 * @param argv1 : argv contains pointer to the string containing to left of the pipe
 * @param size_of_comm : size_of_comm tells the length of the string // this wont be
 * required I guess
 * */
void pipe_handling(char **try, int size_of_comm){
    /* 
     * fd is the pipe
     * temp_fd is the temporary file desc
     * pid is the process id returned by the fork
     * */
    int fd[2], temp_fd, pid;
    // char ptr is pointed to the command --> sorry for the var_name
    char *ptr = *try;
    //char ptr[100] = "ls | grep";
    // index_ptr is the index for the ptr and index_command is the index for the string
    // command
    int index_ptr = 0, index_command = 0;
    // each command in the pipe handling command
    char *command = (char *)malloc(sizeof(char) * size_of_comm);
    char **process;
    /* 
     * creating a pipe() in each loop and the reading side of command(fd[0])
     * is stored in temp_fd which is then dup2() to 0 in next iteration
     * */
    while(1){
	//printf("%c", ptr[index_ptr]);
	// tokenizing the string with delimeter | 
	if(ptr[index_ptr] == '|' || ptr[index_ptr] == '\0'){
	    command[index_command] = '\0';
	    if(strchr(command, '>') != NULL || strchr(command, '<') != NULL){
		pipe(fd);
		fork_ret = fork();
		if(fork_ret == 0){
		    signal_child();
		    dup2(temp_fd, 0);
		    close(fd[0]);
		    if(ptr[index_ptr] != '\0'){
			close(1);
			dup2(fd[1], 1);
		    }
		    //redirect_handling(&command, strlen(command));
		    char *ptr = command;
		    //printf("%s\n", ptr);
		    int index_ptr, index_process, index_filename;
		    /*char *process = (char *)malloc(sizeof(char) * size_of_comm);
		    char *filename = (char *)malloc(sizeof(char) * size_of_comm); */
		    char output_redir = '>';
		    char input_redir = '<'; 
		    if(strchr(command, output_redir) != NULL){
			char *process = strtok(ptr, &output_redir);
			char *output_file = strtok(NULL, &output_redir);
			output_file = clean_string(output_file);
			char **argv = parse(&process, size_of_comm);
			close(1);
			int fd = open(output_file, O_WRONLY | O_RDONLY | O_CREAT , S_IWUSR | S_IWGRP | S_IWOTH | S_IRUSR | S_IRGRP | S_IROTH);
			execvp(argv[0], argv);
		    }
		    else{
			char *process = strtok(ptr, &input_redir);
			char *input_file = strtok(NULL, &input_redir);
			input_file = clean_string(input_file);
			char **argv = parse(&process, size_of_comm);
			close(0);
			int fd = open(input_file, O_WRONLY | O_RDONLY | O_CREAT , S_IWUSR | S_IWGRP | S_IWOTH | S_IRUSR | S_IRGRP | S_IROTH);
			execvp(argv[0], argv);
	
		    }
		}
		else{
		    if(!backgroundproc)
			wait(0);
		    backgroundproc = 0;
		    close(fd[1]);
		    temp_fd = fd[0];
		}
	    }
	    else{
		process = parse(&command, strlen(command));
		pipe(fd);
		fork_ret = fork();
		if(fork_ret == 0){
		    signal_child();
		    dup2(temp_fd, 0);
		    close(fd[0]);
		    if(ptr[index_ptr] != '\0'){
			close(1);
			dup2(fd[1], 1);
		    }
		    execvp(process[0], process);
		}	
		else{
		    if(!backgroundproc)
			waitpid(fork_ret, 0, WUNTRACED);
		    backgroundproc = 0;	
		    close(fd[1]);
		    temp_fd = fd[0];
		}

	    }
	    index_command = 0;
	    
	    if(ptr[index_ptr] == '\0'){
		break;
	    }
	    index_ptr++;
	}
	else{
	    command[index_command++] = ptr[index_ptr++];
	}
    }

}

/* 
 * @breif redirection handling handles the output as well as input 
 *	  redirection
 * @param try : is the input command (sorry for the very bad variable name)
 * @param size_of_comm : size of the try command (not needed)
 * */
void redirect_handling(char **try, int size_of_comm){
    char *ptr = *try;
    //printf("%s\n", ptr);
    int index_ptr, index_process, index_filename;
    /*char *process = (char *)malloc(sizeof(char) * size_of_comm);
    char *filename = (char *)malloc(sizeof(char) * size_of_comm); */
    char output_redir = '>';
    char input_redir = '<'; 
    if(strchr(*try, output_redir) != NULL){
	char *process = strtok(ptr, &output_redir);
	char *output_file = strtok(NULL, &output_redir);
	output_file = clean_string(output_file);
	char **argv = parse(&process, size_of_comm);
	int fork_ret = fork();
	if(fork_ret == 0){
	    signal_child();
	    close(1);
	    int fd = open(output_file, O_WRONLY | O_RDONLY | O_CREAT , S_IWUSR | S_IWGRP | S_IWOTH | S_IRUSR | S_IRGRP | S_IROTH);
	    execvp(argv[0], argv);
	    //dup2(fd, 4);
	    //printf("%d\n", fd);
	    //close(fd);
	}
	else{
	    if(!backgroundproc){
		waitpid(fork_ret, 0, WUNTRACED);
	    }
	    backgroundproc = 0;
	    //printf("%d\n", fd);
	    //close(fd);
	} 
    }   
    else{
	char *process = strtok(ptr, &input_redir);
	char *input_file = strtok(NULL, &input_redir);
	input_file = clean_string(input_file);
	char **argv = parse(&process, size_of_comm);
	int fork_ret = fork();
	if(fork_ret == 0){
	    signal_child();
	    close(0);
	    int fd = open(input_file, O_WRONLY | O_RDONLY | O_CREAT , S_IWUSR | S_IWGRP | S_IWOTH | S_IRUSR | S_IRGRP | S_IROTH);
	    //dup2(fd, 0);
	    execvp(argv[0], argv);
	    /*dup(fd);
	    printf("%d\n", fd);
	    close(fd); */
	
	}
	else{
	    if(!backgroundproc){
		waitpid(fork_ret, 0, WUNTRACED);
	    }
	    backgroundproc = 0;
	}
    }
}

/* 
 * @breif cleans the string in the sense removes the whitespace
 *	  characters from the string(from the start and the end)
 * @param string : string to be cleansed
* */
char *clean_string(char *string){
    while(string[0] == ' ' || string[0] == '\t'){
	string = string + 1;

    }
    while(string[strlen(string) - 1] == ' '){
	string[strlen(string) - 1] = '\0';
    }
    return string;
}
/*
 * @breif initializes the signal with the respective handlers and
 *	  flags.
 */
void signal_init(){
    struct sigaction sigint, sigtstp;
    sigint.sa_handler = int_handler;
    sigtstp.sa_handler = stp_handler;
    sigtstp.sa_flags = SA_RESTART;
    sigint.sa_flags =  SA_RESTART;
    sigemptyset(&sigtstp.sa_mask);
    sigemptyset(&sigint.sa_mask);
    sigaction(SIGINT, &sigint, NULL);
    sigaction(SIGTSTP, &sigtstp, NULL);

}


/*
 * @breif Signal handler for Ctrl-C sends the signal to the child
 *	  if there is no child process then we continue
 */
void int_handler(int x){
    if(!ischildprocess){
	printf("\nprompt>");
	fflush(stdout);
	return;
    }
    llist_remove(&stopped_jobs, 0, fork_ret);
    kill(fork_ret, SIGKILL);
    waitpid(fork_ret, 0, WEXITED | WSTOPPED);
    printf("\n");
    return;
}

/*
 * @breif Signal handler for Ctrl-Z sends the signal to the child
 *	  inserts the process in the linked list so that we can
 *	  continue later with bg/fg command
 */
void stp_handler(int x){
    if(!ischildprocess){
	printf("\nprompt>");
	fflush(stdout);
	return;
    }
    stopped_jobs_counter = llist_insert(&stopped_jobs, fork_ret);
    printf("\n[");
    printf("%d", stopped_jobs_counter);
    printf("] + Stopped\n");    
    kill(fork_ret, SIGTSTP);
    fflush(stdout);    
}


/*
 * @breif Called in child to initialize the signl hnandler to default
 */
void signal_child(){
    struct sigaction sigint, sigtstp;
    sigint.sa_handler = SIG_DFL;
    sigtstp.sa_handler = SIG_DFL;
    sigint.sa_flags = sigtstp.sa_flags = SA_RESTART;
    sigemptyset(&sigint.sa_mask);
    sigemptyset(&sigtstp.sa_mask);
    sigaction(SIGKILL, &sigint, NULL);
    sigaction(SIGTSTP, &sigtstp, NULL);
    sigaction(SIGINT, &sigint, NULL);
}


/*
* @breif   Checks if the the command has to be execute in background 
 *	    i.e. if there & at the end of command, and remove the '&'
 *	    from the end.
 * @param str: pointer to the string(command)
 * @retval  true if it is bg process otherwise false
 */
bool isbackgroundproc(char **str){
    char *try = *str;
    int i = strlen(try);
    i--;
    for(int j = i; j >= 0; i--){
	if(try[j] == ' ' || try[j] == '\t')
	    continue;
	else if(try[j] == '&'){
	    try[j] = '\0';
	    return 1;
	}
	else
	    return 0;
    }
    return 0;
}

/*
 *  @breif  : the function checks if the command entered is fg/bg 
 *	      and set the flag accordingly
 *  @param try : the string(command)
 *  @retval : If it is fg/bg command then returns pid of the respective
 *	      process or returns 0 otherwise.
 */
int isfgbg(char *try){
    int pos, debug;
    int i;
    pid_t pid;
    try = clean_string(try);
    if((try[0] == 'f' || try[0] == 'b') && try[1] == 'g'){
	if(try[0] == 'f'){
	    fg_proc = 1;
	}
	else{
	    bg_proc = 1;
	}
	//code to generate the job_number from the command
	i = strlen(try) - 1;
	while(1){
	    if(48 <= try[i] && try[i] <= 57){
		i = i - 1;
	    }
	    else{
		break;
	    }
	}
	if(i != (strlen(try) - 1)){
	    pos = atoi(&try[i+1]);
	    if((pid =  llist_remove(&stopped_jobs, pos, 0)) != 0){
		ischildprocess = 1;
		fork_ret = pid;
		kill(pid, SIGCONT);
	        if(fg_proc){
		    fg_proc = 0;
		    waitpid(pid, &debug, WUNTRACED);
		    ischildprocess = 0;
	        }
		bg_proc = 0;
	        return pid;
	    }
	    else{
		printf("fg / bg : %d : No such job\n", pos);
		return 1;

	    }
	}
    }	    
    bg_proc = 0;
    return 0;
}
