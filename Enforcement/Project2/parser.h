#include "Scanner.h"

/* 
 * Structure declarations here, definitions below
 */

int testing = 0;

typedef struct
{
	char *symbol;
	int token;
} dict;

const dict dictionary[] = {
	{"program", PROGRAM},
	{"begin", BEGIN},
	{"end", END},
	{"int", INT},
	{"define", DEFINE},
	{"endfunc", ENDFUNC},
	{"class", CLASS},
	{"extends", EXTENDS},
	{"endclass", ENDCLASS},
	{"if", IF},
	{"then", THEN},
	{"else", ELSE},
	{"while", WHILE},
	{"endwhile", ENDWHILE},
	{"endif", ENDIF},
	{";", SEMICOLON},
	{"(", LPAREN},
	{")", RPAREN},
	{",", COMMA},
	{"=", ASSIGN},
	{"!", NEGATION},
	{"or", OR},
	{"==", EQUAL},
	{"<", LESS},
	{"<=", LESSEQUAL},
	{"+", ADD},
	{"-", SUB},
	{"*", MULT},
	{"input", INPUT},
	{"output", OUTPUT},
	{"const", CONST},
	{"id", ID},
	//{"EOS", EOS}
};

typedef struct program_node program_node;
typedef struct decl_seq_node decl_seq_node;
typedef struct stmt_seq_node stmt_seq_node;

typedef struct expression expression;
typedef struct factor factor;
typedef struct term term;
typedef struct cmpr cmpr;
typedef struct condition condition;
typedef struct statement statement;

typedef struct factor
{
	int con_val;
	char *id;
	expression *expr;
} factor;

typedef struct term
{
	factor *fct;
	term *_term;
} term;
typedef struct expression
{
	term *_term;

	int operator;

	expression *expr;

} expression;

typedef struct cmpr
{
	expression *a;
	expression *b;
	int operator;
} cmpr;

typedef struct condition
{
	int negative;
	cmpr *a;
	condition *cond;
} condition;
typedef struct statement
{
	int type;

	/*Assign statement*/
	char *assgn_id;
	expression *assign_expr;

	/*If statement*/
	condition *if_cond;
	struct statement *if_stm_seq;
	struct statement *if_else_stm_seq;

	/*Loop statement*/
	condition *while_cond;
	struct statement *while_stm_seq;

	/*Input statement*/
	char *input_id;

	/*Output statement*/
	expression *output_expr;

	/*Statement sequence*/
	//statement **stm_seq;

	struct statement *next;
} statement;

enum Statement_type
{
	STATEMENT_SEQ
};

typedef struct LinkItem
{
	char *name;
	int value;
	struct LinkItem *next;
} LinkItem;

typedef struct dec_statement
{
	LinkItem *variable_head;
	LinkItem *variable_tail;

	struct dec_statement *next;
} dec_statement;
/* 
 * struct definitions below
 * Each struct represents a type of node in the parse tree
 * Each nonterminals of the grammar should have a type of node
 */

// program_node represents the root of the parse tree
struct program_node
{
	decl_seq_node *dsn; // Link to the decl_seq child or root
	stmt_seq_node *ssn; // Link to the stmt_seq child of root
	LinkItem *data;
};

// decl_seq_node represents decl_seq nodes in the parse tree
struct decl_seq_node
{
	/* You will fill in here what you need
	 * I put the int here to supress compiler errors, replace it
	 */
	int numOfVariables;
	LinkItem **variables;
	dec_statement *dec_head;
	dec_statement *dec_tail;
};

// stmt_seq_node represents stmt_seq nodes in the parse tree
struct stmt_seq_node
{
	/* You will fill in here what you need
	 * I put the int here to supress compiler errors, replace it
	 */
	int numOfVariables;
	LinkItem **variables;
	statement *root;
};

/*
 * Function declarations here, definitions below
 */

void parser_program(program_node *prog, char *codeFileName);
void printer_program(program_node *prog);

void delete_program(program_node *prog);

void parser_decl_seq(decl_seq_node *dsn);
void printer_decl_seq(decl_seq_node *prog, int tabbing);

void executor_program(program_node *prog, char *dataFileName);
void executor_decl_seq(decl_seq_node *dsn);
void executor_stmt_seq(statement *ssn, LinkItem **variables, int length);

void printer_variable_list(LinkItem **list, int length);

statement *parser_input_seq();
statement *parser_while_seq(stmt_seq_node *ssn);
statement *parser_output_seq(stmt_seq_node *ssn);
statement *parser_stmt_seq(stmt_seq_node *ssn);
statement *parser_assign_seq(LinkItem **variables, int length);
statement *parser_if_seq(stmt_seq_node *ssn);
expression *parser_expression(LinkItem **variables, int length);
cmpr *parser_cmpr(LinkItem **variables, int length);
condition *parser_condition(LinkItem **variables, int length);
factor *parser_factor(LinkItem **variables, int length);
term *parser_term(LinkItem **variables, int length);
expression *parser_expression(LinkItem **variables, int length);
statement *parser_assign_seq(LinkItem **variables, int length);

void printer_cmpr(cmpr *_cmpr);
void printer_indent(int tabbing);
void printer_stmt_seq(statement *root, int tabbing);
void printer_condition(condition *cond);
void printer_factor(factor *fct);
void printer_term(term *_term);
void printer_expression(expression *expr, int indent);

int executor_cmpr(cmpr *_cmpr, LinkItem **variables, int length);
int executor_condition(condition *cond, LinkItem **variables, int length);
int executor_factor(factor *fct, LinkItem **variables, int length);
int executor_term(term *_term, LinkItem **variables, int length);
int executor_expression(expression *expr, LinkItem **variables, int length);

int negation(int condition);
/* 
 * Function definitions below
 * Each struct should have 4 functions associated with it:
 * parser_* - this handles the recursive descent parsing, and builds the parse tree
 * printer_* - this nicely prints the program from the parse tree, to verify the tree
 * executor_* - this walks over the tree and executes the statements of the program
 * delete_* - this recursively dismantles the tree, releasing allocated memory
 */

/* program functions */

void parser_program(program_node *prog, char *codeFileName)
{
	if (testing)
	{
		printf("Parsing Program.\n");
	}
	// Initialize the scanner

	Scanner(codeFileName);

	if (testing)
	{
		printf("Finished scanning.\n");
	}

	nextToken();
	if (currentToken() != PROGRAM)
	{
		printf("ERROR: Expected PROGRAM token\n");
		exit(5);
	}
	else
	{
		//printf("PROGRAM\n");
	}
	nextToken();

	(*prog).dsn = (decl_seq_node *)malloc(sizeof(decl_seq_node));
	decl_seq_node *dsn = prog->dsn;
	dsn->numOfVariables = 0;
	dsn->dec_head = NULL;
	dsn->dec_tail = NULL;

	//printf("Initialized dsn\n");

	parser_decl_seq(dsn);
	if (testing)
	{
		printf("parsed dsn\n");
	}

	// After parsing the declaration sequence, should have BEGIN token
	if (currentToken() != BEGIN)
	{
		printf("ERROR: Expected BEGIN token: while get %s.\n", dictionary[currentToken()].symbol);
		exit(0);
	}
	else
	{
		//printf("BEGIN\n");
	}

	nextToken(); // Consume the BEGIN token

	// Scanner should be at the start of the statement sequence
	(*prog).ssn = (stmt_seq_node *)malloc(sizeof(stmt_seq_node));
	stmt_seq_node *ssn = prog->ssn;
	if (testing)
	{
		printf("Initialized ssn.\n");
	}

	ssn->root = parser_stmt_seq(ssn);
	if (testing)
	{
		//printf("Parsed statements\n");
	}
	// After parsing the declaration sequence, should have END token, then EOS
	if (currentToken() != END)
	{
		printf("ERROR: Expected END token\n");
		exit(0);
	}
	nextToken(); // Consume the END token
	if (currentToken() != EOS)
	{
		printf("ERROR: Unexpected token after END, found %s.\n", dictionary[currentToken()].symbol);
		exit(0);
	}
	if (testing)
	{
		printf("Parsed program.\n");
	}

	char *datafile = (char *)malloc(sizeof(char) * (strlen(codeFileName) + 1));

	strcpy(datafile, codeFileName);
	//printf("%s\n", datafile);parser_prog
	datafile[strlen(datafile) - 1] = 'a';
	datafile[strlen(datafile) - 2] = 't';
	datafile[strlen(datafile) - 3] = 'a';
	datafile[strlen(datafile) - 4] = 'd';

	//printf("%s\n", datafile);

	Scanner(datafile);
	while (currentToken() == EOS)
	{
		nextToken();
	}
}

void printer_program(program_node *prog)
{
	printf("program\n");
	printer_decl_seq((*prog).dsn, 1);
	printf("begin\n");
	printer_stmt_seq((*prog).ssn->root, 1);
	printf("end\n");
}

int findInLinkItem(dec_statement *head, char *target)
{
	dec_statement *current = head;
	int result = 0;
	//printf("findInLinkItem:\n");
	while (current != NULL)
	{
		LinkItem *current_val = current->variable_head;
		while (current_val != NULL)
		{
			if (strcmp(current_val->name, target) == 0)
			{
				result++;
			}
			current_val = current_val->next;
		}
		current = current->next;
	}
	return result;
}

LinkItem *findVariableInLinkItemList(LinkItem **list, int length, char *target)
{
	if (list == NULL)
	{
		printf("Error finding variable %s, list is NULL of length %d.\n", target, length);
		exit(1);
	}
	LinkItem *result = NULL;
	for (int i = 0; i < length; i++)
	{
		if (strcmp(target, list[i]->name) == 0)
		{
			if (result == NULL)
			{
				result = list[i];
			}
			else
			{
				printf("Repeat variable found in variable list, %s\n", list[i]->name);
			}
		}
	}
	return result;
}

void delete_program(program_node *prog)
{
	/* You will probably need to add here delete funtions for the children dsn and ssn, 
	 * which call the delete functions for their children, ect
	 */
	free((*prog).dsn);
	free((*prog).ssn);
}

/* decl_seq functions */

void parser_decl_seq(decl_seq_node *dsn)
{
	//printf("parsing declaration\n");
	if (currentToken() == INT)
	{
		//printf("int ");
		if (dsn->dec_head == NULL)
		{
			dsn->dec_head = (dec_statement *)malloc(sizeof(dec_statement));
			dsn->dec_tail = dsn->dec_head;
		}
		else
		{
			dsn->dec_tail->next = (dec_statement *)malloc(sizeof(dec_statement));
			dsn->dec_tail = dsn->dec_tail->next;
		}

		dec_statement *current_statement = dsn->dec_tail;
		current_statement->next = NULL;
		current_statement->variable_head = NULL;
		current_statement->variable_tail = NULL;

		nextToken();

		while (currentToken() == ID)
		{
			LinkItem *item = (LinkItem *)malloc(sizeof(LinkItem));
			item->next = NULL;
			item->name = (char *)malloc(255 * sizeof(char));
			getID(item->name);
			//printf("%s", item->name);

			if (findInLinkItem(dsn->dec_head, item->name) > 0)
			{
				printf("Error: Repeating declaration of %s.\n", item->name);
				exit(1);
			}

			if (current_statement->variable_head == NULL)
			{
				current_statement->variable_head = item;
				current_statement->variable_tail = item;
			}
			else
			{
				current_statement->variable_tail->next = item;
				current_statement->variable_tail = item;
			}

			dsn->numOfVariables++;

			nextToken();

			switch (currentToken())
			{
			case COMMA:
				//printf(",");
				nextToken();
				break;
			case SEMICOLON:
				//printf(";\n");
				nextToken();
				//printf("???????????\n");
				break;
			default:
				printf("Error Found when parsing declaration sequence, unexpected token: %d\n.", currentToken());
				exit(0);
			}

			//printf("parsing internal finished\n");
		}

		//printf("parsing decleration finished\n");
		parser_decl_seq(dsn);
	}
}

void printer_decl_seq(decl_seq_node *dsn, int tabbing)
{
	// For readability, do some tabbing

	dec_statement *current = dsn->dec_head;
	while (current != NULL)
	{
		for (int i = 0; i < tabbing; i++)
		{
			printf("\t");
		}

		printf("int ");

		LinkItem *current_val = current->variable_head;
		while (current_val != NULL)
		{
			printf("%s", current_val->name);
			if (current_val->next == NULL)
			{
				printf(";\n");
			}
			else
			{
				printf(",");
			}
			current_val = current_val->next;
		}

		current = current->next;
	}
}

statement *parser_while_seq(stmt_seq_node *ssn)
{
	statement *result = (statement *)malloc(sizeof(statement));
	if (currentToken() == WHILE)
	{
		result->type = WHILE;
		//printf("while ");
		nextToken();
		result->while_cond = parser_condition(ssn->variables, ssn->numOfVariables);
		if (currentToken() == BEGIN)
		{
			//printf(" begin ");
			nextToken();
			result->while_stm_seq = parser_stmt_seq(ssn);
			if (currentToken() == ENDWHILE)
			{
				//printf("endwhile");
				nextToken();
				if (currentToken() == SEMICOLON)
				{
					//printf(";\n");
					nextToken();
				}
				else
				{
					printf("Error while parsing while, expected ;, found %s.\n", dictionary[currentToken()].symbol);
					exit(1);
				}
			}
			else
			{
				printf("Error when parsing while, expected ENDWHILE, found %s.\n", dictionary[currentToken()].symbol);
			}
		}
		else
		{
			printf("Error when parsing while, expected BEGIN, found %s.\n", dictionary[currentToken()].symbol);
		}

		//nextToken();
	}
	else
	{
		printf("Error when parsing While, expected while found %s.\n", dictionary[currentToken()].symbol);
	}
	return result;
}

statement *parser_input_seq()
{
	statement *result = (statement *)malloc(sizeof(statement));

	if (currentToken() == INPUT)
	{
		result->type = INPUT;

		nextToken();

		if (currentToken() == ID)
		{
			result->input_id = (char *)malloc(255 * sizeof(char));
			getID(result->input_id);

			nextToken();
			if (currentToken() == SEMICOLON)
			{
				nextToken();
			}
		}
	}
	else
	{
		printf("Error parsing input statement, expected input, found %s\n", dictionary[currentToken()].symbol);
		exit(1);
	}
	return result;
}

statement *parser_output_seq(stmt_seq_node *ssn)
{
	statement *result = (statement *)malloc(sizeof(statement));
	if (currentToken() == OUTPUT)
	{
		result->type = OUTPUT;
		//printf("output ");
		nextToken();
		result->output_expr = parser_expression(ssn->variables, ssn->numOfVariables);
		if (currentToken() == SEMICOLON)
		{
			//printf(";\n");
			nextToken();
		}
		else
		{
			printf("Error when parsing output statements, expected ;, found %s.\n", dictionary[currentToken()].symbol);
			exit(1);
		}
	}
	else
	{
		printf("Error when parsing output statements, expected output, found %s.\n", dictionary[currentToken()].symbol);
		exit(1);
	}
	return result;
}
/* stmt_seq functions */

statement *parser_stmt_seq(stmt_seq_node *ssn)
{
	statement *current = NULL;
	if (testing)
	{
		//printf("parsing statements %d.\n", currentToken());
	}
	switch (currentToken())
	{
	case ID:
		//printf("parsing as")
		current = parser_assign_seq(ssn->variables, ssn->numOfVariables);
		break;
	case IF:
		current = parser_if_seq(ssn);
		break;
	case WHILE:
		current = parser_while_seq(ssn);
		break;
	case INPUT:
		current = parser_input_seq();
		break;
	case OUTPUT:
		current = parser_output_seq(ssn);
		//printf("???????\n");
		break;
	case ENDWHILE:
		break;
	case ENDIF:
		break;
	case ELSE:
		break;
	default:
		printf("Error when parsing statements, found %s.", dictionary[currentToken()].symbol);
		exit(1);
	}

	if ((currentToken() != END && currentToken() != ENDIF && currentToken() != ENDWHILE && currentToken() != ELSE))
	{
		current->next = parser_stmt_seq(ssn);
	}
	else
	{
		current->next = NULL;
	}

	return current;
	// The grammar will guide you on what needs to go here
}

int negation(int condition)
{
	if (condition)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
void printer_cmpr(cmpr *_cmpr)
{
	if (_cmpr->a == NULL || _cmpr->b == NULL)
	{
		printf("Error printer cmpr, either a and b are NULL.\n");
		exit(1);
	}
	else
	{
		printer_expression(_cmpr->a, 0);
		if (_cmpr->operator== EQUAL)
		{
			printf("==");
		}
		else if (_cmpr->operator== LESS)
		{
			printf("<");
		}
		else if (_cmpr->operator== LESSEQUAL)
		{
			printf("<=");
		}
		else
		{
			printf("Error printing comparason, operator is %d.\n", currentToken());
			exit(1);
		}

		printer_expression(_cmpr->b, 0);
	}
}
cmpr *parser_cmpr(LinkItem **variables, int length)
{
	if (testing)
	{
		//printf("Parsing cmpr.\n");
	}
	cmpr *result = (cmpr *)malloc(sizeof(cmpr));

	result->a = parser_expression(variables, length);

	if (currentToken() == EQUAL)
	{
		if (testing)
		{
			printf(" == ");
		}
		result->operator= EQUAL;

		nextToken();
		result->b = parser_expression(variables, length);
	}
	else if (currentToken() == LESS)
	{
		if (testing)
		{
			printf(" < ");
		}
		result->operator= LESS;

		nextToken();
		result->b = parser_expression(variables, length);
	}
	else if (currentToken() == LESSEQUAL)
	{
		if (testing)
		{
			printf(" <= ");
		}
		result->operator= LESSEQUAL;
		nextToken();
		result->b = parser_expression(variables, length);
	}
	else
	{
		printf("Error parsing comparason, found %s.\n", dictionary[currentToken()].symbol);
	}

	return result;
}

void printer_condition(condition *cond)
{
	if (cond->negative)
	{
		printf("!(");
		if (cond->cond != NULL)
		{
			printer_condition(cond->cond);
		}
		else
		{
			printf("Error printing condition, !(null).\n");
		}
		printf(")");
	}
	else
	{
		printer_cmpr(cond->a);
		if (cond->cond != NULL)
		{
			printf("or");

			printer_condition(cond->cond);
		}
	}
}
condition *parser_condition(LinkItem **variables, int length)
{
	if (testing)
	{
		//printf("Parsing condition statement.\n");
	}
	condition *result = (condition *)malloc(sizeof(condition));
	//printf("???????");
	if (currentToken() == NEGATION)
	{
		if (testing)
		{
			printf("!");
		}
		nextToken();
		if (currentToken() == LPAREN)
		{
			if (testing)
			{
				printf("(");
			}
			nextToken();
			result->negative = 1;
			result->cond = parser_condition(variables, length);
			//condition = negation(subcond);

			if (currentToken() == RPAREN)
			{
				if (testing)
				{
					printf(")");
				}
				nextToken();
			}
			else
			{
				printf("Error found parsing condition, expected ),found %s.\n", dictionary[currentToken()].symbol);
				exit(1);
			}
		}
		else
		{
			printf("Error found parsing condition, expected (\n");
		}
	}
	else
	{
		result->negative = 0;
		result->a = parser_cmpr(variables, length);
		if (currentToken() == OR)
		{
			if (testing)
			{
				printf(" or ");
			}
			nextToken();
			result->cond = parser_condition(variables, length);
		}
	}
	if (testing)
	{
		//printf("Parsed condition.\n");
	}

	return result;
}

statement *parser_if_seq(stmt_seq_node *ssn)
{
	if (testing)
	{
		//printf("Parsing if statement.\n");
	}
	statement *result = (statement *)malloc(sizeof(statement));

	result->type = IF;
	if (currentToken() == IF)
	{
		//printf("????????");
		if (testing)
		{
			printf("if ");
		}
		nextToken();

		result->if_cond = parser_condition(ssn->variables, ssn->numOfVariables);
		if (currentToken() == THEN)
		{
			if (testing)
			{
				printf(" then ");
			}
			nextToken();
			if (testing)
			{
				//printf("%d\n", currentToken());
			}
			result->if_stm_seq = parser_stmt_seq(ssn);
			//printer_stmt_seq(result->if_stm_seq, 10);

			if (result->if_stm_seq == NULL)
			{
				printf("Error found parsing if, !!!!!!!!\n");
				exit(1);
			}

			if (currentToken() == ELSE)
			{
				if (testing)
				{
					printf(" else ");
				}
				nextToken();

				result->if_else_stm_seq = parser_stmt_seq(ssn);
			}
			else
			{
				result->if_else_stm_seq = NULL;
			}

			if (currentToken() == ENDIF)
			{
				if (testing)
				{
					printf("endif");
				}
				nextToken();
				if (currentToken() == SEMICOLON)
				{
					nextToken();
					if (testing)
					{
						printf(";\n");
					}
				}
			}
		}
	}
	else
	{
		printf("--------");
	}

	//printer_stmt_seq(result, 10);
	return result;
}

void printer_factor(factor *fct)
{
	if (fct->id != NULL)
	{
		if (fct->expr == NULL)
		{
			printf("%s", fct->id);
		}
		else
		{
			printf("Error printing factor, neither of id and expr is not null\n");
		}
	}
	else if (fct->expr != NULL)
	{
		if (fct->id == NULL)
		{
			printf("(");
			printer_expression(fct->expr, 0);
			printf(")");
		}
		else
		{
			printf("Error printing factor, neither of id and expr is not null\n");
		}
	}
	else
	{
		printf("%d", fct->con_val);
	}
}

factor *parser_factor(LinkItem **variables, int length)
{
	factor *result = (factor *)malloc(sizeof(factor));
	if (currentToken() == ID)
	{
		char *id = (char *)malloc(255 * sizeof(char));
		getID(id);
		if (testing)
		{
			printf("%s", id);
		}
		result->id = id;
		result->expr = NULL;

		//LinkItem *variable = findVariableInLinkItemList(variables, length, id);
		nextToken();
	}
	else if (currentToken() == CONST)
	{

		result->con_val = getCONST();
		result->id = NULL;
		result->expr = NULL;
		if (testing)
		{
			printf("%d", result->con_val);
		}
		nextToken();
	}
	else if (currentToken() == LPAREN)
	{
		if (testing)
		{
			printf("(");
		}
		nextToken();
		result->expr = parser_expression(variables, length);
		result->id = NULL;
		if (currentToken() == RPAREN)
		{
			if (testing)
			{
				printf(")");
			}
			nextToken();
		}
		else
		{
			printf("Error found parsing factor, Expected ).\n");
			exit(1);
		}
	}
	else
	{
		printf("Error found parsing factor. found %s.\n", dictionary[currentToken()].symbol);
		;
		exit(1);
	}

	return result;
}

void printer_term(term *_term)
{

	if (_term->fct == NULL)
	{
		printf("Error printing term, fct is null.\n");
		exit(1);
	}

	printer_factor(_term->fct);

	if (_term->_term != NULL)
	{
		printf("*");
		printer_term(_term->_term);
	}
}
term *parser_term(LinkItem **variables, int length)
{
	term *result = (term *)malloc(sizeof(term));

	result->fct = parser_factor(variables, length);
	if (currentToken() == MULT)
	{
		if (testing)
		{
			printf(" * ");
		}
		nextToken();
		result->_term = parser_term(variables, length);
	}
	else
	{
		result->_term = NULL;
	}

	return result;
}

void printer_expression(expression *expr, int indent)
{
	printer_indent(indent);

	if (expr->_term == NULL)
	{
		printf("Error printing expression, _term is null.\n");
		exit(1);
	}

	printer_term(expr->_term);

	if (expr->expr != NULL)
	{
		if (expr->operator== ADD)
		{
			printf("+");
		}
		else if (expr->operator== SUB)
		{
			printf("-");
		}
		else
		{
			printf("Error printing expression, operator unexpected: %d.\n", expr->operator);
		}

		printer_expression(expr->expr, 0);
	}
}
expression *parser_expression(LinkItem **variables, int length)
{
	if (testing)
	{
		//printf("Parsing expression.\n");
	}
	expression *result = (expression *)malloc(sizeof(expression));

	result->_term = parser_term(variables, length);

	if (currentToken() == ADD)
	{
		result->operator= ADD;
		if (testing)
		{
			printf("+");
		}
		nextToken();
		result->expr = parser_expression(variables, length);
	}
	else if (currentToken() == SUB)
	{
		result->operator= SUB;
		if (testing)
		{
			printf("-");
		}
		nextToken();
		result->expr = parser_expression(variables, length);
	}
	else if (currentToken() == SEMICOLON)
	{
		result->expr = NULL;
	}
	else
	{
		//printf("Error found parser expression, Expected +,-,;, found %d.\n", currentToken());
	}
	if (testing)
	{
		//printf("Parsed expression.\n");
	}

	return result;
}

statement *parser_assign_seq(LinkItem **variables, int length)
{

	statement *asn_stm = (statement *)malloc(sizeof(statement));
	asn_stm->type = ASSIGN;
	if (testing)
	{
		//printf("parsing assign statements\n");
	}

	if (currentToken() == ID)
	{
		if (testing)
		{
			printf("int ");
		}
		char *id = (char *)malloc(255 * sizeof(char));
		getID(id);
		asn_stm->assgn_id = id;
		if (testing)
		{
			printf("%s", id);
		}
		nextToken();
		//LinkItem *variable = findVariableInLinkItemList(variables, length, id);
		if (currentToken() == ASSIGN)
		{
			if (testing)
			{
				printf("=");
			}
			nextToken();
			asn_stm->assign_expr = parser_expression(variables, length);
			if (currentToken() == SEMICOLON)
			{
				if (testing)
				{
					printf(";\n");
				}
				nextToken();
			}
		}
		else
		{
			printf("Error parsing assigning statements, Expected ASSIGN.\n");
			exit(1);
		}
	}
	else
	{
		printf("Error parsing assigning statements, Expected ID.\n");
		exit(1);
	}

	return asn_stm;
	//printf("parsed assign statements\n");
}

void printer_stmt_seq(statement *root, int tabbing)
{
	if (root == NULL)
	{
		printf("Error printing statement. root is null\n");
		exit(1);
	}
	// For readability, do some tabbing

	printer_indent(tabbing);
	switch (root->type)
	{
	case -1:
		break;
	case ASSIGN:
		printf("%s=", root->assgn_id);
		printer_expression(root->assign_expr, 0);
		printf(";\n");
		break;
	case IF:
		printf("if ");
		printer_condition(root->if_cond);
		printf(" then\n");
		if (root->if_stm_seq == NULL)
		{
			printf("???????????\n");
			exit(1);
		}
		printer_stmt_seq(root->if_stm_seq, tabbing + 1);
		if (root->if_else_stm_seq != NULL)
		{
			printer_indent(tabbing);
			printf("else\n");
			printer_stmt_seq(root->if_else_stm_seq, tabbing + 1);
			//printf(";\n");
		}
		printer_indent(tabbing);
		printf("endif;\n");
		break;
	case OUTPUT:
		printf("output ");
		printer_expression(root->output_expr, 0);
		printf(";\n");
		break;
	case INPUT:
		printf("input %s;\n", root->input_id);
		break;
	case WHILE:
		printf("while ");
		printer_condition(root->while_cond);
		printf(" begin\n");
		printer_stmt_seq(root->while_stm_seq, tabbing + 1);
		printer_indent(tabbing);
		printf("endwhile;\n");

		break;
	}
	if (root->next != NULL)
	{
		printer_stmt_seq(root->next, tabbing);
	}
	//printf("You will add code to print the stmt_seq\n");
}

void printer_indent(int tabbing)
{
	for (int i = 0; i < tabbing; i++)
	{
		printf("\t");
	}
}

void executor_program(program_node *prog, char *dataFileName)
{
	decl_seq_node *dsn = prog->dsn;
	stmt_seq_node *ssn = prog->ssn;

	executor_decl_seq(dsn);
	ssn->variables = dsn->variables;
	ssn->numOfVariables = dsn->numOfVariables;
	executor_stmt_seq(ssn->root, ssn->variables, ssn->numOfVariables);
}

void executor_decl_seq(decl_seq_node *dsn)
{
	//printf("Executing decleration.\n");
	if (dsn->numOfVariables > 0)
	{
		dsn->variables = (LinkItem **)malloc(dsn->numOfVariables * sizeof(LinkItem *));
	}

	dec_statement *current = dsn->dec_head;

	int i = 0;

	while (current != NULL)
	{
		LinkItem *current_val = current->variable_head;
		while (current_val != NULL)
		{
			dsn->variables[i++] = current_val;
			current_val = current_val->next;
		}
		current = current->next;
	}
	//printf("Executed decleration.\n");
	//printer_variable_list(dsn->variables, dsn->numOfVariables);
	// You will add code to set up space for the variables being declared
}

void executor_stmt_seq(statement *root, LinkItem **variables, int length)
{
	//printf("Executing statements.\n");
	///*
	// You will add code to execute the program
	switch (root->type)
	{
	case -1:
		break;
	case ASSIGN:
		findVariableInLinkItemList(variables, length, root->assgn_id)->value = executor_expression(root->assign_expr, variables, length);
		break;
	case IF:
		if (executor_condition(root->if_cond, variables, length))
		{
			executor_stmt_seq(root->if_stm_seq, variables, length);
		}
		else if (root->if_else_stm_seq != NULL)
		{
			executor_stmt_seq(root->if_else_stm_seq, variables, length);
			//printf(";\n");
		}
		else
		{
		}

		break;
	case OUTPUT:
		printf("%d\n", executor_expression(root->output_expr, variables, length));
		break;
	case INPUT:

		//printf("%d ", currentToken());
		findVariableInLinkItemList(variables, length, root->input_id)->value = getCONST();
		nextToken();
		//printf("%d -> %d\n", findVariableInLinkItemList(variables, length, root->input_id)->value, getCONST());
		break;
	case WHILE:
		while (executor_condition(root->while_cond, variables, length))
		{
			executor_stmt_seq(root->while_stm_seq, variables, length);
		}

		break;
	}
	if (root->next != NULL)
	{
		executor_stmt_seq(root->next, variables, length);
	}
	//*/
}

void printer_variable_list(LinkItem **list, int length)
{
	for (int i = 0; i < length; i++)
	{
		printf("%s -> %d\n", list[i]->name, list[i]->value);
	}
}

int executor_expression(expression *expr, LinkItem **variables, int length)
{
	int result = executor_term(expr->_term, variables, length);
	if (expr->expr != NULL)
	{
		if (expr->operator== ADD)
		{
			result += executor_expression(expr->expr, variables, length);
		}
		else if (expr->operator== SUB)
		{
			result -= executor_expression(expr->expr, variables, length);
		}
		else
		{
			printf("Error executing expression, operator unexpected: %d.\n", expr->operator);
		}
	}
	return result;
}

int executor_term(term *_term, LinkItem **variables, int length)
{
	if (_term->fct == NULL)
	{
		printf("Error printing term, fct is null.\n");
		exit(1);
	}

	int result = executor_factor(_term->fct, variables, length);

	if (_term->_term != NULL)
	{
		result *= executor_term(_term->_term, variables, length);
	}

	return result;
}

int executor_factor(factor *fct, LinkItem **variables, int length)
{
	int result = 0;
	if (fct->id != NULL)
	{
		if (fct->expr == NULL)
		{
			result = findVariableInLinkItemList(variables, length, fct->id)->value;
		}
		else
		{
			printf("Error printing factor, neither of id and expr is not null\n");
			exit(1);
		}
	}
	else if (fct->expr != NULL)
	{
		if (fct->id == NULL)
		{
			result = executor_expression(fct->expr, variables, length);
		}
		else
		{
			printf("Error printing factor, neither of id and expr is not null\n");
			exit(1);
		}
	}
	else
	{
		result = fct->con_val;
	}

	return result;
}

int executor_condition(condition *cond, LinkItem **variables, int length)
{
	int result = 0;
	if (cond->negative)
	{
		if (cond->cond != NULL)
		{
			result = negation(executor_condition(cond->cond, variables, length));
		}
		else
		{
			printf("Error executing condition, !(null).\n");
			exit(1);
		}
	}
	else
	{
		result = executor_cmpr(cond->a, variables, length);
		if (cond->cond != NULL)
		{
			if (result)
			{
			}
			else
			{
				result = executor_condition(cond->cond, variables, length);
			}
		}
	}
	return result;
}

int executor_cmpr(cmpr *_cmpr, LinkItem **variables, int length)
{
	int result = 0;
	if (_cmpr->a == NULL || _cmpr->b == NULL)
	{
		printf("Error executing cmpr, either a and b are NULL.\n");
		exit(1);
	}
	else
	{
		int a = executor_expression(_cmpr->a, variables, length);
		int b = executor_expression(_cmpr->b, variables, length);

		if (_cmpr->operator== EQUAL)
		{
			if (a == b)
			{
				result = 1;
			}
		}
		else if (_cmpr->operator== LESS)
		{
			if (a < b)
			{
				result = 1;
			}
		}
		else if (_cmpr->operator== LESSEQUAL)
		{
			if (a <= b)
			{
				result = 1;
			}
		}
		else
		{
			printf("Error executing comparason, operator is %d.\n", currentToken());
			exit(1);
		}
	}
	return result;
}