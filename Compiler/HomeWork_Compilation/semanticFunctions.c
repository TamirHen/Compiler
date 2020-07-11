#include "semanticFunctions.h"

int usedIdMustBeDeclared(char* id, int lineNumber)
{
	struct Table* t = lastTable;
	//if (SearchIdInTableAndIncUses(id, t) == 1) // search in local definitions
	//{
	//	return 1;
	//}

	//t = firstTable;
	while (t->prev != NULL) 
	{
		if (SearchIdInTableAndIncUses(id, t) == 1)
		{
			return 1;
		}
		t = t->prev;
	}

	if (SearchIdInTableAndIncUses(id, t) == 1) // search in the root table
	{
		return 1;
	}

	printf("Semantic error in line %d - id \"%s\" not declared\n", lineNumber, id);
	return 0;
}

int duplicateDeclarationInSameScopeForbidden(struct SymbolNode* node)
{
	if (node == NULL)
	{
		return;
	}
	struct Table* t = node->table;
	if (SearchNode(t->tableNumber, node) == 1)
	{
		back_token();
		Token* token = next_token();
		printf("Semantic error in line %d - id \"%s\" is already declared in this scope\n", token->lineNumber, node->name);
		return 1;
	}
	return 0;
}

int varMustBeUsedAtLeastOnce(struct Table* t, int lineNumber)
{
	int i;
	struct SymbolNode* p = t->firstNode;
	for (i = 0; i < t->size; i++)
	{
		if (p->numOfUses == 0 && strcmp(p->role, "variable") == 0)
		{
			printf("Semantic error in line %d - id \"%s\" is declared but never used\n", lineNumber, p->name);
		}
		p = p->next;
	}
}

void assignmentToArrayPtr(char* type, int lineNumber)
{
	if (type == NULL)
	{
		return;
	}
	if (strstr(type, "[]")) //check if type is array of any kind
	{
		printf("Semantic error in line %d - using type \"%s\" is forbidden, please use the array's element\n", lineNumber, type);
	}
}

void assignmentToFunction(char* role, int lineNumber)
{
	if (strstr(role, "definition")) //check if type is function of any kind
	{
		printf("Semantic error in line %d - assignment to fucntions is forbidden\n", lineNumber);
	}
}

void variableWithBracketsMustBeDeclaredAsArray(char* name, char* type, char* role, int lineNumber)
{
	if (!strstr(type, "[]") && strcmp(role, "variable") == 0) //check if type is array of any kind
	{
		printf("Semantic error in line %d - variable \"%s\" called with BRACKETS but it is not array\n", lineNumber, name);
	}
}

void arrayDimensionSizeMustBeAsInDeclaration(char* name, char* type, int numOfExpectedDimensions, int numOfActualDimensions, int lineNumber)
{
	if (strstr(type, "[]") && numOfActualDimensions != numOfExpectedDimensions) //check if type is array of any kind
	{
		printf("Semantic error in line %d - variable \"%s\" called with %d dimensions but declared as %d dimensions\n", lineNumber, name, numOfActualDimensions, numOfExpectedDimensions);
	}
}

void typeOfExpr_i_mustBeIntegerInArrayCalls(char* type, int lineNumber)
{
	if (!strstr(type, "int"))
	{
		if (strstr(type, "float"))
		{
			type = "float";
		}
		printf("Semantic error in line %d - expression type \"%s\" in array element call is forbidden\n", lineNumber, type);
	}
}

void leftAndRightSideSameTypeOrLeftFloatAndRightInt(char* left, char* right, int lineNumber)
{
	if (strstr(left, "int"))
	{
		left = "int";
	}
	if (strstr(left, "float"))
	{
		left = "float";
	}	
	if (strstr(right, "int"))
	{
		right = "int";
	}
	if (strstr(right, "float"))
	{
		right = "float";
	}

	if (strcmp(left, right) != 0)
	{
		if (!strstr(left, right) && !strstr(right, left))
		{
			if (strstr(left, "float") && strstr(right, "int"))
			{
				// its ok - do nothing
			}
			else //print error
			{
				printf("Semantic error in line %d - assignment type \"%s\" to type \"%s\" is forbidden\n", lineNumber, right, left);
			}
		}
	}
}

void declaredFunctionMustBeImplemented(int lineNumber)
{
	struct SymbolNode* node = firstTable->firstNode;
	struct SymbolNode* tempNode;
	int flag = 0;
	while (node->next != NULL)
	{
		if (strcmp(node->role, "pre-definition") == 0)
		{
			tempNode = node;
			while (tempNode->next != NULL)
			{
				if (strcmp(tempNode->role, "full definition") == 0 && strcmp(tempNode->name, node->name) == 0)
				{
					flag = 1;
					break;
				}
				tempNode = tempNode->next;
			}
			if (strcmp(tempNode->role, "full definition") == 0 && strcmp(tempNode->name, node->name) == 0) // check the last node as well
			{
				flag = 1;
				//amountOfParamsMustBeMatchWithFuncDeclaration(node, tempNode, lineNumber);
				//typeOfParamsMustBeMatchWithFuncDeclaration(node, tempNode, lineNumber);
			}
			if (flag == 0)
			{
				printf("Semantic error in line %d - function \"%s\" declared but not implemented\n", lineNumber, node->name);
			}
		}
		flag = 0;
		node = node->next;
	}
	flag = 0;
	if (strcmp(node->role, "pre-definition") == 0) // check the last node as well
	{
		tempNode = node;
		while (tempNode->next != NULL)
		{
			if (strcmp(tempNode->role, "full definition") == 0 && strcmp(tempNode->name, node->name) == 0)
			{
				flag = 1;
				break;
			}
		}
		if (strcmp(tempNode->role, "full definition") == 0 && strcmp(tempNode->name, node->name) == 0) // check the last node as well
		{
			flag = 1;
			//amountOfParamsMustBeMatchWithFuncDeclaration(node, tempNode, lineNumber);
			//typeOfParamsMustBeMatchWithFuncDeclaration(node, tempNode, lineNumber);
		}
		if (flag == 0)
		{
			printf("Semantic error in line %d - function \"%s\" declared but not implemented\n", lineNumber, node->name);
		}
	}
}

void amountOfParamsMustBeMatchWithFuncDeclaration(struct SymbolNode* preFunc, struct SymbolNode* fullFunc, int lineNumber)
{
	if (preFunc == NULL || strcmp(preFunc->role, "full definition") == 0)
	{
		// funct has no declaration
		return;
	}
	if (preFunc->numOfParams != fullFunc->numOfParams)
	{
		printf("Semantic error in line %d - function \"%s\" declared with %d parameters but implemented with %d parameters\n", lineNumber, preFunc->name, preFunc->numOfParams, fullFunc->numOfParams);
	}
}

void typeOfParamsMustBeMatchWithFuncDeclaration(struct SymbolNode* preFunc, struct SymbolNode* fullFunc, int lineNumber)
{
	int i, length = 0;
	if (preFunc == NULL || strcmp(preFunc->role, "full definition") == 0)
	{
		// func has no declaration
		return;
	}
	if (preFunc->numOfParams!= fullFunc->numOfParams)
	{
		printf("Semantic error in line %d - function \"%s\" declared with different parameters types than implemented\n", lineNumber, preFunc->name);
		return;
	}
	length = preFunc->numOfParams;

	for (i = 0; i < length; i++)
	{
		if (strcmp(preFunc->listOfParameterTypes[i], fullFunc->listOfParameterTypes[i]) != 0)
		{
			printf("Semantic error in line %d - function \"%s\" declared with different parameter types than implemented\n", lineNumber, preFunc->name);
			break;
		}
	}

}

void funcCallValidityCheck(struct SymbolNode* fullFunc, int amountOfParamsInFuncCall, char** listOfParametersTypesInCall, int lineNumber)
{
	if (fullFunc == NULL)
	{
		printf("Semantic error in line %d - there is no function with body with this name\n", lineNumber);
		return;
	}
	amountOfParamsInFunctionCallShouldMatchFuncDefinition(fullFunc, amountOfParamsInFuncCall, lineNumber);
	typesOfParamsInFunctionCallShouldMatchFuncDefinition(fullFunc, amountOfParamsInFuncCall, listOfParametersTypesInCall, lineNumber);
}

void amountOfParamsInFunctionCallShouldMatchFuncDefinition(struct SymbolNode* fullFunc, int amountOfParamsInFuncCall, int lineNumber)
{
	if (fullFunc == NULL)
	{
		return;
	}

	if (fullFunc->numOfParams != amountOfParamsInFuncCall)
	{
		printf("Semantic error in line %d - function \"%s\" called with %d parameters but expected %d parameters\n", lineNumber, fullFunc->name, amountOfParamsInFuncCall, fullFunc->numOfParams);
	}

}

void typesOfParamsInFunctionCallShouldMatchFuncDefinition(struct SymbolNode* fullFunc, int amountOfParamsInFuncCall, char** listOfParametersTypesInCall, int lineNumber)
{
	int i;
	if (fullFunc == NULL)
	{
		return;
	}
	if (fullFunc->numOfParams != amountOfParamsInFuncCall)
	{
		printf("Semantic error in line %d -  function \"%s\" received variables with incompatible types\n", lineNumber, fullFunc->name);
		return;
	}
	for (i = 0; i < fullFunc->numOfParams; i++)
	{
		if (strcmp(fullFunc->listOfParameterTypes[i], listOfParametersTypesInCall[i]) != 0)
		{
			printf("Semantic error in line %d -  function \"%s\" received variables with incompatible types\n", lineNumber, fullFunc->name);
			break;
		}
	}

}

void nonVoidFunctionMustHaveAtListOneReturnStmt(struct Table* t, int lineNumber)
{
	if (t->function != NULL)
	{
		if (strcmp(t->function->type, "void") != 0 && strcmp(t->function->type, "type error") != 0)
		{
			if (t->isContainReturnStmt == 0)
			{
				printf("Semantic error in line %d - function \"%s\" with return type \"%s\" must have at least one return statement with value\n", lineNumber, t->function->name, t->function->type);
			}
		}
	}
}

void expressionTypeInReturnStmtOfFunctionMustMatchWithTheFunctionType(struct Table* table, char* returnType, int lineNumber)
{
	if (table->function != NULL)
	{
		if (strcmp(table->function->type, returnType) != 0)
		{
			printf("Semantic error in line %d - function \"%s\" returning type \"%s\" but should return type \"%s\"\n", lineNumber, table->function->name, returnType, table->function->type);
		}
	}
}

void voidOrMainProgramMustNotReturnValue(struct Table* table, int lineNumber)
{
	if (table->function != NULL)
	{
		if (table->tableNumber == 0)
		{
			printf("Semantic error in line %d - main program can not return value\n", lineNumber);
		}
		else if (strcmp(table->function->type, "void") == 0)
		{
			printf("Semantic error in line %d - function \"%s\"return value although its return type is \"void\"\n", lineNumber, table->function->name);
		}
	}
}

void conditionMustHaveNonArrayType(char* type, int lineNumber)
{
	if (strstr(type, "[]")) // check if type is array of any kind.
	{
		printf("Semantic error in line %d - array of type \"%s\" inside the condition. please use the array's element.\n", lineNumber, type);
	}
}


