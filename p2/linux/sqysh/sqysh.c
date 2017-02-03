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

int batch_flag = 0; 
FILE *stream = NULL;
int n = 0, i; 

//parses command 
char** parseCmd (char* in) {
		char *ret; 

		//trim leading space 
		while(isspace((unsigned char)*in)) in++;

		//all spaces 
		if(*in == 0) {

		}

		//trim trailing space 
		ret = in + strlen(in) - 1; 
		while(ret > in && isspace((unsigned char)*ret)) ret--; 
		*(ret+1) = 0; 

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

		res = realloc (res, sizeof(char*) * (n+1));
		res[n] = 0; 
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
		if(!batch_flag) {
			write (STDOUT_FILENO, PROMPT, strlen(PROMPT));
		}
		 char buffer[MAX_ARGS];
		 if (fgets(buffer, MAX_ARGS, stdin)!= NULL) {
		 	if(strlen(buffer) == 0) {
		 		goto INTERACTIVE_START;
		 	}
		 	char **cmds = parseCmd(buffer);
		 	for (i = 0; i < n; ++i) {
		 		printf ("cmds[%d] = %s\n", i, cmds[i]);
		 	}
		 	n = 0;










		 	free(cmds); //TODO free whole 

		 } else {
		 			write (STDERR_FILENO, ERROR, strlen(ERROR));
					exit(EXIT_FAILURE);
		 }
	}
	return 0;
}
