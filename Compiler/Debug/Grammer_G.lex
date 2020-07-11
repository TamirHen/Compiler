%option noyywrap

%{
#include "Token.h";

char* eTokensString[] =
{
	"NUM_INT_TOKEN",			 /* 0 */
	"NUM_FLOAT_TOKEN",			 /* 1 */
	"ADD_OP_TOKEN",				 /* 2 (+) */
	"MUL_OP_TOKEN",				 /* 3 (*) */
	"LESS_OP_TOKEN",			 /* 4 (<) */
	"GREAT_OP_TOKEN",			 /* 5 (>) */
	"LOE_OP_TOKEN",				 /* 6 (<=) */
	"GOE_OP_TOKEN",				 /* 7 (>=) */
	"EQUAL_OP_TOKEN",			 /* 8 (==) */
	"NOT_EQUAL_OP_TOKEN",		 /* 9 (!=) */
	"ASSIGNMENT_OP_TOKEN",		 /* 10 (=) */
	"INT_TOKEN",				 /* 11 (int) */
	"FLOAT_TOKEN",				 /* 12 (float) */
	"VOID_TOKEN",				 /* 13 (void) */
	"IF_TOKEN",					 /* 14 (if) */
	"RETURN_TOKEN",				 /* 15 (return) */
	"ID_TOKEN",					 /* 16 */
	"COMMA_TOKEN",				 /* 17 (",) */
	"COLON_TOKEN",				 /* 18 (:) */
	"SEMICOLON_TOKEN",			 /* 19 (;) */
	"OPEN_PARENTHESES_TOKEN",	 /* 20 ('(') */
	"CLOSE_PARENTHESES_TOKEN",	 /* 21 (')') */
	"OPEN_BRACKETS_TOKEN",		 /* 22 ('[') */
	"CLOSE_BRACKETS_TOKEN",		 /* 23 (']') */
	"OPEN_CURLY_BRACES_TOKEN",	 /* 24 ('{') */
	"CLOSE_CURLY_BRACES_TOKEN",	 /* 25 ('}') */
	"EOF_TOKEN"					 /* 26 */
};
	/* C Declarations (define, global vars etc...)*/
int lineCounter = 1;

%}
	/* Lexical Definitions */
DIGIT    [0-9]
LCALPHA	 [a-z]
UCALPHA	 [A-Z]
%x comment
%%

"/*"	BEGIN(comment);
<comment>\n		++lineCounter;
<comment>[^"*"\n]*	{}
<comment>"*"+"/"	{BEGIN(0);}

	/* Lexical Translation Rules */
<<EOF>>    {create_and_store_token(EOF_TOKEN, "", lineCounter); fprintf(yyout ,"Token of type %s, lexeme: EOF, found in line: %d\n", eTokensString[EOF_TOKEN], lineCounter); return 0;}
\n		lineCounter++;
([1-9]{DIGIT}*)|0	{create_and_store_token(NUM_INT_TOKEN, yytext, lineCounter); fprintf(yyout ,"Token of type %s, lexeme: %s, found in line: %d\n", eTokensString[NUM_INT_TOKEN], yytext, lineCounter); return 1;}
(([1-9]{DIGIT}*)|0)("."{DIGIT}+("e"|"E")([+]|[-])?(([1-9]{DIGIT}*)|0))		{create_and_store_token(NUM_FLOAT_TOKEN, yytext, lineCounter);fprintf(yyout ,"Token of type %s, lexeme: %s, found in line: %d\n", eTokensString[NUM_FLOAT_TOKEN], yytext, lineCounter); return 1;}
[+]		{create_and_store_token(ADD_OP_TOKEN, yytext, lineCounter);fprintf(yyout ,"Token of type %s, lexeme: %s, found in line: %d\n", eTokensString[ADD_OP_TOKEN], yytext, lineCounter); return 1;}
[*]		{create_and_store_token(MUL_OP_TOKEN, yytext, lineCounter);fprintf(yyout ,"Token of type %s, lexeme: %s, found in line: %d\n", eTokensString[MUL_OP_TOKEN], yytext, lineCounter); return 1;}
[<]		{create_and_store_token(LESS_OP_TOKEN, yytext, lineCounter);fprintf(yyout ,"Token of type %s, lexeme: %s, found in line: %d\n", eTokensString[LESS_OP_TOKEN], yytext, lineCounter); return 1;}
[>]		{create_and_store_token(GREAT_OP_TOKEN, yytext, lineCounter);fprintf(yyout ,"Token of type %s, lexeme: %s, found in line: %d\n", eTokensString[GREAT_OP_TOKEN], yytext, lineCounter); return 1;}
"<="	{create_and_store_token(LOE_OP_TOKEN, yytext, lineCounter);fprintf(yyout ,"Token of type %s, lexeme: %s, found in line: %d\n", eTokensString[LOE_OP_TOKEN], yytext, lineCounter); return 1;}
">="	{create_and_store_token(GOE_OP_TOKEN, yytext, lineCounter);fprintf(yyout ,"Token of type %s, lexeme: %s, found in line: %d\n", eTokensString[GOE_OP_TOKEN], yytext, lineCounter); return 1;}
"=="	{create_and_store_token(EQUAL_OP_TOKEN, yytext, lineCounter);fprintf(yyout ,"Token of type %s, lexeme: %s, found in line: %d\n", eTokensString[EQUAL_OP_TOKEN], yytext, lineCounter); return 1;}
"!="	{create_and_store_token(NOT_EQUAL_OP_TOKEN, yytext, lineCounter);fprintf(yyout ,"Token of type %s, lexeme: %s, found in line: %d\n", eTokensString[NOT_EQUAL_OP_TOKEN], yytext, lineCounter); return 1;}
[=]		{create_and_store_token(ASSIGNMENT_OP_TOKEN, yytext, lineCounter);fprintf(yyout ,"Token of type %s, lexeme: %s, found in line: %d\n", eTokensString[ASSIGNMENT_OP_TOKEN], yytext, lineCounter); return 1;}
"void"		{create_and_store_token(VOID_TOKEN, yytext, lineCounter);fprintf(yyout ,"Token of type %s, lexeme: %s, found in line: %d\n", eTokensString[VOID_TOKEN], yytext, lineCounter); return 1;}
"int"		{create_and_store_token(INT_TOKEN, yytext, lineCounter);fprintf(yyout ,"Token of type %s, lexeme: %s, found in line: %d\n", eTokensString[INT_TOKEN], yytext, lineCounter); return 1;}
"float"		{create_and_store_token(FLOAT_TOKEN, yytext, lineCounter);fprintf(yyout ,"Token of type %s, lexeme: %s, found in line: %d\n", eTokensString[FLOAT_TOKEN], yytext, lineCounter); return 1;}
"if"		{create_and_store_token(IF_TOKEN, yytext, lineCounter);fprintf(yyout ,"Token of type %s, lexeme: %s, found in line: %d\n", eTokensString[IF_TOKEN], yytext, lineCounter); return 1;}
"return"		{create_and_store_token(RETURN_TOKEN, yytext, lineCounter);fprintf(yyout ,"Token of type %s, lexeme: %s, found in line: %d\n", eTokensString[RETURN_TOKEN], yytext, lineCounter); return 1;}
{LCALPHA}+("_"?({UCALPHA}|{LCALPHA}|{DIGIT})+)*		{create_and_store_token(ID_TOKEN, yytext, lineCounter);fprintf(yyout ,"Token of type %s, lexeme: %s, found in line: %d\n", eTokensString[ID_TOKEN], yytext, lineCounter); return 1;}
,		{create_and_store_token(COMMA_TOKEN, yytext, lineCounter);fprintf(yyout ,"Token of type %s, lexeme: %s, found in line: %d\n", eTokensString[COMMA_TOKEN], yytext, lineCounter); return 1;}
:		{create_and_store_token(COLON_TOKEN, yytext, lineCounter);fprintf(yyout ,"Token of type %s, lexeme: %s, found in line: %d\n", eTokensString[COLON_TOKEN], yytext, lineCounter); return 1;}
;		{create_and_store_token(SEMICOLON_TOKEN, yytext, lineCounter);fprintf(yyout ,"Token of type %s, lexeme: %s, found in line: %d\n", eTokensString[SEMICOLON_TOKEN], yytext, lineCounter); return 1;}
"("		{create_and_store_token(OPEN_PARENTHESES_TOKEN, yytext, lineCounter);fprintf(yyout ,"Token of type %s, lexeme: %s, found in line: %d\n", eTokensString[OPEN_PARENTHESES_TOKEN], yytext, lineCounter); return 1;}
")"		{create_and_store_token(CLOSE_PARENTHESES_TOKEN, yytext, lineCounter);fprintf(yyout ,"Token of type %s, lexeme: %s, found in line: %d\n", eTokensString[CLOSE_PARENTHESES_TOKEN], yytext, lineCounter); return 1;}
"["		{create_and_store_token(OPEN_BRACKETS_TOKEN, yytext, lineCounter);fprintf(yyout ,"Token of type %s, lexeme: %s, found in line: %d\n", eTokensString[OPEN_BRACKETS_TOKEN], yytext, lineCounter); return 1;}
"]"		{create_and_store_token(CLOSE_BRACKETS_TOKEN, yytext, lineCounter);fprintf(yyout ,"Token of type %s, lexeme: %s, found in line: %d\n", eTokensString[CLOSE_BRACKETS_TOKEN], yytext, lineCounter); return 1;}
"{"		{create_and_store_token(OPEN_CURLY_BRACES_TOKEN, yytext, lineCounter);fprintf(yyout ,"Token of type %s, lexeme: %s, found in line: %d\n", eTokensString[OPEN_CURLY_BRACES_TOKEN], yytext, lineCounter); return 1;}
"}"		{create_and_store_token(CLOSE_CURLY_BRACES_TOKEN, yytext, lineCounter);fprintf(yyout ,"Token of type %s, lexeme: %s, found in line: %d\n", eTokensString[CLOSE_CURLY_BRACES_TOKEN], yytext, lineCounter); return 1;}
[ \t\n] 			{}
.		{fprintf(yyout ,"Character %c in line: %d does not begin any legal token in the language.\n", yytext[0], lineCounter); return 1;}

%%

	/* C User Subroutines */

/* C User Subroutines */
void main() {
	int x = 1;
	while (x != 0)
	{
		lineCounter = 1;
		printf("Menu:\nTo run both tests and exit the program press 1\nTo run test1.txt press 2\nTo run test2.txt press 3\nTo exit the program press 0\n");
		scanf("%d", &x);
		if (x==1) {
			yyin = fopen("C:\\temp\\test1.txt", "r");
			yyrestart(yyin);
			yyout = fopen("C:\\temp\\test1_204536676_204863633_204780969_lex.txt", "w");
			while (yylex())
			{

			}
			lineCounter = 1;
			yyin = fopen("C:\\temp\\test2.txt", "r");
			yyrestart(yyin);
			yyout = fopen("C:\\temp\\test2_204536676_204863633_204780969_lex.txt", "w");
			while (yylex())
			{

			}
			break;
		}
		else if (x == 2)
		{
			yyin = fopen("C:\\temp\\test1.txt", "r");
			yyrestart(yyin);
			yyout = fopen("C:\\temp\\test1_204536676_204863633_204780969_lex.txt", "w");
			while (yylex())
			{

			}
		}
		else if (x == 3)
		{
			yyin = fopen("C:\\temp\\test2.txt", "r");
			yyrestart(yyin);
			yyout = fopen("C:\\temp\\test2_204536676_204863633_204780969_lex.txt", "w");
			while (yylex())
			{

			}

		}
		else if (x != 0)
		{
			printf("\nPlease enter valid number\n");
		}
	}
}
