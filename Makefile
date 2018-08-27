CC=g++
CFLAGS = -std=c++14

DEPS = promethee/*.h

OBJ = promethee/main.o promethee/data.o  promethee/promethee.o promethee/linear_function.o promethee/inputreader.o promethee/normalize.o
OBJ += promethee/outputwriter.o promethee/promethee_function.o promethee/vanilla/comparison_function.o promethee/vanilla/gaussian_function.o
OBJ += promethee/vanilla/level_function.o promethee/vanilla/linear_function.o promethee/vanilla/linear_indifference_function.o
OBJ += promethee/vanilla/quasi_function.o promethee/vanilla/usual_function.o promethee/promethee_function_adapter.o
OBJ += promethee/promethee_vanilla.o

all: run

promethee/%.o: promethee/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

promethee/vanilla/%.o: promethee/vanilla/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

run: $(OBJ)
	$(CC) -o $@ $^ 

clean: 
	rm -rf promethee/*.o
	rm -rf promethee/vanilla/*.o
	rm -f run
