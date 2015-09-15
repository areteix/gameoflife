CC=gcc
CFLAGS=-I. -g -O3
DEPS = helper.h
OBJ = helper.o life.o
LIBS = -lpthread 
#-ltcmalloc
%.o: %.c $(DEPS)
		$(CC) -c -o $@ $< $(CFLAGS)

life_matrix_malloc: $(OBJ)
		gcc -o $@ $^ $(CFLAGS) $(LIBS)

life_matrix_tcmalloc: $(OBJ)
		gcc -o $@ $^ $(CFLAGS) $(LIBS) -ltcmalloc

clean:
		rm -rf *.o life_matrix_malloc life_matrix_tcmalloc
