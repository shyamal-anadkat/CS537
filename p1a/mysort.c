#include <stdlib.h>
#include <stdio.h>

/* See `man 3 getopt()` for how to use the getopt function. */
#include <getopt.h>

int main(int argc, char** argv)
{
	/* Your code goes here! */
	int nlines;
        int opt;

	nlines = 0;

	while((opt = getopt(argc, argv, "rn:")) != -1) {
		switch(opt) {
		case 'r':
		  printf("just read from stdin");
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
	/* If successful, exit with a zero status. */
	return 0;
}
