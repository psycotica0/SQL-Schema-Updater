schemaUpdate: lex.yy.c y.tab.c schemaUpdate.c schemaUpdate.h
	$(CC) lex.yy.c y.tab.c schemaUpdate.c -o schemaUpdate

lex.yy.c: sql.l
	lex sql.l

y.tab.c: sql.y schemaUpdate.h
	yacc -d sql.y

