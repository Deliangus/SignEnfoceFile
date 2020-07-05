#include "Scanner.c"

// CHANGE NOTHING IN THIS FILE

// Initializes the scanner
void Scanner(char* filename);

// Finds the next token
void nextToken();

// Returns the current token
int currentToken();

// If the current token is ID, returns the string value of the identifier
void getID(char* value);

// If the current token is CONST, returns the value of the constant
int getCONST();
