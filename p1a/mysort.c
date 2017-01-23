#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>

#define MAX_CHAR 1024


void
usage(char *prog)
{
	fprintf(stderr,"usage: %s <option>\n", prog);
	exit(1);

}

int cmpstr(const void* a, const void* b) { 
	    const char* aa = *(const char**)a;
	        const char* bb = *(const char**)b;
		    return strcmp(aa, bb);
}
 
int main(int argc, char** argv)
{
	/* Your code goes here! */
	int nlines,linecnt,i;
        int opt;
	char *input[MAX_CHAR];
	char buffer[1024];
	//char **input = malloc(5*sizeof(char*));
        //FILE *fileIN, *fileOUT;

	if(argc == 1) {
		printf("just read from stdin\n");
		while(fgets(buffer, MAX_CHAR, stdin)!= NULL) 
		{  
	         //input[i] = (char*) malloc(MAX_CHAR);	
		 //input =  realloc(input, strlen(input)+1+strlen(buffer));
		 //input[i++] = malloc(i * sizeof(char *));
		 //input[i] = buffer
		 //memcpy(input[i],buffer,strlen(buffer));
		 input[i++] = strdup(buffer);
		 //strcat(input, buffer);
		 linecnt++;
		}

          
          qsort(input,linecnt,sizeof(char*),cmpstr);
	  //char *sorted = malloc(sizeof(*input));
	  for(i = 0; i < linecnt; i++) {
	  printf("%s",input[i]);
	   }
	  printf("%d\n",linecnt);
	  //free(input);	  
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
		 usage(argv[0]);
		 //exit(EXIT_FAILURE);
	 }
        }

	/*If successful, exit with a zero status. */
	return 0;
}
