%option noyywrap

%{
	
	/* C Declarations (define, global vars etc...)*/
	int lineCounter = 0;
%}

	/* Lexical Definitions */
	NUM_INT_TOKEN   [-]?[1-9][0-9]*
	NUM_FLOAT_TOKEN   [-]?[1-9][0-9]*("."[0-9]+)?
	ADD_OP_TOKEN   +
	MUL_OP_TOKEN   *
	LESS_OP_TOKEN   <
	GREAT_OP_TOKEN   >
	LOE_OP_TOKEN   <=
	GOE_OP_TOKEN   >=
	EQUAL_OP_TOKEN   ==
	NOT_EQUAL_OP_TOKEN   !=
	ASSIGNMENT_OP_TOKEN   =
	INT_TOKEN   "int "
	FLOAT_TOKEN   "float "
	VOID_TOKEN   "void "
	IF_TOKEN   "if"
	RETURN_TOKEN   "return "
	ID_TOKEN   [a-z]+ ( [_]? ([A-Z] | [a-z] | [0-9])+ )*
	COMMA_TOKEN   ","
	COLON_TOKEN   ":"
	SEMICOLON_TOKEN   ";"
	OPEN_PARENTHESES_TOKEN   "("
	CLOSE_PARENTHESES_TOKEN   ")"
	OPEN_BRACKETS_TOKEN   "["
	CLOSE_BRACKETS_TOKEN   "]"
	OPEN_CURLY_BRACES_TOKEN   "{"
	CLOSE_CURLY_BRACES_TOKEN   "}"
	COMMENTS_TOKEN   ("/*")([.]*[\n]*)*("*/")
%%

	/* Lexical Translation Rules */
	\n		lineCounter++;
	{NUM_INT_TOKEN}		{create_and_store_token(0, yytext, lineCounter);}
	{NUM_FLOAT_TOKEN}		{create_and_store_token(1, yytext, lineCounter);}
	{ADD_OP_TOKEN}		{create_and_store_token(2, yytext, lineCounter);}
	{MUL_OP_TOKEN}		{create_and_store_token(3, yytext, lineCounter);}
	{LESS_OP_TOKEN}		{create_and_store_token(4, yytext, lineCounter);}
	{GREAT_OP_TOKEN}		{create_and_store_token(5, yytext, lineCounter);}
	{LOE_OP_TOKEN}		{create_and_store_token(6, yytext, lineCounter);}
	{GOE_OP_TOKEN}		{create_and_store_token(7, yytext, lineCounter);}
	{EQUAL_OP_TOKEN}		{create_and_store_token(8, yytext, lineCounter);}
	{NOT_EQUAL_OP_TOKEN}		{create_and_store_token(9, yytext, lineCounter);}
	{ASSIGNMENT_OP_TOKEN}		{create_and_store_token(10, yytext, lineCounter);}
	{INT_TOKEN}		{create_and_store_token(11, yytext, lineCounter);}
	{FLOAT_TOKEN}		{create_and_store_token(12, yytext, lineCounter);}
	{VOID_TOKEN}		{create_and_store_token(13, yytext, lineCounter);}
	{IF_TOKEN}		{create_and_store_token(14, yytext, lineCounter);}
	{RETURN_TOKEN}		{create_and_store_token(15, yytext, lineCounter);}
	{ID_TOKEN}		{create_and_store_token(16, yytext, lineCounter);}
	{COMMA_TOKEN}		{create_and_store_token(17, yytext, lineCounter);}
	{COLON_TOKEN}		{create_and_store_token(18, yytext, lineCounter);}
	{SEMICOLON_TOKEN}		{create_and_store_token(19, yytext, lineCounter);}
	{OPEN_PARENTHESES_TOKEN}		{create_and_store_token(20, yytext, lineCounter);}
	{CLOSE_PARENTHESES_TOKEN}		{create_and_store_token(21, yytext, lineCounter);}
	{OPEN_BRACKETS_TOKEN}		{create_and_store_token(22, yytext, lineCounter);}
	{CLOSE_BRACKETS_TOKEN}		{create_and_store_token(23, yytext, lineCounter);}
	{OPEN_CURLY_BRACES_TOKEN}		{create_and_store_token(24, yytext, lineCounter);}
	{CLOSE_CURLY_BRACES_TOKEN}		{create_and_store_token(25, yytext, lineCounter);}
	{COMMENTS_TOKEN}		{create_and_store_token(25, yytext, lineCounter);}
	[ \t\n] 			{}
	.	fprintf(yyout ,"Error was found in line %d.\nThe letter that doesn't start any token is \"%c\"", lineCounter, yytext[0]);

%%

	/* C User Subroutines */
	int main(int argc, char **argv )
	{
		++argv, --argc;  /* skip over program name */
		if ( argc > 0 )
			yyin = fopen( argv[0], "r" );
		else
			yyin = stdin;

		if ( argc > 1 )
			yyout = fopen( argv[1], "w" );
		else
			yyout = stdout;

		yylex();
	}
