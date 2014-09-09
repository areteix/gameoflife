#include "helper.h"

void print(world w){
    int i,j;
    usleep(100000);
    printf("\033[H\033[J");
    for(i=0; i<w.size.rows; i++){
        for(j=0; j<w.size.cols; j++){
            if(w.space[i][j] == alive){
                //                printf("\u25A0");
                printf("#");
            }
            else{
                printf(" ");
            }
        }
        printf("\n");
    }
}

void create_block(world w, int i, int j){
    if(i+1 < w.size.rows && j+1 < w.size.cols){
        w.space[i][j] = alive;
        w.space[i][j+1] = alive;
        w.space[i+1][j] = alive;
        w.space[i+1][j+1] = alive;
    }
}

void create_glider(world w, int i, int j){
    if(i+2<w.size.rows && j+2<w.size.cols){
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
    if(i+n<w.size.rows){
        while(n-->0){
            w.space[i+n][j] = alive;
        }
    }
}

void create_hline(world w, int i, int j, int n){
    if(j+n<w.size.cols){
        while(n-->0){
            w.space[i][j+n] = alive;
        }
    }
}

void create_hblinker(world w, int x, int y){
    int i, j;
    if(x+2<w.size.rows && y+2<w.size.cols){
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
    if(x+2<w.size.rows && y+2<w.size.cols){
        for(i=0;i<2;i++){
            for(j=0; j<2; j++){
                w.space[x][y] = dead;
            }
        }
        create_vline(w, i, j+1, 3);
    }
}

void create_pulsar(world w, int i, int j){
    if(i+15<w.size.rows && j+15<w.size.cols){
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

world parse_world(char* filename){
    int r, c, i , j, in, n=0;
    world_size_t size;
    FILE *fin = fopen(filename, "r");
    fscanf(fin, "%d", &r);
    fscanf(fin, "%d", &c);
    size.rows = r;
    size.cols = c;
    world w = create_world(size);
    i = 0;
    j = 0;
    while((in = getc(fin)) != EOF){
        if(in=='.' || in=='*'){
            w.space[i][j] = in=='*'? alive : dead;
            j++;
            if(j==c){
                j=0;
                i++;
            }
        }
        if(i==r-1 && j==c-1) {
            break;
        }
    }
    return w;
}

void kill_world(world w){
    int i, j;
    for(i=0; i<w.size.rows; i++){
        for(j=0; j<w.size.cols; j++){
            w.space[i][j] = dead;
        }
    }
}

world create_world(world_size_t size){
    int i;
    world w;
    w.size = size;
    w.space = (bool **)malloc(size.rows * sizeof(bool *));
    if(NULL == w.space) {
        printf("Err #1\n");
    }
    for(i = 0; i<size.rows; i++){
        w.space[i] = (bool *)malloc(size.cols * sizeof(bool));
    }
    kill_world(w);
    return w;
}

int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y){

    /* Perform the carry for the later subtraction by updating y. */
    if (x->tv_usec < y->tv_usec) {
        int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
        y->tv_usec -= 1000000 * nsec;
        y->tv_sec += nsec;
    }
    if (x->tv_usec - y->tv_usec > 1000000) {
        int nsec = (x->tv_usec - y->tv_usec) / 1000000;
        y->tv_usec += 1000000 * nsec;
        y->tv_sec -= nsec;
    }

    /* Compute the time remaining to wait.
       tv_usec is certainly positive. */
    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;

    /* Return 1 if result is negative. */
    return x->tv_sec < y->tv_sec;
}

