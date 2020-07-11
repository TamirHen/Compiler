%option noyywrap

%{
	
/* C Declarations (define, global vars etc...)*/
int lineCounter = 0;
%}

DIGIT    [0-9]
LC_ALPHA	 [a-z]
UC_ALPHA	 [A-Z]

%%

/* Lexical Translation Rules */
\n		lineCounter++;
-?[1-9]{DIGIT}*		{create_and_store_token(0, yytext, lineCounter);}
-?[1-9]{DIGIT}*("."{DIGIT}+)?		{create_and_store_token(1, yytext, lineCounter);}
+		{create_and_store_token(2, yytext, lineCounter);}
*		{create_and_store_token(3, yytext, lineCounter);}
<		{create_and_store_token(4, yytext, lineCounter);}
>		{create_and_store_token(5, yytext, lineCounter);}
<=		{create_and_store_token(6, yytext, lineCounter);}
>=		{create_and_store_token(7, yytext, lineCounter);}
==		{create_and_store_token(8, yytext, lineCounter);}
!=		{create_and_store_token(9, yytext, lineCounter);}
=		{create_and_store_token(10, yytext, lineCounter);}
"int "		{create_and_store_token(11, yytext, lineCounter);}
"float "		{create_and_store_token(12, yytext, lineCounter);}
"void "		{create_and_store_token(13, yytext, lineCounter);}
"if"		{create_and_store_token(14, yytext, lineCounter);}
"return"		{create_and_store_token(15, yytext, lineCounter);}
{LC_ALPHA}+ ( [_]? ({UC_ALPHA} | {LC_ALPHA} | {DIGIT})+ )*		{create_and_store_token(16, yytext, lineCounter);}
,		{create_and_store_token(17, yytext, lineCounter);}
:		{create_and_store_token(18, yytext, lineCounter);}
;		{create_and_store_token(19, yytext, lineCounter);}
(		{create_and_store_token(20, yytext, lineCounter);}
)		{create_and_store_token(21, yytext, lineCounter);}
[		{create_and_store_token(22, yytext, lineCounter);}
]		{create_and_store_token(23, yytext, lineCounter);}
{		{create_and_store_token(24, yytext, lineCounter);}
}		{create_and_store_token(25, yytext, lineCounter);}
("/*")([.]*(\n)*)*("*/")		{create_and_store_token(26, yytext, lineCounter);}
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
