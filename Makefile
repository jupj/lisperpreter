CFLAGS=-std=c99 -Wall -g

objects = main.o lex.o parse.o eval.o

lp: $(objects)
	$(CC) $(CFLAGS) $(objects) -ledit -o lp

main.o : lex.h parse.h eval.h
lex.o : lex.h
parse.o : lex.h parse.h
eval.o : eval.h parse.h lex.h

.PHONY : clean
clean:
	-rm lp $(objects)
