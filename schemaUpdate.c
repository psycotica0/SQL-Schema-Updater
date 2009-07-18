#include <stdio.h>
#include <string.h>
#include "schemaUpdate.h"

int yyparse();

struct Node* Tables=NULL;
struct Node* TempValues=NULL;

int newColumn(char* column) {
	printf("Column: %s\n", column);
}

int newTable(char* table) {
	printf("Table: %s\n", table);
}

int newValue(char* value) {
	printf("Value: %s\n", value);
}

int newRow(char* row) {
	printf("Row: %s\n", row);
}

int main() {
	yyparse();
}
