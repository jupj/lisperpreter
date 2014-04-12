CFLAGS=-std=c99 -Wall -g

objects = main.o lex.o

lp: $(objects)
	$(CC) $(CFLAGS) $(objects) -ledit -o lp

main.o : lex.h
lex.o : lex.h

.PHONY : clean
clean:
	-rm lp $(objects)
