%{
#include <stdio.h>
#include "schemaUpdate.h"
#define YYSTYPE char*

void yyerror(const char* message) {
	fprintf(stderr, "Parse Error: %s\n", message);
}

int yywrap() {
	return 1;
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
		newTable($4);
	}

insert_statement:
	INSERT INTO tableName VALUES LPAREN valueList RPAREN {
		newRow($3);
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
		newColumn($1);
	}
	|
	WORD garbage LPAREN garbage RPAREN {
		newColumn($1);
	}
	;

valueList:
	value
	|
	valueList COMMA value
	;

value:
	NUMBER {
		newValue($1);
	}
	|
	WORD {
		newValue($1);
	}
	|
	SQL_NULL {
		newValue("NULL");
	}
	|
	STRING {
		newValue($1);
	}
	;
