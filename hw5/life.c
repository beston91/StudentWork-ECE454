#include "life.h"
#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

/*------------------------------------------------------------------*
  |  Summary
  |  Parallelization strategy:
  |    Split work into sub blocks(by rows of the world) according to the number of threads
  |    Calculate boundary rows of each sub blocks in sequential to avoid lock and race condition 
  |    Calculate the rest of the sub block in parallel 
  |   Optimizations:
  |    Optimize data structures to reduce memory access and cache miss
  |
 *-------------------------------------------------------------------*/

char*
game_of_life (char* outboard,
    char* inboard,
    const int nrows,
    const int ncols,
    const int gens_max)
{
  return optimized_game_of_life(outboard, inboard, nrows, ncols, gens_max);
}

/*------------------------------------------------------------------*
  |
  |  Function: threaded_game_of_life
  |
  |  Purpose: Process the game of life with multiple threads.
  |
  |  Parameters:
  |     @ outboard : output board
  |     @ inboard : input board
  |     @ ncols : total length of the row
  |     @ nrows : total row number of board
  |     @ gens_max : max generations times
  |
  |  Returns: the result of world after generations
  |
 *-------------------------------------------------------------------*/
char* optimized_game_of_life (char* outboard,
                char* inboard,
                const int nrows,
                const int ncols,
                const int gens_max)
{
    int curgen, i, j;

    // Set up threading variables
    int num_threads = sysconf(_SC_NPROCESSORS_ONLN);
    pthread_t *tid;
    tid_create(num_threads);

    struct sub_task *task_block = malloc(num_threads*sizeof(struct sub_task));

    int start_row = 0;
    int chunk_size = nrows/num_threads;
    int end_row = chunk_size;

    task_split(num_threads);


    /*
     * Special case for the 1st and last row, no oversize count
    */
    #pragma omp for
    for (curgen = 0; curgen < gens_max; curgen++) {
        memcpy (outboard, inboard, nrows * ncols * sizeof (char));
        for (i = 0; i < num_threads; i++) {
          process_one_line((task_block[i].begin - 1), ncols, nrows,inboard, outboard);
          process_one_line((task_block[i].end), ncols, nrows, inboard, outboard);
          //pointer access slow on this machine
          //process_one_line((task_block+i)->begin - 1), ncols, nrows,inboard, outboard);
          //process_one_line((task_block+i]->end), ncols, nrows, inboard, outboard);
        }

        thread_create(num_threads);

        thread_join(num_threads);

        SWAP_BOARDS(outboard, inboard);
    }

    #pragma omp for
    for (j = 0; j < ncols; j++) {
        const int nrowsxj = nrows * j;
        for (i = 0; i < nrows; i++) {
            BOARD(inboard,i, nrowsxj) = (BOARD(inboard,i,nrowsxj) & 0xFFFFFFF0) >> 4;
        }
    }

    free(task_block);
    return inboard;
}

/*------------------------------------------------------------------*
  |
  |  Function: thread_worker
  |
  |  Purpose: Process a given sub block of data.
  |
  |  Parameters:
  |     @ ptr : pointer which point to a package structure
  |
  |  Returns: Void
  |
 *-------------------------------------------------------------------*/
void *thread_worker(void *ptr) {
  struct sub_task *p = (struct sub_task *) ptr;
  int start_row = p->begin;
  int end_row = p->end;
  int ncols = p->ncols;
  int nrows = p->nrows;
  elementNode_t* inboard = (elementNode_t*)p-> inboard;
  elementNode_t* outeleList = (elementNode_t*) p-> outboard;
  int i, j;

  int inorth;
  int isouth;
  int jwest;
  int jeast;
  #pragma omp for
  for (j = 0; j < ncols; j++)
  {
      const int nrowsxj = nrows * j;
      for (i = start_row; i < end_row; i++)
      {
          elementNode_t cell = (elementNode_t)BOARD(inboard, i, nrowsxj);
          if (!cell.isAlive)
          {
              if (cell.neighborCnt == 3)
              {   
                  /*
                   * Only boudry case, need mod function, other can just use
                   * basic add or subtract
                  */
                  BOARD(outeleList, i, nrowsxj).isAlive = 1;
                  if( i == 0 || j == 0 || i == nrows - 1 || j == ncols - 1){
                    inorth = mod (i-1, nrows);
                    isouth = mod (i+1, nrows);
                    jwest = mod (j-1, ncols);
                    jeast = mod (j+1, ncols);
                  }
                  else{
                    inorth = i - 1;
                    isouth = i + 1;
                    jwest = j - 1;
                    jeast = j + 1;
                  }
                  const int nrows_jwest = nrows * jwest;
                  const int nrows_jeast = nrows * jeast;
                  BOARD(outeleList, inorth, nrows_jwest).neighborCnt++;
                  BOARD(outeleList, inorth, nrowsxj).neighborCnt++;
                  BOARD(outeleList, inorth, nrows_jeast).neighborCnt++;
                  BOARD(outeleList, i, nrows_jwest).neighborCnt++;
                  BOARD(outeleList, i, nrows_jeast).neighborCnt++;
                  BOARD(outeleList, isouth, nrows_jwest).neighborCnt++;
                  BOARD(outeleList, isouth, nrowsxj).neighborCnt++;
                  BOARD(outeleList, isouth, nrows_jeast).neighborCnt++;
              }
          }
          else if (cell.neighborCnt <= 1 || cell.neighborCnt >= 4)
          {
              BOARD(outeleList, i, nrowsxj).isAlive = 0;
              /*
                * Only boudry case, need mod function, other can just use
                * basic add or subtract
              */
              if( i == 0 || j == 0 || i == nrows - 1 || j == ncols - 1){
                    inorth = mod (i-1, nrows);
                    isouth = mod (i+1, nrows);
                    jwest = mod (j-1, ncols);
                    jeast = mod (j+1, ncols);
                  }
              else{
                inorth = i - 1;
                isouth = i + 1;
                jwest = j - 1;
                jeast = j + 1;
              }
              const int nrows_jwest = nrows * jwest;
              const int nrows_jeast = nrows * jeast;
              BOARD(outeleList, inorth, nrows_jwest).neighborCnt--;
              BOARD(outeleList, inorth, nrowsxj).neighborCnt--;
              BOARD(outeleList, inorth, nrows_jeast).neighborCnt--;
              BOARD(outeleList, i, nrows_jwest).neighborCnt--;
              BOARD(outeleList, i, nrows_jeast).neighborCnt--;
              BOARD(outeleList, isouth, nrows_jwest).neighborCnt--;
              BOARD(outeleList, isouth, nrowsxj).neighborCnt--;
              BOARD(outeleList, isouth, nrows_jeast).neighborCnt--;
          }
      }
  }
  pthread_exit(NULL);
}

/*------------------------------------------------------------------*
  |
  |  Function process_one_line
  |
  |  Purpose: Process only one line with given row number
  |
  |  Parameters:
  |     @ i : row number
  |     @ ncols : total length of the row
  |     @ nrows : total row number of board
  |     @ inboard : input board
  |     @ outboard : output board
  |
  |  Returns: Void
  |
 *-------------------------------------------------------------------*/
void process_one_line(int i, int ncols, int nrows, char* inboard, char* outboard)
{
    int j;
    int inorth;
  int isouth;
  int jwest;
  int jeast;
    elementNode_t *outeleList = (elementNode_t *)outboard;
   
    #pragma omp for
    for (j = 0; j < ncols; j++) {
        const int nrowsxj = nrows * j;
        elementNode_t cell = (elementNode_t)BOARD(inboard, i, nrowsxj);
        if (!cell.isAlive)
        {
            if (cell.neighborCnt == 3)
            {
              BOARD(outeleList, i, nrowsxj).isAlive = 1;
              /*
                * Only boudry case, need mod function, other can just use
                * basic add or subtract
              */
              if( i == 0 || j == 0 || i == nrows - 1 || j == ncols - 1){
                inorth = mod (i-1, nrows);
                isouth = mod (i+1, nrows);
                jwest = mod (j-1, ncols);
                jeast = mod (j+1, ncols);
              }
              else{
                inorth = i - 1;
                isouth = i + 1;
                jwest = j - 1;
                jeast = j + 1;
              }
              
              const int nrows_jwest = nrows * jwest;
              const int nrows_jeast = nrows * jeast;
              BOARD(outeleList, inorth, nrows_jwest).neighborCnt++;
              BOARD(outeleList, inorth, nrowsxj).neighborCnt++;
              BOARD(outeleList, inorth, nrows_jeast).neighborCnt++;
              BOARD(outeleList, i, nrows_jwest).neighborCnt++;
              BOARD(outeleList, i, nrows_jeast).neighborCnt++;
              BOARD(outeleList, isouth, nrows_jwest).neighborCnt++;
              BOARD(outeleList, isouth, nrowsxj).neighborCnt++;
              BOARD(outeleList, isouth, nrows_jeast).neighborCnt++;
            }
        }
        else if (cell.neighborCnt <= 1 || cell.neighborCnt >= 4)
        {
            BOARD(outeleList, i, nrowsxj).isAlive = 0;
            /*
              * Only boudry case, need mod function, other can just use
              * basic add or subtract
            */
            if( i == 0 || j == 0 || i == nrows - 1 || j == ncols - 1){
              inorth = mod (i-1, nrows);
              isouth = mod (i+1, nrows);
              jwest = mod (j-1, ncols);
              jeast = mod (j+1, ncols);
            }
            else{
              inorth = i - 1;
              isouth = i + 1;
              jwest = j - 1;
              jeast = j + 1;
            }
            const int nrows_jwest = nrows * jwest;
            const int nrows_jeast = nrows * jeast;
            BOARD(outeleList, inorth, nrows_jwest).neighborCnt--;
            BOARD(outeleList, inorth, nrowsxj).neighborCnt--;
            BOARD(outeleList, inorth, nrows_jeast).neighborCnt--;
            BOARD(outeleList, i, nrows_jwest).neighborCnt--;
            BOARD(outeleList, i, nrows_jeast).neighborCnt--;
            BOARD(outeleList, isouth, nrows_jwest).neighborCnt--;
            BOARD(outeleList, isouth, nrowsxj).neighborCnt--;
            BOARD(outeleList, isouth, nrows_jeast).neighborCnt--;
        }
    }
}
