#define _CRT_SECURE_NO_DEPRECATE
#include "Token.h"

/* This package describes the storage of tokens identified in the input text.
* The storage is a bi-directional list of nodes.
* Each node is an array of tokens; the size of this array is defined as TOKEN_ARRAY_SIZE.
* Such data structure supports an efficient way to manipulate tokens.

There are three functions providing an external access to the storage:
- function create_and_store_tokens ; it is called by the lexical analyzer when it identifies a legal token in the input text.
- functions next_token and back_token; they are called by parser during the syntax analysis (the second stage of compilation)
*/

int restartFlag = 0;
int currentIndex = 0;
Node* currentNode = NULL;
int balance = 0;
#define TOKEN_ARRAY_SIZE 1000

/*
* This function creates a token and stores it in the storage.
*/
void create_and_store_token(eTOKENS kind, char* lexeme, int numOfLine)
{
	int length = strlen(lexeme) + 1;

	// case 1: there is still no tokens in the storage.
	if (currentNode == NULL)
	{
		currentNode = (Node*)malloc(sizeof(Node));

		if (currentNode == NULL)
		{
			fprintf(yyout, "\nUnable to allocate memory! \n");
			exit(0);
		}
		currentNode->tokensArray =
			(Token*)calloc(sizeof(Token), TOKEN_ARRAY_SIZE);
		if (currentNode->tokensArray == NULL)
		{
			fprintf(yyout, "\nUnable to allocate memory! \n");
			exit(0);
		}
		currentNode->prev = NULL;
		currentNode->next = NULL;
	}

	// case 2: at least one token exsits in the storage.
	else
	{
		// the array (the current node) is full, need to allocate a new node
		if (currentIndex == TOKEN_ARRAY_SIZE - 1)
		{
			currentIndex = 0;
			currentNode->next = (Node*)malloc(sizeof(Node));

			if (currentNode == NULL)
			{
				fprintf(yyout, "\nUnable to allocate memory! \n");
				exit(0);
			}
			currentNode->next->prev = currentNode;
			currentNode = currentNode->next;
			currentNode->tokensArray =
				(Token*)calloc(sizeof(Token), TOKEN_ARRAY_SIZE);

			if (currentNode->tokensArray == NULL)
			{
				fprintf(yyout, "\nUnable to allocate memory! \n");
				exit(0);
			}
			currentNode->next = NULL;
		}

		// the array (the current node) is not full
		else
		{
			currentIndex++;
		}
	}

	currentNode->tokensArray[currentIndex].kind = kind;
	currentNode->tokensArray[currentIndex].lineNumber = numOfLine;

	currentNode->tokensArray[currentIndex].lexeme = (char*)malloc(sizeof(char) * length);
#ifdef _WIN32
	strcpy_s(currentNode->tokensArray[currentIndex].lexeme, length, lexeme);
#else
	strcpy(currentNode->tokensArray[currentIndex].lexeme, lexeme);
#endif		

}

/*
* This function returns the token in the storage that is stored immediately before the current token (if exsits).
*/
Token* back_token() 
{
	balance--;
	currentIndex--;
	if (currentIndex == -1)
	{
		if (currentNode == NULL)
		{
			return;
		}
		if (currentNode->prev == NULL)
		{
			fprintf(yyout, "\nUnable to find token! \n");
			return;
		}
		currentNode = currentNode->prev;
		currentIndex = TOKEN_ARRAY_SIZE-1;

	}
	if (&currentNode->tokensArray[currentIndex] == NULL)
	{
		fprintf(yyout, "\nUnable to find token! \n");
		return;
	}

	return &currentNode->tokensArray[currentIndex];
}

/*
* If the next token already exists in the storage (this happens when back_token was called before this call to next_token):
*  this function returns the next stored token.
* Else: continues to read the input file in order to identify, create and store a new token (using yylex function);
*  returns the token that was created.
*/
Token* next_token()
{
	if (balance == 0)
	{
		if (restartFlag == 0)
		{
			fclose(yyout);
			yyout = fopen("C:\\temp\\test1_204536676_204863633_204780969_lex.txt", "a");
		}
		else if (restartFlag == 1)
		{
			fclose(yyout);
			yyout = fopen("C:\\temp\\test2_204536676_204863633_204780969_lex.txt", "a");
		}
		else
		{
			printf("\n---- Error ----\n");
			return;
		}

		yylex();

		if (restartFlag == 0)
		{
			fclose(yyout);
			yyout = fopen("C:\\temp\\test1_204536676_204863633_204780969_syntactic.txt", "a");
		}
		else if (restartFlag == 1)
		{
			fclose(yyout);
			yyout = fopen("C:\\temp\\test2_204536676_204863633_204780969_syntactic.txt", "a");
		}
		else
		{
			printf("\n---- Error ----\n");
			return;
		}
	}
	else
	{
		balance++;
		currentIndex++;
		if (currentIndex == TOKEN_ARRAY_SIZE)
		{
			if (currentNode->next == NULL)
			{
				fprintf(yyout, "\nUnable to find token! \n");
				exit(0);
			}
			currentNode = currentNode->next;
			currentIndex = 0;
		}
	}
	return &currentNode->tokensArray[currentIndex];
}

Token* match(eTOKENS expectedKind)
{
	Token* t = next_token();
	if (t->kind != expectedKind)
	{
		errorHandler(expectedKind, t);
	}
	return t;
}

Token* match_rel_op(eTOKENS* expectedKinds)
{
	Token* t = next_token();
	int i;
	for (i = 0; i < 6; i++)
	{
		if (t->kind == expectedKinds[i])
		{
			return t;
		}
	}
	fprintf(yyout, "Expected tokens:\n");
	for (i = 0; i < 6; i++)
	{
		if (i == 5)
		{
			fprintf(yyout, "%s ", eTokensString[expectedKinds[i]]);
		}
		else
		{
			fprintf(yyout, "%s or ", eTokensString[expectedKinds[i]]);
		}
	}
	fprintf(yyout, "at line: %d,\nActual token of type %s, lexeme: %s.\n", t->lineNumber, eTokensString[t->kind], t->lexeme);
	return t;
}

void errorHandler(eTOKENS expectedKind, Token* actual)
{
	fprintf(yyout, "Expected token of type %s at line: %d,\nActual token of type %s, lexeme: %s.\n", eTokensString[expectedKind], actual->lineNumber, eTokensString[actual->kind], actual->lexeme);
}

void errorHandlerAndRecovery(eTOKENS* expectedKinds, int kindsSize, Token* actual, eTOKENS* followTokensToReover, int followTokensSize)
{
	int i;
	bool flag = 0;
	fprintf(yyout, "Expected tokens:\n");
	for (i = 0; i < kindsSize; i++)
	{
		if (i == kindsSize-1)
		{
			fprintf(yyout, "%s ", eTokensString[expectedKinds[i]]);
		}
		else
		{
			fprintf(yyout, "%s or ", eTokensString[expectedKinds[i]]);
		}
	}
	fprintf(yyout, "at line: %d,\nActual token of type %s, lexeme: %s.\n", actual->lineNumber, eTokensString[actual->kind], actual->lexeme);

	// recovery:

	while (actual->kind != EOF_TOKEN)
	{
		actual = next_token();
		for (i = 0; i < followTokensSize; i++)
		{
			if (actual->kind == followTokensToReover[i])
			{
				flag = 1;
				break;
			}
		}
		if (flag == 1)
		{
			break;
		}
	}
	back_token();
}

eTOKENS lookaheadAfterPrototype()
{
	Token* t;
	int count = 0;
	eTOKENS afterPrototype;

	while (true)
	{
		t = next_token();
		count++;
		if (t->kind == EOF_TOKEN)
		{
			while (count != 0)
			{
				back_token();
				count--;
			}
			break;
		}
		if (t->kind == CLOSE_PARENTHESES_TOKEN)
		{
			t = next_token();
			count++;
			afterPrototype = t->kind;
			while (count != 0)
			{
				back_token();
				count--;
			}
			break;
		}
	}
	return afterPrototype;
}

void restart()
{
	currentIndex = 0;
	currentNode = NULL;
	balance = 0;
	restartFlag = 1;
}
