CC=gcc
CFLAGS=-I. -g -O3
DEPS = sparsematrix.h
OBJ = sparsematrix.o life_points.o
LIBS = -lpthread
%.o: %.c $(DEPS)
		$(CC) -c -o $@ $< $(CFLAGS)

life_points_malloc: $(OBJ)
		gcc -o $@ $^ $(CFLAGS) $(LIBS)

life_points_tcmalloc: $(OBJ)
		gcc -o $@ $^ $(CFLAGS) $(LIBS) -ltcmalloc

clean:
		rm -rf *.o life_points_tcmalloc life_points_malloc
