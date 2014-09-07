#include <stdio.h>
#include <stdlib.h>

#define alive 0x1
#define dead 0x0

typedef short bool;

typedef struct _world_size_t {
    int rows;
    int cols;
} world_size_t;

typedef struct _world {
    bool **space;
    world_size_t size;
} world;

void print(world w);

void create_block(world w, int i, int j);

void create_glider(world w, int i, int j);

void create_vline(world w, int i, int j, int n);

void create_hline(world w, int i, int j, int n);

void create_hblinker(world w, int x, int y);

void create_vblinker(world w, int x, int y);

void create_pulsar(world w, int i, int j);

void kill_world(world w);

world create_world(world_size_t size);

world parse_world(char* filename);

int timeval_subtract(struct timeval* result, struct timeval* x, struct timeval* y);
