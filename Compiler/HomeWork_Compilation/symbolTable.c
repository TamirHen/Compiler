#include "symbolTable.h"
int tableSize = 0;
#define NULL 0
int nextTableNumber = 0;
int getTableNumber()
{
	return nextTableNumber++;
}

struct Table* CreateTable()
{
	struct Table* t;
	t = malloc(sizeof(struct Table));
	t->size = 0;
	t->isContainReturnStmt = 0;
	t->function = NULL;
	t->tableNumber = getTableNumber();
	t->firstNode = NULL;
	t->next = NULL;
	if (tableSize == 0)
	{
		t->prev = NULL;
		firstTable = t;
		lastTable = t;
	}
	else
	{
		t->prev = lastTable;
		lastTable->next = t;
		lastTable = t;
	}
	tableSize++;

	//printf("----- Table created {%d} -----\n", t->tableNumber);

	return t;
}

int DeleteTable(int tableNumber)
{
	//printf("----- Before delete -----\nTable Content:\n");

	struct Table* t = firstTable;
	while (t->next != NULL)
	{
		if (t->tableNumber == tableNumber)
		{
			if (t->prev != NULL) // check that not the root table
			{
				(t->prev)->next = t->next;
				(t->next)->prev = t->prev;
			}
			else // is root node
			{
				(t->next)->prev = NULL;
				firstTable = t->next;
			}
			struct SymbolNode* node = t->firstNode;
			if (node != NULL)
			{
				while (node->next != NULL)
				{
					struct SymbolNode* toDelete = node;
					node = node->next;
					//printf("name{%s} role{%s} type{%s}\n", toDelete->name, toDelete->role, toDelete->type);
					free(toDelete);
				}
				//printf("name{%s} role{%s} type{%s}\n", node->name, node->role, node->type);
				free(node);
			}
			free(t);
			tableSize--;

			//printf("----- Table deleted {%d} -----\n", tableNumber);

			return 1;
		}
		t = t->next;
	}
	if (t->tableNumber == tableNumber) // check the last tableNode as well
	{
		if (t->prev != NULL)
		{
			(t->prev)->next = NULL;
			lastTable = t->prev;
		}
		struct SymbolNode* node = t->firstNode;
		if (node != NULL)
		{
			while (node->next != NULL)
			{
				struct SymbolNode* toDelete = node;
				node = node->next;
				//printf("name{%s} role{%s} type{%s}\n", toDelete->name, toDelete->role, toDelete->type);
				free(toDelete);
			}
			//printf("name{%s} role{%s} type{%s}\n", node->name, node->role, node->type);
			free(node);
		}

		free(t);
		tableSize--;

		//printf("----- Table deleted {%d} -----\n", tableNumber);

		return 1;
	}
	//printf("table to delete not found\n");
	return 0;
}

struct SymbolNode* CreateNode(char* name, char* role, char* type, int* listOfArraySizes, int sizeArraySizeList, char** listOfParameterTypes, int sizeParamList)
{
	int i, j;
	struct SymbolNode* p;
	p = malloc(sizeof(struct SymbolNode));
	strcpy(p->name, name);
	strcpy(p->role, role);
	strcpy(p->type, type);
	p->numOfUses = 0;
	p->numOfParams = sizeParamList;
	p->numOfDimensions = sizeArraySizeList;
	if (strcmp(role, "full definition") == 0) // it is function with body
	{
		p->table = lastTable->prev; // full definition functions always in the prev scope (and not in their own)
	}
	else
	{
		p->table = lastTable;
	}
	for (i = 0; i < sizeArraySizeList; i++)
	{
		p->listOfArraySizes[i] = listOfArraySizes[i];
	}
	for (i = 0; i < sizeParamList ; i++)
	{
		strcpy(p->listOfParameterTypes[i], listOfParameterTypes[i]);
	}

	return p;
}

void InsertNode(int tableNumber, struct SymbolNode* p)
{
	struct Table* t = firstTable;
	while (t->next != NULL)
	{
		if (t->tableNumber == tableNumber)
		{
			break;
		}
		t = t->next;
	}

	if (duplicateDeclarationInSameScopeForbidden(p) == 0)
	{
		if (t->size == 0)
		{
			p->prev = NULL;
			p->next = NULL;
			t->firstNode = p;
			t->lastNode = p;
		}
		else
		{
			p->next = NULL;
			p->prev = t->lastNode;
			t->lastNode->next = p;
			t->lastNode = p;
		}
		t->size++;
	}
	else
	{
		free(p);
	}

		//printf("----- Node inserted in table {%d} -----\n", tableNumber);

	//}
}

int DeleteNode(int tableNumber, char* name)
{
	struct Table* t = firstTable;
	while (t->next != NULL)
	{
		if (t->tableNumber == tableNumber)
		{
			break;
		}
		t = t->next;
	}
	int a;
	struct SymbolNode* p, * q;
	p = t->firstNode;
	while (p->next != NULL)
	{
		if (strcmp(p->name, name) == 0)
		{
			if (p->prev != NULL) // check if not root node
			{
				(p->prev)->next = p->next;
				(p->next)->prev = p->prev;
			}
			else // is root node
			{
				(p->next)->prev = NULL;
				t->firstNode = p->next;
			}
			free(p);
			t->size--;

			//printf("----- Node deleted from table {%d} -----\n", tableNumber);

			return 1;
		}
		p = p->next;
	}
	if (strcmp(p->name, name) == 0) // check the last node as well
	{
		(p->prev)->next = NULL;
		t->lastNode = p->prev;
		free(p);
		t->size--;

		//printf("----- Node deleted from table {%d} -----\n", tableNumber);

		return 1;
	}
	return 0;
	//a = SearchNode(tableNumber, name);
	//if (a == 0)
	//	printf("\n\tLabel not found\n");
	//else
	//{
		//if (strcmp(t->firstNode->name, name) == 0)
		//	t->firstNode = t->firstNode->next;
		//else if (strcmp(t->lastNode->name, name) == 0)
		//{
		//	q = p->next;
		//	while (strcmp(q->name, name) != 0)
		//	{
		//		p = p->next;
		//		q = q->next;
		//	}
		//	p->next = NULL;
		//	t->lastNode = p;
		//}
		//else
		//{
		//	q = p->next;
		//	while (strcmp(q->name, name) != 0)
		//	{
		//		p = p->next;
		//		q = q->next;
		//	}
		//	p->next = q->next;
		//}
		//t->size--;
		//printf("\n\tNode deleted\n");
	//}
}

int SearchNode(int tableNumber, struct SymbolNode* snToCheck)
{
	struct Table* t = firstTable;
	while (t->next != NULL)
	{
		if (t->tableNumber == tableNumber)
		{
			break;
		}
		t = t->next;
	}

	int i, flag = 0;
	struct SymbolNode* p;
	p = t->firstNode;
	for (i = 0; i < t->size; i++)
	{
		if (strcmp(p->name, snToCheck->name) == 0 )
			if (!(strcmp(p->role, "pre-definition") == 0 && strcmp(snToCheck->role, "full definition") == 0)) // check that this name not belong to function with declaration
			{
				flag = 1;
				break;
			}
		p = p->next;
	}
	return flag;
}

int SearchIdInTableAndIncUses(char* id, struct Table* t)
{
	struct SymbolNode* p = t->firstNode;
	int i;
	for (i = 0; i < t->size; i++)
	{
		if (strcmp(p->name, id) == 0)
		{
			p->numOfUses++;
			return 1;
		}
		p = p->next;
	}
	return 0;
}


//char* getIdType(char* id)
//{
//	struct Table* t = lastTable;
//	struct SymbolNode* p = t->firstNode;
//	int i;
//	for (i = 0; i < t->size; i++)
//	{
//		if (strcmp(id, p->name) == 0) // search in local definitions
//		{
//			return p->type;
//		}
//		p = p->next;
//	}
//
//	t = firstTable;
//	while (t->next != NULL) // search in global definitions (without last table)
//	{
//		p = t->firstNode;
//		for (i = 0; i < t->size; i++)
//		{
//			if (strcmp(id, p->name) == 0) // search in local definitions
//			{
//				return p->type;
//			}
//			p = p->next;
//		}
//		t = t->next;
//	}
//
//	return "not declared";
//}

struct SymbolNode* getNodeById(char* id)
{
	struct Table* t = lastTable;
	struct SymbolNode* p = t->firstNode;
	int i;
	//for (i = 0; i < t->size; i++)
	//{
	//	if (strcmp(id, p->name) == 0) // search in local definitions
	//	{
	//		return p;
	//	}
	//	p = p->next;
	//}

	//t = firstTable;
	while (t->prev != NULL) 
	{
		p = t->firstNode;
		for (i = 0; i < t->size; i++)
		{
			if (strcmp(id, p->name) == 0) // search in local definitions
			{
				return p;
			}
			p = p->next;
		}
		t = t->prev;
	}

	p = t->firstNode;
	for (i = 0; i < t->size; i++) // check in the root table
	{
		if (strcmp(id, p->name) == 0) 
		{
			return p;
		}
		p = p->next;
	}

	return NULL; // node not found

}

struct SymbolNode* getFuncFullBodyById(char* id)
{
	struct Table* t = lastTable;
	struct SymbolNode* p = t->firstNode;
	int i;

	while (t->prev != NULL)
	{
		p = t->firstNode;
		for (i = 0; i < t->size; i++)
		{
			if (strcmp(id, p->name) == 0 && strcmp(p->role, "full definition") == 0)
			{
				return p;
			}
			p = p->next;
		}
		t = t->prev;
	}

	p = t->firstNode;
	for (i = 0; i < t->size; i++) // check in the root table
	{
		if (strcmp(id, p->name) == 0 && strcmp(p->role, "full definition") == 0)
		{
			return p;
		}
		p = p->next;
	}

	return NULL; // node not found

}