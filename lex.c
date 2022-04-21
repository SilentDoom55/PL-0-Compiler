#include "compiler.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

// Cameron Bernard
// COP3402
// Spring 2021
// Homework #2 (Lexical Analyzer)

//Global variables based on the Lexical Conventions in the homework document
/*typedef enum {
    modsym  =  1,  identsym  =  2,  numbersym  =  3,  plussym  =  4,
    minussym  =  5,  multsym  =  6,  slashsym = 7, oddsym = 8,
    eqlsym = 9, neqsym = 10, lessym = 11, leqsym = 12, gtrsym  =  13,
    geqsym  =  14,  lparentsym  =  15,  rparentsym  =  16,  commasym  =  17,
    semicolonsym  =  18,  periodsym  =  19,  becomessym  =  20,
    beginsym  =  21,  endsym  =  22, ifsym  =  23,  thensym  =  24,
    whilesym  =  25,  dosym  =  26,  callsym  =  27,  constsym  =  28,
    varsym = 29, procsym = 30, writesym = 31, readsym = 32, elsesym = 33, returnsym = 34
} token_type;*/

lexeme* lex_analyze(char *inputFile, int lflag) {
    int input[5000];
    int inputLength = strlen(inputFile);
    lexeme *lex = malloc(500 * sizeof(lexeme));
    int symbols[200];
    int i = 0, j = 0, k = 0;
    char temp;
    int errorList[200];

    // Initializing some of the arrays
    for(i = 0; i < 500; i++)
    {
        errorList[i] = 0;
        lex[i].name = malloc(10 * sizeof(char));
        lex[i].type = 0;
        lex[i].value = 0;
    }
    i = 0;

    // Reading in the file
    for(i = 0; i < inputLength; i++){
        input[i] = (int)inputFile[i];
    }
    i = 0;

    // Main loop
    while(i < inputLength)
    {
        // Filters out comments
        if(input[i] == '/' && input[i + 1] == '*')
        {
            while(!(input[i-1] == '*' && input[i] == '/'))
            {
                i++;
            }
            i++;
        }
        // Makes sure the next character isnt a white space
        if(!isspace(input[i]))
        {
            // This massive if/else tree simply determines what the next character/word/number is and does the appropriate action
            // In addition, it adds the appropriate lexeme and symbol to the lists
            // If it is more than 1 character it increments i by the correct ammount
            if(input[i] == '%')
            {
                lex[j].name = "%";
                lex[j].type = modsym;
            }
            else if (input[i] == '+')
            {
                lex[j].name = "+";
                lex[j].type = plussym;
            }
            else if (input[i] == '-')
            {
                lex[j].name = "-";
                lex[j].type = minussym;
            }
            else if (input[i] == '*')
            {
                lex[j].name = "*";
                lex[j].type = multsym;
            }
            else if (input[i] == '/')
            {
                lex[j].name = "/";
                lex[j].type = slashsym;
            }
            else if (input[i] == '=')
            {
                lex[j].name = "=";
                lex[j].type = eqlsym;
            }
            else if (input[i] == '(')
            {
                lex[j].name = "(";
                lex[j].type = lparentsym;
            }
            else if (input[i] == ')')
            {
                lex[j].name = ")";
                lex[j].type = rparentsym;
            }
            else if (input[i] == ',')
            {
                lex[j].name = ",";
                lex[j].type = commasym;
            }
            else if (input[i] == ';')
            {
                lex[j].name = ";";
                lex[j].type = semicolonsym;
            }
            else if (input[i] == '.')
            {
                lex[j].name = ".";
                lex[j].type = periodsym;
            }
            else if (input[i] == '<')
            {
                if(input[i + 1] == '>')
                {
                    lex[j].name = "<>";
                    lex[j].type = neqsym;
                    i = i + 1;
                }
                else if(input[i + 1] == '=')
                {
                    lex[j].name = "<=";
                    lex[j].type = leqsym;
                    i = i + 1;
                }
                else
                {
                    lex[j].name = "<";
                    lex[j].type = lessym;
                }
            }
            else if (input[i] == '>')
            {
                if(input[i + 1] == '=')
                {
                    lex[j].name = ">=";
                    lex[j].type = geqsym;
                    i = i + 1;
                }
                else
                {
                    lex[j].name = ">";
                    lex[j].type = gtrsym;
                }
            }
            else if(input[i] == ':' && input[i + 1] == '=')
            {
                lex[j].name = ":=";
                lex[j].type = becomessym;
                i = i + 1;
            }
            else if(input[i] == 'b' && input[i + 1] == 'e' && input[i + 2] == 'g' && input[i + 3] == 'i' && input[i + 4] == 'n')
            {
                lex[j].name = "begin";
                lex[j].type = beginsym;
                i = i + 4;
            }
            else if(input[i] == 'c' && input[i + 1] == 'a' && input[i + 2] == 'l'&& input[i + 3] == 'l')
            {
                lex[j].name = "call";
                lex[j].type = callsym;
                i = i + 3;
            }
            else if(input[i] == 'c' && input[i + 1] == 'o' && input[i + 2] == 'n' && input[i + 3] == 's' && input[i + 4] == 't')
            {
                lex[j].name = "const";
                lex[j].type = constsym;
                i = i + 4;
            }
            else if(input[i] == 'd' && input[i + 1] == 'o')
            {
                lex[j].name = "do";
                lex[j].type = dosym;
                i = i + 1;
            }
            else if(input[i] == 'e' && input[i + 1] == 'l' && input[i + 2] == 's' && input[i + 3] == 'e')
            {
                lex[j].name = "else";
                lex[j].type = elsesym;
                i = i + 3;
            }
            else if(input[i] == 'e' && input[i + 1] == 'n' && input[i + 2] == 'd')
            {
                lex[j].name = "end";
                lex[j].type = endsym;
                i = i + 2;
            }
            else if(input[i] == 'i' && input[i + 1] == 'f')
            {
                lex[j].name = "if";
                lex[j].type = ifsym;
                i = i + 1;
            }
            else if(input[i] == 'o' && input[i + 1] == 'd' && input[i + 2] == 'd')
            {
                lex[j].name = "odd";
                lex[j].type = oddsym;
                i = i + 2;
            }
            else if(input[i] == 'p' && input[i + 1] == 'r' && input[i + 2] == 'o' && input[i + 3] == 'c' && input[i + 4] == 'e' && input[i + 5] == 'd' && input[i + 6] == 'u' && input[i + 7] == 'r' && input[i + 8] == 'e')
            {
                lex[j].name = "procedure";
                lex[j].type = procsym;
                i = i + 8;
            }
            else if(input[i] == 'r' && input[i + 1] == 'e' && input[i + 2] == 'a' && input[i + 3] == 'd')
            {
                lex[j].name = "read";
                lex[j].type = readsym;
                i = i + 3;
            }
            else if(input[i] == 'r' && input[i + 1] == 'e' && input[i + 2] == 't' && input[i + 3] == 'u' && input[i + 4] == 'r' && input[i + 5] == 'n')
            {
                lex[j].name = "return";
                lex[j].type = returnsym;
                i = i + 5;
            }
            else if(input[i] == 't' && input[i + 1] == 'h' && input[i + 2] == 'e' && input[i + 3] == 'n')
            {
                lex[j].name = "then";
                lex[j].type = thensym;
                i = i + 3;
            }
            else if(input[i] == 'v' && input[i + 1] == 'a' && input[i + 2] == 'r' && input[i+3] != 'i')
            {
                lex[j].name = "var";
                lex[j].type = varsym;
                i = i + 2;
            }
            else if(input[i] == 'w' && input[i + 1] == 'h' && input[i + 2] == 'i' && input[i + 3] == 'l' && input[i + 4] == 'e')
            {
                lex[j].name = "while";
                lex[j].type = whilesym;
                i = i + 4;
            }
            else if(input[i] == 'w' && input[i + 1] == 'r' && input[i + 2] == 'i' && input[i + 3] == 't' && input[i + 4] == 'e')
            {
                lex[j].name = "write";
                lex[j].type = writesym;
                i = i + 4;
            }
                // Determines if it is an appropriate identifier
            else if(isalpha(input[i]))
            {
                k = 0;
                int identifier[100];
                char *idenShort = (char*)malloc(12*sizeof(char));
                int stop = 0;
                while(isalpha(input[i]) || isdigit(input[i]))
                {
                    if(k > 11)
                    {
                        stop = 1;
                    }
                    identifier[k] = input[i];
                    i++;
                    k++;
                }
                if(!stop)
                {
                    idenShort[k] = '\0';
                    k--;
                    while(k >= 0)
                    {
                        idenShort[k] = identifier[k];
                        k--;
                    }
                    lex[j].name = idenShort;
                    lex[j].type = identsym;
                }
                else {
                    //Error : Identifier names cannot exceed 11 characters
                    errorList[j] = 1;
                }
                i--;
            }
                // Determines if it is an appropriate number
            else if(isdigit(input[i]))
            {
                int digit[100];
                char *num = (char*)malloc(6*sizeof(char));
                int stop = 0;
                k = 0;
                while(isalpha(input[i]) || isdigit(input[i]))
                {
                    if(isalpha(input[i]))
                    {
                        stop = 1;
                    }
                    digit[k] = input[i];
                    i++;
                    k++;
                }
                if(k >= 5)
                {
                    //Numbers cannot exceed 5 digits error
                    errorList[j] = 3;
                }
                else if(!stop)
                {
                    num[k] = '\0';
                    k--;
                    while(k >= 0)
                    {
                        num[k] = digit[k];
                        k--;
                    }
                    lex[j].name = num;
                    lex[j].type = numbersym;
                    lex[j].value = strtol(num, &num, 10);
                }
                else {
                    //Error: Identifiers cannot begin with a digit
                    errorList[j] = 4;
                }
                i--;
            }
            else
            {
                // Error: Invalid Symbol
                errorList[j] = 2;
            }
            j++;
        }
        i++;
    }

    // Prints out the lexeme table'
    if(lflag == 1)
    {
        printf("Lexeme Table:\n");
        printf("\t lexeme\t\ttoken type\n");
    }
    for(int l = 0; l < j; l++)
    {
        //First will throw the error if possible before going to the next item
        if(errorList[l] != 0)
        {
            if(errorList[l] == 1)
            {
                printf("Error : Identifier names cannot exceed 11 characters\n");
            }
            if(errorList[l] == 2)
            {
                printf("Error : Invalid Symbol\n");
            }
            if(errorList[l] == 3)
            {
                printf("Error : Numbers cannot exceed 5 digits\n");
            }
            if(errorList[l] == 4)
            {
                printf("Error : Identifiers cannot begin with a digit\n");
            }
            continue;
        }
        // Prints the lexeme and symbol if there is no error
        if(lflag == 1)
            printf("%15s\t\t%d\n", lex[l].name, lex[l].type);
    }

    if(lflag == 1)
    {
        //Prints the token list
        printf("\nToken List:\n");
        for(int l = 0; l < j; l++)
        {
            // If this occurs that means that there was an error with this
            if(lex[l].type == 0)
            {
                continue;
            }

            //Prints the symbol and variable/identifier if appropriate
            printf("%d ", lex[l].type);
            if(lex[l].type == 2 || lex[l].type == 3)
            {
                printf("%s ", lex[l].name);
            }
        }
        printf("\n\n");
    }


    return lex;
}