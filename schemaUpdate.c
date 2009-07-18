#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "schemaUpdate.h"

int yyparse();

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

int newColumn(char* column) {
	appendChain(&(getCurrentTable()->values), strdup(column));
}

int newTable(char* table) {
	getCurrentTable()->name = strdup(table);
	appendChain(&Tables, tableAllocator());
}

int newValue(char* value) {
	printf("Value: %s\n", value);
}

int newRow(char* row) {
	printf("Row: %s\n", row);
}

void printTable(SqlTable* table) {
	printf("CREATE TABLE %s(", table->name);
	Node* head = table->values;
	while (head != NULL) {
		printf(" %s", head->value);
		head = head->next;
		if (head != NULL) printf(",");
	}
	printf(");\n");
}

int main() {
	yyparse();
	Node* head=Tables;
	while (head != NULL) {
		printTable(head->value);
		head = head->next;
	}
}
