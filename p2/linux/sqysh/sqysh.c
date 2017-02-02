#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_ARGS 256

const char PROMPT[] = "sqysh$ ";
const char ERROR[] = "error occured.";
const char FILE_ERROR[] = "file cannot be read.";
int batch_flag = 0; 
FILE *stream = NULL;

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

	while(1) {
		if(!batch_flag) {
			write (STDOUT_FILENO, PROMPT, strlen(PROMPT));
		}
		 char buffer[MAX_ARGS];
		 if (fgets(buffer, MAX_ARGS, stdin)!= NULL) {
		 		printf("%s",buffer);
		 }
	}
	return 0;
}
