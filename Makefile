sqlParse: lex.yy.c y.tab.c
	$(CC) lex.yy.c y.tab.c -o sqlParse

lex.yy.c: sql.l
	lex sql.l

y.tab.c: sql.y
	yacc -d sql.y

