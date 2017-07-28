#include <stdio.h>
#include <stdlib.h>

// retrieval
void get_grid_size(FILE *f);
int ** get_grid(FILE *f);
Cage * get_cages(FILE *f);
int ** get_boxes(int **grid);

//verification
bool square_grid(int **grid);
bool valid_values(int **grid);

bool valid_cage_mapping(Cage *cages);
