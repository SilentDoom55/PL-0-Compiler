typedef enum {
    modsym  =  1,  identsym  =  2,  numbersym  =  3,  plussym  =  4,
    minussym  =  5,  multsym  =  6,  slashsym = 7, oddsym = 8,
    eqlsym = 9, neqsym = 10, lessym = 11, leqsym = 12, gtrsym  =  13,
    geqsym  =  14,  lparentsym  =  15,  rparentsym  =  16,  commasym  =  17,
    semicolonsym  =  18,  periodsym  =  19,  becomessym  =  20,
    beginsym  =  21,  endsym  =  22, ifsym  =  23,  thensym  =  24,
    whilesym  =  25,  dosym  =  26,  callsym  =  27,  constsym  =  28,
    varsym = 29, procsym = 30, writesym = 31, readsym = 32, elsesym = 33, returnsym = 34
} token_type;

typedef struct lexeme
{
	char *name;
	int value;
	int type;
	
} lexeme;

typedef struct instruction
{
	int opcode;
	char op[4];
	int l;
	int m;
	
} instruction;

typedef struct symbol
{
	int kind;
	char name[10];
	int val;
	int level;
	int addr;
	int mark;
	int param;
} symbol;


lexeme* lex_analyze(char *inputfile, int print);
void execute(instruction *code, int print);
instruction *parse(lexeme *tokenList, int print);