#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"

// Cameron Bernard
// COP3402
// Spring 2021
// Homework #3 (Parser)

// Declares and Initializes many important variables and functions
instruction *code;
symbol *table;
lexeme token;
int codeIdx = 0;
int emitIdx = 0;
int instIdx = 0;
int symTabIdx = 0;
char *errormsg[] = {
        "",
        "Error : program must end with period\n",
        "Error : const, var, and read keywords must be followed by identifier\n",
        "Error : symbol name has already been declared\n",
        "Error : constants must be assigned with =\n",
        "Error : constants must be assigned an integer value\n",
        "Error : constant and variable declarations must be followed by a semicolon\n",
        "Error : undeclared symbol\n",
        "Error : only variable values may be altered\n",
        "Error : assignment statements must use :=\n",
        "Error : begin must be followed by end\n",
        "Error : if must be followed by then\n",
        "Error : while must be followed by do\n",
        "Error : condition must contain comparison operator\n",
        "Error : right parenthesis must follow left parenthesis\n",
        "Error : arithmetic equations must contain operands, parentheses, numbers, or symbols\n"
};

int symbolTableCheck(char *s);
void program(lexeme *tokenList);
void block(lexeme *tokenList);
void constDec(lexeme *tokenList);
int varDec(lexeme *tokenList);
void statement(lexeme *tokenList);
void condition(lexeme *tokenList);
void expression(lexeme *tokenList);
void term(lexeme *tokenList);
void factor(lexeme *tokenList);
void enter(int kind, char name[12], int val, int level, int addr);
void error(int errorNum);
void emit(int opcode, char op[], int l, int m);

instruction *parse(lexeme *tokenList, int print){

    // Declares and Initializes variables
    code = malloc(500 * sizeof(instruction));
    table = malloc(500 * sizeof(symbol));
    token = tokenList[codeIdx];

    program(tokenList);

    // Prints the assembly
    if(print == 1)
    {
        printf("Generated Assembly:\n");
        printf("Line\tOP\tL\tM\n");
        int i;
        for(i = 0; i < emitIdx; i++)
        {
            printf("%d\t%s\t%d\t%d\n", i, code[i].op, code[i].l, code[i].m);
        }
        printf("\n");
    }
    return code;
}

// Checks the symbol table to see if the string is in the table
// Returns the index of it if it is
// Returns -1 if it is not
int symbolTableCheck(char *s)
{
    int i;
    for(i = 0; i < 500; i++)
    {
        if(strcmp(table[i].name, s) == 0)
        {
            return i;
        }
    }
    return -1;
}

void program(lexeme *tokenList)
{
    // Runs the block
    block(tokenList);
    // Makes sure the last item is a period
    if(token.type != periodsym)
    {
        error(1);
    }
    // Ends the program with a halt instruction
    emit(9, "SYS", 0, 3);

}

void block(lexeme *tokenList)
{
    //Runs the block
    constDec(tokenList);
    int numVars = varDec(tokenList);
    emit(6, "INC", 0, 4 + numVars);//emit INC (M = 4 + numVars)
    statement(tokenList);

}

// NOTE: most of the following code simply reads in the lexemes one at a time,
// making sure that they are followed/prefaced
// by correct lexemes before adding the appropriate instruction
// If it is not followed/prefaced then it gives an appropriate error and stops the program

// For declaring a constant
void constDec(lexeme *tokenList)
{
    if(token.type == constsym)
    {
        do{
            codeIdx++;
            token = tokenList[codeIdx];
            if(token.type != identsym)
            {
                error(9);
            }
            if(symbolTableCheck(token.name) != -1)
            {
                error(7);
            }
            int identNum = token.value;
            char *identName;//Save Ident Name
            strcpy(identName, token.name);
            codeIdx++;
            token = tokenList[codeIdx];
            if(token.type != eqlsym)
            {
                error(4);
            }
            codeIdx++;
            token = tokenList[codeIdx];
            if(token.type != numbersym)
            {
                error(5);
            }
            enter(1, identName, token.value, 0, 0);
            codeIdx++;
            token = tokenList[codeIdx];
        }while(token.type == commasym);
        if(token.type != semicolonsym)
        {
            error(6);
        }
        codeIdx++;
        token = tokenList[codeIdx];
    }
}

// For declaring a variable
int varDec(lexeme *tokenList)
{
    int numVars = 0;
    if(token.type == varsym)
    {
        do{
            numVars++;
            codeIdx++;
            token = tokenList[codeIdx];
            if(token.type != identsym)
            {
                error(9);
            }
            if(symbolTableCheck(token.name) != -1)
            {
                error(3);
            }
            enter(2, token.name, 0, 0, numVars + 3);
            codeIdx++;
            token = tokenList[codeIdx];
        }while(token.type == commasym);
        if(token.type != semicolonsym)
        {
            error(6);
        }
    }
    codeIdx++;
    token = tokenList[codeIdx];
    return numVars;
}

// For statements
void statement(lexeme *tokenList)
{
    int symIdx;
    if(token.type == identsym)
    {
        symIdx = symbolTableCheck(token.name);
        if(symIdx == -1)
        {
            error(7);
        }
        if(table[symIdx].kind != 2)
        {
            error(8);
        }
        codeIdx++;
        token = tokenList[codeIdx];
        if(token.type != becomessym)
        {
            error(9);
        }
        codeIdx++;
        token = tokenList[codeIdx];
        expression(tokenList);
        emit(4, "STO", 0, table[symIdx].addr);//emit STO (M = table[symIdx].addr
        return;
    }
    if(token.type == beginsym)
    {
        do{
            codeIdx++;
            token = tokenList[codeIdx];
            statement(tokenList);
        }while(token.type == semicolonsym);
        if(token.type != endsym)
        {
            error(10);
        }
        codeIdx++;
        token = tokenList[codeIdx];
        return;
    }
    if(token.type == ifsym)
    {
        codeIdx++;
        token = tokenList[codeIdx];
        condition(tokenList);
        int jpcIdx = emitIdx;
        emit(8, "JPC", 0, emitIdx);//emit JPC FIX
        if(token.type != thensym)
        {
            error(11);
        }
        codeIdx++;
        token = tokenList[codeIdx];
        statement(tokenList);
        code[jpcIdx].m = emitIdx;
        return;
    }
    if(token.type == whilesym)
    {
        codeIdx++;
        token = tokenList[codeIdx];
        int loopIdx = emitIdx;
        condition(tokenList);
        if(token.type != dosym)
        {
            error(12);
        }
        codeIdx++;
        token = tokenList[codeIdx];
        int jpcIdx = emitIdx;
        emit(8, "JPC", 0, jpcIdx);//emit JPC FIX
        statement(tokenList);
        emit(7, "JMP", 0, loopIdx);//emit JMP (M = loopIdx)
        code[jpcIdx].m = emitIdx;
        return;
    }
    if(token.type == readsym)
    {
        codeIdx++;
        token = tokenList[codeIdx];
        if(token.type != identsym)
        {
            error(2);
        }
        int symIdx = symbolTableCheck(token.name);
        if(symIdx == -1)
        {
            error(7);
        }
        if(table[symIdx].kind != 2)
        {
            error(8);
        }
        codeIdx++;
        token = tokenList[codeIdx];
        emit(9, "SYS", 0, 2);//emit READ
        emit(4, "STO", 0, table[symIdx].addr);//emit STO( M = table[symIdx].addr)
        return;
    }
    if(token.type == writesym)
    {
        codeIdx++;
        token = tokenList[codeIdx];
        expression(tokenList);
        emit(9, "SYS", 0, 1);//emit WRITE
        return;
    }
}

// For conditions
void condition(lexeme *tokenList)
{
    if(token.type == oddsym)
    {
        codeIdx++;
        token = tokenList[codeIdx];
        expression(tokenList);
        emit(2, "OPR", 0, 6);//emit ODD;
    }
    else
    {
        expression(tokenList);
        if(token.type == eqlsym)
        {
            codeIdx++;
            token = tokenList[codeIdx];
            expression(tokenList);
            emit(2, "OPR", 0, 8);//emit EQL
        }
        else if(token.type == neqsym)
        {
            codeIdx++;
            token = tokenList[codeIdx];
            expression(tokenList);
            emit(2, "OPR", 0, 9);//emit NEQ
        }
        else if(token.type == lessym)
        {
            codeIdx++;
            token = tokenList[codeIdx];
            expression(tokenList);
            emit(2, "OPR", 0, 10);//emit LSS
        }
        else if(token.type == leqsym)
        {
            codeIdx++;
            token = tokenList[codeIdx];
            expression(tokenList);
            emit(2, "OPR", 0, 11);//emit LEQ
        }
        else if(token.type == gtrsym)
        {
            codeIdx++;
            token = tokenList[codeIdx];
            expression(tokenList);
            emit(2, "OPR", 0, 12);//emit GTR
        }
        else if(token.type == geqsym)
        {
            codeIdx++;
            token = tokenList[codeIdx];
            expression(tokenList);
            emit(2, "OPR", 0, 13);//emit GEQ
        }
        else{
            error(13);
        }
    }
}

// For expressions
void expression(lexeme *tokenList)
{
    if(token.type == minussym)
    {
        codeIdx++;
        token = tokenList[codeIdx];
        term(tokenList);
        emit(2, "OPR", 0, 1);//emit NEG
        while(token.type == plussym || token.type == minussym)
        {
            if(token.type == plussym)
            {
                codeIdx++;
                token = tokenList[codeIdx];
                term(tokenList);
                emit(2, "OPR", 0, 2);//emit ADD
            }
            else
            {
                codeIdx++;
                token = tokenList[codeIdx];
                term(tokenList);
                emit(2, "OPR", 0, 3);//emit SUB
            }
        }
    }
    else
    {
        if(token.type == plussym)
        {
            codeIdx++;
            token = tokenList[codeIdx];
        }
        term(tokenList);
        while(token.type == plussym || token.type == minussym)
        {
            if(token.type == plussym)
            {
                codeIdx++;
                token = tokenList[codeIdx];
                term(tokenList);
                emit(2, "OPR", 0, 2);//emit ADD
            }
            else
            {
                codeIdx++;
                token = tokenList[codeIdx];
                term(tokenList);
                emit(2, "OPR", 0, 3);//emit SUB
            }
        }

    }
}

// For terms
void term(lexeme *tokenList)
{
    factor(tokenList);
    while(token.type == multsym || token.type == slashsym || token.type == modsym)
    {
        if(token.type == multsym)
        {
            codeIdx++;
            token = tokenList[codeIdx];
            factor(tokenList);
            emit(2, "OPR", 0, 4);//emit MUL
        }
        else if(token.type == slashsym)
        {
            codeIdx++;
            token = tokenList[codeIdx];
            factor(tokenList);
            emit(2, "OPR", 0, 5);//emit DIV
        }
        else
        {
            codeIdx++;
            token = tokenList[codeIdx];
            factor(tokenList);
            emit(2, "OPR", 0, 7);//emit MOD
        }
    }
}

// For factors
void factor(lexeme *tokenList)
{
    if(token.type == identsym)
    {
        int symIdx = symbolTableCheck(token.name);
        if(symIdx == -1)
        {
            error(7);
        }
        if(table[symIdx].kind == 1)
        {
            emit(1, "LIT", 0, table[symIdx].val);//emit LIT (m = table[symIdx].value
        }
        else
        {
            emit(3, "LOD", 0, table[symIdx].addr);//emit LOD (m = table[symIdx].addr
        }
        codeIdx++;
        token = tokenList[codeIdx];
    }
    else if(token.type == numbersym)
    {
        emit(1, "LIT", 0, token.value);//emit LIT
        codeIdx++;
        token = tokenList[codeIdx];
    }
    else if(token.type == lparentsym)
    {
        codeIdx++;
        token = tokenList[codeIdx];
        expression(tokenList);
        if(token.type != rparentsym)
        {
            error(14);
        }
        codeIdx++;
        token = tokenList[codeIdx];
    }
    else
    {
        error(15);
    }
}

//Enters a new item into the symbol table
void enter(int kind, char name[12], int val, int level, int addr)
{
    if(kind == 1){
        table[symTabIdx].kind = kind;
        strcpy(table[symTabIdx].name, name);
        table[symTabIdx].val = val;
    }
    else
    {
        table[symTabIdx].kind = kind;
        strcpy(table[symTabIdx].name, name);
        table[symTabIdx].level = level;
        table[symTabIdx].addr = addr;
    }
    symTabIdx++;
}

// Prints the appropriate error and then exits the program
void error(int errorNum)
{
    printf(errormsg[errorNum]);
    exit(1);
}

// Emits (adds) the instruction to the output code
void emit(int opcode, char op[], int l, int m)
{
    code[emitIdx].opcode = opcode;
    strcpy(code[emitIdx].op, op);
    code[emitIdx].l = l;
    code[emitIdx].m = m;
    emitIdx++;
}