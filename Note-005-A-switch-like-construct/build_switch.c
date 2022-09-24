/*
   Build a "n" level selector for JSFX
   (a kind of switch/case C, with continuous
   integer values from 0 to 2^N-1.)
*/
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <getopt.h>

#define sz 120
char spaces[sz+1] = "                              "
                   "                              "
                   "                              "
                   "                              " ;
int gl_counter;

char Src[64];
char Err[64];
char Sep[64];
char xedsep[80];
char msgerr[64];
int codsiz;
int indent;
int tabsiz;

void usage()
{
    printf("build_switch: Build a \"N\" level selector for JSFX, like a C switch,\n");
    printf("    with continuous integer values from 0 to 2^N-1.\n");
    printf("Syntax:\n"); 
    printf("    build_switch [-h] [-e Evar] [-v Vvar] [-b Vsize] [-i Inc] [-d Ind] [-s Sep]\n");
    printf("where:\n");
    printf("    Vvar is the tested variable, with values from 0 to 2^N-1 (default: \"Cmd\")\n");
    printf("    Evar is an \"error\" variable, and is set to the tested variable as a default action.(default: \"Err\")\n");
    printf("    Vsize is the log of the number of cases to test : use 3 for 8 cases, 5 for 32 cases, etc.\n");
    printf("    Inc is the global increment (in spaces) for the whole construct.\n");
    printf("    Ind is the additionnal increment (in spaces) for each case.\n");
    printf("    Sep is the separator string, default \"//===%%i===//\"\n");
}

void gerror(char * msg)
{
    fprintf(stderr, "*** Error : %s\n", msg);
    usage();
    exit(1);
}

void printlevel(int dec)
{
    char * spc = spaces + sz - dec -tabsiz;
    --gl_counter;
    printf("\n");
    sprintf(xedsep, Sep, gl_counter);
    printf("%s%s\n", spc, xedsep);
    printf("%s%s=%d;\n", spc, Err, gl_counter);
    printf("%s%s\n", spc, xedsep);
    printf("%s", spc + tabsiz);
}

void dolevel(int level, int dec)
{
    int val = 1 << level;
    printf("(%s&0x%x)?(", Src, val);
    if (level <= 0) {
        printlevel(dec);
    } else {
        dolevel(level - 1, dec);
    }
    printf("):(");
    if (level < 1) {
        printlevel(dec);
    } else {
        dolevel(level - 1, dec);
    }
    printf(");");
}

void makeselector(int level)
{
    level = (level < 1) ? 1 : (level > 15) ? 15 : level;
    gl_counter = 1 << level; 
    printf("\n%s", spaces + sz - indent);
    dolevel(level-1, indent);
    printf("\n\n");
}

int main(int argc, char * argv[])
{
    int c, w;
    char * pend;
    // Provide some default value
    strcpy(Src, "Cmd");
    strcpy(Err, "Err");
    strcpy(Sep, "//===%i===//");
    codsiz = 3;
    indent = 4;
    tabsiz = 4;
    // decode the parameters
    while ((c = getopt(argc, argv, "hb:e:s:i:t:v:")) > 0)
    {
        switch(c)
        {
            case '?':
                sprintf(msgerr, "Unknown option: \"%c\".", optopt);
                gerror(msgerr);

            case 'h':
                usage();
                exit(0);

            case 'b':
                w = strtol(optarg, &pend, 0);
                if (((*optarg)==0) || ((*pend)!=0) || (w < 1) || (w > 12))
                    gerror("Invalid -b argument. A number between 1 and 12 is expected.");
                codsiz = w;
                break;

            case 'e':
		strncpy(Err, optarg, 63);
                break;

            case 'i':
                w = strtol(optarg, &pend, 0);
                if (((*optarg)==0) || ((*pend)!=0) || (w < 0) || (w > 40))
                    gerror("Invalid -i argument. A number between 0 and 40 is expected.");
                indent = w;
                break;

            case 't':
                w = strtol(optarg, &pend, 0);
                if (((*optarg)==0) || ((*pend)!=0) || (w < 0) || (w > 10))
                    gerror("Invalid -t argument. A number between 0 and 10 is expected.");
                tabsiz = w;
                break;

            case 's':
		strncpy(Sep, optarg, 63);
                break;

            case 'v':
		strncpy(Src, optarg, 63);
                break;

        }
    }

    makeselector(codsiz);
    exit(0);
}


