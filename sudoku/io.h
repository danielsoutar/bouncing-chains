#include <stdio.h>
#include <stdlib.h>

// retrieval
void get_grid_size(FILE *f);
int ** get_grid(FILE *f);
Cage * get_cages(FILE *f);

//verification
bool valid_cage_mapping(Cage *cages);
