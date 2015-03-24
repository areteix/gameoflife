#include "sparsematrix.h"
#include <pthread.h>
#include <sys/time.h>

#define TRUE 1
#define FALSE 0


typedef struct _process_active_cell_params_t {
    sp_mat* world;
    int r;
    int c;
    sp_mat *kill;
    sp_mat* spawn;
} process_active_cell_params_t;

typedef struct _process_spawn_kill_params_t {
    sp_mat* world;
    sp_mat* active_region;
    sp_mat in_coords;
} process_spawn_kill_params_t;

typedef short bool;

void seed(sp_mat *w){
    insert_coord(w, 0, 10);
    insert_coord(w, 0, 11);
    insert_coord(w, 1, 10);
    insert_coord(w, 1, 11);

    insert_coord(w, 15, 7);
    insert_coord(w, 15, 6);
    insert_coord(w, 15, 5);

    insert_coord(w, 0, 1);
    insert_coord(w, 1, 2);
    insert_coord(w, 2, 0);
    insert_coord(w, 2, 1);
    insert_coord(w, 2, 2);
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



void display_world(sp_mat world){
    int i=0, j=0;
    usleep(100000);
    printf("\033[H\033[J");
    row_entry * curr_row = world;
    while (curr_row != NULL){
        col_entry * curr_col = curr_row->columns;
        j=0;
        while(curr_col != NULL){
            while(i<curr_row->row_num){
                while(j<curr_col->col_num){
                    printf(" ");
                    j++;
                }
                printf("\n");
                i++;
                j=0;
            }
            while(j < curr_col->col_num){
                printf(" ");
                j++;
            }
            printf("#");
            j++;
            curr_col = curr_col->next;
        }
        i++;
        printf("\n");
        curr_row = curr_row->next;
    }
}

void activate_region(sp_mat * active_region, int r, int c){
    int i, j;
    for(i=-1; i<=1; i++){
        for(j=-1; j<=1; j++){
            if(i==0 && j==0){
                continue;
            }
            insert_coord(active_region, r+i, c+j);
        }
    }
}

void initial_active_region(sp_mat world, sp_mat * active_region){
    int i, j;
    row_entry * curr_row = world;
    while (curr_row != NULL){
        col_entry * curr_col = curr_row->columns;
        while(curr_col != NULL){
            activate_region(active_region, curr_row->row_num, curr_col->col_num);
            insert_coord(active_region, curr_row->row_num, curr_col->col_num);
            curr_col = curr_col->next;
        }
        curr_row = curr_row->next;
    }
}

bool is_alive(sp_mat world, int r, int c){
    row_entry * row = NULL;
    col_entry * col = NULL;
    find_coord(world, r, c, &row, &col);
    if(row == NULL && col == NULL){
        return FALSE;
    }
    else if (row != NULL && col != NULL){
        return TRUE;
    }
    else{
        printf("Invalid state!!\n");
        return FALSE;
    }
}

int count_neigh(sp_mat world, int r, int c){
    int i, j, n=0;
    for(i=-1; i<=1; i++){
        for(j=-1; j<=1; j++){
            if(!(i==0 && j==0)){
                if(is_alive(world, r+i, c+j) == TRUE){
                    n++;
                }
            }
        }
    }
    return n;
}

void * process_spawn(void *args){
    process_spawn_kill_params_t * params = (process_spawn_kill_params_t *) args;
    sp_mat* world = params->world;
    sp_mat* active_region = params->active_region;
    sp_mat spawn = params->in_coords;
//    printf("spawn ");
//    print(spawn);
    row_entry * curr_row = spawn;
    while (curr_row != NULL){
        col_entry * curr_col = curr_row->columns;
        while(curr_col != NULL){
            activate_region(active_region, curr_row->row_num, curr_col->col_num);
            insert_coord(world, curr_row->row_num, curr_col->col_num);
            curr_col = curr_col->next;
        }
        curr_row = curr_row->next;
    }
}

void * process_kill(void *args){
    process_spawn_kill_params_t * params = (process_spawn_kill_params_t *) args;
    sp_mat* world = params->world;
    sp_mat* active_region = params->active_region;
    sp_mat kill = params->in_coords;
//    printf("kill ");
//    print(kill);
    row_entry * curr_row = kill;
    while (curr_row != NULL){
        col_entry * curr_col = curr_row->columns;
        while(curr_col != NULL){
            activate_region(active_region, curr_row->row_num, curr_col->col_num);
            delete_coord(world, curr_row->row_num, curr_col->col_num);
            curr_col = curr_col->next;
        }
        curr_row = curr_row->next;
    }
}

void * process_active_cell(void *args){
//    sp_mat* world, int r, int c, sp_mat *kill, sp_mat* spawn);
    process_active_cell_params_t * params = (process_active_cell_params_t*) args;
    int n;
    n = count_neigh(*params->world, params->r, params->c);
    if(is_alive(*params->world, params->r, params->c) == TRUE){
        if(n<2 || n>3){
            insert_coord(params->kill, params->r, params->c);
        }
    }
    else if(n==3){
        insert_coord(params->spawn, params->r, params->c);
    }
}

void evolve(sp_mat* world, sp_mat *active_region, int num_threads, int manage_active){
    int r, c, n, wait_for=0, i;
    sp_mat spawn = NULL;
    sp_mat kill = NULL;
    pthread_t* pac_threads = (pthread_t*) malloc(num_threads * sizeof(pthread_t));
    process_active_cell_params_t* pac_params = (process_active_cell_params_t *) malloc(num_threads * sizeof(process_active_cell_params_t));
    process_spawn_kill_params_t* psk_params = (process_spawn_kill_params_t *) malloc(2 * sizeof(process_spawn_kill_params_t));
    row_entry * curr_row = *active_region;
    while (curr_row != NULL){
        col_entry * curr_col = curr_row->columns;
        while(curr_col != NULL){
            if(wait_for == num_threads){
                for(i=0;i<wait_for; i++){
                    pthread_join(pac_threads[i], NULL);
                }
                wait_for = 0;
            }
            else{
                pac_params[wait_for].world = world;
                pac_params[wait_for].kill = &kill;
                pac_params[wait_for].spawn = &spawn;
                pac_params[wait_for].r = curr_row->row_num;
                pac_params[wait_for].c = curr_col->col_num;
                pthread_create(&pac_threads[wait_for], NULL, &process_active_cell, (void *)(pac_params+wait_for));

                // process_active_cell(world, r, c, &kill, &spawn);
                /*
                   n = count_neigh(*world, r, c);
                // printf("(%d, %d) has %d neighs\n", r, c, n);
                if(is_alive(*world, r, c) == TRUE){
                if(n<2 || n>3){
                insert_coord(&kill, r, c);
                }
                }
                else if(n==3){
                insert_coord(&spawn, r, c);
                }
                */
                curr_col = curr_col->next;
                wait_for++;
            }
        }
        curr_row = curr_row->next;
    }

    for(i=0;i<wait_for; i++){
        pthread_join(pac_threads[i], NULL);
    }
    free(pac_threads);
    free(pac_params);
    if(manage_active > 0){
        delete_mat(active_region);
    }
    psk_params[0].world = world;
    psk_params[0].active_region = active_region;
    psk_params[0].in_coords = spawn;

    psk_params[1].world = world;
    psk_params[1].active_region = active_region;
    psk_params[1].in_coords = kill;

    if(num_threads > 1){
        pthread_t spawn_thread, kill_thread;
        pthread_create(&spawn_thread, NULL, &process_spawn, (void *)(&psk_params[0]));
        pthread_create(&kill_thread, NULL, &process_kill, (void *)(&psk_params[1]));

        pthread_join(spawn_thread, NULL);
        pthread_join(kill_thread, NULL);
    }
    else{
        process_spawn(&psk_params[0]);
        process_kill(&psk_params[1]);
    }
    free(psk_params);
    delete_mat(&spawn);
    delete_mat(&kill);
}

void parse_world(sp_mat *world, char* filename){
    int r, c, i , j, in, n=0;
    FILE *fin = fopen(filename, "r");
    fscanf(fin, "%d", &r);
    fscanf(fin, "%d", &c);
    i = 0;
    j = 0;
    while((in = getc(fin)) != EOF){
        if(in=='.' || in=='*'){
            if(in=='*'){
                insert_coord(world, i, j);
            }
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
}

int main(int argc, char *argv[]){
    int t, num_threads=1, manage_active = 1, num_gen = 10;
    sp_mat world = NULL;
    sp_mat active_region = NULL;
    if(argc < 5){
        printf("%s <num_threads> <in_file> <manage_active> <num_gen>\n", argv[0]);
        return 0;
    }
    num_threads = atoi(argv[1]);

    if(strcmp(argv[2], "none") == 0){
        seed(&world);
    }
    else{
        parse_world(&world, argv[2]);
    }
    manage_active = atoi(argv[3]);
    num_gen = atoi(argv[4]);

    initial_active_region(world, &active_region);

 //   print(world);
 //   printf("\n");
 //   print(active_region);
    struct timeval start_time, end_time, diff_time;
    gettimeofday(&start_time, 0);

    for(t=0;t<num_gen;t++){
        evolve(&world, &active_region, num_threads, manage_active);
//        display_world(world);
//        print(world);
    }
    gettimeofday(&end_time, 0);
    if(timeval_subtract(&diff_time, &end_time, &start_time) != 1){
        printf("num_threads: %d man_act: %d num_gen: %d time: %ld us\n", num_threads, manage_active, num_gen, diff_time.tv_sec * 1000000 + diff_time.tv_usec);
    }
//    display_world(world);
    delete_mat(&world);
    delete_mat(&active_region);
}

