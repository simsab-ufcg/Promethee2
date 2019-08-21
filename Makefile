CC=g++
CFLAGS = -std=c++14 -O2
LIBS = -lpthread -ltiff

DEPS = promethee/*.h

SRCS = $(shell find promethee -name *.cpp)
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
