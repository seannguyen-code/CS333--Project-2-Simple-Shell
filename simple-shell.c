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
#define MAX_HISTORY 10	//The maximum length of the history array

int ampersand = 0; //check ampersand
int hasPipe = 0; //check pipe "|"
int prompt_cmd(char* cmd);
int parse_cmd(char* cmd, char** params); 
void execute_cmd_with_pipe(char** params, int argc, char** params1, char** params2);
void execute_cmd(char** params, int argc);

int main(void) {
	char command[MAX_LENGTH];
	char *args[MAX_LENGTH_PARAMS]; 
	char *args1[MAX_LENGTH_PARAMS/2]; 
	char *args2[MAX_LENGTH_PARAMS/2]; 
	
	int should_run = 1;
	
	while (should_run) {
		hasPipe = 0;
		if (!prompt_cmd(command)) break;  //Prompt type

		int argc = parse_cmd(command, args); //Parse command and arguments.
		if (strcmp(args[0], "exit") == 0) break; 
		if (strcmp(args[argc-1], "&") == 0) { //check ampersand
			ampersand = 1;
			args[--argc] = NULL;
		}
		
		execute_cmd_with_pipe(args, argc, args1, args2);//If there is a pipe symbol the bellow command will not execute
		if (hasPipe == 0) execute_cmd(args, argc); 
	}
	return 0;
};

int prompt_cmd(char* cmd) {
	static int first_time = 1;
	if (first_time) { //clear screen on first run
		const char* CLEAR_SCREEN = "\e[1;1H\e[2J";
		write(STDOUT_FILENO, CLEAR_SCREEN, 12);
		first_time = 0;
	}

	printf("ssh>>");
	fflush(stdout);
	if (fgets(cmd, MAX_LENGTH, stdin) == NULL) return 0;
	if(cmd[strlen(cmd)-1] == '\n')  
		cmd[strlen(cmd)-1] = '\0'; //remove newline char    
	return 1;
};

int parse_cmd(char* cmd, char** params) {
	int i,n=-1;
	for(i=0; i<MAX_LENGTH_PARAMS; i++) { //parse command to args from cmd
		params[i] = strsep(&cmd, " ");
		n++;
		if(params[i] == NULL) break;
	}	
	return n;
};

void execute_cmd(char** params, int argc) {
	int fd[2] = {-1, -1};	//input, output redirection
	
	while(argc >= 3) {//it's either an input or an ouput redirection
		if (strcmp(params[argc-2], ">") == 0) { // output redirection
			fd[1] = open(params[argc-1], O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP|S_IWGRP);//output file
			if(fd[1] == -1) {
				perror("error open");
				return;
			}
			params[argc-2] = NULL;
			argc -= 2;
		}else if (strcmp(params[argc-2], "<") == 0) { // input redirection
			fd[0] = open(params[argc-1], O_RDONLY); ////input file
			if(fd[0] == -1) {
				perror("error open");
				return;
			}
			params[argc-2] = NULL;
			argc -= 2;
		} else break;
	}

	int childstatus;
	pid_t pid = fork(); //fork process
	switch(pid){
		case -1: //error
			perror("error fork!!");
			break;
		case 0: // child
			if(fd[0] != -1){ //input redirection
				if(dup2(fd[0], STDIN_FILENO) != STDIN_FILENO){
					perror("error dup2");
					exit(1);
				}
			}
			if(fd[1] != -1){ //output redirection
				if(dup2(fd[1], STDOUT_FILENO) != STDOUT_FILENO){
					perror("error dup2");
					exit(1);
				}
			}
			execvp(params[0], params);
			printf("Unknow command");
			exit(0);
		default: // parent
			close(fd[0]);close(fd[1]);
			if(!ampersand)//handle ampersand
				waitpid(pid, &childstatus, 0);
			ampersand = 0;	
			break;
	}	
}

void execute_cmd_with_pipe(char** params, int argc, char** params1, char** params2) {
	//seprate agruments between | 
	int i=0,j=0;
	int pipeFound = 0;
	while (i < argc) {
		if (strcmp(params[i], "|") == 0) {
			pipeFound = 1;
			hasPipe = 1;
			break;
		}
		params1[i] = params[i];
		i++;
	}
	params1[i] = NULL;
	if (pipeFound == 0) return;
	while (i < argc) {
		if (strcmp(params[i], "|") == 0) {++i;continue;}
		params2[j] = params[i];
		j++;
		i++;
	}
	params2[j] = NULL;
			
	//pipe handleing
	int fd[2];
	if (pipe(fd) != 0)
		perror("error pipe");
		
	pid_t childPid;
	if ((childPid = fork()) == -1)
		perror("failed to fork");
	if (childPid == 0)
	{
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		close(fd[1]);
		execvp(params1[0], params1);
		perror("error exec 1");
	}
	else
	{
		close(fd[1]);
		dup2(fd[0], STDIN_FILENO);
		close(fd[0]);
		execvp(params2[0], params2);
		perror("error exec 2");
	}
	return;
}
