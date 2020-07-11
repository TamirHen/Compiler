#define _CRT_SECURE_NO_WARNINGS

#include "parser.h"
void parse()
{
	int lineNumber;
	CreateTable();
	parse_PROG();
	lineNumber = match(EOF_TOKEN)->lineNumber;
	declaredFunctionMustBeImplemented(lineNumber);
	if (firstTable != NULL)
	{
		varMustBeUsedAtLeastOnce(firstTable, lineNumber);
		nonVoidFunctionMustHaveAtListOneReturnStmt(firstTable, lineNumber);
		DeleteTable(firstTable->tableNumber);
	}
}

void parse_PROG()
{
	Token* t;
	fprintf(yyout, "Rule (PROG -> GLOBAL_VARS FUNC_PREDEFS FUNC_FULL_DEFS)\n");
	parse_GLOBAL_VARS();
	fprintf(yyout, "Rule (FUNC_PREDEFS -> FUNC_PROTOTYPE; FUNC_PREDEFS_tag)\n");
	CreateTable();
	parse_FUNC_PROTOTYPE("pre-definition");
	t = next_token();

	while (t->kind == SEMICOLON_TOKEN && lookaheadAfterPrototype() == SEMICOLON_TOKEN) // also the next definition will be prototype
	{
		fprintf(yyout, "Rule (FUNC_PREDEFS_tag -> FUNC_PROTOTYPE; FUNC_PREDEFS_tag)\n");
		CreateTable();
		parse_FUNC_PROTOTYPE("pre-definition");
		t = next_token();
	}   

	fprintf(yyout, "Rule (FUNC_PREDEFS_tag -> Epsilon)\n");
	parse_FUNC_FULL_DEFS();

}  

void parse_GLOBAL_VARS()
{
	Token* t;
	fprintf(yyout, "Rule (GLOBAL_VARS -> VAR_DEC GLOBAL_VARS_tag)\n");
	parse_VAR_DEC();

	next_token(); next_token();
	t = next_token(); /* lookahead 3 tokens */
	back_token(); back_token(); back_token();

	while (t->kind == SEMICOLON_TOKEN || t->kind == OPEN_BRACKETS_TOKEN) { // GLOBAL_VARS_tag()
		fprintf(yyout, "Rule (GLOBAL_VARS_tag -> VAR_DEC GLOBAL_VARS_tag)\n");
		parse_VAR_DEC();
		next_token(); next_token();
		t = next_token(); /* lookahead 3 tokens */
		back_token(); back_token(); back_token();
	}
	fprintf(yyout, "Rule (GLOBAL_VARS_tag -> Epsilon)\n");

}

void parse_VAR_DEC()
{
	char name[30], role[20] = "variable", type[20];
	int* listOfArraySizes = (int*)malloc(15*sizeof(int));
	int sizeOfArraySizeList = 0;

	fprintf(yyout, "Rule (VAR_DEC -> TYPE id VAR_DEC_tag)\n");
	strcpy(type, parse_TYPE());
	Token* t = match(ID_TOKEN);
	strcpy(name, t->lexeme);
	parse_VAR_DEC_tag(listOfArraySizes, &sizeOfArraySizeList, type);
	struct SymbolNode* node = CreateNode(name, role, type, listOfArraySizes, sizeOfArraySizeList, NULL, 0);
	//duplicateDeclarationInSameScopeForbidden(node);
	InsertNode(lastTable->tableNumber, node);
}

void parse_VAR_DEC_tag(int* listOfArraySizes, int* listSizePtr, char* varType)
{
	eTOKENS followTokens[] = { RETURN_TOKEN };
	Token* t = next_token();
	eTOKENS expectedKinds[] = { CLOSE_BRACKETS_TOKEN, SEMICOLON_TOKEN };
	switch (t->kind)
	{
	case SEMICOLON_TOKEN:
		fprintf(yyout, "Rule (VAR_DEC_tag -> ;)\n");
		break;

	case OPEN_BRACKETS_TOKEN: // var is array
		fprintf(yyout, "Rule (VAR_DEC_tag -> [DIM_SIZES];)\n");
		parse_DIM_SIZES(listOfArraySizes, listSizePtr, varType);
		match(CLOSE_BRACKETS_TOKEN);
		match(SEMICOLON_TOKEN);
		break;

	default:
		errorHandlerAndRecovery(expectedKinds, 2, t, followTokens, 1);
		break;
	}
}

char* parse_TYPE()
{
	eTOKENS followTokens[] = { ID_TOKEN };
	Token* t = next_token();
	eTOKENS expectedKinds[] = { INT_TOKEN, FLOAT_TOKEN };

	switch (t->kind)
	{
	case INT_TOKEN:
		fprintf(yyout, "Rule (TYPE -> int)\n");
		return "int";
		break;

	case FLOAT_TOKEN:
		fprintf(yyout, "Rule (TYPE -> float)\n");
		return "float";
		break;

	default:
		errorHandlerAndRecovery(expectedKinds, 2, t, followTokens, 1);
		return "error";
		break;
	}
}

void parse_DIM_SIZES(int* listOfArraySizes, int* listSizePtr, char* varType)
{
	fprintf(yyout, "Rule (DIM_SIZES -> int_num DIM_SIZES_tag)\n");
	Token* t = match(NUM_INT_TOKEN);
	strcat(varType, "[]");
	(*listSizePtr)++;
	//listOfArraySizes = (int*)realloc(listOfArraySizes, sizeof(int) * (*listSizePtr));
	listOfArraySizes[(*listSizePtr) - 1] = atoi(t->lexeme);

	parse_DIM_SIZES_tag(listOfArraySizes, listSizePtr, varType);
}

void parse_DIM_SIZES_tag(int* listOfArraySizes, int* listSizePtr, char* varType)
{
	eTOKENS followTokens[] = { CLOSE_BRACKETS_TOKEN };
	Token* t = next_token();
	eTOKENS expectedKinds[] = { CLOSE_BRACKETS_TOKEN, COMMA_TOKEN };
	switch (t->kind)
	{
	case CLOSE_BRACKETS_TOKEN:
		fprintf(yyout, "Rule (DIM_SIZES_tag -> Epsilon)\n");
		back_token();
		break;

	case COMMA_TOKEN:
		fprintf(yyout, "Rule (DIM_SIZES_tag -> , DIM_SIZES)\n");
		parse_DIM_SIZES(listOfArraySizes, listSizePtr, varType);
		break;

	default:
		errorHandlerAndRecovery(expectedKinds, 2, t, followTokens, 1);
		break;
	}
}

void parse_FUNC_PROTOTYPE(char* role)
{
	char name[30], type[20];
	char** listOfParameterTypes;
	listOfParameterTypes = (char**)malloc(30 * sizeof(char*));
	int paramListSize = 0;

	fprintf(yyout, "Rule (FUNC_PROTOTYPE -> RETURNED_TYPE id(PARAMS))\n");
	strcpy(type, parse_RETURNED_TYPE());
	Token* t = match(ID_TOKEN);
	strcpy(name, t->lexeme);
	match(OPEN_PARENTHESES_TOKEN);
	parse_PARAMS(listOfParameterTypes, &paramListSize, role);
	match(CLOSE_PARENTHESES_TOKEN);
	if (strcmp(role, "full definition") == 0) // if function is full definition its details need to be insert to the perent table
	{
		struct SymbolNode* node = CreateNode(name, role, type, NULL, 0, listOfParameterTypes, paramListSize);
		//duplicateDeclarationInSameScopeForbidden(node);
		InsertNode(lastTable->prev->tableNumber, node);
		if (node != NULL)
		{
			amountOfParamsMustBeMatchWithFuncDeclaration(getNodeById(name), node, t->lineNumber);
			typeOfParamsMustBeMatchWithFuncDeclaration(getNodeById(name), node, t->lineNumber);
		
			lastTable->function = node;
		}
	}
	else
	{
		struct SymbolNode* node = CreateNode(name, role, type, NULL, 0, listOfParameterTypes, paramListSize);
		//duplicateDeclarationInSameScopeForbidden(node);
		InsertNode(lastTable->tableNumber, node);
	}
}

void parse_FUNC_FULL_DEFS()
{
	fprintf(yyout, "Rule (FUNC_FULL_DEFS -> FUNC_WITH_BODY FUNC_FULL_DEFS_tag)\n");
	parse_FUNC_WITH_BODY();
	parse_FUNC_FULL_DEFS_tag();
}

void parse_FUNC_FULL_DEFS_tag()
{
	eTOKENS followTokens[] = { EOF_TOKEN };
	Token* t = next_token();
	eTOKENS expectedKinds[] = { INT_TOKEN, FLOAT_TOKEN, VOID_TOKEN };
	switch (t->kind)
	{
	case EOF_TOKEN:
		fprintf(yyout, "Rule (FUNC_FULL_DEFS_tag -> Epsilon)\n");
		back_token();
		break;

	case INT_TOKEN:
	case FLOAT_TOKEN:
	case VOID_TOKEN:
		fprintf(yyout, "Rule (FUNC_FULL_DEFS_tag -> FUNC_FULL_DEFS)\n");
		back_token();
		parse_FUNC_FULL_DEFS();
		break;

	default:
		errorHandlerAndRecovery(expectedKinds, 3, t, followTokens, 1);
		break;
	}
}

void parse_FUNC_WITH_BODY()
{
	int lineNumber;
	fprintf(yyout, "Rule (FUNC_WITH_BODY -> FUNC_PROTOTYPE COMP_STMT)\n");
	CreateTable();
	parse_FUNC_PROTOTYPE("full definition");
	lineNumber = parse_COMP_STMT();
	if (lastTable != NULL)
	{
		varMustBeUsedAtLeastOnce(lastTable, lineNumber);
		nonVoidFunctionMustHaveAtListOneReturnStmt(lastTable, lineNumber);
		DeleteTable(lastTable->tableNumber);
	}

}

char* parse_RETURNED_TYPE()
{
	Token* t = next_token();
	switch (t->kind)
	{
	case VOID_TOKEN:
		fprintf(yyout, "Rule (RETURNED_TYPE -> void )\n");
		return "void";
		break;

	default:
		fprintf(yyout, "Rule (RETURNED_TYPE -> TYPE)\n");
		back_token();
		return parse_TYPE();
		break;
	}
}

void parse_PARAMS(char** listOfParameterTypes, int* listSizePtr, char* funcRole)
{
	Token* t = next_token();
	switch (t->kind)
	{
	case CLOSE_PARENTHESES_TOKEN:
		fprintf(yyout, "Rule (PARAMS -> Epsilon)\n");
		back_token();
		if (strcmp(funcRole, "pre-definition") == 0)
		{
			DeleteTable(lastTable->tableNumber);
		}
		break;

	default:
		fprintf(yyout, "Rule (PARAMS -> PARAM_LIST)\n");
		back_token();
		parse_PARAM_LIST(listOfParameterTypes, listSizePtr, funcRole);
		if (strcmp(funcRole, "pre-definition") == 0)
		{
			DeleteTable(lastTable->tableNumber);
		}
		break;
	}
}

void parse_PARAM_LIST(char** listOfParameterTypes, int* listSizePtr, char* funcRole)
{
	fprintf(yyout, "Rule (PARAM_LIST -> PARAM PARAM_LIST_tag)\n");
	parse_PARAM(listOfParameterTypes, listSizePtr, funcRole);
	parse_PARAM_LIST_tag(listOfParameterTypes, listSizePtr, funcRole);
}

void parse_PARAM_LIST_tag(char** listOfParameterTypes, int* listSizePtr, char* funcRole)
{
	eTOKENS followTokens[] = { CLOSE_PARENTHESES_TOKEN };
	Token* t = next_token();
	eTOKENS expectedKinds[] = { COMMA_TOKEN, CLOSE_PARENTHESES_TOKEN };
	switch (t->kind)
	{
	case COMMA_TOKEN:
		fprintf(yyout, "Rule (PARAM_LIST_tag -> , PARAM PARAM_LIST_tag)\n");
		parse_PARAM(listOfParameterTypes, listSizePtr, funcRole);
		parse_PARAM_LIST_tag(listOfParameterTypes, listSizePtr, funcRole);
		break;

	case CLOSE_PARENTHESES_TOKEN:
		fprintf(yyout, "Rule (PARAM_LIST_tag -> Epsilon)\n");
		back_token();
		break;

	default:
		errorHandlerAndRecovery(expectedKinds, 2, t, followTokens, 1);
		break;
	}
}

void parse_PARAM(char** listOfParameterTypes, int* listSizePtr, char* funcRole)
{
	char name[30], role[20] = "variable";
	int* listOfArraySizes = (int*)malloc(sizeof(int));
	int sizeOfArraySizeList = 0;
	char paramType[30];
	fprintf(yyout, "Rule (PARAM -> TYPE id PARAM_tag)\n");
	strcpy(paramType, parse_TYPE());
	Token* t = match(ID_TOKEN);
	strcpy(name, t->lexeme);

	parse_PARAM_tag(listOfParameterTypes, listSizePtr, paramType, listOfArraySizes, &sizeOfArraySizeList);
	//if (strcmp(funcRole, "full definition") == 0)
	//{
		struct SymbolNode* node = CreateNode(name, role, paramType, listOfArraySizes, sizeOfArraySizeList, NULL, 0);
		//duplicateDeclarationInSameScopeForbidden(node);
		InsertNode(lastTable->tableNumber, node);
	//}
}

void parse_PARAM_tag(char** listOfParameterTypes, int* listSizePtr, char* paramType, int* listOfArraySizes, int* sizeOfListArraySizePtr)
{
	eTOKENS followTokens[] = { COMMA_TOKEN, CLOSE_PARENTHESES_TOKEN };
	Token* t = next_token();
	eTOKENS expectedKinds[] = { COMMA_TOKEN, CLOSE_PARENTHESES_TOKEN, OPEN_BRACKETS_TOKEN };
	switch (t->kind)
	{
	case COMMA_TOKEN:
	case CLOSE_PARENTHESES_TOKEN:
		fprintf(yyout, "Rule (PARAM_tag -> Epsilon)\n");
		(*listSizePtr)++;
		//listOfParameterTypes = (char**)realloc(listOfParameterTypes, sizeof(char*) * (*listSizePtr));
		listOfParameterTypes[(*listSizePtr) - 1] = (char*)malloc(sizeof(char) * (strlen(paramType) + 1));
		strcpy(listOfParameterTypes[(*listSizePtr) - 1], paramType);
		//listOfArraySizes = NULL;
		back_token();
		break;

	case OPEN_BRACKETS_TOKEN:
		fprintf(yyout, "Rule (PARAM_tag -> [DIM_SIZES])\n");
		(*listSizePtr)++;
		//listOfParameterTypes = (char**)realloc(listOfParameterTypes, sizeof(char**) * (*listSizePtr));
		
		//char* newType = (char*)malloc(sizeof(char) * (strlen(paramType) + 3));
		//strcpy(newType, paramType);
		//strcat(newType, "[]");
		//listOfParameterTypes[(*listSizePtr) - 1] = (char*)malloc(sizeof(char) * (strlen(newType) + 1));
		//strcpy(listOfParameterTypes[(*listSizePtr) - 1], newType);

		parse_DIM_SIZES(listOfArraySizes, sizeOfListArraySizePtr, paramType);
		//////////////////
		listOfParameterTypes[(*listSizePtr) - 1] = (char*)malloc(sizeof(char) * (strlen(paramType) + 2));
		strcpy(listOfParameterTypes[(*listSizePtr) - 1], paramType);

		match(CLOSE_BRACKETS_TOKEN);
		break;

	default:
		errorHandlerAndRecovery(expectedKinds, 3, t, followTokens, 2);
		break;
	}
}

int parse_COMP_STMT()
{
	fprintf(yyout, "Rule (COMP_STMT -> {VAR_DEC_LIST STMT_LIST})\n");
	match(OPEN_CURLY_BRACES_TOKEN);
	parse_VAR_DEC_LIST();
	parse_STMT_LIST();
	return match(CLOSE_CURLY_BRACES_TOKEN)->lineNumber;
}

void parse_VAR_DEC_LIST()
{
	fprintf(yyout, "Rule (VAR_DEC_LIST -> Epsilon VAR_DEC_LIST_tag)\n");
	parse_VAR_DEC_LIST_tag();
}

void parse_VAR_DEC_LIST_tag()
{
	eTOKENS followTokens[] = { ID_TOKEN, OPEN_CURLY_BRACES_TOKEN, IF_TOKEN, RETURN_TOKEN };
	Token* t = next_token();
	eTOKENS expectedKinds[] = { ID_TOKEN, OPEN_CURLY_BRACES_TOKEN, IF_TOKEN, RETURN_TOKEN, INT_TOKEN, FLOAT_TOKEN };
	switch (t->kind)
	{
	case ID_TOKEN:
	case OPEN_CURLY_BRACES_TOKEN:
	case IF_TOKEN:
	case RETURN_TOKEN:
		fprintf(yyout, "Rule (VAR_DEC_LIST_tag -> Epsilon)\n");
		back_token();
		break;

	case INT_TOKEN:
	case FLOAT_TOKEN:
		fprintf(yyout, "Rule (VAR_DEC_LIST_tag -> VAR_DEC VAR_DEC_LIST_tag)\n");
		back_token();
		parse_VAR_DEC();
		parse_VAR_DEC_LIST_tag();		
		break;

	default:
		errorHandlerAndRecovery(expectedKinds, 6, t, followTokens, 4);
		break;
	}
}

void parse_STMT_LIST()
{
	fprintf(yyout, "Rule (STMT_LIST -> STMT STMT_LIST_tag)\n");
	parse_STMT();
	parse_STMT_LIST_tag();
}

void parse_STMT_LIST_tag()
{
	eTOKENS followTokens[] = { CLOSE_CURLY_BRACES_TOKEN };
	Token* t = next_token();
	eTOKENS expectedKinds[] = { SEMICOLON_TOKEN, CLOSE_CURLY_BRACES_TOKEN };
	switch (t->kind)
	{
	case SEMICOLON_TOKEN:
		fprintf(yyout, "Rule (STMT_LIST_tag -> ; STMT STMT_LIST_tag)\n");
		parse_STMT();
		parse_STMT_LIST_tag();
		break;

	case CLOSE_CURLY_BRACES_TOKEN:
		fprintf(yyout, "Rule (STMT_LIST_tag -> Epsilon)\n");
		back_token();
		break;

	default:
		errorHandlerAndRecovery(expectedKinds, 2, t, followTokens, 1);
		break;
	}
}

void parse_STMT()
{
	eTOKENS followTokens[] = { SEMICOLON_TOKEN, CLOSE_CURLY_BRACES_TOKEN };
	Token* t = next_token();
	eTOKENS expectedKinds[] = { ID_TOKEN, OPEN_CURLY_BRACES_TOKEN, IF_TOKEN, RETURN_TOKEN };
	switch (t->kind)
	{
	case ID_TOKEN:
		fprintf(yyout, "Rule (STMT -> id STMT_tag)\n");
		usedIdMustBeDeclared(t->lexeme, t->lineNumber);

		parse_STMT_tag(/*getIdType(t->lexeme)*/ getNodeById(t->lexeme), t);
		break;

	case OPEN_CURLY_BRACES_TOKEN:
		fprintf(yyout, "Rule (STMT -> COMP_STMT)\n");
		back_token();
		CreateTable();
		int lineNumber = parse_COMP_STMT();
		if (lastTable != NULL)
		{
			varMustBeUsedAtLeastOnce(lastTable, lineNumber);
			nonVoidFunctionMustHaveAtListOneReturnStmt(lastTable, lineNumber);
			DeleteTable(lastTable->tableNumber);
		}
		break;

	case IF_TOKEN:
		fprintf(yyout, "Rule (STMT -> IF_STMT)\n");
		back_token();
		parse_IF_STMT();
		break;

	case RETURN_TOKEN:
		fprintf(yyout, "Rule (STMT -> RETURN_STMT)\n");
		back_token();
		parse_RETURN_STMT();
		break;

	default:
		errorHandlerAndRecovery(expectedKinds, 4, t, followTokens, 2);
		break;
	}
}

void parse_STMT_tag(struct SymbolNode* node, Token* id)
{
	int numOfParamsInCall = 0;
	char** listOfParametersTypesInCall;
	listOfParametersTypesInCall = (char**)malloc(30 * sizeof(char*));
	Token* t = next_token();
	char* leftExprType;
	if (node != NULL)
	{
		leftExprType = node->type;
	}
	else
	{
		leftExprType = "type error";
	}
	char* rightExprType;
	switch (t->kind)
	{
	case OPEN_PARENTHESES_TOKEN:
		fprintf(yyout, "Rule (STMT_tag -> (ARGS))\n");
		parse_ARGS(&numOfParamsInCall, listOfParametersTypesInCall);
		funcCallValidityCheck(getFuncFullBodyById(id->lexeme), numOfParamsInCall, listOfParametersTypesInCall, id->lineNumber);
		match(CLOSE_PARENTHESES_TOKEN);
		break;

	default:
		fprintf(yyout, "Rule (STMT_tag -> VAR_tag = EXPR)\n");
		back_token();
		parse_VAR_tag(node, id);
		Token* t = match(ASSIGNMENT_OP_TOKEN);
		rightExprType = parse_EXPR();
		leftAndRightSideSameTypeOrLeftFloatAndRightInt(leftExprType, rightExprType, t->lineNumber);
		break;
	}
}

void parse_IF_STMT()
{
	fprintf(yyout, "Rule (IF_STMT -> if(CONDITION) STMT)\n");
	match(IF_TOKEN);
	match(OPEN_PARENTHESES_TOKEN);
	parse_CONDITION();
	match(CLOSE_PARENTHESES_TOKEN);
	parse_STMT();
}

void parse_CALL()
{
	int numOfParamsInCall = 0;
	char** listOfParametersTypesInCall;
	listOfParametersTypesInCall = (char**)malloc(30 * sizeof(char*));
	fprintf(yyout, "Rule (CALL -> id(ARGS))\n");
	Token* t = match(ID_TOKEN);
	match(OPEN_PARENTHESES_TOKEN);
	parse_ARGS(&numOfParamsInCall, listOfParametersTypesInCall);
	funcCallValidityCheck(getFuncFullBodyById(t->lexeme), numOfParamsInCall, listOfParametersTypesInCall, t->lineNumber);
	match(CLOSE_PARENTHESES_TOKEN);
}

void parse_ARGS(int* numOfParamsInCallPtr, char** listOfParametersTypesInCall)
{
	Token* t = next_token();
	switch (t->kind)
	{
	case CLOSE_PARENTHESES_TOKEN:
		fprintf(yyout, "Rule (ARGS -> Epsilon)\n");
		back_token();
		break;

	default:
		fprintf(yyout, "Rule (ARGS -> ARG_LIST)\n");
		back_token();
		parse_ARG_LIST(numOfParamsInCallPtr, listOfParametersTypesInCall);
		break;
	}
}

void parse_ARG_LIST(int* numOfParamsInCallPtr, char** listOfParametersTypesInCall)
{
	fprintf(yyout, "Rule (ARG_LIST -> EXPR ARG_LIST_tag)\n");
	char* paramType = parse_EXPR(1);

	(*numOfParamsInCallPtr)++;
	listOfParametersTypesInCall[(*numOfParamsInCallPtr) - 1] = (char*)malloc(sizeof(char) * (strlen(paramType) + 1));
	listOfParametersTypesInCall[(*numOfParamsInCallPtr) - 1] = paramType;

	parse_ARG_LIST_tag(numOfParamsInCallPtr, listOfParametersTypesInCall);
}

void parse_ARG_LIST_tag(int* numOfParamsInCallPtr, char** listOfParametersTypesInCall)
{
	eTOKENS followTokens[] = { CLOSE_PARENTHESES_TOKEN };
	Token* t = next_token();
	eTOKENS expectedKinds[] = { COMMA_TOKEN, CLOSE_PARENTHESES_TOKEN };
	switch (t->kind)
	{
	case COMMA_TOKEN:
		fprintf(yyout, "Rule (ARG_LIST_tag -> , EXPR ARG_LIST_tag)\n");
		char* paramType = parse_EXPR();

		(*numOfParamsInCallPtr)++;
		listOfParametersTypesInCall[(*numOfParamsInCallPtr) - 1] = (char*)malloc(sizeof(char) * (strlen(paramType) + 1));
		listOfParametersTypesInCall[(*numOfParamsInCallPtr) - 1] = paramType;

		parse_ARG_LIST_tag(numOfParamsInCallPtr, listOfParametersTypesInCall);
		break;

	case CLOSE_PARENTHESES_TOKEN:
		fprintf(yyout, "Rule (ARG_LIST_tag -> Epsilon)\n");
		back_token();
		break;

	default:
		errorHandlerAndRecovery(expectedKinds, 2, t, followTokens, 1);
		break;
	}
}

void parse_RETURN_STMT()
{
	fprintf(yyout, "Rule (RETURN_STMT -> return RETURN_STMT_tag)\n");
	match(RETURN_TOKEN);
	parse_RETURN_STMT_tag();
}

void parse_RETURN_STMT_tag()
{
	eTOKENS followTokens[] = { SEMICOLON_TOKEN, CLOSE_CURLY_BRACES_TOKEN };
	Token* t = next_token();
	eTOKENS expectedKinds[] = { SEMICOLON_TOKEN, CLOSE_CURLY_BRACES_TOKEN, ID_TOKEN, NUM_INT_TOKEN, NUM_FLOAT_TOKEN, OPEN_PARENTHESES_TOKEN };
	switch (t->kind)
	{
	case SEMICOLON_TOKEN:
	case CLOSE_CURLY_BRACES_TOKEN:
		fprintf(yyout, "Rule (RETURN_STMT_tag -> Epsilon)\n");
		back_token();
		break;

	case ID_TOKEN:
	case NUM_INT_TOKEN:
	case NUM_FLOAT_TOKEN:
	case OPEN_PARENTHESES_TOKEN:
		fprintf(yyout, "Rule (RETURN_STMT_tag -> EXPR)\n");
		struct Table* table = lastTable;
		back_token();
		char* returnType = parse_EXPR();
		while (table->prev != NULL) // check tables until see table associate with function and connect the return stmt to it (not includes the first table)
		{
			if (table->function != NULL)
			{
				table->isContainReturnStmt = 1;
				expressionTypeInReturnStmtOfFunctionMustMatchWithTheFunctionType(table, returnType, t->lineNumber);
				voidOrMainProgramMustNotReturnValue(table, t->lineNumber);
			}
			table = table->prev;
		}
		break;

	default:
		errorHandlerAndRecovery(expectedKinds, 6, t, followTokens, 2);
		break;
	}
}

void parse_VAR()
{
	fprintf(yyout, "Rule (VAR -> id VAR_tag)\n");
	Token* t = match(ID_TOKEN);
	parse_VAR_tag(/*getIdType(t->lexeme)*/ getNodeById(t->lexeme), t, 0);
}

char* parse_VAR_tag(/*char* type*/ struct SymbolNode* node, Token* id)
{
	int numOfDimensions = 0;
	eTOKENS followTokens[] = { ASSIGNMENT_OP_TOKEN, MUL_OP_TOKEN, ADD_OP_TOKEN, LESS_OP_TOKEN, GREAT_OP_TOKEN, LOE_OP_TOKEN, GOE_OP_TOKEN, EQUAL_OP_TOKEN, NOT_EQUAL_OP_TOKEN, CLOSE_PARENTHESES_TOKEN, COMMA_TOKEN, SEMICOLON_TOKEN, CLOSE_CURLY_BRACES_TOKEN, CLOSE_BRACKETS_TOKEN };
	Token* t = next_token();
	eTOKENS expectedKinds[] = { ASSIGNMENT_OP_TOKEN, MUL_OP_TOKEN, ADD_OP_TOKEN, LESS_OP_TOKEN, GREAT_OP_TOKEN, LOE_OP_TOKEN, GOE_OP_TOKEN, EQUAL_OP_TOKEN, NOT_EQUAL_OP_TOKEN, CLOSE_PARENTHESES_TOKEN, COMMA_TOKEN, SEMICOLON_TOKEN, CLOSE_CURLY_BRACES_TOKEN, CLOSE_BRACKETS_TOKEN, OPEN_BRACKETS_TOKEN };
	switch (t->kind)
	{
	case ASSIGNMENT_OP_TOKEN:
	case MUL_OP_TOKEN:
	case ADD_OP_TOKEN:
	case LESS_OP_TOKEN:
	case GREAT_OP_TOKEN:
	case LOE_OP_TOKEN:
	case GOE_OP_TOKEN:
	case EQUAL_OP_TOKEN:
	case NOT_EQUAL_OP_TOKEN:
	case CLOSE_PARENTHESES_TOKEN:
	case COMMA_TOKEN:
	case SEMICOLON_TOKEN:
	case CLOSE_CURLY_BRACES_TOKEN:
	case CLOSE_BRACKETS_TOKEN:
		fprintf(yyout, "Rule (VAR_tag -> Epsilon)\n");
		if (node != NULL)
		{
			////////
			////////
			// important - need to add a check - if array without brackets is send to a function that expect array with same dimmention size and type it is allowed
			////////
			////////
			assignmentToArrayPtr(node->type, id->lineNumber);
			assignmentToFunction(node->role, id->lineNumber);
		}
		back_token();
		if (node != NULL)
		{
			return node->type;
		}
		else
		{
			return "type error";
		}
		break;

	case OPEN_BRACKETS_TOKEN: // the var is array
		fprintf(yyout, "Rule (VAR_tag -> [EXPR_LIST])\n");
		if (node != NULL)
		{
			variableWithBracketsMustBeDeclaredAsArray(node->name, node->type, node->role, id->lineNumber);
		}
		numOfDimensions++;
		parse_EXPR_LIST(&numOfDimensions);
		if (node != NULL)
		{
			arrayDimensionSizeMustBeAsInDeclaration(node->name, node->type, node->numOfDimensions, numOfDimensions, id->lineNumber);
		}
		match(CLOSE_BRACKETS_TOKEN);
		if (node != NULL)
		{
			if (strstr(node->type, "int"))
			{
				return "int";
			}
			else if (strstr(node->type, "float"))
			{
				return "float";
			}
			else if (strstr(node->type, "void"))
			{
				return "void";
			}
		}
		else return "type error";
		break;

	default:
		errorHandlerAndRecovery(expectedKinds, 15, t, followTokens, 14);
		return "error";
		break;
	}
}

void parse_EXPR_LIST(int* numOfDimensionsPtr)
{
	char* exprType;
	int lineNumber;
	fprintf(yyout, "Rule (EXPR_LIST -> EXPR EXPR_LIST_tag)\n");
	lineNumber = next_token()->lineNumber;
	back_token();
	exprType = parse_EXPR();
	typeOfExpr_i_mustBeIntegerInArrayCalls(exprType, lineNumber);
	parse_EXPR_LIST_tag(numOfDimensionsPtr);
}

void parse_EXPR_LIST_tag(int* numOfDimensionsPtr)
{
	eTOKENS followTokens[] = { CLOSE_BRACKETS_TOKEN };
	Token* t = next_token();
	eTOKENS expectedKinds[] = { CLOSE_BRACKETS_TOKEN, COMMA_TOKEN };
	switch (t->kind)
	{
	case CLOSE_BRACKETS_TOKEN:
		fprintf(yyout, "Rule (EXPR_LIST_tag -> Epsilon)\n");
		back_token();
		break;

	case COMMA_TOKEN:
		fprintf(yyout, "Rule (EXPR_LIST_tag -> , EXPR EXPR_LIST_tag)\n");
		(*numOfDimensionsPtr)++;
		int lineNumber = next_token()->lineNumber;
		back_token();
		char* exprType = parse_EXPR();
		typeOfExpr_i_mustBeIntegerInArrayCalls(exprType, lineNumber);
		parse_EXPR_LIST_tag(numOfDimensionsPtr);
		break;

	default:
		errorHandlerAndRecovery(expectedKinds, 2, t, followTokens, 1);
		break;
	}
}

void parse_CONDITION()
{
	eTOKENS rel_op_kinds[] = { LESS_OP_TOKEN, GREAT_OP_TOKEN, LOE_OP_TOKEN, GOE_OP_TOKEN, EQUAL_OP_TOKEN, NOT_EQUAL_OP_TOKEN };
	fprintf(yyout, "Rule (CONDITION -> EXPR rel_op EXPR)\n");
	char* exprType = parse_EXPR();
	Token* t = match_rel_op(rel_op_kinds);
	conditionMustHaveNonArrayType(exprType, t->lineNumber);
	exprType = parse_EXPR();
	conditionMustHaveNonArrayType(exprType, t->lineNumber);
}

char* parse_EXPR()
{
	char* termType;
	char* exprType;
	fprintf(yyout, "Rule (EXPR -> TERM EXPR_tag)\n");
	termType = parse_TERM();
	exprType = parse_EXPR_tag();
	if (strcmp(termType, "float") == 0 || strcmp(exprType, "float") == 0)
	{
		return "float";
	}
	else
	{
		return termType;
	}
}

char* parse_EXPR_tag()
{
	eTOKENS followTokens[] = { LESS_OP_TOKEN, GREAT_OP_TOKEN, LOE_OP_TOKEN, GOE_OP_TOKEN, EQUAL_OP_TOKEN, NOT_EQUAL_OP_TOKEN, CLOSE_PARENTHESES_TOKEN, CLOSE_BRACKETS_TOKEN, CLOSE_CURLY_BRACES_TOKEN, COMMA_TOKEN, SEMICOLON_TOKEN };
	Token* t = next_token();
	eTOKENS expectedKinds[] = { ADD_OP_TOKEN, LESS_OP_TOKEN, GREAT_OP_TOKEN, LOE_OP_TOKEN, GOE_OP_TOKEN, EQUAL_OP_TOKEN, NOT_EQUAL_OP_TOKEN, CLOSE_PARENTHESES_TOKEN, COMMA_TOKEN, SEMICOLON_TOKEN, CLOSE_CURLY_BRACES_TOKEN, CLOSE_BRACKETS_TOKEN };
	switch (t->kind)
	{

	case ADD_OP_TOKEN:
		fprintf(yyout, "Rule (EXPR_tag -> + TERM EXPR_tag)\n");
		char* termType = parse_TERM();
		char* exprType = parse_EXPR_tag();
		if (strcmp(termType, "float") == 0 || strcmp(exprType, "float") == 0)
		{
			return "float";
		}
		else
		{
			return termType;
		}
		break;

	case LESS_OP_TOKEN:
	case GREAT_OP_TOKEN:
	case LOE_OP_TOKEN:
	case GOE_OP_TOKEN:
	case EQUAL_OP_TOKEN:
	case NOT_EQUAL_OP_TOKEN:
	case CLOSE_PARENTHESES_TOKEN:
	case COMMA_TOKEN:
	case SEMICOLON_TOKEN:
	case CLOSE_CURLY_BRACES_TOKEN:
	case CLOSE_BRACKETS_TOKEN:
		fprintf(yyout, "Rule (EXPR_tag -> Epsilon)\n");
		back_token();
		break;

	default:
		errorHandlerAndRecovery(expectedKinds, 12, t, followTokens, 11);
		break;
	}
}

char* parse_TERM()
{
	char* factorType;
	char* termType;
	fprintf(yyout, "Rule (TERM -> FACTOR TERM_tag)\n");
	factorType = parse_FACTOR();
	termType = parse_TERM_tag();
	if (strcmp(termType, "float") == 0 || strcmp(factorType, "float") == 0)
	{
		return "float";
	}
	else
	{
		return factorType;
	}
}

char* parse_TERM_tag()
{
	eTOKENS followTokens[] = { ADD_OP_TOKEN, LESS_OP_TOKEN, GREAT_OP_TOKEN, LOE_OP_TOKEN, GOE_OP_TOKEN, EQUAL_OP_TOKEN, NOT_EQUAL_OP_TOKEN, CLOSE_PARENTHESES_TOKEN, COMMA_TOKEN, SEMICOLON_TOKEN, CLOSE_CURLY_BRACES_TOKEN, CLOSE_BRACKETS_TOKEN };
	Token* t = next_token();
	eTOKENS expectedKinds[] = { MUL_OP_TOKEN, ADD_OP_TOKEN, LESS_OP_TOKEN, GREAT_OP_TOKEN, LOE_OP_TOKEN, GOE_OP_TOKEN, EQUAL_OP_TOKEN, NOT_EQUAL_OP_TOKEN, CLOSE_PARENTHESES_TOKEN, COMMA_TOKEN, SEMICOLON_TOKEN, CLOSE_CURLY_BRACES_TOKEN, CLOSE_BRACKETS_TOKEN };
	switch (t->kind)
	{

	case MUL_OP_TOKEN:
		fprintf(yyout, "Rule (TERM_tag -> * FACTOR TERM_tag)\n");
		char* factorType = parse_FACTOR();
		char* termType = parse_TERM_tag();
		if (strcmp(termType, "float") == 0 || strcmp(factorType, "float") == 0)
		{
			return "float";
		}
		else
		{
			return factorType;
		}
		break;

	case ADD_OP_TOKEN:
	case LESS_OP_TOKEN:
	case GREAT_OP_TOKEN:
	case LOE_OP_TOKEN:
	case GOE_OP_TOKEN:
	case EQUAL_OP_TOKEN:
	case NOT_EQUAL_OP_TOKEN:
	case CLOSE_PARENTHESES_TOKEN:
	case COMMA_TOKEN:
	case SEMICOLON_TOKEN:
	case CLOSE_CURLY_BRACES_TOKEN:
	case CLOSE_BRACKETS_TOKEN:
		fprintf(yyout, "Rule (TERM_tag -> Epsilon)\n");
		back_token();
		break;

	default:
		errorHandlerAndRecovery(expectedKinds, 13, t, followTokens, 12);
		break;
	}
}

char* parse_FACTOR()
{
	eTOKENS followTokens[] = { MUL_OP_TOKEN, ADD_OP_TOKEN, LESS_OP_TOKEN, GREAT_OP_TOKEN, LOE_OP_TOKEN, GOE_OP_TOKEN, EQUAL_OP_TOKEN, NOT_EQUAL_OP_TOKEN, CLOSE_PARENTHESES_TOKEN, COMMA_TOKEN, SEMICOLON_TOKEN, CLOSE_CURLY_BRACES_TOKEN, CLOSE_BRACKETS_TOKEN };
	Token* t = next_token();
	eTOKENS expectedKinds[] = { ID_TOKEN, NUM_INT_TOKEN, NUM_FLOAT_TOKEN, OPEN_PARENTHESES_TOKEN };
	switch (t->kind)
	{
	case ID_TOKEN:
		fprintf(yyout, "Rule (FACTOR -> id FACTOR_tag)\n");
		char* factorType;
		int isDeclared = usedIdMustBeDeclared(t->lexeme, t->lineNumber);
		struct SymbolNode* node = getNodeById(t->lexeme);
		factorType = parse_FACTOR_tag(node, t);
		if (isDeclared == 0)
		{
			return "type error";
		}
		return factorType;
		break;

	case NUM_INT_TOKEN:
		fprintf(yyout, "Rule (FACTOR -> int_num)\n");
		return "int";
		break;

	case NUM_FLOAT_TOKEN:
		fprintf(yyout, "Rule (FACTOR -> float_num)\n");
		return "float";
		break;

	case OPEN_PARENTHESES_TOKEN:
		fprintf(yyout, "Rule (FACTOR -> (EXPR))\n");
		char* exprType = parse_EXPR();
		match(CLOSE_PARENTHESES_TOKEN);
		return exprType;
		break;

	default:
		errorHandlerAndRecovery(expectedKinds, 4, t, followTokens, 13);
		return "error";
		break;
	}
}

char* parse_FACTOR_tag(struct SymbolNode* node, Token* id)
{
	int numOfParamsInCall = 0;
	char** listOfParametersTypesInCall;
	listOfParametersTypesInCall = (char**)malloc(30 * sizeof(char*));;
	Token* t = next_token();
	switch (t->kind)
	{
	case OPEN_PARENTHESES_TOKEN:
		fprintf(yyout, "Rule (FACTOR_tag -> (ARGS))\n");
		parse_ARGS(&numOfParamsInCall, listOfParametersTypesInCall);
		funcCallValidityCheck(getFuncFullBodyById(id->lexeme), numOfParamsInCall, listOfParametersTypesInCall, t->lineNumber);
		match(CLOSE_PARENTHESES_TOKEN);
		if (node != NULL)
		{
			return node->type;
		}
		else return "type error";
		break;

	default:
		fprintf(yyout, "Rule (FACTOR_tag -> VAR_tag)\n");
		back_token();
		return parse_VAR_tag(node, t);
		break;
	}
}