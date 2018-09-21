CC=g++
CFLAGS = -std=c++14

DEPS = promethee/*.h

SRCS = $(shell find -name *.cpp)
OBJS := $(addsuffix .o,$(basename $(SRCS)))

all: run

promethee/%.o: promethee/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

promethee/vanilla/%.o: promethee/vanilla/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

run: $(OBJS)
	$(CC) -o $@ $^ 

clean: 
	rm -rf $(OBJS)
	rm -f run
