CC = gcc
TARGET = librstack.so
CFLAGS=-Wall -Wextra -Wno-implicit-fallthrough -std=gnu23 -fPIC -O2 -c
CFLAGSEXTRA=-g -O0 -shared -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc -Wl,--wrap=reallocarray -Wl,--wrap=free -Wl,--wrap=strdup -Wl,--wrap=strndup
OBJS = rstack.o memory_tests.o list.o

.PHONY: librstack.so clean test example

librstack.so: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGSEXTRA) -o $@ $^

rstack.o: rstack.c rstack.h
	$(CC) $(CFLAGS)  rstack.c

memory_tests.o: memory_tests.c memory_tests.h
	$(CC) $(CFLAGS) memory_tests.c

list.o: list.c list.h
	$(CC) $(CFLAGS) list.c

clean:
	rm -f $(OBJS) $(TARGET)

test: $(TARGET)
	$(CC) -std=gnu23 main.c -L. -lrstack -Wl,-rpath=. -o testuj
	./testuj
	
example:
	$(CC) rstack_example.c -L. -lrstack -Wl,-rpath=. -o testuj

