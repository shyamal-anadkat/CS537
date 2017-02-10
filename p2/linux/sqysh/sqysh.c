/***************************
* Author:  Shyamal H Anadkat
* CS537 -- P1 Sqysh      
****************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_ARGS 256

int str_equal(char *s1, char *s2);

const char PROMPT[] = "sqysh$ ";
const char ERROR[] = "Error occured.\n";
const char ERROR_SPECIAL[] = "Error: invalid special syntax.\n";
const char FILE_ERROR[] = "file cannot be read.\n";
const char SPECIAL_ARG_ERROR[] = "Error: too many args. \n";

//struct for command 
typedef struct {
	char *in_file; 
	char *out_file;
	int is_background;
	char **arg; 
	int arglen; 
	int is_special;
	int is_built_in;  
	pid_t pid; 
	int status; 
} Cmd; 

char buffer[MAX_ARGS];
int batch_flag = 0; 
FILE *stream = NULL;
int n = 0, i; 
int internal_cmd = 0; 
int external_cmd = 0; 
char *cmd = NULL; 


//init before loop
void init() {
	internal_cmd = 0; 
	external_cmd = 0; 
}


void init_cmd (Cmd *cmd) {
		//init cmd params for new command
		cmd->in_file = NULL; 
		cmd->is_background = 0; 
		cmd->out_file = NULL;
		cmd->arg = NULL;
		cmd->arglen = 0; 
		cmd->is_special = 0;
		cmd->is_built_in = 0;  
		cmd->pid = -1; 
}


//parses command 
int parseCmd (char* in, Cmd *cmd) {
		n = 0;
		int i;  
		char *ret; 

		init_cmd (cmd); 

		//trim leading space 
		while(isspace((unsigned char)*in)) in++;

		//trim trailing space 
		ret = in + strlen(in) - 1; 
		while(ret > in && isspace((unsigned char)*ret)) ret--; 
		*(ret+1) = 0; 
        
        if(strlen(in) == 0 || in == NULL) {
        	return 1; 
        }
		//split line based on whitespace delimit
		char *p = strtok(in, " ");
		char **res = NULL;
		while(p) {
			++n; 
			cmd->arg = realloc(cmd->arg, sizeof(char*) * n);
			res = realloc(res, sizeof(char*) * n);
			if(res == NULL) {
				exit(EXIT_FAILURE); //memalloc fail 
			}
			res[n-1] = p; 
			cmd->arg[n-1] = res[n-1]; 
			p = strtok(NULL, " ");
		}

		res = realloc (res, sizeof (char*) * (n+1));
		res[n] = 0;
		cmd->arg = realloc (cmd->arg, sizeof (char*) * (n+1));
		cmd->arg[n] = 0;

		int numArgs = n; 
		cmd->arglen = n; 


		//check for builtins
		if(strcmp(res[0], "cd") == 0 || strcmp(res[0], "exit") == 0 || 
			strcmp(res[0], "pwd") == 0 || strcmp(res[0], "clear") == 0 ){ 
			cmd->is_built_in = 1; 
		} 

		//can't start with < , >, & 
		 if(str_equal(res[0], "<") || str_equal(res[0], ">") || str_equal(res[0], "&")
		  || str_equal(res[numArgs-1], ">") || str_equal(res[numArgs-1], "<") ) {
		 	write (STDERR_FILENO, ERROR_SPECIAL, strlen(ERROR_SPECIAL));
		 	return 1;
		 }

		int cnt1 = 0; 
		int cnt2 = 0;  
		for(i = 1; i < numArgs; i++) {

			if(!strcmp(res[i], "<")) {
				cnt1++;
				if(cnt1>1) {
					write (STDERR_FILENO, ERROR_SPECIAL, strlen(ERROR_SPECIAL));
				return 1; 
				}
				if(res[i+1] != NULL) {
/*					if(res[i+5] != NULL) {
						write (STDERR_FILENO, SPECIAL_ARG_ERROR, strlen(SPECIAL_ARG_ERROR));
						return 1; 
					}*/
					cmd->in_file = res[i+1];
				} else {
					write (STDERR_FILENO, ERROR_SPECIAL, strlen(ERROR_SPECIAL));
					return 1; 
				}

			} else if (!strcmp(res[i], ">")) {
				cmd->is_special = 1; 
				cnt2++; 
				if(cnt2>1) {
					write (STDERR_FILENO, ERROR_SPECIAL, strlen(ERROR_SPECIAL));
				return 1; 
				}
				if(res[i+1] != NULL) {
/*					if(res[i+5] != NULL) {
						write (STDERR_FILENO, SPECIAL_ARG_ERROR, strlen(SPECIAL_ARG_ERROR));
						return 1; 
					}*/
					cmd->out_file = res[i+1];
				} else {
					write (STDERR_FILENO, ERROR_SPECIAL, strlen(ERROR_SPECIAL));
					return 1; 
				}

			} 
		}

		//Check for background execution 
		if(!strcmp(res[numArgs-1], "&")) {
			cmd->is_background = 1; 
			//set the last & to NULL, we dont want to factor that in
			cmd->arg[cmd->arglen-1] = NULL; 
		}
		
    	free(res); 
		return 0; 
	}


//manage redirection [ >, < ] 
void redirHandler(Cmd cmd) {
	int fd1,fd2; 
	pid_t pid; 
	char **parsd = NULL;

	for(i = 0; i < cmd.arglen; i++) {
			parsd = realloc(parsd, sizeof(char*) * (i+1));
			if(parsd == NULL) {
				exit(EXIT_FAILURE); //memalloc fail 
			}
			if((strcmp(cmd.arg[i], "<" ) != 0) && (strcmp(cmd.arg[i], ">") != 0)) {
					parsd[i] = cmd.arg[i]; 
			} 
		}
		
	if ((pid = fork()) < 0) {
		write (STDERR_FILENO, ERROR, strlen(ERROR));
	} 

	else if (pid == 0) {
			if(cmd.out_file != NULL && cmd.in_file == NULL) {
			fd1 = open (cmd.out_file, O_CREAT | O_TRUNC | O_WRONLY,0644);
			dup2(fd1, STDOUT_FILENO);
			close(fd1);
		} else if (cmd.out_file != NULL && cmd.in_file !=NULL) {
			fd1 = open(cmd.in_file, O_RDONLY);  
			dup2(fd1, STDIN_FILENO);
			close(fd1);

			fd2 = open (cmd.out_file, O_CREAT | O_TRUNC | O_WRONLY,0644);
			dup2(fd2, STDOUT_FILENO);
			close(fd2);
		} else if (cmd.out_file == NULL && cmd.in_file !=NULL) {
			fd1 = open(cmd.in_file, O_RDONLY);  
			dup2(fd1, STDIN_FILENO);
			close(fd1);
		}
		if(execvp(parsd[0], parsd) == -1) {
			write (STDERR_FILENO, ERROR, strlen(ERROR));
		}

	}
	if(!cmd.is_background) {
		waitpid(pid, NULL, 0);
	}
}


//checks if two strings are equal 
int str_equal(char *s1, char *s2){
    if (!strcmp(s1, s2)){
        return 1;
    }
    return 0;
}

//can run in foreground or background 
int start_prog(Cmd cmd, char **cmds, int is_background, int arglen) {

	int par_state; 
	pid_t pid; 

	pid = fork(); 
	cmd.pid = pid; 


	if(pid==0) { //child
		if(execvp(cmds[0], cmds) < 0) {
			write (STDERR_FILENO, ERROR, strlen(ERROR));
		}
		exit(EXIT_FAILURE);

	} else if (pid == -1) {
		write (STDERR_FILENO, ERROR, strlen(ERROR));
		return 1; 

	} else if(!is_background) {
		do { //parent to wait for child 
			waitpid (pid, &par_state, WUNTRACED);
		} while(!WIFEXITED(par_state) && !WIFSIGNALED(par_state));
	}
	return 0; 
}



int main(int argc, char** argv)
{
	if(isatty (fileno(stdin)) && argc==1) {
		stream = stdin; 
	} else if (argc==2) {
		stream = fopen(argv[1], "r");
		batch_flag = 1; 
		if(stream==NULL) {
			write (STDERR_FILENO, FILE_ERROR, strlen(FILE_ERROR));
			exit(EXIT_FAILURE);
		}

	} else {
		write (STDERR_FILENO, ERROR, strlen(ERROR));
		exit(EXIT_FAILURE);
	}


	INTERACTIVE_START: while(1) {
		init();
		Cmd cmd;
		if(!batch_flag) {
			write (STDOUT_FILENO, PROMPT, strlen(PROMPT));
		}

		 if (fgets(buffer, MAX_ARGS, stream)!= NULL) {

		 	if(strlen(buffer) == 0) {
		 		goto INTERACTIVE_START;
		 	}

		 	if(parseCmd(buffer, &cmd)) {
		 		goto INTERACTIVE_START;
		 	}

		 //**handling all internal commmands**//
		 if(cmd.is_built_in) {
		 //CD 
		 if (strcmp(cmd.arg[0], "cd") == 0) {
		 	internal_cmd = 1;
		 	int chd; 
		 	if (n>2) {
		 		fprintf(stderr, "cd: too many arguments\n");
		 		goto INTERACTIVE_START;
		 	}
		 	else if(n==1) {
		 		chd = chdir(getenv("HOME"));
		 		if(chd != 0) {
		 			fprintf(stderr, "cd: %s\n", strerror(errno));
		 			continue;
		 		}
		 	} else if (n==2) {
		 			chd = chdir(cmd.arg[n-1]);
		 			if (chd != 0) {
		 			fprintf(stderr, "cd: %s: %s\n", cmd.arg[n-1], strerror(errno));
		 			goto INTERACTIVE_START;
		 		}
		 	}  
		 }

		 //EXIT
		  if (strcmp(cmd.arg[0], "exit") == 0) {
		  	internal_cmd = 1;
		  	if (n != 1) {
		  		fprintf(stderr, "exit: too many arguments\n");
		 		goto INTERACTIVE_START;
		  	} else {
		  		exit(0);
		  	}
		  }

		  //PWD
		  if (strcmp(cmd.arg[0], "pwd") == 0) {
		  	internal_cmd = 1;
		  	if (n != 1) {
		  		fprintf(stderr, "pwd: too many arguments\n");
		 		goto INTERACTIVE_START;
		 	} else {
		 		char cwd[1024];
		 		if(getcwd(cwd, sizeof(cwd)) != NULL) {
		 			write (STDOUT_FILENO, cwd, strlen(cwd));
		 			write (STDOUT_FILENO, "\n", strlen("\n"));
		 		} else {
		 			fprintf(stderr, "pwd: %s\n", strerror(errno));
		 			continue;
		 		}
		 	}
		  }
		}
		//>
		int redir = 0; 
		if(cmd.out_file!=NULL || cmd.in_file !=NULL) {
			redir = 1; 
			redirHandler(cmd);
		} 

		 if(!redir && !cmd.is_built_in && start_prog(cmd, cmd.arg, cmd.is_background, cmd.arglen)) {
		 	fprintf(stderr, "%s: command not supported.\n", cmd.arg[0]);
		 	continue; 
		 }

		 if(cmd.is_background) {
		 	int bg_status; 
		 	waitpid(cmd.pid, &bg_status ,WNOHANG);
		 	fprintf(stderr, "[%s (%d) completed with status %d]\n", cmd.arg[0], cmd.pid, bg_status);
		}

		 } else {
		 		write (STDERR_FILENO, ERROR, strlen(ERROR));
				continue;
		 }

		//free(&cmd); 
	}

	return 0;
}
