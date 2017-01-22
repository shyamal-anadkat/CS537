#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>

#define MAX_CHAR 1024


int main(int argc, char** argv)
{
	/* Your code goes here! */
	int nlines;
        int opt;
	char *input = calloc(1,1), buffer[MAX_CHAR];
	FILE *fileIN, *fileOUT;
	nlines = 0;

	if(argc == 1) {
		printf("just read from stdin\n");
		while(fgets(buffer, MAX_CHAR, stdin)) 
		{      
		 input = realloc(input, strlen(input)+1+strlen(buffer));
		 strcat(input, buffer); 
		}

          //qsort(words,count,size,strcmp);
	  char *sorted = malloc(sizeof(*input));
	  
	  printf("\ninput:\n%s",input);
	  printf("%d",sizeof(input));	
	  // printf("\nsorted:\n%s",sorted);	
		
	}
	
	while((opt = getopt(argc, argv, "rn:")) != -1) {
		switch(opt) {
		case 'r':
		  printf("read from stdin and reverse");
		  break;
		case 'n':
		  nlines = atoi(optarg);
		  printf("%d lines",nlines);
		  break;
		default: /* '?' */
		 printf("Usage is wrong");
		 exit(EXIT_FAILURE);
	 }
        }

	/*If successful, exit with a zero status. */
	return 0;
}
