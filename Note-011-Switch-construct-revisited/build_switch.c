/*
   Build a selector for JSFX
   (a kind of switch/case C, with continuous
   integer values from "N" to "P")

   Jack461 - October 2022
*/
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <getopt.h>

#define sz 120
char spaces[sz+1] =   "                              "
                      "                              "
                      "                              "
                      "                              " ;

char Src[64];
char Err[64];
char Sep[64];
char Xpr[128];
char Tmp[128];
char xedsep[80];
char msgerr[64];
char * spc;
int cmmnt;
int indent;
int tabsiz;

int first, last;
int prvdef;
int pivot;
int count = 1024;
int dbg;

void usage(void)
{
    printf("\n");
    printf("build_switch: Build an integer selector for JSFX, like a C switch,\n");
    printf("    with continuous integer values from \"first\" to \"last\"\n");
    printf("Syntax:\n"); 
    printf("    build_switch [-f First] [-l Last] [-h] [-e Evar] [-v Vvar] [-i Inc] [-d] [-s Sep]\n");
    printf("        [-c Cselect] [-p Pivot] [-x Expr] [-t Tab] \n");
    printf("where:\n");
    printf("    First is the first value of the range to test (default: 0).\n");
    printf("    Last is the last value of the range to test (default: 7).\n");
    printf("    [-h] prints this help.\n");
    printf("    [-d] Add \"default\" cases (actually, use First-1 and Last+1 to extends the switch).\n");
    printf("    Vvar is the tested variable, with values from First to Last (default: \"Cmd\")\n");
    printf("    Evar is an \"error\" variable, and is set to the tested variable as a default action. (default: \"Err\").\n");
    printf("    Inc is the global increment (in spaces) for the whole construct.\n");
    printf("    Tab is the additionnal increment (in spaces) for each case.\n");
    printf("    Sep is the separator string, default \"//===%%i===//\".\n");
    printf("    Cselect lets you choose the comments you want around the action:\n");
    printf("        0: no comment;  1: upper comment only;  2: lower comment only;  3: both comments.\n");
    printf("    Pivot is a value in [First .. Last]. Default: (First+Last)/2.\n");
    printf("    Expr is any JSFX expression inserted as default on the action. Default: \"Err=n;\".\n");
    printf("\n");
}

void gerror(char * msg)
{
    fprintf(stderr, "*** Error : %s\n", msg);
    usage();
    exit(1);
}

void printlevel(int val)
{   
    spc = spaces + sz - indent - tabsiz;
    printf("\n");
    sprintf(xedsep, Sep, val);
    if (cmmnt & 1) printf("%s%s\n", spc, xedsep);
    if (Xpr[0] != 0) {
        sprintf(Tmp, Xpr, val);
        printf("%s%s\n", spc, Tmp);
    }
    else
    {
        printf("%s%s=%d;\n", spc, Err, val);
    }
    if (cmmnt & 2) printf("%s%s\n", spc, xedsep);
    printf("%s", spc + tabsiz);
}

void dolevel(int piv, int fst, int lst)
{
    int lvz = fst >= lst;
    int pv2, pv3;
    if (dbg & 1) {
        printf("\n  cnt: %d    piv:%d  --  fst:%d  --  lst:%d  --  lvz:%d\n", count, piv, fst, lst, lvz);
    }
    if (dbg & 2) {
        count -=1;
    }

  if (count > 0) {

    printf("(%s<%d)?(", Src, piv);
    if (piv <= fst) // if (lvz)
    {
        printlevel(piv-1);
    }
    else
    {
        pv2 = (fst+piv-1)/2;
        dolevel(pv2, fst, piv-1);
    }

    printf("):(");

    if (piv >= lst) // if (lvz)
    {   
        printlevel(piv);
    }
    else
    {   
        pv3 = (piv+1+lst)/2;
        dolevel(pv3, piv+1, lst);
    }

    printf(");");
  }
}

void makeselector(void)
{
    spc = spaces + sz - indent ;
    printf("\n%s", spc);
    first += 1 - prvdef;
    last += prvdef;
    dolevel(pivot, first, last);
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
    Xpr[0] = 0;
    cmmnt = 3;
    indent = 4;
    tabsiz = 4;
    prvdef = 0;
    first = 0;
    last = 7;
    pivot = -1;
    // decode the parameters
    while ((c = getopt(argc, argv, "c:de:f:hi:l:p:s:t:v:x:z:")) > 0)
    {
        switch(c)
        {
            case '?':  // unrecognized option
                sprintf(msgerr, "Unknown option: \"%c\".", optopt);
                gerror(msgerr);

            case 'c':  // comment generation
                w = strtol(optarg, &pend, 0);
                if (((*optarg)==0) || ((*pend)!=0) || (w < 0) || (w > 3))
                    gerror("Invalid -b argument. A number between 0 and 3 is expected.");
                cmmnt = w;
                break;

            case 'd':  // default option generation
                prvdef = 1;
                break;

            case 'e':  // "Err" name definition
		strncpy(Err, optarg, 63);
                break;

            case 'f':  // first number to recognize
                w =  strtol(optarg, &pend, 0);
                if (((*optarg)==0) || ((*pend)!=0) || (w < 0) || (w > 1024))
                    gerror("Invalid -f argument. A number between 0 and 1024 is expected.");	
                first = w ;
                break;

            case 'h':  // help
                usage();
                exit(0);

            case 'i':  // increment for the whole construct
                w = strtol(optarg, &pend, 0);
                if (((*optarg)==0) || ((*pend)!=0) || (w < 0) || (w > 40))
                    gerror("Invalid -i argument. A number between 0 and 40 is expected.");
                indent = w;
                break;

            case 'l':  // last number to recognize
                w =  strtol(optarg, &pend, 0);
                if (((*optarg)==0) || ((*pend)!=0) || (w < 0) || (w > 1024))
                    gerror("Invalid -l argument. A number between 0 and 1024 is expected.");	
                last = w ;
                break;

            case 'p':  // pivot to use
                w =  strtol(optarg, &pend, 0);
                if (((*optarg)==0) || ((*pend)!=0) || (w < 0) || (w > 1024))
                    gerror("Invalid -p argument. A number between 0 and 1024 is expected.");	
                pivot = w ;
                break;

            case 's':  // string seperator in comment
		strncpy(Sep, optarg, 63);
                break;

            case 't':  // tabulation for inner command
                w = strtol(optarg, &pend, 0);
                if (((*optarg)==0) || ((*pend)!=0) || (w < 0) || (w > 10))
                    gerror("Invalid -t argument. A number between 0 and 10 is expected.");
                tabsiz = w;
                break;

            case 'v':  // variable name of the value to test
		strncpy(Src, optarg, 63);
                break;

            case 'x':  // executable statement replacing "Err=val;"
		strncpy(Xpr, optarg, 127);
                break;

            case 'z':  // debug option
                dbg = strtol(optarg, &pend, 0);
        }
    }

    if (first >= last) {
         gerror("Invalid -f/-l arguments; first should be less than last.");
    }
    if (pivot < 0) {
        pivot = (first + last + 1) / 2;
    }
    if (pivot < first || pivot > last) {
         gerror("Invalid pivot value.");
    }
    makeselector();
    exit(0);
}


