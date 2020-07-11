#pragma once
#include "Token.h"
//#include "symbolTable.h"
#include "semanticFunctions.h"

void parse();
void parse_PROG();
void parse_GLOBAL_VARS();
void parse_VAR_DEC();
void parse_VAR_DEC_tag(int*, int*, char*);
char* parse_TYPE();
void parse_DIM_SIZES(int*, int*, char*);
void parse_DIM_SIZES_tag(int*, int*, char*);
void parse_FUNC_PROTOTYPE(char*);
void parse_FUNC_FULL_DEFS();
void parse_FUNC_FULL_DEFS_tag();
void parse_FUNC_WITH_BODY();
char* parse_RETURNED_TYPE();
void parse_PARAMS(char**, int*, char*);
void parse_PARAM_LIST(char**, int*, char*);
void parse_PARAM_LIST_tag(char**, int*, char*);
void parse_PARAM(char**, int*, char*);
void parse_PARAM_tag(char**, int*, char*, int*, int*);
int parse_COMP_STMT();
void parse_VAR_DEC_LIST();
void parse_VAR_DEC_LIST_tag();
void parse_STMT_LIST();
void parse_STMT_LIST_tag();
void parse_STMT();
void parse_STMT_tag(struct SymbolNode*, Token*);
void parse_IF_STMT();
void parse_CALL();
void parse_ARGS(int*, char**);
void parse_ARG_LIST(int*, char**);
void parse_ARG_LIST_tag(int*, char**);
void parse_RETURN_STMT();
void parse_RETURN_STMT_tag();
void parse_VAR();
char* parse_VAR_tag(struct SymbolNode*, Token*);
void parse_EXPR_LIST(int*);
void parse_EXPR_LIST_tag(int*);
void parse_CONDITION();
char* parse_EXPR();
char* parse_EXPR_tag();
char* parse_TERM();
char* parse_TERM_tag();
char* parse_FACTOR();
char* parse_FACTOR_tag(struct SymbolNode*, Token*);
