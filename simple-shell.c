/**********************************************************************************************************************************************************
 * Simple UNIX Shell
 * @author: 
 * 
 **/


#include <stdio.h>
#include <stdlib.h>   
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LENGTH 80 	     // The maximum length of the commands
#define MAX_LENGTH_PARAMS 41 // The maximum length of the args
#define MAX_HISTORY 10


int parse_cmd(char* cmd, char** params);
int prompt_cmd(char* cmd);
int execute_cmd(char **params);

int main(void) {
	char command[MAX_LENGTH];
	char *args[MAX_LENGTH_PARAMS]; 
	int should_run = 1;
	
	int ampersand = 0;

	while (should_run) {
		if (!prompt_cmd(command)) break;  //Prompt type

		int argc = parse_cmd(command, args); //Parse command and arguments	
		if (strcmp(args[0], "exit") == 0) break;

		if (strcmp(args[argc-1], "&") == 0) { //check ampersand
			ampersand = 1;
			args[--argc] = NULL; //remove ampersand
		}

		//If command contains output redirection argument
		//	fork a child process invoking fork() system call and perform the followings in the child process:
		//		open the redirected file in write only mode invoking open() system call
		//		copy the opened file descriptor to standard output file descriptor (STDOUT_FILENO) invoking dup2() system call
		//		close the opened file descriptor invoking close() system call
		//		change the process image with the new process image according to the UNIX command using execvp() system call
		//	If command does not conatain & (ampersand) at the end
		//		invoke wait() system call in parent process.
		//
		//		
		//If command contains input redirection argument
		//	fork a child process invoking fork() system call and perform the followings in the child process:
		//		open the redirected file in read  only mode invoking open() system call
		//		copy the opened file descriptor to standard input file descriptor (STDIN_FILENO) invoking dup2() system call
		//		close the opened file descriptor invoking close() system call
		//		change the process image with the new process image according to the UNIX command using execvp() system call
		//	If command does not conatain & (ampersand) at the end
		//		invoke wait() system call in parent process.
		//
		//	
		
		//If command contains pipe argument
		//	fork a child process invoking fork() system call and perform the followings in the child process:
		//		create a pipe invoking pipe() system call
		//		fork another child process invoking fork() system call and perform the followings in this child process:
		//			close the write end descriptor of the pipe invoking close() system call
		//			copy the read end  descriptor of the pipe to standard input file descriptor (STDIN_FILENO) invoking dup2() system call
		//			change the process image of the this child with the new image according to the second UNIX command after the pipe symbol (|) using execvp() system call
		//		close the read end descriptor of the pipe invoking close() system call
		//		copy the write end descriptor of the pipe to standard output file descriptor (STDOUT_FILENO) invoking dup2() system call
		//		change the process image with the new process image according to the first UNIX command before the pipe symbol (|) using execvp() system call
		//	If command does not conatain & (ampersand) at the end
		//		invoke wait() system call in parent process.
		//
		//
		//If command does not contain any of the above
		//	fork a child process using fork() system call and perform the followings in the child process.
		//		change the process image with the new process image according to the UNIX command using execvp() system call
		//	If command does not conatain & (ampersand) at the end
		//		invoke wait() system call in parent process.
		if (execute_cmd(args) == 0) break;
	}

	return 0;
}

int prompt_cmd(char* cmd) {
	static int first_time = 1;
	if (first_time) { //clear screen
		const char* CLEAR_SCREEN = "\e[1;1H\e[2J";
		write(STDOUT_FILENO, CLEAR_SCREEN, 12);
		first_time = 0;
	}

	printf("ssh>>");
	fflush(stdout);
	if (fgets(cmd, sizeof(cmd), stdin) == NULL) return 0;
	if(cmd[strlen(cmd)-1] == '\n')  
		cmd[strlen(cmd)-1] = '\0'; //remove newline char    
	return 1;
}

int parse_cmd(char* cmd, char** params) {
	int i,n=-1;
	for(i=0; i<MAX_LENGTH_PARAMS; i++) {
		params[i] = strsep(&cmd, " ");
		n++;
		if(params[i] == NULL) break;
	}	
	return n;
};

int execute_cmd(char **params) {
	pid_t pid = fork(); //fork process

	if (pid == -1) { //error
		char *error = strerror(errno);
		printf("error fork!!\n");
		return 1;
	} else if (pid == 0) { // child process
		execvp(params[0], params); //exec cmd
		char *error = strerror(errno);
		printf("unknown command\n");
		return 0;
	} else { // parent process
		int childstatus;
		if (!ampersand) {
			waitpid(pid, &childstatus, 0);    
		}
		ampersand = 0;		
		return 1;
	}
}

