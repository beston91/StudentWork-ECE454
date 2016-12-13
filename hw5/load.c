#include "load.h"
#include <assert.h>
#include <stdlib.h>
#include "util.h"
#include "string.h"

elementNode_t*
make_board (const int nrows, const int ncols)
{
  elementNode_t* board = NULL;

  /* Allocate the board and fill in with 'Z' (instead of a number, so
     that it's easy to diagnose bugs */
  board = malloc (2 * nrows * ncols * sizeof (char));
  assert (board != NULL);
  memset(board, 0, 2 * nrows * ncols * sizeof (char));
 
  return board;
}

static void
load_dimensions (FILE* input, int* nrows, int* ncols)
{
  int ngotten = 0;
  ngotten = fscanf (input, "P1\n%d %d\n", nrows, ncols);
  if (ngotten < 1)
    {
      fprintf (stderr, "*** Failed to read 'P1' and board dimensions ***\n");
      fclose (input);
      exit (EXIT_FAILURE);
    }
  if (*nrows < 1)
    {
      fprintf (stderr, "*** Number of rows %d must be positive! ***\n", *nrows);
      fclose (input);
      exit (EXIT_FAILURE);
    }
  if (*ncols < 1)
    {
      fprintf (stderr, "*** Number of cols %d must be positive! ***\n", *ncols);
      fclose (input);
      exit (EXIT_FAILURE);
    }

  int map_size = (*ncols) * (*nrows);

  if(map_size > 100000000) 
  {
      fprintf (stderr, "*** Map Size %d exceeds Max Boundry! ***\n", map_size);
      fclose (input);
      exit (EXIT_FAILURE);
  }
}

static char*
load_board_values (FILE* input, const int nrows, const int ncols)
{
    elementNode_t* board = NULL;
    int ngotten = 0;
    char scanValue;
    int i = 0;

    /* Make a new board */
    board = make_board (nrows, ncols);

    /* Fill in the board with values from the input file */
    #pragma omp for
    for (i = 0; i < nrows * ncols; i++)
    {
        ngotten = fscanf (input, "%c\n", &scanValue);
        if (ngotten < 1)
        {
            fprintf (stderr, "*** Ran out of input at item %d ***\n", i);
            fclose (input);
            exit (EXIT_FAILURE);
        }
        else {
            /* ASCII '0' is not zero; do the conversion */
            if((scanValue - '0'))
            {
                board[i].isAlive = 1;
            }
        }
    }

    board_map_init(board, nrows, ncols);
    return (char *)board;
}

char *
load_board (FILE* input, int* nrows, int* ncols)
{
  load_dimensions (input, nrows, ncols);
  return (char*)load_board_values (input, *nrows, *ncols);
}

/*****************************************************************************
 * init_neighbour_cnts
 * Helper function to notify all neighbours about a cell that's alive
 ****************************************************************************/
void
board_map_init (elementNode_t * board, const int nrows, const int ncols){
  int i,j;
  #pragma omp for
    for (j = 0; j< ncols; j++) {
      const int nrowsxj = nrows * j;
      const int jwest = mod (j-1, ncols);
      const int jeast = mod (j+1, ncols);
      const int nrows_jwest = nrows * jwest;
      const int nrows_jeast = nrows * jeast;

      for (i = 0; i < nrows; i++) {
      if ( (BOARD(board, i, nrowsxj)).isAlive ) {
          //printf("\r (%d, %d) is alive.", i, j);
          fflush(stdout);
          const int inorth = mod (i-1, nrows);
          const int isouth = mod (i+1, nrows);
       
          BOARD(board, inorth, nrows_jwest).neighborCnt++;
          BOARD(board, inorth, nrowsxj).neighborCnt++;
          BOARD(board, inorth, nrows_jeast).neighborCnt++;
          BOARD(board, i, nrows_jwest).neighborCnt++;
          BOARD(board, i, nrows_jeast).neighborCnt++;
          BOARD(board, isouth, nrows_jwest).neighborCnt++;
          BOARD(board, isouth, nrowsxj).neighborCnt++;
          BOARD(board, isouth, nrows_jeast).neighborCnt++;
      }
    }
  }
}

