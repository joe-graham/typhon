CC=gcc
DEPS = arghandler.h
OBJ = typhon.o arghandler.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

typhon: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)
