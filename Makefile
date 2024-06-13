CC=g++
CFLAGS=-I. -std=c++11 -Wall -g


DEPS = token.h scanner.h parser.h node.h statSem.h codeGen.h
OBJ = main.o scanner.o parser.o node.o statSem.o codeGen.o


%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


compfs: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)


.PHONY: clean


clean:
	rm -f *.o compfs
