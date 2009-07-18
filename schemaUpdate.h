#ifndef _SCHEMAUPDATE_H
#define _SCHEMAUPDATE_H

struct Node {
	void* value;
	struct Node* next;
};

struct SqlValue {
	char* columnName;
	char* value;
};

struct SqlTable {
	char* name;
	/* oldNew says whether this table is in the old schema or the new one */
	int oldNew; /* 0 for Old, 1 for New */
	struct Node* values;
};

struct SqlRow {
	struct SqlTable* table;
	struct Node* values;
};

int newColumn(char*);
int newTable(char*);

int newValue(char*);
int newRow(char*);

#endif
