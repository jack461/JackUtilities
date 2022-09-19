/*
     Build an array of JSFX strings that can be later
     used for dynamic allocation

     Jack461
*/
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <getopt.h>

char Err[64]; 
char msgerr[64];
char StrFirst[64];
char StrLast[64];
char StrName[64];
char StrModel[] = "#%s%05d=\"\";";
int strcnt;
int strcol;
int stroffset;

void usage(void)
{
    printf("build_strings: Build an array of strings that can be used for "
           "dynamic allocation\n");  
    printf("Syntax:\n");
    printf("    build_strings [-h] [-c count] [-w cols] [-o off] [-f first] [-l last] [-p pref]\n");
    printf("        -h: print this help\n");
    printf("        -c count: number of strings to generate\n");
    printf("        -w cols: number of definitions on each line\n");
    printf("        -f str: name for the first string\n");
    printf("        -l str: name for the last string\n");
    printf("        -p pref: prefix for the other strings\n");
    printf("        -o off: a numeric offset added to the strings numbers\n");
};

void gerror(char * msg)
{
    fprintf(stderr, "*** Error : %s\n", msg);
    usage();
    exit(1);
}


int main(int argc, char * argv[])
{
    int c, w, i, j;
    char * pend;
    // define default values
    strcpy(StrFirst, "strFirst");
    strcpy(StrLast, "strLast");
    strcpy(StrName, "strx");
    strcnt = 16;
    strcol = 5;
    stroffset = 0;

    // decode the parameters
    while ((c = getopt(argc, argv, "hc:f:l:o:p:w:")) > 0)
    {
        switch(c)
        {
            case '?':
                sprintf(msgerr, "Unknown option: \"%c\".", optopt);
                gerror(msgerr);

            case 'h':
                usage();
                exit(0);

            case 'c':
                w = strtol(optarg, &pend, 0);
                if (((*optarg)==0) || ((*pend)!=0) || (w <4) || (w > 1000000))
                    gerror("Invalid -c argument. A number between 4 and 1000000 is expected.");
                strcnt = w;
                break;

            case 'o':
                w = strtol(optarg, &pend, 0);
                if (((*optarg)==0) || ((*pend)!=0) || (w < 0) || (w > 100000))
                    gerror("Invalid -o argument. A number between 0 and 100000 is expected.");
                stroffset = w;
                break;

            case 'w':
                w = strtol(optarg, &pend, 0);
                if (((*optarg)==0) || ((*pend)!=0) || (w < 1) || (w > 16))
                    gerror("Invalid -w argument. A number between 1 and 16 is expected.");
                strcol = w;
                break;

            case 'f':
                strncpy(StrFirst, optarg, 63);
                break;

            case 'l':
                strncpy(StrLast, optarg, 63);
                break;

            case 'p':
                strncpy(StrName, optarg, 63);
                break;

        }
    }


    printf("@init\n\n");

    j = 0;
    for (i=1; i<=strcnt; i++)
    {
        if (i==1) 
        {
            printf("#%s=\"\";\n", StrFirst);
            j = 0;
        }
        else if (i==strcnt)
        {
            if (j != 0) printf("\n");
            printf("#%s=\"\";\n", StrLast);
        }
        else
        {
            printf(StrModel, StrName, i+stroffset);
            if (++j >= strcol) { printf("\n"); j=0; }
        }
    }
}

