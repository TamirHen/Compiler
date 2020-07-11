#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<conio.h>
#include<string.h>
#include<stdlib.h>
#include"semanticFunctions.h"
struct Table* CreateTable();
int DeleteTable(int tableNumber);
struct SymbolNode* CreateNode(char* , char* , char* , int* , int , char** , int );
void InsertNode(int tableNumber, struct SymbolNode* p);
int DeleteNode(int tableNumber, char* name);
int SearchNode(int , struct SymbolNode*);
int SearchIdInTableAndIncUses(char*, struct Table*);
//char* getIdType(char*);
struct SymbolNode* getNodeById(char*);
struct SymbolNode* getFuncFullBodyById(char*);

struct SymbolNode
{
	char name[30], role[20], type[20];
	int listOfArraySizes[15];
	char listOfParameterTypes[30][20];
	int numOfUses, numOfDimensions;
	int numOfParams;
	struct Table* table;
	struct SymbolNode* next, *prev;
};

struct Table
{
	int tableNumber;
	int size;
	int isContainReturnStmt;
	struct SymbolNode* function;
	struct SymbolNode* firstNode, * lastNode;
	struct Table* next, *prev;
};
struct Table* firstTable, * lastTable;
