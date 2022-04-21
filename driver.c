#include "compiler.h"
#include <stdio.h>
#include <stdlib.h>

// Cameron Bernard
// COP3402
// Spring 2021
// Homework #4

int main(int argc, char **argv) {
    //Declaring Variables
    //Note: mostly taken from example program
    int lflag = 0, aflag = 0, vflag = 0;
    FILE *ifp;
    char *inputfile;
    char c;
    int i;
    //error if there is no file name, otherwise sets the print flags
    if (argc < 2)
    {
        printf("Error : please include the file name");
        return 0;
    }
    else if (argc > 4)
    {
        lflag = 1;
        aflag = 1;
        vflag = 1;
    }
    else if (argc == 3 || argc == 4)
    {
        if (argv[2][1] == 'l')
            lflag = 1;
        else if (argv[2][1] == 'a')
            aflag = 1;
        else
            vflag = 1;
        if (argc == 4)
        {
            if (argv[3][1] == 'l')
                lflag = 1;
            else if (argv[3][1] == 'a')
                aflag = 1;
            else
                vflag = 1;
        }
    }
    //opens the file and declares more variables
    ifp = fopen(argv[1], "r");
    inputfile = malloc(500 * sizeof(char));
    c = fgetc(ifp);
    i = 0;
    //reads in the file
    while (c != EOF)
    {
        inputfile[i++] = c;
        c = fgetc(ifp);
    }
    inputfile[i] = '\0';
    //runs the program
    lexeme *list = lex_analyze(inputfile, lflag);
    instruction *codeOut = parse(list, aflag);
    execute(codeOut, vflag);
    return 0;
}