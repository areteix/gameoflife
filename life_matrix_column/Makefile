CC=gcc
CFLAGS=-I. -g
DEPS = helper.h
OBJ = helper.o life.o
LIBS = -lpthread 
#-ltcmalloc
%.o: %.c $(DEPS)
		$(CC) -c -o $@ $< $(CFLAGS)

life: $(OBJ)
		gcc -o $@ $^ $(CFLAGS) $(LIBS)

clean:
		rm -rf *.o life
