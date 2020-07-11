%option noyywrap

%{
#include "parser.h";

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
<comment>["""]*	{}
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
void main() 
{
	static int fileIndex = 0;
	int* globalvar;
	printf("Press any key to run test1.txt and then test2.txt\n");

	yyin = fopen("C:\\temp\\test1.txt", "r");
	yyout = fopen("C:\\temp\\test1_204536676_204863633_204780969_syntactic.txt", "w");
	fopen("C:\\temp\\test1_204536676_204863633_204780969_lex.txt", "w");
	parse();
	restart();
	yyin = fopen("C:\\temp\\test2.txt", "r");
	yyout = fopen("C:\\temp\\test2_204536676_204863633_204780969_syntactic.txt", "w");
	fopen("C:\\temp\\test2_204536676_204863633_204780969_lex.txt", "w");
	parse();
}
