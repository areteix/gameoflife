#include <stdio.h>
#include <stdlib.h>

typedef struct _col_entry{
    int col_num;
    struct _col_entry *next;
} col_entry;

typedef col_entry * col_list;

typedef struct _row_entry{
    int row_num;
    col_list columns;
    struct _row_entry *next;
} row_entry;


typedef row_entry * sp_mat;

void insert_col_entry(row_entry* row, int c);

void insert_coord(sp_mat *m, int r, int c);

void find_col_entry(row_entry * row, int c, col_entry ** col);

void find_coord(sp_mat m, int r, int c, row_entry** row, col_entry** col);

void free_row(row_entry * row);

void delete_row(sp_mat *m, row_entry* row);

void delete_mat(sp_mat *m);

void delete_coord(sp_mat *m, int r, int c);

void print(sp_mat m);
