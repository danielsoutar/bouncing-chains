#include <stdio.h>
#include <stdlib.h>
#include "sudoku.h"
#include "io.h"

void print_and_close(Sudoku *sudoku, FILE *f) {
  print_sudoku(sudoku);
  destroy_sudoku(sudoku);  

  fclose(f);
}

int main(int argc, char **argv) {
  if(argc != 2) {
    fprintf(stderr, "No file provided\n");
    exit(0);
  }

  FILE *f = fopen(argv[1], "r");

  if(!f) {
    fprintf(stderr, "Error when opening file\n");
    exit(0);
  }

  get_grid_size(f);

  int **grid = get_grid(f);

  Cage *cages = get_cages(f);

  int **boxes = get_boxes(grid);

  Sudoku *sudoku = construct_sudoku(grid, cages, boxes);
  if(!valid_cage_mapping(cages)) {
    fprintf(stderr, "INVALIDPROBLEM");
    exit(0);
  }
  else
    printf("Valid mapping!\n");

  int array[GRID_SIZE];
  int a;
  for (a = 0; a < GRID_SIZE; a++)
    array[a] = 0;

  int i;
  for(i = 0; i < NUM_CAGES; i++) {
    if(!check_cage(sudoku, i, array)) {
      printf("INVALIDSOL\n");
      exit(0);
    }
  }
  printf("Valid cages!\n");

  int x;
  for(x = 0; x < GRID_SIZE; x++) {
    if(!check_row(sudoku, x, array) || !check_col(sudoku, x, array) || !check_box(sudoku, x, array)) {
      printf("INVALIDSOL\n");
      exit(0);
    }
  }
  printf("Valid rows, cols, and boxes!\n");

  if(is_complete(sudoku)) {
    printf("SOLVED\n");
    print_and_close(sudoku, f);
    exit(0);
  }
  else {
    printf("INCOMPLETE\n");
    solve(sudoku, 0, 0, array);
    print_and_close(sudoku, f);
    exit(0);
  }
}
