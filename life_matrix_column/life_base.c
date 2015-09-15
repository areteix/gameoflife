#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define alive 0x1
#define dead 0x0
#define spawn 0x2
#define dying 0x4

typedef short bool;

typedef struct _world {
    bool **space;
    int size;
} world;

void print(world w){
    int i,j;
    usleep(100000);
    printf("\033[H\033[J");
    for(i=0; i<w.size; i++){
        for(j=0; j<w.size; j++){
            if(w.space[i][j] == alive){
                //                printf("\u25A0");
                printf("*");
            }
            else{
                printf(".");
            }
        }
        printf("\n");
    }
}

void create_block(world w, int i, int j){
    if(i+1 < w.size && j+1 < w.size){
        w.space[i][j] = alive;
        w.space[i][j+1] = alive;
        w.space[i+1][j] = alive;
        w.space[i+1][j+1] = alive;
    }
}

void create_glider(world w, int i, int j){
    if(i+2<w.size && j+2<w.size){
        w.space[i][j] = dead;
        w.space[i][j+1] = alive;
        w.space[i][j+2] = dead;

        w.space[i+1][j] = dead;
        w.space[i+1][j+1] = dead;
        w.space[i+1][j+2] = alive;

        w.space[i+2][j] = alive;
        w.space[i+2][j+1] = alive;
        w.space[i+2][j+2] = alive;
    }
}

void create_vline(world w, int i, int j, int n){
    if(i+n<w.size){
        while(n-->0){
            w.space[i+n][j] = alive;
        }
    }
}

void create_hline(world w, int i, int j, int n){
    if(j+n<w.size){
        while(n-->0){
            w.space[i][j+n] = alive;
        }
    }
}

void create_hblinker(world w, int x, int y){
    int i, j;
    if(x+2<w.size && y+2<w.size){
        for(i=0;i<2;i++){
            for(j=0; j<2; j++){
                w.space[x][y] = dead;
            }
        }
        create_hline(w, i+1, j, 3);
    }
}

void create_vblinker(world w, int x, int y){
    int i, j;
    if(x+2<w.size && y+2<w.size){
        for(i=0;i<2;i++){
            for(j=0; j<2; j++){
                w.space[x][y] = dead;
            }
        }
        create_vline(w, i, j+1, 3);
    }
}

void create_pulsar(world w, int i, int j){
    if(i+15<w.size && j+15<w.size){
        w.space[i][j] = dead;
    }

    create_hline(w, i+1, j+3, 3);
    create_hline(w, i+1, j+9, 3);
    create_vline(w, i+3, j+1, 3);
    create_vline(w, i+3, j+6, 3);
    create_vline(w, i+3, j+8, 3);
    create_vline(w, i+3, j+13, 3);
    create_hline(w, i+6, j+3, 3);
    create_hline(w, i+6, j+9, 3);

    create_hline(w, i+8, j+3, 3);
    create_hline(w, i+8, j+9, 3);
    create_vline(w, i+9, j+1, 3);
    create_vline(w, i+9, j+6, 3);
    create_vline(w, i+9, j+8, 3);
    create_vline(w, i+9, j+13, 3);
    create_hline(w, i+13, j+3, 3);
    create_hline(w, i+13, j+9, 3);
}

void seed(world w){
    int i, j;
    /*  for(i=0; i<50; i++){
        world[rand()%w.size][rand()%w.size] = alive;
        }
        */

    create_glider(w, 5, 5);
    create_block(w, 1, 1);
//    create_pulsar(w, 2, 2);
}

int get_neigh(world w, int x, int y){
    int n=0, i, j, a, b;
    for(i=-1; i<=1; i++){
        for(j=-1; j<=1; j++){
            if(i==0 && j==0) {
                continue;
            }
            a = x+i;
            b = y+j;
            if(a >=0 && a < w.size && b >=0 && b < w.size){
                if(w.space[a][b] == alive || w.space[a][b] == dying){
                    n++;
                }
            }
        }
    }
    return n;
}

world create_world(int size){
    int i;
    world w;
    w.size = size;
    w.space = (bool **)malloc(size * sizeof(bool *));
    if(NULL == w.space) {
        printf("Err #1\n");
    }
    for(i = 0; i<size; i++){
        w.space[i] = (bool *)malloc(size * sizeof(bool));
    }
    return w;
}

void free_world(world w){
    int i;
    for(i=0; i<w.size;i++){
        free(w.space[i]);
    }
    free(w.space);
}

void populate_world(world w, world neww){
    int i, j;
    for(i=0; i<w.size; i++){
        for(j=0; j<w.size; j++){
            neww.space[w.size+i][w.size+j] = w.space[i][j];
        }
    }
}

world evolve(world w) {
    int i, j, n, nedge=0;
    world newworld = w;
    // TODO: threading
    for(i=0; i<w.size; i++){
        for(j=0; j<w.size; j++){
            n = get_neigh(w, i, j);
            // printf("%d, %d - %d\n", i, j, n);;
            if(w.space[i][j]==alive){
                if(n<2 || n>3){
                    w.space[i][j] = dying;
                }
            }
            else if (n==3){
                w.space[i][j] = spawn;
            }
        }
    }

    // TODO: wait for all threads

    // TODO: threading
    for(i=0; i<w.size; i++){
        for(j=0; j<w.size; j++){
            if(w.space[i][j] == spawn){
                w.space[i][j] = alive;
            }
            if(w.space[i][j] == dying){
                w.space[i][j] = dead;
            }
        }
    }

    // 4 threads
    for(i=0; i<w.size; i++){
        nedge += w.space[0][i];
        nedge += w.space[i][0];
        nedge += w.space[w.size-1][i];
        nedge += w.space[i][w.size-1];
    }
    if(nedge>0){
        printf("Expanding world...");
        newworld = create_world(w.size*3);
        populate_world(w, newworld);
        free_world(w);
    }
    return newworld;
}


int main (int argc, char * argv){
    int i;
    srand(time(NULL));
    world w = create_world(10);
    seed(w);
    print(w);
    for(i=0; i<50; i++){
        w = evolve(w);
        print(w);
    }
    return 0;
}
