#pragma once
#include"symbolTable.h"
#include"parser.h"
int usedIdMustBeDeclared(char*, int);
int duplicateDeclarationInSameScopeForbidden(struct SymbolNode*);
int varMustBeUsedAtLeastOnce(struct Table*, int);
void assignmentToArrayPtr(char*, int);
void assignmentToFunction(char*, int);
void variableWithBracketsMustBeDeclaredAsArray(char*, char*, char*, int);
void arrayDimensionSizeMustBeAsInDeclaration(char*, char*, int, int, int);
void typeOfExpr_i_mustBeIntegerInArrayCalls(char*, int);
void leftAndRightSideSameTypeOrLeftFloatAndRightInt(char*, char*, int);
void declaredFunctionMustBeImplemented(int);
void amountOfParamsMustBeMatchWithFuncDeclaration(struct SymbolNode*, struct SymbolNode*, int);
void typeOfParamsMustBeMatchWithFuncDeclaration(struct SymbolNode* , struct SymbolNode* , int);
void funcCallValidityCheck(struct SymbolNode*, int, char**, int);
void amountOfParamsInFunctionCallShouldMatchFuncDefinition(struct SymbolNode*, int, int);
void typesOfParamsInFunctionCallShouldMatchFuncDefinition(struct SymbolNode*, int, char**, int);
void nonVoidFunctionMustHaveAtListOneReturnStmt(struct Table*, int);
void expressionTypeInReturnStmtOfFunctionMustMatchWithTheFunctionType(struct Table*, char*, int);
void voidOrMainProgramMustNotReturnValue(struct Table*, int);
void conditionMustHaveNonArrayType(char*, int);