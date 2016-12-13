#ifndef _life_h
#define _life_h
#include "util.h"

#define tid_create(x) {\
tid = (pthread_t *)malloc((x) * sizeof(pthread_t));\
}

#define thread_join(x)  \
do{\
  int i;\
  for(i = 0; i < (x); i++){\
    pthread_join(*(tid+i), NULL);\
  }\
}while(0)

#define task_split(x) \
do{\
      int i;\
      for(i = 0; i < (x); i++){\
        ((task_block)+i)->begin = start_row + 1;\
        ((task_block)+i)->end = end_row - 1;\
        start_row = end_row;\
        end_row = end_row + chunk_size;\
        ((task_block)+i)->nrows = nrows;\
        ((task_block)+i)->ncols = ncols;\
      }\
}while(0)

#define thread_create(x)  \
  do{\
    int i;\
    for(i = 0; i < (x); i++){   \
      ((task_block) + i)->inboard = inboard;\
      ((task_block) + i)->outboard = outboard;\
      pthread_create(((tid)+i), NULL, thread_worker, (void*)((task_block) + i));\
  }\
}while(0)

char*
game_of_life (char* outboard,
            char* inboard,
            const int nrows,
            const int ncols,
            const int gens_max);

char*
sequential_game_of_life (char* outboard,
            char* inboard,
            const int nrows,
            const int ncols,
            const int gens_max);

char * optimized_game_of_life(char* outboard,
            char* inboard,
            const int nrows,
            const int ncols,
            const int gens_max);

void *thread_worker(void *ptr);
void process_one_line(int i, int ncols, int nrows, char* inboard, char* outboard);
#endif /* _life_h */
