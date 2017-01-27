#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#define MAX_CHAR 1024

//declarations
static int numLines(FILE *f);
static char** readFile(FILE *in, char** data);
static int cmpstr(const void* a, const void* b);
static int cmpstrRev(const void* a, const void* b);


//prints out usage 
void usage(char *prog)
{
    fprintf(stderr,"usage: %s <r> <n:> <optional text file>\n", prog);
}

//ascending strcmp wrapper
int cmpstr(const void* a, const void* b) {
    const char* aa = *(const char**)a;
    const char* bb = *(const char**)b;
    return strcmp(aa, bb);
}

//descending strcmp wrapper
int cmpstrRev(const void* a, const void* b) {
    const char* aa = *(const char**)a;
    const char* bb = *(const char**)b;
    return -strcmp(aa, bb);
}

//read input from file into array  
char** readFile(FILE * in, char** data) {
    int i = 0;
    char line[MAX_CHAR];
    while(fgets(line,MAX_CHAR,in)!=NULL) {
        data[i] = strdup(line);
        i++;
    }
    return data;
}

//one pass file line reader 
int
numLines(FILE *fp) {
    int lines = 0;
    int ch;
    while(!feof(fp))
    {
        ch = fgetc(fp);
        if(ch == '\n') {
            lines++;
        }
    }
    return lines;
}

int main(int argc, char** argv)
{
    int linecnt, i, nlines;
    linecnt = 0;
    long val;
    int opt, rflag;
    char **input = malloc(sizeof(char*));
    char buffer[1024];
    char *endptr, *str;
    rflag = 0;
    nlines = -1; 
    i = 0; 

    while((opt = getopt(argc, argv, "rn:")) != -1) {
    switch(opt) {
        case 'r':
            rflag = 1; 
            break;
        case 'n':
            errno = 0; 
            str = optarg;
            val = strtol(str, &endptr,10);
             //input validation 
             if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
                   || (errno != 0 && val == 0)) {
               perror("strtol");
               exit(EXIT_FAILURE);
            }
             if (endptr == str) {
               fprintf(stderr, "No digits found\n");
               exit(EXIT_FAILURE);
            }
            if (*endptr != '\0') {
               fprintf(stderr, "Expecting integer arg.\n");
               exit(EXIT_FAILURE);
            }
            if (val < 0) {
               fprintf(stderr, "Expecting positive integer.\n");
               exit(EXIT_FAILURE);
            }
            nlines = val; 
            break;
        default: // '?'
            usage(argv[0]);
            return -1;
        }
    }

    if(argv[optind] != NULL) {

                FILE *fp = fopen(argv[optind],"r");
                if(fp == NULL) {
                    fprintf(stderr,"Error opening file.\n");
                    return -1;
                }
                int lines = numLines(fp);                
                char **text = malloc(sizeof(char *) * lines);
                
                //reset file pointer 
                lseek(fileno(fp), 0, SEEK_SET);
                
                text = readFile(fp, text);
                
              if(rflag == 1) {
                qsort(text, lines, sizeof(char*),cmpstrRev);
            } else {
                qsort(text, lines, sizeof(char*),cmpstr);
            }
                
        if(nlines != -1) {
            if(nlines > lines) {
                fprintf(stderr,"Invalid number of lines.\n");
                return -1;
            }
            for(i = 0; i < nlines; i++) {
            printf("%s",text[i]);
             } } else {
                    for(i = 0; i < lines; i++) {
                    printf("%s",text[i]);
                }}
    } else {
            
            while(fgets(buffer, MAX_CHAR, stdin)!= NULL)
            {
	    input = realloc(input, (i+1)* sizeof(*input));
            input[i] = malloc(strlen(buffer)+1);
            strcpy(input[i], buffer);
            i++;
	    linecnt++;
            }
         //free(input);
        
	   if(rflag==1) {
             qsort(input,linecnt,sizeof(char*),cmpstrRev);
        }  else {

             qsort(input,linecnt,sizeof(char*),cmpstr);
        }
        
        if(nlines != -1) {
                if(nlines > linecnt) {
                fprintf(stderr,"Invalid number of lines.\n");
                return -1;
            }
        for(i = 0; i < nlines; i++) {
            printf("%s",input[i]);
        } } else {
            for(i = 0; i < linecnt; i++) {
            printf("%s",input[i]);
            }
        }
    }
    free(input);
    free(text); 
    //printf("optind: %s\n", argv[optind]);
    //printf("optarg: %s\n", optarg);
    return 0;
}
