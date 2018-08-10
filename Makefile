CC=g++
CFLAGS = -std=c++14
DEPS = promethee/data.h promethee/matrix_meta_data.h promethee/promethee.h promethee/linear_function.h promethee/types.h promethee/inputreader.h promethee/normalize.h promethee/outputwriter.h promethee/promethee_function.h
OBJ = promethee/main.o promethee/data.o  promethee/promethee.o promethee/linear_function.o promethee/inputreader.o promethee/normalize.o promethee/outputwriter.o promethee/promethee_function.o

all: run clean

promethee/%.o: promethee/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

run: $(OBJ)
	$(CC) -o $@ $^ 

clean: 
	rm -rf promethee/*.o
