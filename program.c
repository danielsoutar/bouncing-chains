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

  // int **boxes = get_boxes(grid);

  Sudoku *sudoku = construct_sudoku(grid, cages);

  // if(!valid_cage_mapping(cages)) {
  //   fprintf(stderr, "INVALIDPROBLEM");
  //   exit(0);
  // }
  // else
  //   printf("Valid mapping!\n");

  int *array = calloc(1, GRID_SIZE * sizeof(int));
  if(!array) {
    fprintf(stderr, "Calloc failed");
    exit(0);
  }

  // for(int i = 0; i < NUM_CAGES; i++) {
  //   if(!check_cage(sudoku, i, array)) {
  //     printf("INVALIDSOL\n");
  //     exit(0);
  //   }
  // }
  // printf("Valid cages!\n");

  for(int x = 0; x < GRID_SIZE; x++) {
    if(!check_row(sudoku, x, array) || !check_col(sudoku, x, array) || !check_box(sudoku, x, array)) {
      printf("INVALIDSOL\n");
      exit(0);
    }
  }
  printf("Valid rows, cols, and boxes!\n");

  if(is_complete(sudoku)) {
    printf("SOLVED\n");
    print_and_close(sudoku, f);
  }
  else {
    printf("INCOMPLETE\n");
    solve(sudoku, 0, 0, array);

    for(int x = 0; x < GRID_SIZE; x++) {
      if(!verify_row(sudoku, x, array) || !verify_col(sudoku, x, array) || !verify_box(sudoku, x, array)) {
        printf("INVALIDSOL\n");
        exit(0);
      }
    }

    int a = sudoku->grid[0][0];
    int b = sudoku->grid[0][1];
    int c = sudoku->grid[0][2];
    // sum += (a * 100) + (b * 10) + c;
    print_and_close(sudoku, f);
  }
}
