#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "sudoku.h"
#include "io.h"

int GRID_SIZE;
int NUM_CAGES;
int VALUE_LIMIT;

void get_grid_size(FILE* f) {
  if(fscanf(f, "%d", &GRID_SIZE) == 0)
    exit(0);
  VALUE_LIMIT = GRID_SIZE + 1;
}

int ** allocate_grid() {
  int **grid = malloc(GRID_SIZE * sizeof(int *));
  if(!grid) {
    fprintf(stderr, "Malloc failed");
    exit(0);
  }

  int i;
  for(i = 0; i < GRID_SIZE; i++) {
    grid[i] = malloc(GRID_SIZE * sizeof(int));
    if(!grid[i]) {
      fprintf(stderr, "Malloc failed");
      exit(0);
    }
  }
  return grid;
}

int ** get_grid(FILE *f) {
  int **grid = allocate_grid();
  int val = 0, i = 0, square = GRID_SIZE * GRID_SIZE;
  int row = 0, col = 0;

  while(fscanf(f, "%d", &val) == 1 && (i != square)) { 
    grid[row][col] = val;
    i++, col++;
    if(i % GRID_SIZE == 0)
      row++, col = 0;
  }

  return grid;
}

int ** get_boxes(int **grid) {
  int **boxes = allocate_grid();
  int counters[GRID_SIZE];

  int i;
  for(i = 0; i < GRID_SIZE; i++)
    counters[i] = 0;

  int row, col;
  int root = sqrt(GRID_SIZE);
  for(row = 0; row < GRID_SIZE; row++)
    for(col = 0; col < GRID_SIZE; col++)
      boxes[col / root + row - row % root][counters[col / root + row - row % root]++] = grid[row][col];

  return boxes;
}

int * reallocate_block(int *block, int t) {
  int *temp = realloc(block, 2 * sizeof(int) * t);
  if(temp != NULL)
    return temp;
  else
    exit(0);
}

Cage * reallocate_cages(Cage *cages, int i) {
  Cage *temp = realloc(cages, (i+1) * sizeof(Cage));
  if(temp != NULL)
    return temp;
  else
    exit(0);
}

Cage * allocate_cages() {
  Cage *temp = malloc(sizeof(Cage));
  if(temp != NULL)
    return temp;
  else
    exit(0);
}


Cage * get_cages(FILE *f) {
  Cage *cages = allocate_cages();
  int i = 0;
  do {
    Cage cage;
    int size = 0, sum = 0, t = 1;
    cage.elements = malloc(2 * sizeof(int) * t);
    int index = 0;
    if(fscanf(f, "size %d\n", &size) == 1)
      if(fscanf(f, "sum %d\n", &sum) == 1) {
        while(fscanf(f, "%d %d\n", &cage.elements[index], &cage.elements[index+1]) == 2) {
          index += 2, t++;
          cage.elements = reallocate_block(cage.elements, t);
        }
        cage.cage_size = size; cage.sum = sum;
        printf("size: %d, sum: %d\n", size, sum);

        cages[i++] = cage;
        cages = reallocate_cages(cages, i);

        if(fscanf(f, "\n") == 1)
          continue;
      }
      else
        exit(0);
    else
      break;
  }
  while(1);

  NUM_CAGES = i;

  return cages;
}

//need to prove that every x,y pair in cages corresponds to exactly one square in grid

bool valid_cage_mapping(Cage *cages) {
  bool bit_map[GRID_SIZE][GRID_SIZE];
  int i, j, x, y;
  for(x = 0; x < GRID_SIZE; x++)
    for(y = 0; y < GRID_SIZE; y++)
      bit_map[x][y] = 0;

  for(i = 0; i < NUM_CAGES; i++) {
    for(j = 0; j < cages[i].cage_size * 2; j += 2) {
      int row = cages[i].elements[j], col = cages[i].elements[j + 1];
      if(bit_map[row][col] == 1)
        return 0;
      else
        bit_map[row][col] = 1;
    }
  }

  //now need to check if bit map contains any 0s - i.e. missing any elements
  for(x = 0; x < GRID_SIZE; x++)
    for(y = 0; y < GRID_SIZE; y++)
      if(bit_map[x][y] == 0)
        return 0;

  return 1;
}
