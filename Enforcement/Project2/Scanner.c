#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include "Core.h"

static FILE *codeFile;
static int currentConstValue;
static int scannedToken;
static char *currentIdValue;

// Advances a token in the Alphanumeric case
// tokenSize is a pointer to assist the realloc of storedID
static int advanceAlphaNum(int *tokenSize, char *tokenString)
{
	int notEndOfToken;
	char singleChar;
	int token;

	notEndOfToken = 1;
	while (notEndOfToken && !(feof(codeFile)))
	{
		(*tokenSize)++;
		singleChar = fgetc(codeFile);
		if (!(isalpha(singleChar)) && !(isdigit(singleChar)))
		{
			notEndOfToken = 0;
			ungetc(singleChar, codeFile);
			if (strcmp(tokenString, "program") == 0)
			{
				token = PROGRAM;
			}
			else if (strcmp(tokenString, "begin") == 0)
			{
				token = BEGIN;
			}
			else if (strcmp(tokenString, "end") == 0)
			{
				token = END;
			}
			else if (strcmp(tokenString, "int") == 0)
			{
				token = INT;
			}
			else if (strcmp(tokenString, "endfunc") == 0)
			{
				token = ENDFUNC;
			}
			else if (strcmp(tokenString, "if") == 0)
			{
				token = IF;
			}
			else if (strcmp(tokenString, "else") == 0)
			{
				token = ELSE;
			}
			else if (strcmp(tokenString, "then") == 0)
			{
				token = THEN;
			}
			else if (strcmp(tokenString, "while") == 0)
			{
				token = WHILE;
			}
			else if (strcmp(tokenString, "endwhile") == 0)
			{
				token = ENDWHILE;
			}
			else if (strcmp(tokenString, "endif") == 0)
			{
				token = ENDIF;
			}
			else if (strcmp(tokenString, "or") == 0)
			{
				token = OR;
			}
			else if (strcmp(tokenString, "input") == 0)
			{
				token = INPUT;
			}
			else if (strcmp(tokenString, "output") == 0)
			{
				token = OUTPUT;
			}
			else
			{
				token = ID;
			}
		}
		else
		{
			tokenString = realloc(tokenString, (*tokenSize) + 1);
			tokenString[(*tokenSize) - 1] = singleChar;
		}
	}
	return token;
}

// Handles the tokenization of a special symbol.
static int advanceSpecial(int tokenSize, char specialChar)
{
	char singleChar;
	int token;
	if (specialChar == ';')
	{
		token = SEMICOLON;
	}
	else if (specialChar == '(')
	{
		token = LPAREN;
	}
	else if (specialChar == ')')
	{
		token = RPAREN;
	}
	else if (specialChar == ',')
	{
		token = COMMA;
	}
	else if (specialChar == '!')
	{
		token = NEGATION;
	}
	else if (specialChar == '+')
	{
		token = ADD;
	}
	else if (specialChar == '-')
	{
		token = SUB;
	}
	else if (specialChar == '*')
	{
		token = MULT;
	}
	else if (specialChar == '=')
	{
		singleChar = fgetc(codeFile);
		if (singleChar == '=')
		{
			token = EQUAL;
		}
		else
		{
			ungetc(singleChar, codeFile);
			token = ASSIGN;
		}
	}
	else if (specialChar == '<')
	{
		singleChar = fgetc(codeFile);
		if (singleChar == '=')
		{
			token = LESSEQUAL;
		}
		else
		{
			ungetc(singleChar, codeFile);
			token = LESS;
		}
	}
	else
	{
		printf("ERROR: '%c' is an invalid character.\n", specialChar);
		token = EOS;
	}
	return token;
}

// Tokenizes the constant case.
static int advanceConstant(int tokenSize, char *tokenString)
{
	int notEndOfToken;
	char singleChar;
	int constant;
	int token;

	notEndOfToken = 1;

	while (notEndOfToken && !(feof(codeFile)))
	{
		tokenSize++;
		tokenString = realloc(tokenString, tokenSize + 1);
		singleChar = fgetc(codeFile);
		if (!(isdigit(singleChar)))
		{
			if (tokenSize > 2 && tokenString[0] == '0')
			{
				printf("ERROR: A constant cannot begin with 0.\n");
				token = EOS;
			}
			else
			{
				constant = atoi(tokenString);
				if ((constant > 1023) || (constant < 0))
				{
					printf("ERROR: Const %i is out of range (0-1023).\n", constant);
					token = EOS;
				}
				else
				{
					token = CONST;
					currentConstValue = constant;
				}
			}
			notEndOfToken = 0;
			ungetc(singleChar, codeFile);
		}
		else
		{
			tokenString[tokenSize - 1] = singleChar;
		}
	}

	return token;
}

// Initialize the scanner
void Scanner(char *filename)
{
	codeFile = fopen(filename, "r");
	currentIdValue = (char *)malloc(1);

	if (codeFile != NULL)
	{
		// Openning was successful, can proceed with scanning
	}
	else
	{
		// Print error message, return EOS token
		printf("ERROR: Unable to open file: %s.\n", strerror(errno));
		scannedToken = EOS;
	}
}

// Advance to the next token
void nextToken()
{
	// TokenSize is used to realloc the string on the tokens as needed and build the string.
	// singleChar is the character used to move through the token.
	// token acts as a holder for the token that is found until it is assigned to the current token.
	// tokenString acts as the holder for the string that will be given to the current ID, it is allocated
	// to 1 byte to allow for the null byte.
	int tokenSize;
	char singleChar;
	int token;
	char *tokenString;

	tokenString = (char *)malloc(1);
	singleChar = ' ';
	tokenSize = 0;
	strcpy(tokenString, "");

	// Remove any leading whitespaces
	while (isspace(singleChar) && !(feof(codeFile)))
	{
		singleChar = fgetc(codeFile);
		if (!(isspace(singleChar)))
		{
			ungetc(singleChar, codeFile);
		}
	}

	// Grabs a character and checks if it is a digit, alphabetic symbol, * or a special symbol
	// Each situation splits off into its own function for special handling
	if (!(feof(codeFile)))
	{
		tokenSize++;

		// tokenSize+1 is accounting for the space of the \0
		tokenString = realloc(tokenString, tokenSize + 1);
		singleChar = fgetc(codeFile);
		tokenString[tokenSize - 1] = singleChar;

		if (isdigit(singleChar))
		{
			// Must be start of a constant
			token = advanceConstant(tokenSize, tokenString);
		}
		else if (isalpha(singleChar))
		{
			// Must be start of ID or keyword
			token = advanceAlphaNum(&tokenSize, tokenString);
		}
		else
		{
			// Handle special character sequences
			token = advanceSpecial(tokenSize, singleChar);
		}

		scannedToken = token;
	}
	else
	{
		scannedToken = EOS;
	}

	//This takes tokenString and puts it into stored ID.
	if (token == ID)
	{
		currentIdValue = (char *)realloc(currentIdValue, tokenSize + 1);
		strcpy(currentIdValue, tokenString);
	}
	free(tokenString);
}

// Return the current token
int currentToken()
{
	return scannedToken;
}

// Copy the ID string value to formal parameter value
void getID(char *value)
{
	if (currentToken() == ID)
	{
		strcpy(value, currentIdValue);
	}
	else
	{
		strcpy(value, "");
	}
}

// Return the CONST numerical value
int getCONST()
{
	return currentConstValue;
}
