CC=g++
CFLAGS = -std=c++14
LIBS = -lpthread

DEPS = promethee/*.h

SRCS = $(shell find -name *.cpp)
OBJS := $(addsuffix .o,$(basename $(SRCS)))

all: run

promethee/%.o: promethee/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

promethee/vanilla/%.o: promethee/vanilla/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

run: $(OBJS)
	$(CC) -o $@ $^ $(LIBS)

clean: 
	rm -rf $(OBJS)
	rm -f run
