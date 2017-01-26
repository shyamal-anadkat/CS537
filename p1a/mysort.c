#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
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
    int opt, rflag;
    char *input[MAX_CHAR];
    char buffer[1024];
    rflag = 0;
    nlines = -1; 
    while((opt = getopt(argc, argv, "rn:")) != -1) {
    switch(opt) {
        case 'r':
            rflag = 1; 
            //printf("reverse flag set\n");
            break;
        case 'n':
            goto TEST;
                for(i = 0; i < strlen(optarg); i++) {
                if( !isdigit(optarg[i]) ){
                 fprintf(stderr, "invalid n option %s - expecting a positive integer.\n", 
                 optarg?optarg:"");
                 exit(EXIT_FAILURE);
                }
            }
            TEST:
            nlines = atoi(optarg);
            //printf("lines set\n");
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
            //printf("just read from stdin\n");
            while(fgets(buffer, MAX_CHAR, stdin)!= NULL)
             {
            //input[i] = (char*) malloc(MAX_CHAR);
            //input =  realloc(input, strlen(input)+1+strlen(buffer));
            //input[i] = buffer
            input[i++] = strdup(buffer);
            linecnt++;
             }
        
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
    //printf("optind: %s\n", argv[optind]);
    //printf("optarg: %s\n", optarg);
    return 0;
}
