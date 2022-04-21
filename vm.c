#include "compiler.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<strings.h>

#define MAX_STACK_HEIGHT = 50;
#define MAX_CODE_LENGTH = 100;

// Cameron Bernard
// COP3402
// Spring 2021
// Homework #1 (P-Machine)
int temp123 = 0;
int base(int stack[], int level, int BP);

void execute(instruction *inst, int vflag) {
    // Setting default values given by assignment
    int i = 0;
    int stack[50] = {0};
    int SP = -1;
    int BP = 0;
    int PC = 0;
    instruction IR;

    // Controls when the program ends
    int halt = 0;

    // Tracks the input line to be printed later.
    int inputLine = 0;

    // Tracks where the Activation Record Bars should be placed in the output
    int ARbars[50] = {0};

    // Printing headers and initial values
    if(vflag == 1)
    {
        printf("\t\tPC   BP   SP   stack\n");
        printf("Initial values: %d    %d    %d   \n", PC, BP, SP);
    }

    // Continues until a halt flag is thrown by a 9 0 3 instruction
    while(!halt){
        // Fetch Cycle
        IR = inst[PC];
        inputLine = PC;
        PC = PC + 1;

        // Execute Cycle
        switch(IR.opcode)
        {
            // Pushes a constant value IR.m onto the stack
            case 1:
                strcpy(IR.op, "LIT");
                SP = SP + 1;
                stack[SP] = IR.m;
                break;
            // Operation to be performed on the data at the top of the stack.
            case 2:
                if(IR.m == 0)
                {
                    strcpy(IR.op, "RTN");
                    ARbars[BP - 1] = 0;
                    stack[BP - 1] = stack[SP];
                    SP = BP - 1;
                    BP = stack[SP + 2];
                    PC = stack[SP + 3];
                }
                if(IR.m == 1)
                {
                    strcpy(IR.op, "NEG");
                    stack[SP] = -1 * stack[SP];
                }
                if(IR.m == 2)
                {
                    strcpy(IR.op, "ADD");
                    SP = SP - 1;
                    stack[SP] = stack[SP] + stack[SP + 1];
                }
                if(IR.m == 3)
                {
                    strcpy(IR.op, "SUB");
                    SP = SP - 1;
                    stack[SP] = stack[SP] - stack[SP + 1];
                }
                if(IR.m == 4)
                {
                    strcpy(IR.op, "MUL");
                    SP = SP - 1;
                    stack[SP] = stack[SP] * stack[SP + 1];
                }
                if(IR.m == 5)
                {
                    strcpy(IR.op, "DIV");
                    SP = SP - 1;
                    stack[SP] = stack[SP] / stack[SP + 1];
                }
                if(IR.m == 6)
                {
                    strcpy(IR.op, "ODD");
                    stack[SP] = stack[SP] % 2;
                }
                if(IR.m == 7)
                {
                    strcpy(IR.op, "MOD");
                    SP = SP - 1;
                    stack[SP] = stack[SP] % stack[SP + 1];
                }
                if(IR.m == 8)
                {
                    strcpy(IR.op, "EQL");
                    SP = SP - 1;
                    stack[SP] = stack[SP] == stack[SP + 1];
                }
                if(IR.m == 9)
                {
                    strcpy(IR.op, "NEQ");
                    SP = SP - 1;
                    stack[SP] = stack[SP] != stack[SP + 1];
                }
                if(IR.m == 10)
                {
                    strcpy(IR.op, "LSS");
                    SP = SP - 1;
                    stack[SP] = stack[SP] < stack[SP + 1];
                }
                if(IR.m == 11)
                {
                    strcpy(IR.op, "LEQ");
                    SP = SP - 1;
                    stack[SP] = stack[SP] <= stack[SP + 1];
                }
                if(IR.m == 12)
                {
                    strcpy(IR.op, "GTR");
                    SP = SP - 1;
                    stack[SP] = stack[SP] > stack[SP + 1];
                }
                if(IR.m == 13)
                {
                    strcpy(IR.op, "GEQ");
                    SP = SP - 1;
                    stack[SP] = stack[SP] >= stack[SP + 1];
                }
                strcpy(IR.op, "OPR");
                break;
            // Load value to top of stack from the stack location at offset IR.m from IR.l lexicographical levels down
            case 3:
                strcpy(IR.op, "LOD");
                SP = SP + 1;
                stack[SP] = stack[base(stack, IR.l, BP) + IR.m];
                break;
            // Store value at top of stack in the stack location at offset IR.m from IR.l lexicographical levels down
            case 4:
                strcpy(IR.op, "STO");
                stack[base(stack, IR.l, BP) + IR.m] = stack[SP];
                SP = SP - 1;
                break;
            // Call procedure at code index IR.m (generates new Activation Record and PC = IR.m)
            // Additionally, notes where this AR begins to mark it in the stack printout
            case 5:
                strcpy(IR.op, "CAL");
                ARbars[SP] = 1;
                stack[SP + 1] = base(stack, IR.l, BP);      // Static Link (SL)
                stack[SP + 2] = BP;                         // Dynamic Link (DL)
                stack[SP + 3] = PC;                         // Return Address (RA)
                stack[SP + 4] = stack[SP];                  // Parameter (P)
                BP = SP + 1;
                PC = IR.m;
                break;
            // Allocate IR.m memory words (increment SP by IR.m).
            case 6:
                strcpy(IR.op, "INC");
                SP = SP + IR.m;
                break;
            // Jump to instruction IR.m (PC = IR.m)
            case 7:
                strcpy(IR.op, "JMP");
                PC = IR.m;
                break;
            // Jump to instruction IR.m if top stack element is 0
            case 8:
                strcpy(IR.op, "JPC");
                if(stack[SP] == 0)
                    PC = IR.m;
                SP = SP - 1;
                break;
            case 9:
                strcpy(IR.op, "SYS");
                // Write the top stack element to the screen
                if(IR.m == 1)
                {
                    printf("Top of Stack Value: %d\n", stack[SP]);
                    SP = SP - 1;
                }
                // Read in input from the user and store it on top of the stack
                if(IR.m == 2)
                {
                    SP = SP + 1;
                    printf("Please Enter an Integer: ");
                    scanf("%d", &stack[SP]);
                }
                // End of program (Set Halt flag to zero)
                if(IR.m == 3)
                {
                    halt = 1;
                }
                break;
            default:
                break;
        }

        if(vflag == 1)
        {
            if(inputLine >= 10)
            {
                printf("%d %s  %d", inputLine, IR.op, IR.l);
            }
            else
            {
                printf(" %d %s  %d", inputLine, IR.op, IR.l);
            }
            if(IR.m >= 10)
            {
                printf(" %d", IR.m);
            }
            else
            {
                printf("  %d", IR.m);
            }
            if(PC >= 10)
            {
                printf("    %d", PC);
            }
            else
            {
                printf("     %d", PC);
            }
            if(BP >= 10)
            {
                printf("   %d", BP);
            }
            else
            {
                printf("    %d", BP);
            }
            if(SP >= 10 || SP == -1)
            {
                printf("   %d   ", SP);
            }
            else
            {
                printf("    %d   ", SP);
            }


            // Prints the stack with pipes to indicate ARs
            for(i = 0; i <= SP; i++)
            {
                if(i > 0)
                    if(ARbars[i - 1] == 1)
                        printf("| ");
                printf("%d", stack[i]);
                if(i != SP)
                    printf(" ");
            }
            printf("\n");
            /*temp123++;
            if(temp123 >= 50)
            {
                exit(1);
            }*/
        }
        //Prints the last instruction with proper formatting

    }

    // Prints an empty line at the end of the program
    if(vflag == 1)
        printf("\n ");
}

// Finds base L levels down
// Given by Appendix D
int base(int stack[], int level, int BP)
{
    int base = BP;
    int counter = level;
    while (counter > 0)
    {
        base = stack[base];
        counter--;
    }
    return base;
}