#ifndef SUDOKU_H
#define SUDOKU_H

#include <stdbool.h>

typedef struct Cage {
  int cage_size;
  int sum;
  int* elements;
} Cage;

extern int NUM_CAGES;
extern int GRID_SIZE;
extern int VALUE_LIMIT;

typedef struct Sudoku {
  int** grid;
  Cage* cages;  //elements of a cage are triples of {row, col}
  int** boxes;
} Sudoku;

//Functions

Sudoku * construct_sudoku(int **grid, Cage *cages, int **boxes);

void print_sudoku(Sudoku *sudoku);

void destroy_sudoku(Sudoku *sudoku);

//Following functions all return true if the region in question satisfies constraints (unique numbers + correct sum in case of cage)
//Upon failing, they should set a flag, and return false, otherwise return true.

bool check_row(Sudoku *sudoku, int row, int *a);
bool check_col(Sudoku *sudoku, int col, int *a);
bool check_box(Sudoku *sudoku, int box, int *a);

bool check_cage(Sudoku *sudoku, int cage, int *a);
bool valid_sum(Sudoku *sudoku, int cage);
bool no_duplicates(Sudoku *sudoku, int cage, int *a);

bool is_complete(Sudoku *sudoku);

bool valid_cell(Sudoku *sudoku, int row, int col, int *a);

bool end(int row, int col);

void get_next(int *cell, int row, int col);

//solve using some back-tracking algorithm
bool solve(Sudoku *sudoku, int row, int col, int *a);

#endif
