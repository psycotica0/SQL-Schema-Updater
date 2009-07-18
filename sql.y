%{
#include <stdio.h>
#include <string.h>
#define YYSTYPE char*

void yyerror(const char* message) {
	fprintf(stderr, "Parse Error: %s\n", message);
}

int yywrap() {
	return 1;
}

int main() {
	yyparse();
}

%}

%token CREATE TABLE LPAREN RPAREN SEMI COMMA DOUBLE_QUOTE WORD NUMBER STRING INSERT INTO VALUES SQL_NULL

%%

statements:
	/* empty */
	|
	statements statement SEMI
	;

statement:
	create_statement
	|
	insert_statement
	|
	unknown_statement {
		fprintf(stderr, "Warning: Unknown statement \"%s\"\n", $1);
	}
	;

create_statement:
	CREATE TABLE garbage tableName LPAREN columnList RPAREN {
		printf("Creating Table \"%s\"\n", $4);
	}

insert_statement:
	INSERT INTO tableName VALUES LPAREN valueList RPAREN {
		printf("Inserted Into Table \"%s\"\n", $3);
	}

unknown_statement:
	/* empty */
	|
	unknown_statement unknown_statement_fragment
	;

unknown_statement_fragment:
	WORD
	|
	LPAREN
	|
	valueList
	|
	RPAREN
	;

/* This just consumes */
garbage:
	/* empty */
	|
	garbage WORD
	|
	garbage NUMBER
	;

tableName:
	WORD
	|
	DOUBLE_QUOTE WORD DOUBLE_QUOTE {
		$$=$2;
	}
	;

columnList:
	columnValue
	|
	columnList COMMA columnValue
	;

columnValue:
	WORD garbage {
		printf("Column: %s\n", $1);
	}
	|
	WORD garbage LPAREN garbage RPAREN {
		printf("Column: %s\n", $1);
	}
	;

valueList:
	value
	|
	valueList COMMA value
	;

value:
	NUMBER
	|
	WORD
	|
	SQL_NULL
	|
	STRING
	;
