CC=gcc
CFLAGS=-I. -g
DEPS = sparsematrix.h
OBJ = sparsematrix.o life_points.o
LIBS = -lpthread
%.o: %.c $(DEPS)
		$(CC) -c -o $@ $< $(CFLAGS)

life: $(OBJ)
		gcc -o $@ $^ $(CFLAGS) $(LIBS)

clean:
		rm -rf *.o life_points
