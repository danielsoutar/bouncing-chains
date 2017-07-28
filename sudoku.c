#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "sudoku.h"

Sudoku * construct_sudoku(int **grid, Cage *cages, int **boxes) {
  Sudoku *sudoku = calloc(1, sizeof(Sudoku));
  
  sudoku->grid = grid;
  sudoku->boxes = boxes;
  int row, col;
  for(row = 0; row < GRID_SIZE; row++) {
    for(col = 0; col < GRID_SIZE; col++) {
      sudoku->grid[row][col] = grid[row][col];
      sudoku->boxes[row][col] = boxes[row][col];
    }
  }

  sudoku->cages = cages;

  return sudoku;  
}

void reset(int *duplicates) {
  int i;
  for(i = 0; i < GRID_SIZE; i++)
    duplicates[i] = 0;
}

bool check_row(Sudoku *sudoku, int row, int *duplicates) {
  reset(duplicates);
  int **grid = sudoku->grid;
  int col;
  for(col = 0; col < GRID_SIZE; col++) {
    if(grid[row][col] == 0)
      continue;
    else if(duplicates[grid[row][col] - 1] == 1)
      return 0;
    else
      duplicates[grid[row][col] - 1] = 1;
  }
  return 1;
}

bool check_col(Sudoku *sudoku, int col, int *duplicates) {
  reset(duplicates);
  int **grid = sudoku->grid;
  int row;
  for(row = 0; row < GRID_SIZE; row++) {
    if(grid[row][col] == 0)
      continue;
    else if(duplicates[grid[row][col] - 1] == 1)
      return 0;
    else
      duplicates[grid[row][col] - 1] = 1;
  }
  return 1;
}

bool check_box(Sudoku *sudoku, int box, int *duplicates) {
  reset(duplicates);
  int **boxes = sudoku->boxes;
  int b;
  for(b = 0; b < GRID_SIZE; b++) {
    if(boxes[box][b] == 0)
      continue;
    else if(duplicates[boxes[box][b] - 1] == 1)
      return 0;
    else
      duplicates[boxes[box][b] - 1] = 1;
  }
  return 1;    
}

bool valid_sum(Sudoku *sudoku, int cage) {
  int is_full = 1;

  int **grid = sudoku->grid;
  Cage c = sudoku->cages[cage];
  int size = c.cage_size;
  int i, sum = 0;
  for(i = 0; i < size * 2; i += 2) {
    int row = c.elements[i], col = c.elements[i + 1];
    int element = grid[row][col];
    if(element)
      sum += element;
    else
      is_full = 0;
  }

  if(is_full && sum == c.sum)
    return 1;
  else if(sum >= c.sum)
    return 0;
  else
    return 1;
}

bool no_duplicates(Sudoku *sudoku, int cage, int *duplicates) {
  reset(duplicates);
  int **grid = sudoku->grid;
  Cage c = sudoku->cages[cage];
  int size = c.cage_size;
  int i;
  for(i = 0; i < size * 2; i += 2) {
    int row = c.elements[i], col = c.elements[i + 1];
    int element = grid[row][col];
    if(element == 0)
      continue;
    else if(duplicates[element - 1] == 1)
      return false;
    else
      duplicates[element - 1] = 1;
  }
  return 1;
}

bool check_cage(Sudoku *sudoku, int cage, int *duplicates) {
  return valid_sum(sudoku, cage) && no_duplicates(sudoku, cage, duplicates);
}

bool is_complete(Sudoku *sudoku) {
  int **grid = sudoku->grid;
  int row, col;
  for(row = 0; row < GRID_SIZE; row++)
    for(col = 0; col < GRID_SIZE; col++)
      if(grid[row][col] == 0)
        return 0;
  return 1;
}

//need to be clever in search here:
//
// iterate through cages:
//  get size of current cage
//  get row and col of current cage's first element
//  if difference between cage's first row and col and searched row, col is larger than the number of elements, 
//  / check next
//  otherwise iterate through cage:
//   if current row and col equal searched row and col, return this cage's index
//   if current row and col could not equal searched row and col, break, check next


int get_cage_index(Sudoku *sudoku, int row, int col) {
  int cage_i;
  for(cage_i = 0; cage_i < NUM_CAGES; cage_i++) {
    Cage c = sudoku->cages[cage_i];
    int size = c.cage_size * 2;
    int e;
    for(e = 0; e < size; e += 2) {
      int cage_row = c.elements[e];
      int cage_col = c.elements[e + 1];
      int delta_row = abs(row - cage_row);
      int delta_col = abs(col - cage_col);
      int gap = (size - e) / 2;
      if(delta_row == 0 && delta_col == 0)
        return cage_i;
      else if(delta_row > gap || delta_col > gap)
        break;
    }
  }
  fprintf(stderr, "Error - Couldn't find value in cages\n");
  exit(0);
}

bool valid_cell(Sudoku *sudoku, int row, int col, int *duplicates) {

  int root = sqrt(GRID_SIZE);
  int box = col / root + row - row % root;

  if(check_row(sudoku, row, duplicates) && 
     check_col(sudoku, col, duplicates) && 
     check_box(sudoku, box, duplicates) && 
     check_cage(sudoku, get_cage_index(sudoku, row, col), duplicates))
    return 1;
  else
    return 0;
}

// when we get to cell, either:

// > is locked 

//  * if valid cell and not end, enter solve(...) for next cell
//  * if valid cell and end, return solved
//  * if invalid cell, return !solved

// > is unlocked

//  * if valid cell and not end, enter solve(...) for next cell
//  * if valid cell and end, return solved
//  * if invalid cell and not max, repeatedly increment and check for validity
//  * if invalid cell and max, set to 0 and return not solved

bool end(int row, int col) {
  return row == GRID_SIZE - 1 && col == GRID_SIZE - 1;
}

void get_next(int *cell, int row, int col) {
  col++;
  if(col % GRID_SIZE == 0) {
    cell[0] = ++row;
    cell[1] = 0;
  }
  else {
    cell[0] = row;
    cell[1] = col;
  }
}

// we can assume that the sudoku is valid if it ever reaches this function.
bool solve(Sudoku *sudoku, int row, int col, int *duplicates) {
  bool locked = sudoku->grid[row][col] == 0 ? 0 : 1;
  bool solved = 1;
  bool is_end = end(row, col);
  if(locked) {
    print_sudoku(sudoku);
    printf("row: %d, col: %d\n", row, col);
    if(!valid_cell(sudoku, row, col, duplicates))
      return !solved;
    else if(is_end)
      return solved;
    else {
      int cell[2] = {0};
      get_next(cell, row, col);
      return solve(sudoku, cell[0], cell[1], duplicates);
    }
  }
  else {
    int cell[2] = {0};
    get_next(cell, row, col);
    for(sudoku->grid[row][col] = 1; sudoku->grid[row][col] < VALUE_LIMIT; sudoku->grid[row][col]++) {
      print_sudoku(sudoku);
      printf("row: %d, col: %d\n", row, col);
      if(valid_cell(sudoku, row, col, duplicates)) {
        if(is_end)
          return solved;
        if(solve(sudoku, cell[0], cell[1], duplicates))
          return solved;
      }
    }
    sudoku->grid[row][col] = 0;
    return !solved;
  }
}

void print_sudoku(Sudoku *sudoku) {
  int **grid = sudoku->grid;
  int row, col;
  printf("\033[2J\033[1;1H");
  for(row = 0; row < GRID_SIZE; row++) {
    for(col = 0; col < GRID_SIZE; col++) {
      if(grid[row][col] > 0)
        printf(" %d ", grid[row][col]);
      else
        printf(" _ ");
    }
    printf("\n");
  }
}

void destroy_sudoku(Sudoku *sudoku) {
  int **grid = sudoku->grid;

  int row = 0;
  for( ; row < GRID_SIZE; row++)
    free(grid[row]);

  free(grid);
}
