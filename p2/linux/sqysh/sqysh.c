#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_ARGS 256

const char PROMPT[] = "sqysh$ ";
const char ERROR[] = "error occured.\n";
const char FILE_ERROR[] = "file cannot be read.\n";

char **cmds = NULL;
char buffer[MAX_ARGS];
int batch_flag = 0; 
FILE *stream = NULL;
int n = 0, i; 
int internal_cmd = 0; 
int external_cmd = 0; 


//init before loop
void init() {
	internal_cmd = 0; 
	external_cmd = 0; 
}

//parses command 
char** parseCmd (char* in) {
		n = 0; 
		char *ret; 

		//trim leading space 
		while(isspace((unsigned char)*in)) in++;

		//trim trailing space 
		ret = in + strlen(in) - 1; 
		while(ret > in && isspace((unsigned char)*ret)) ret--; 
		*(ret+1) = 0; 
        
        if(strlen(in) == 0) {
        	return NULL; 
        }
		//split line based on whitespace delimit
		char *p = strtok(in, " ");
		char **res = NULL;
		while(p) {
			res = realloc(res, sizeof(char*) * ++n);
			if(res == NULL) {
				exit(EXIT_FAILURE); //memalloc fail 
			}
			res[n-1] = p; 
			p = strtok(NULL, " ");
		}

		//res = realloc (res, sizeof(char*) * (n+1));
		//res[n] = 0; 
		return res; 
	}


int main(int argc, char** argv)
{
	if(isatty (fileno(stdin)) && argc==1) {
		//stream = stdin; 
		//write (STDOUT_FILENO, PROMPT, strlen(PROMPT));
	} else if(argc==2) {
		stream = fopen(argv[1], "r");
		batch_flag = 1; 
		if(stream==NULL) {
			write (STDERR_FILENO, FILE_ERROR, strlen(FILE_ERROR));
			exit(EXIT_FAILURE);
		}
		dup2(fileno(stream), STDIN_FILENO); //re-direct to stdin

	} else {
		write (STDERR_FILENO, ERROR, strlen(ERROR));
		exit(EXIT_FAILURE);
	}


	INTERACTIVE_START: while(1) {
		init();

		if(!batch_flag) {
			write (STDOUT_FILENO, PROMPT, strlen(PROMPT));
		}

		 if (fgets(buffer, MAX_ARGS, stdin)!= NULL) {

		 	if(strlen(buffer) == 0) {
		 		goto INTERACTIVE_START;
		 	}
		 	cmds = parseCmd(buffer);

		 	if(cmds==NULL) {
		 		goto INTERACTIVE_START;
		 	}

		 //**handling all internal commmands**//

		 //CD 
		 if (strcmp(cmds[0], "cd") == 0) {
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
		 			chd = chdir(cmds[n-1]);
		 			if (chd != 0) {
		 			fprintf(stderr, "cd: %s: %s\n", cmds[n-1], strerror(errno));
		 			goto INTERACTIVE_START;
		 		}
		 	}  
		 }

		 //EXIT
		  if (strcmp(cmds[0], "exit") == 0) {
		  	internal_cmd = 1;
		  	if (n != 1) {
		  		fprintf(stderr, "exit: too many arguments\n");
		 		goto INTERACTIVE_START;
		  	} else {
		  		exit(0);
		  	}
		  }

		  //PWD
		  if (strcmp(cmds[0], "pwd") == 0) {
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

		 if(!internal_cmd) {
		 	fprintf(stderr, "%s: command not supported.\n", cmds[0]);
		 	continue; 
		 }
		 } else {
		 		write (STDERR_FILENO, ERROR, strlen(ERROR));
				continue;
		 }
	}
	//free(cmds); //TODO free whole 
	return 0;
}
