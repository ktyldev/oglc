#include "random.h"

void randomInit()
{
    srand(time(NULL));
}

float randomFloat()
{
    return (float)rand()/(float)RAND_MAX;
}
