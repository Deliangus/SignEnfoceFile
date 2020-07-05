enum Core {
	// Keywords for program definition
	PROGRAM,
	BEGIN,
	END,
	
	// Keywords for declaring variables/functions/classes
	INT,
	DEFINE,
	ENDFUNC,
	CLASS,
	EXTENDS,
	ENDCLASS,
	
	// Keywords for while loop, if statements
	IF,
	THEN,
	ELSE,
	WHILE,
	ENDWHILE,
	ENDIF,
	
	// Operators and special symbols
	SEMICOLON,
	LPAREN,
	RPAREN,
	COMMA,
	ASSIGN,
	NEGATION,
	OR,
	EQUAL,
	LESS,
	LESSEQUAL,
	ADD,
	SUB,
	MULT,
	
	// Keywords for input/output statements
	INPUT,
	OUTPUT,
	
	// Tokens that pass with extra information
	CONST,
	ID,

	// Special token to indicate end of stream
	EOS
};
