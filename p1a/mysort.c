#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#define MAX_CHAR 1024

static int numLines(FILE *f);
static char** readFile(FILE *in, char** data);
static int cmpstr(const void* a, const void* b);
static int cmpstrRev(const void* a, const void* b);


void
usage(char *prog)
{
    fprintf(stderr,"usage: %s <option>\n", prog);
    exit(1);

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
    //printf("lines: %d\n",lines);
    return lines;
}

int main(int argc, char** argv)
{
 
    int linecnt, i;
    int opt;
    char *input[MAX_CHAR];
    char buffer[1024];
    int rflag = 0;
    int nlines = -1; 
    //char **input = malloc(5*sizeof(char*));
    while((opt = getopt(argc, argv, "rn:")) != -1) {
    switch(opt) {
        case 'r':
        rflag = 1; 
            //if(argv[optind] != NULL) {
            //}
            printf("reverse flag set\n");
            break;
        case 'n':
            nlines = atoi(optarg);
            printf("lines set\n");
            break;
        default: // '?'
            usage(argv[0]);
            //exit(EXIT_FAILURE);
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
                
                lseek(fileno(fp), 0, SEEK_SET);
                
                text = readFile(fp, text);
                
                if(rflag == 1) {
                qsort(text, lines, sizeof(char*),cmpstrRev);
            } else {
                qsort(text, lines, sizeof(char*),cmpstr);
            }
                
                if(nlines != -1) {
        for(i = 0; i < nlines; i++) {
            printf("[%d]: %s",i,text[i]);
        } } else {
                for(i = 0; i < lines; i++) {
                   printf("[%d]: %s",i,text[i]);
                }}
    } else {
        printf("just read from stdin\n");
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
    }else {
        qsort(input,linecnt,sizeof(char*),cmpstr);
    }
        if(nlines != -1) {
        for(i = 0; i < nlines; i++) {
            printf("[%d]: %s",i,input[i]);
        } }
        else {
            for(i = 0; i < linecnt; i++) {
            printf("[%d]: %s",i,input[i]);
        }

        }
    }
    //}
    printf("optind: %s\n", argv[optind]);
    printf("optarg: %s\n", optarg);
    return 0;
}
