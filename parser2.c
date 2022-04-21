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
int symTabIdx = 0;
int procedureCount = 0;
char *errormsg[] = {
        "",
        "Error : program must end with period\n",
        "Error : const, var, and read keywords must be followed by identifier\n",
        "Error : competing symbol declarations at the same level\n",
        "Error : constants must be assigned with =\n",
        "Error : constants must be assigned an integer value\n",
        "Error : constant and variable declarations must be followed by a semicolon\n",
        "Error : undeclared variable or constant in equation\n",
        "Error : only variable values may be altered\n",
        "Error : assignment statements must use :=\n",
        "Error : begin must be followed by end\n",
        "Error : if must be followed by then\n",
        "Error : while must be followed by do\n",
        "Error : condition must contain comparison operator\n",
        "Error : right parenthesis must follow left parenthesis\n",
        "Error : arithmetic equations must contain operands, parentheses, numbers, or symbols\n",
        "Error : undeclared procedure for call\n",
        "Error : parameters may only be specified by an identifier\n",
        "Error : parameters must be declared\n",
        "Error : cannot return from main\n"
};

void printTable();
void printCode();
int findProcedure(int i);
void mark(int count);
int symbolTableCheck(char *s, int level);
int symbolTableSearch(char *s, int level, int kind);
void program(lexeme *tokenList);
void block(lexeme *tokenList, int level, int param, int procedureIndex);
int constDec(lexeme *tokenList, int level);
int varDec(lexeme *tokenList, int level, int param);
int procedureDec(lexeme *tokenList, int level);
void statement(lexeme *tokenList, int level);
void condition(lexeme *tokenList, int level);
void expression(lexeme *tokenList, int level);
void term(lexeme *tokenList, int level);
void factor(lexeme *tokenList, int level);
void enter(int kind, char name[12], int val, int level, int addr, int mark, int param);
void error(int errorNum);
void emit(int opcode, char op[], int l, int m);

int findProcedure(int idx)
{
    int i;
    for(i = 0; i < 500; i++)
    {
        if(table[i].kind == 3)
        {
            if(idx == 0)
            {
                return i;
            }
            idx--;
        }
    }
    return -1;
}

void mark(int count)
{
    int i = symTabIdx;
    for(; count > 0; i--)
    {
        if(table[i].mark == 0)
        {
            table[i].mark = 1;
            count--;
        }
        if(i == 0)
        {
            return;
        }
    }

}

// Checks the symbol table to see if the string is in the table
// Returns the index of it if it is
// Returns -1 if it is not
int symbolTableCheck(char *s, int level)
{
    int i;
    for(i = 0; i < 500; i++)
    {
        if(strcmp(table[i].name, s) == 0 && table[i].level == level && table[i].mark == 0)
        {
            return i;
        }
    }
    return -1;
}

int symbolTableSearch(char *s, int level, int kind)
{
    int i;
    for(i = 0; i < 500; i++)
    {
        if(strcmp(table[i].name, s) == 0 && table[i].level == level && table[i].kind == kind && table[i].mark == 0)
        {
            return i;
        }
    }
    if(level != 0)
    {
        return(s, level - 1, kind);
    }
    return -1;
}


instruction *parse(lexeme *tokenList, int print){
    printf("\n");
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
    //printTable();
    return code;
}

void program(lexeme *tokenList)
{
    int numProc = 1;
    int i;
    lexeme tempT;
    emit(7, "JMP", 0, -1);
    for(i = 0; i < 500; i++)
    {
        tempT = tokenList[i];
        if(tempT.type == procsym)
        {
            numProc++;
            emit(7, "JMP", 0, -1);
        }
    }
    enter(3, "main", 0, 0, 0, 0, 0);
    procedureCount++;
    // Runs the block
    block(tokenList, 0, 0, 0);
    // Makes sure the last item is a period
    if(token.type != periodsym)
    {
        error(1);
    }
    for(i = 0; i < numProc; i++)
    {
        code[i].m = table[findProcedure(i)].addr;
    }
    //printTable();
    //printCode();
    for(i = 0; i < 500; i++)
    {
        if(code[i].opcode == 5)
        {
            code[i].m = table[findProcedure(code[i].m)].addr;
        }
    }
    // Ends the program with a halt instruction
    emit(9, "SYS", 0, 3);

}

void block(lexeme *tokenList, int level, int param, int procedureIndex)
{
    //Runs the block
    int c = constDec(tokenList, level);
    int v = varDec(tokenList, level, param);
    int p = procedureDec(tokenList, level);
    table[procedureIndex].addr = emitIdx;
    emit(6, "INC", level, 4 + v);//emit INC (M = 4 + numVars)
    statement(tokenList, level);
    if(token.value == semicolonsym)
    {
        codeIdx++;
        token = tokenList[codeIdx];
    }
    mark(c + v + p);
}

// NOTE: most of the following code simply reads in the lexemes one at a time,
// making sure that they are followed/prefaced
// by correct lexemes before adding the appropriate instruction
// If it is not followed/prefaced then it gives an appropriate error and stops the program

// For declaring a constant
int constDec(lexeme *tokenList, int level)
{
    int numConst = 0;
    if(token.type == constsym)
    {
        do{
            numConst++;
            codeIdx++;
            token = tokenList[codeIdx];
            if(token.type != identsym)
            {
                error(9);
            }
            if(symbolTableCheck(token.name, level) != -1)
            {
                error(7);
            }
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
            enter(1, identName, token.value, level, 0, 0, 0);
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
    return numConst;
}

// For declaring a variable
int varDec(lexeme *tokenList, int level, int param)
{
    int numVars = 0;
    if(param == 1)
    {
        numVars++;
    }
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
            if(symbolTableCheck(token.name, level) != -1)
            {
                error(3);
            }
            enter(2, token.name, 0, level, numVars + 3, 0, 0);
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
    return numVars;
}

int procedureDec(lexeme *tokenList, int level)
{
    int numProc = 0;
    int procIdx;
    if(token.type == procsym)
    {
        do{
            numProc++;
            codeIdx++;
            token = tokenList[codeIdx];
            if(token.type != identsym)
            {
                error(9);
            }
            if(symbolTableCheck(token.name, level) != -1)
            {
                error(3);
            }
            procIdx = symTabIdx;
            enter(3, token.name, procedureCount, level, 0 , 0, 0);
            procedureCount++;
            codeIdx++;
            token = tokenList[codeIdx];
            if(token.type == lparentsym)
            {
                codeIdx++;
                token = tokenList[codeIdx];
                if(token.type != identsym)
                {
                    error(17);
                }
                enter(2, token.name, 0, level + 1, 3, 0, 0);
                table[procIdx].param = 1;
                codeIdx++;
                token = tokenList[codeIdx];
                if(token.type != rparentsym)
                {
                    error(14);
                }
                codeIdx++;
                token = tokenList[codeIdx];
                if(token.type != semicolonsym)
                {
                    error(6);
                }
                codeIdx++;
                token = tokenList[codeIdx];
                block(tokenList, level + 1, 1, procIdx);
            }
            else
            {
                if(token.type != semicolonsym)
                {
                    error(6);
                }
                codeIdx++;
                token = tokenList[codeIdx];
                block(tokenList, level + 1, 0, procIdx);
                if(code[emitIdx - 1].opcode != 2 && code[emitIdx - 1].m != 0)
                {
                    emit(1, "LIT", level, 0);
                    emit(2, "RTN", level, 0);//Emit RTN
                }
                if(token.type != semicolonsym)
                {
                    error(6);
                }
                codeIdx++;
                token = tokenList[codeIdx];
            }
        }while(token.type == procsym);

    }
    return numProc;
}

// For statements
void statement(lexeme *tokenList, int level)
{
    int symIdx;
    if(token.type == identsym)
    {
        symIdx = symbolTableSearch(token.name, level, 2);
        if(symIdx == -1)
        {
            printf("statement, %s %d\n", token.name, level);

            error(7);
        }
        codeIdx++;
        token = tokenList[codeIdx];
        if(token.type != becomessym)
        {
            error(9);
        }
        codeIdx++;
        token = tokenList[codeIdx];
        expression(tokenList, level);
        emit(4, "STO", level - table[symIdx].level, table[symIdx].addr);//emit STO (M = table[symIdx].addr
        return;
    }
    if(token.type == callsym)
    {
        codeIdx++;
        token = tokenList[codeIdx];
        if(token.type != identsym)
        {
            error(16);
        }
        symIdx = symbolTableSearch(token.name, level, 3);
        if(symIdx == -1)
        {
            error(7);
        }
        codeIdx++;
        token = tokenList[codeIdx];
        if(token.type == lparentsym)
        {
            codeIdx++;
            token = tokenList[codeIdx];
            if(table[symIdx].param != 1)
            {
                error(18);
            }
            expression(tokenList, level);
            if(token.type != rparentsym)
            {
                error(14);
            }
            codeIdx++;
            token = tokenList[codeIdx];
        } else
        {
            emit(1, "LIT", level, 0);
        }
        emit(5, "CAL", level - table[symIdx].level, table[symIdx].val);//emit CAL (L = lexlevel - symboltable[symIdx].level, M = symboltable[symIdx].value)
        return;
    }
    if(token.type == returnsym)
    {
        if(level == 0)
        {
            error(19);
        }
        codeIdx++;
        token = tokenList[codeIdx];
        if(token.type = lparentsym)
        {
            codeIdx++;
            token = tokenList[codeIdx];
            expression(tokenList, level);
            emit(2, "RTN", level, 0);//emit RTN
            if(token.type != rparentsym)
            {
                error(14);
            }
            codeIdx++;
            token = tokenList[codeIdx];
        } else
        {
            emit(1, "LIT", level, 0);//emit LIT 0
            emit(2, "RTN", level, 0);//emit RTN
        }
        return;
    }
    if(token.type == beginsym)
    {
        do{
            codeIdx++;
            token = tokenList[codeIdx];
            statement(tokenList, level);
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
        condition(tokenList, level);
        int jpcIdx = emitIdx;
        emit(8, "JPC", level, emitIdx);//emit JPC FIX
        if(token.type != thensym)
        {
            error(11);
        }
        codeIdx++;
        token = tokenList[codeIdx];
        statement(tokenList, level);
        if(token.type == elsesym)
        {
            codeIdx++;
            token = tokenList[codeIdx];
            int jmpIdx = emitIdx;
            emit(7, "JMP", level, 0);//emit JMP
            code[jpcIdx].m = emitIdx;
            statement(tokenList, level);
            code[jmpIdx].m = emitIdx;
        } else
        {
            code[jpcIdx].m = emitIdx;
        }
        return;
    }
    if(token.type == whilesym)
    {
        codeIdx++;
        token = tokenList[codeIdx];
        int loopIdx = emitIdx;
        condition(tokenList, level);
        if(token.type != dosym)
        {
            error(12);
        }
        codeIdx++;
        token = tokenList[codeIdx];
        int jpcIdx = emitIdx;
        emit(8, "JPC", level, jpcIdx);//emit JPC FIX
        statement(tokenList, level);
        emit(7, "JMP", level, loopIdx);//emit JMP (M = loopIdx)
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
        int symIdx = symbolTableSearch(token.name, level, 2);
        if(symIdx == -1)
        {
            error(7);
        }
        codeIdx++;
        token = tokenList[codeIdx];
        emit(9, "SYS", level, 2);//emit READ
        emit(4, "STO", level, table[symIdx].addr);//emit STO( M = table[symIdx].addr)
        return;
    }
    if(token.type == writesym)
    {
        codeIdx++;
        token = tokenList[codeIdx];
        expression(tokenList, level);
        emit(9, "SYS", level, 1);//emit WRITE
        return;
    }
}

// For conditions
void condition(lexeme *tokenList, int level)
{
    if(token.type == oddsym)
    {
        codeIdx++;
        token = tokenList[codeIdx];
        expression(tokenList, level);
        emit(2, "OPR", level, 6);//emit ODD;
    }
    else
    {
        expression(tokenList, level);
        if(token.type == eqlsym)
        {
            codeIdx++;
            token = tokenList[codeIdx];
            expression(tokenList, level);
            emit(2, "OPR", level, 8);//emit EQL
        }
        else if(token.type == neqsym)
        {
            codeIdx++;
            token = tokenList[codeIdx];
            expression(tokenList, level);
            emit(2, "OPR", level, 9);//emit NEQ
        }
        else if(token.type == lessym)
        {
            codeIdx++;
            token = tokenList[codeIdx];
            expression(tokenList, level);
            emit(2, "OPR", level, 10);//emit LSS
        }
        else if(token.type == leqsym)
        {
            codeIdx++;
            token = tokenList[codeIdx];
            expression(tokenList, level);
            emit(2, "OPR", level, 11);//emit LEQ
        }
        else if(token.type == gtrsym)
        {
            codeIdx++;
            token = tokenList[codeIdx];
            expression(tokenList, level);
            emit(2, "OPR", level, 12);//emit GTR
        }
        else if(token.type == geqsym)
        {
            codeIdx++;
            token = tokenList[codeIdx];
            expression(tokenList, level);
            emit(2, "OPR", level, 13);//emit GEQ
        }
        else{
            error(13);
        }
    }
}

// For expressions
void expression(lexeme *tokenList, int level)
{
    if(token.type == minussym)
    {
        codeIdx++;
        token = tokenList[codeIdx];
        term(tokenList, level);
        emit(2, "OPR", level, 1);//emit NEG
        while(token.type == plussym || token.type == minussym)
        {
            if(token.type == plussym)
            {
                codeIdx++;
                token = tokenList[codeIdx];
                term(tokenList, level);
                emit(2, "OPR", level, 2);//emit ADD
            }
            else
            {
                codeIdx++;
                token = tokenList[codeIdx];
                term(tokenList, level);
                emit(2, "OPR", level, 3);//emit SUB
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
        term(tokenList, level);
        while(token.type == plussym || token.type == minussym)
        {
            if(token.type == plussym)
            {
                codeIdx++;
                token = tokenList[codeIdx];
                term(tokenList, level);
                emit(2, "OPR", level, 2);//emit ADD
            }
            else
            {
                codeIdx++;
                token = tokenList[codeIdx];
                term(tokenList, level);
                emit(2, "OPR", level, 3);//emit SUB
            }
        }

    }
}

// For terms
void term(lexeme *tokenList, int level)
{
    factor(tokenList,  level);
    while(token.type == multsym || token.type == slashsym || token.type == modsym)
    {
        if(token.type == multsym)
        {
            codeIdx++;
            token = tokenList[codeIdx];
            factor(tokenList, level);
            emit(2, "OPR", level, 4);//emit MUL
        }
        else if(token.type == slashsym)
        {
            codeIdx++;
            token = tokenList[codeIdx];
            factor(tokenList, level);
            emit(2, "OPR", level, 5);//emit DIV
        }
        else
        {
            codeIdx++;
            token = tokenList[codeIdx];
            factor(tokenList, level);
            emit(2, "OPR", level, 7);//emit MOD
        }
    }
}

// For factors
void factor(lexeme *tokenList, int level)
{
    if(token.type == identsym)
    {
        int symIdxV = symbolTableSearch(token.name, level, 2);
        int symIdxC = symbolTableSearch(token.name, level, 1);
        if(symIdxV == -1 && symIdxC == -1)
        {
            error(7);
        }
        else if(symIdxC == -1 || (symIdxV != -1 && table[symIdxV].level > table[symIdxC].level))
        {
            emit(3, "LOD", level - table[symIdxV].level, table[symIdxV].addr);//emit LOD (L = lexlevel - symboltable[symIdxV].level, M = symboltable[symIdxV].addr)
        }
        else
        {
            emit(1, "LIT", level, table[symIdxC].val);//emit LIT (M = symboltable[symIdxC].value)
        }
        codeIdx++;
        token = tokenList[codeIdx];
    }
    else if(token.type == numbersym)
    {
        emit(1, "LIT", level, token.value);//emit LIT
        codeIdx++;
        token = tokenList[codeIdx];
    }
    else if(token.type == lparentsym)
    {
        codeIdx++;
        token = tokenList[codeIdx];
        expression(tokenList, level);
        if(token.type != rparentsym)
        {
            error(14);
        }
        codeIdx++;
        token = tokenList[codeIdx];
    }
    else if(token.type == callsym)
    {
        statement(tokenList, level);
    }
    else
    {
        error(15);
    }
}

//Enters a new item into the symbol table
void enter(int kind, char name[12], int val, int level, int addr, int mark, int param)
{
    if(kind == 1){
        table[symTabIdx].kind = kind;
        strcpy(table[symTabIdx].name, name);
        table[symTabIdx].val = val;
        table[symTabIdx].mark = mark;
        table[symTabIdx].param = param;
    }
    else if(kind == 2)
    {
        table[symTabIdx].kind = kind;
        strcpy(table[symTabIdx].name, name);
        table[symTabIdx].level = level;
        table[symTabIdx].addr = addr;
        table[symTabIdx].mark = mark;
        table[symTabIdx].param = param;
    }
    else if(kind == 3)
    {
        table[symTabIdx].kind = kind;
        strcpy(table[symTabIdx].name, name);
        table[symTabIdx].val = val;
        table[symTabIdx].level = level;
        table[symTabIdx].addr = addr;
        table[symTabIdx].mark = mark;
        table[symTabIdx].param = param;
    }
    symTabIdx++;
}

// Prints the appropriate error and then exits the program
void error(int errorNum)
{
    printf(errormsg[errorNum]);
    printTable();
    printCode();
    printf("%d\n", codeIdx);
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

void printTable()
{
    int i;
    printf("kind\tname\tval\tlevel\taddr\tmark\tparam\n");
    for(i = 0; i < 20; i++)
    {
        printf("%d\t%s\t%d\t%d\t%d\t%d\t%d\n", table[i].kind, table[i].name, table[i].val, table[i].level, table[i].addr, table[i].mark, table[i].param);
    }
}
void printCode()
{
    printf("Line\tOP\tL\tM\n");
    int i;
    for(i = 0; i < emitIdx; i++)
    {
        printf("%d\t%s\t%d\t%d\n", i, code[i].op, code[i].l, code[i].m);
    }
    printf("\n");
}