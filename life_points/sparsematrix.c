#include "sparsematrix.h"

void insert_col_entry(row_entry* row, int c){
    col_entry * curr_col = row->columns;
    int begin = 0;
    while(curr_col != NULL){
        if(curr_col->col_num == c){
            return;
        }
        else if(curr_col->col_num < c){
            if(curr_col->next != NULL && curr_col->next->col_num <= c){
                curr_col = curr_col->next;
            }
            else {
                break;
            }
        }
        else{
            begin = 1;
            break;
        }
    }
    col_entry * new_col = (col_entry *) malloc(sizeof(col_entry));
    new_col->col_num = c;
    if(curr_col == NULL || curr_col->col_num > c){
        row->columns = new_col;
        new_col->next = curr_col;
    }
    else {
        if(begin == 1){
            new_col->next = curr_col;
            row->columns = new_col;
        }
        else{
            new_col->next = curr_col->next;
            curr_col->next = new_col;
        }
    }
}

void insert_coord(sp_mat *m, int r, int c){
    row_entry * curr_row = *m;
    int begin = 0;
    while(curr_row != NULL){
        if(curr_row->row_num == r){
            insert_col_entry(curr_row, c);
            return;
        }
        else if (curr_row->row_num < r){
            if(curr_row->next != NULL && curr_row->next->row_num <= r) {
                curr_row = curr_row->next;
            }
            else {
                break;
            }
        }
        else{
            begin = 1;
            break;
        }
    }
    row_entry * new_row = (row_entry *) malloc(sizeof(row_entry));
    new_row->row_num = r;
    new_row->columns = NULL;
    if(curr_row == NULL) {
        curr_row = new_row;
        curr_row->next = NULL;
        *m = curr_row;
    }
    else{
        if(begin == 1){
            new_row->next = curr_row;
            *m = new_row;
        }
        else{
            new_row->next = curr_row->next;
            curr_row->next = new_row;
        }
    }
    insert_col_entry(new_row, c);
}

void find_col_entry(row_entry * row, int c, col_entry ** col){
    col_entry * curr_col = row->columns;
    *col = NULL;
    while(curr_col != NULL){
        if(curr_col->col_num == c){
            *col = curr_col;
            break;
        }
        else {
            if(curr_col->next != NULL && curr_col->next->col_num <= c){
                curr_col = curr_col->next;
            }
            else{
                break;
            }
        }
    }
}

void find_coord(sp_mat m, int r, int c, row_entry** row, col_entry** col){
    row_entry * curr_row = m;
    *row = NULL;
    *col = NULL;
    while(curr_row != NULL){
        if(curr_row->row_num == r){
            find_col_entry(curr_row, c, col);
            if(*col != NULL){
                *row = curr_row;
            }
            break;
        }
        else if (curr_row->row_num < r){
            if(curr_row->next != NULL && curr_row->next->row_num <= r) {
                curr_row = curr_row->next;
            }
            else {
                break;
            }
        }
        else{
            break;
        }
    }
}

void free_row(row_entry * row){
    col_entry * curr_col = row->columns;
    col_entry * next_col;
    while(curr_col != NULL){
        next_col = curr_col->next;
        free(curr_col);
        curr_col = next_col;
    }
    free(row);
}

void delete_row(sp_mat *m, row_entry* row){
    row_entry * curr_row = *m;
    if(curr_row == row){
        *m = row->next;
        free_row(row);
    }
    else{
        while(curr_row->next != NULL){
            if(curr_row->next == row){
                curr_row->next = row->next;
                free_row(row);
                break;
            }
            else{
                curr_row = curr_row->next;
            }
        }
    }
}

void delete_mat(sp_mat *m){
    row_entry * curr_row = *m;
    row_entry * next_row;
    while(curr_row != NULL){
        next_row = curr_row->next;
        free_row(curr_row);
        curr_row = next_row;
    }
    *m = NULL;
}

void delete_coord(sp_mat *m, int r, int c){
    row_entry * row;
    col_entry * col;
    col_entry * curr_col;
    find_coord(*m, r, c, &row, &col);
    if(row!=NULL && col!=NULL){
        curr_col = row->columns;
        if(curr_col == col){
            row->columns = col->next;
            free(col);
            if(row->columns == NULL){
              delete_row(m, row);
            }
        }
        else{
            while (curr_col->next != NULL){
                if(curr_col->next == col){
                    curr_col->next = col->next;
                    free(col);
                    break;
                }
                else{
                    curr_col = curr_col->next;
                }
            }
        }
    }
}

void print(sp_mat m){
    row_entry * curr_row = m;
    while (curr_row != NULL){
        col_entry * curr_col = curr_row->columns;
        while(curr_col != NULL){
            printf("(%d, %d)\t", curr_row->row_num, curr_col->col_num);
            curr_col = curr_col->next;
        }
        printf("\n");
        curr_row = curr_row->next;
    }
}

int test (int argc, char *argv[]){
    int i, j;
    sp_mat m = NULL;
    for (i = -12; i < 10; i++)
        for(j=0; j< 10; j++)
            insert_coord(&m, i, j);
    print(m);
    for (i = -12; i < 10; i++)
        for(j=0; j<=i; j++)
            delete_coord(&m, i, j);
    printf("\n");
    print(m);
    return 0;
}
