#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
extern FILE* yyin, * yyout;

typedef enum eTOKENS
{
	NUM_INT_TOKEN,			 /* 0 */
	NUM_FLOAT_TOKEN,		 /* 1 */
	ADD_OP_TOKEN,			 /* 2 (+) */
	MUL_OP_TOKEN,			 /* 3 (*) */
	LESS_OP_TOKEN,			 /* 4 (<) */
	GREAT_OP_TOKEN,			 /* 5 (>) */
	LOE_OP_TOKEN,			 /* 6 (<=) */
	GOE_OP_TOKEN,			 /* 7 (>=) */
	EQUAL_OP_TOKEN,			 /* 8 (==) */
	NOT_EQUAL_OP_TOKEN,		 /* 9 (!=) */
	ASSIGNMENT_OP_TOKEN,	 /* 10 (=) */
	INT_TOKEN,				 /* 11 (int) */
	FLOAT_TOKEN,			 /* 12 (float) */
	VOID_TOKEN,				 /* 13 (void) */
	IF_TOKEN,				 /* 14 (if) */
	RETURN_TOKEN,			 /* 15 (return) */
	ID_TOKEN,				 /* 16 */
	COMMA_TOKEN,			 /* 17 (,) */
	COLON_TOKEN,			 /* 18 (:) */
	SEMICOLON_TOKEN,		 /* 19 (;) */
	OPEN_PARENTHESES_TOKEN,	 /* 20 ('(') */
	CLOSE_PARENTHESES_TOKEN, /* 21 (')') */
	OPEN_BRACKETS_TOKEN,	 /* 22 ('[') */
	CLOSE_BRACKETS_TOKEN,	 /* 23 (']') */
	OPEN_CURLY_BRACES_TOKEN, /* 24 ('{') */
	CLOSE_CURLY_BRACES_TOKEN,/* 25 ('}') */
	EOF_TOKEN				 /* 26 */
}eTOKENS;

static char* eTokensString[] =
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

typedef struct Token
{
	eTOKENS kind;
	char* lexeme;
	int lineNumber;
}Token;

typedef struct Node
{
	Token* tokensArray;
	struct Node* prev;
	struct Node* next;
} Node;


void create_and_store_token(eTOKENS kind, char* lexeme, int numOfLine);
Token* back_token();
Token* next_token();
Token* match(eTOKENS expectedKind);
Token* match_rel_op(eTOKENS* expectedKinds);
void errorHandler(eTOKENS expectedKind, Token* actual);
void errorHandlerAndRecovery(eTOKENS* expectedKinds, int kindsSize, Token* actual, eTOKENS* followTokensToReover, int followTokensSize);
eTOKENS lookaheadAfterPrototype();
void restart();

#endif