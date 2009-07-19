#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "schemaUpdate.h"

int yyparse();
extern FILE* yyin;

Node* Tables=NULL;
Node* TempValues=NULL;
int oldOrNew=0;

/* This function gets the last node in the chain */
/* If there are no nodes then it makes one */
Node* getTail(Node** head) {
	if ((*head) == NULL) {
		*head = malloc(sizeof(Node));
		(*head)->next = NULL;
		(*head)->value = NULL;
		return *head;
	} else if ((*head)->next == NULL) {
		return *head;
	} else {
		return getTail(&((*head)->next));
	}
}

/* This function gets the value of the last node */
/* If the last node doesn't have a value, a new one is made with allocator */
void* getLastValue(Node** head, void* (*allocator)()) {
	Node* tail = getTail(head);
	if (tail->value != NULL) return tail->value;

	tail->value = (*allocator)();
	return tail->value;
}

void* tableAllocator() {
	SqlTable* temp = malloc(sizeof(SqlTable));
	temp->oldNew = oldOrNew;
	temp->name = NULL;
	temp->values = NULL;
	return temp;
}

void* valueAllocator() {
	SqlValue* temp = malloc(sizeof(SqlValue));
	temp->columnName = NULL;
	temp->value = NULL;
	return temp;
}

/* This adds a new item onto the end of a chain */
void appendChain(Node** head, void* value) {
	Node* last = getTail(head);

	/* This node was created fresh for us */
	if (last->value == NULL) {
		last->value = value;
		return;
	}

	/* So, here I'm using getTail as an allocator so that it's only in one place... */
	/* If that upsets you, look elsewhere */
	appendChain(&(last->next), value);
}

/* This function gets the last SqlTable object */
/* If one didn't exist before this call, there will be one after it */
SqlTable* getCurrentTable() {
	return (SqlTable*)getLastValue(&Tables, &tableAllocator);
}

/* This function find an old or new table of a given name */
SqlTable* getTable(char* name, int oldOrNew) {
	Node* head=Tables;

	while (head != NULL) {
		if (head->value != NULL && ((SqlTable*)head->value)->name != NULL) {
			if (strcmp(((SqlTable*)head->value)->name, name) == 0 && ((SqlTable*)head->value)->oldNew == oldOrNew) {
				return (SqlTable*)head->value;
			}
		}
		head = head->next;
	}

	return NULL;
}

/* This command takes in the list of values and only outputs the data that has a column in filterThrough */
SqlRow* filterRow(Node* listOfValues, SqlTable* filterThrough) {
	Node* head = listOfValues;

	SqlRow* temp = malloc(sizeof(SqlRow));
	temp->values = NULL;
	temp->table = filterThrough;
	if (filterThrough == NULL) return temp;

	while (head != NULL) {
		Node* column = filterThrough->values;
		while (column != NULL) {
			if (head->value != NULL && column->value != NULL) {
				if (strcmp(((SqlValue*)head->value)->columnName, column->value) == 0) {
					/* In Both */
					appendChain(&(temp->values), head->value);
				}
			}
			column = column->next;
		}
		head = head->next;
	}

	return temp;
}

/* This prints a row */
void printRow(SqlRow* row) {
	if (row->table == NULL) return;
	printf("INSERT INTO %s (", row->table->name);
	Node* head = row->values;
	while (head != NULL) {
		printf("%s", ((SqlValue*)head->value)->columnName);
		head = head->next;
		if (head != NULL) printf(", ");
	}

	printf(") VALUES (");

	head = row->values;
	while (head != NULL) {
		printf("%s", ((SqlValue*)head->value)->value);
		head = head->next;
		if (head != NULL) printf(", ");
	}

	printf(");\n");
}

int newColumn(char* column) {
	SqlTable* current = getCurrentTable();
	if (current->name != NULL) {
		/* We've already filled this one */
		appendChain(&Tables, tableAllocator());
		current = getCurrentTable();
	}
	appendChain(&(current->values), strdup(column));
}

int newTable(char* table) {
	getCurrentTable()->name = strdup(table);
}

int newValue(char* value) {
	SqlValue* temp = valueAllocator();
	appendChain(&TempValues, temp);
	temp->value = strdup(value);
}

int newRow(char* row) {
	SqlTable* table = getTable(row, 0);
	if (table != NULL) {
		Node* head = TempValues;
		Node* columns = table->values;

		while (head != NULL) {
			((SqlValue*)head->value)->columnName = columns->value;
			head = head->next;
			columns = columns->next;
		}

		printRow(filterRow(TempValues, getTable(row, 1)));
	}
	/* XXX: THIS IS CLEARLY NOT GOOD MEMORY MANAGEMENT */
	TempValues = NULL;
}

int main(int argc, char** argv) {
	/* Read in the New schema */
	oldOrNew = 1;
	if (argc <= 1) {
		fputs("Expecting Filename Of New Schema.\n", stderr);
		return 1;
	}

	FILE* newSchema = fopen(argv[1], "r");
	yyin = newSchema;
	yyparse();
	close(newSchema);

	/* Now the old data */
	oldOrNew = 0;
	yyin = stdin;
	yyparse();

	return 0;
}
