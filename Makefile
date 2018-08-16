CC=g++
CFLAGS = -std=c++14
DEPS = promethee/data.h promethee/matrix_meta_data.h promethee/promethee.h promethee/linear_function.h
DEPS += promethee/types.h promethee/inputreader.h promethee/normalize.h promethee/outputwriter.h promethee/promethee_function.h
DEPS += promethee/promethee_function_adapter.h promethee/promethee_vanilla.h promethee/vanilla/comparison_function.h
DEPS += promethee/vanilla/gaussian_function.h promethee/vanilla/level_function.h promethee/vanilla/linear_function.h
DEPS += promethee/vanilla/linear_indifference_function.h promethee/vanilla/quasi_function.h promethee/vanilla/usual_function.h
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
	rm run
