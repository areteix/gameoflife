#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "helper.h"

typedef struct _check_bound_params {
    int pos;
    int *res;
    world w;
} check_bound_params;

typedef struct _evolve_param {
    int start_row;
    int end_row;
    world w;
} evolve_param_t;

void * check_boundary(void *cbp);

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

void * check_boundary(void *cbparams) {
    check_bound_params *cbp = ( check_bound_params *) cbparams;
    world w = cbp->w;
    int *res = cbp->res;
    int i;
    switch(cbp->pos){
        case 0:
            for(i=0; i<w.size; i++){
                if((w.space[0][i]) == alive){
                    *res += 1;
                    break;
                }
            }
            break;
        case 1:
            for(i=0; i<w.size; i++){
                if((w.space[i][0]) == alive){
                    *res += 1;
                    break;
                }
            }
            break;
        case 2:
            for(i=0; i<w.size; i++){
                if((w.space[w.size-1][i]) == alive){
                    *res += 1;
                    break;
                }
            }
            break;
        case 3:
            for(i=0; i<w.size; i++){
                if((w.space[i][w.size-1]) == alive){
                    *res += 1;
                    break;
                }
            }
            break;
    }
}

// NOTE: mutex and lock
void process_world_slice(void* args){
    int i, j, n;
    evolve_param_t *evp = (evolve_param_t *) args;
    for(i = evp->start_row; i<evp->end_row; i++){
        for(j=0; j<evp->w.size; j++){
            n = get_neigh(evp->w, i, j);
            if(evp->w.space[i][j]==alive){
                if(n<2 || n>3){
                    evp->w.space[i][j] = dying;
                }
            }
            else if (n==3){
                evp->w.space[i][j] = spawn;
            }
        }
    }
}

world evolve(world w, int num_threads) {
    int i, j, n, nedge=0;
    pthread_t bound_th[4];
    pthread_t *evolve_th = (pthread_t *)malloc(num_threads * sizeof(pthread_t));

    world newworld = w;
    // TODO: threading
    for(i=0; i<num_threads; i++){
        evolve_param_t evp;
        evp.start_row = i * (w.size / num_threads);
        if(i==num_threads-1) {
            evp.end_row = w.size;
        }
        else {
            evp.end_row = (i+1) * (w.size / num_threads);
        }
        evp.w = w;
        pthread_create(&evolve_th[i], NULL, &process_world_slice, (void *)&evp);
    }

    /*
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
    */

    // TODO: wait for all threads
    free(evolve_th);

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

    check_bound_params cbp;
    cbp.res = &nedge;
    cbp.w = w;
    // 4 threads
    for(i=0; i<4;i++){
        cbp.pos = i;
        pthread_create(&bound_th[i], NULL, &check_boundary, (void *)&cbp);
    }
    for(i=0;i<4;i++){
        pthread_join(bound_th[i], NULL);
    }

    /*
       check_boundary(0, &nedge, w);
       check_boundary(1, &nedge, w);
       check_boundary(2, &nedge, w);
       check_boundary(3, &nedge, w);
       */

    if(nedge>0){
        printf("Expanding world...");
        newworld = create_world(w.size*3);
        populate_world(w, newworld);
        free_world(w);
    }
    return newworld;
}


int main (int argc, char * argv){
    int i, num_threads;
    srand(time(NULL));
    world w = create_world(10);
    seed(w);
    print(w);
    for(i=0; i<50; i++){
        w = evolve(w, num_threads);
        print(w);
    }
    return 0;
}
