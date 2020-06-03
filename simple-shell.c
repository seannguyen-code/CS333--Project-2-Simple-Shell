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
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LENGTH 80 	     // The maximum length of the commands
#define MAX_LENGTH_PARAMS 41 // The maximum length of the args
#define MAX_HISTORY 10

int ampersand = 0;
int parse_cmd(char* cmd, char** params);
int prompt_cmd(char* cmd);
void execute_cmd(char** params, int argc);


int main(void) {
	char command[MAX_LENGTH];
	char *args[MAX_LENGTH_PARAMS]; 
	int should_run = 1;

	while (should_run) {
		if (!prompt_cmd(command)) break;  //Prompt type

		int argc = parse_cmd(command, args); //Parse command and arguments.
		if (strcmp(args[0], "exit") == 0) break;

		if (strcmp(args[argc-1], "&") == 0) { //check ampersand
			ampersand = 1;
			args[--argc] = NULL;
		}
	
		execute_cmd(args, argc);
	}

	return 0;
};

int prompt_cmd(char* cmd) {
	static int first_time = 1;
	if (first_time) { //clear screen
		const char* CLEAR_SCREEN = "\e[1;1H\e[2J";
		write(STDOUT_FILENO, CLEAR_SCREEN, 12);
		first_time = 0;
	}

	printf("ssh>>");
	fflush(stdout);
	if (fgets(cmd, sizeof(cmd)*5, stdin) == NULL) return 0;
	if(cmd[strlen(cmd)-1] == '\n')  
		cmd[strlen(cmd)-1] = '\0'; //remove newline char    
	return 1;
};

int parse_cmd(char* cmd, char** params) {
	int i,n=-1;
	for(i=0; i<MAX_LENGTH_PARAMS; i++) {
		params[i] = strsep(&cmd, " ");
		n++;
		if(params[i] == NULL) break;
	}	
	return n;
};

void execute_cmd(char** params, int argc) {
	int fd[2] = {-1, -1};	//input, output
	
	while(argc >= 3){
		if (strcmp(params[argc-2], ">") == 0) {	// output redirection
			fd[1] = open(params[argc-1], O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP|S_IWGRP);
			if(fd[1] == -1) {
				perror("open");
				return;
			}
			params[argc-2] = NULL;
			argc -= 2;
		}else if (strcmp(params[argc-2], "<") == 0) { // input redirection
			fd[0] = open(params[argc-1], O_RDONLY);
			if(fd[0] == -1) {
				perror("open");
				return;
			}
			params[argc-2] = NULL;
			argc -= 2;
		}else {
			break;
		}
	}

	int childstatus;
	pid_t pid = fork(); //fork process
	switch(pid){
		case -1: //error
			perror("error fork!!");
			break;
		case 0:	// child
			if(fd[0] != -1){ //input
				if(dup2(fd[0], STDIN_FILENO) != STDIN_FILENO){
					perror("error dup2");
					exit(1);
				}
			}
			if(fd[1] != -1){ //output
				if(dup2(fd[1], STDOUT_FILENO) != STDOUT_FILENO){
					perror("error dup2");
					exit(1);
				}
			}
			execvp(params[0], params);
			perror("error execvp");
			exit(0);
		default: // parent
			close(fd[0]);close(fd[1]);
			if(!ampersand)//handle ampersand
				waitpid(pid, &childstatus, 0);
			ampersand = 0;	
			break;
	}	
}


