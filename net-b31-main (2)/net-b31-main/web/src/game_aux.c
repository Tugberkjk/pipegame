/**
 * @file game_aux.c
 * @copyright University of Bordeaux. All rights reserved, 2024.
 **/
#include "game_aux.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_ext.h"
#include "game_private.h"
#include "game_struct.h"

/* ************************************************************************** */
/*                             LOCAL DEFINITIONS                              */
/* ************************************************************************** */

#define DN NORTH
#define DE EAST
#define DS SOUTH
#define DW WEST

#define SE EMPTY
#define SN ENDPOINT
#define SS SEGMENT
#define SC CORNER
#define ST TEE
#define SX CROSS

int DIR2OFFSET[][2] = {
    [NORTH] = {-1, 0},
    [EAST] = {0, 1},
    [SOUTH] = {1, 0},
    [WEST] = {0, -1},
};

#define OPPOSITE_DIR(d) ((d + 2) % NB_DIRS)
#define NEXT_DIR_CW(d) ((d + 1) % NB_DIRS)
#define NEXT_DIR_CCW(d) ((d + 3) % NB_DIRS)

/* ****************************** DEFAULT GAME ****************************** */
static shape default_p[] = {
    SC, SN, SN, SC, SN, /* row 0 */
    ST, ST, ST, ST, ST, /* row 1 */
    SN, SN, ST, SN, SS, /* row 2 */
    SN, ST, ST, SC, SS, /* row 3 */
    SN, ST, SN, SN, SN, /* row 4 */
};

static direction default_o[] = {
    DW, DN, DW, DN, DS, /* row 0 */
    DS, DW, DN, DE, DE, /* row 1 */
    DE, DN, DW, DW, DE, /* row 2 */
    DS, DS, DN, DW, DN, /* row 3 */
    DE, DW, DS, DE, DS, /* row 4 */
};

static direction default_s[] = {
    DE, DW, DE, DS, DS, /* row 0 */
    DE, DS, DS, DN, DW, /* row 1 */
    DN, DN, DE, DW, DS, /* row 2 */
    DE, DS, DN, DS, DN, /* row 3 */
    DE, DN, DW, DN, DN, /* row 4 */
};

/* ************************************************************************** */
/*                             GAME AUX FUNCTIONS                             */
/* ************************************************************************** */

/* ******************************* GAME PRINT ******************************* */
void game_print(cgame g) {
  assert(g);

  uint h = game_nb_rows(g);
  uint w = game_nb_cols(g);

  /*
   * Print the game (Exemple for game default (5x5))
   *    0 1 2 3 4
   *    ----------
   * 0 |┘ ^ < └ v |
   * 1 |┬ ┤ ┴ ├ ├ |
   * 2 |> ^ ┤ < - |
   * 3 |v ┬ ┴ ┘ | |
   * 4 |> ┤ v > v |
   *    ----------
   */
  printf("     ");
  for (uint i = 0; i < w; i++) printf("%d ", i);
  printf("\n     ");
  for (uint i = 0; i < 2 * w; i++) printf("-");
  printf("\n");
  for (uint i = 0; i < h; i++) {
    printf("  %d |", i);
    for (uint j = 0; j < w; j++) {
      shape s = game_get_piece_shape(g, i, j);
      direction o = game_get_piece_orientation(g, i, j);
      char* ch = _square2str(s, o);
      printf("%s ", ch);
    }
    printf("|\n");
  }
  printf("     ");
  for (uint i = 0; i < 2 * w; i++) printf("-");
  printf("\n");
}

/* ****************************** GAME DEFAULT ****************************** */
game game_default(void) { return game_new(default_p, default_o); }

/* ************************* GAME DEFAULT SOLUTION ************************** */
game game_default_solution(void) { return game_new(default_p, default_s); }

/* ************************ GAME GET AGJACENT SQUARE ************************ */
bool game_get_ajacent_square(cgame g, uint i, uint j, direction d, uint* pi_next, uint* pj_next) {
  assert(g);

  // Get game size and test if i and j are correct
  uint h = game_nb_rows(g);
  uint w = game_nb_cols(g);
  assert(i < h && j < w);

  // convert direction to offset
  int i_offset = DIR2OFFSET[d][0];
  int j_offset = DIR2OFFSET[d][1];

  // move to the next square in a given direction
  int ii = i + i_offset;
  int jj = j + j_offset;

  if (game_is_wrapping(g)) {
    ii = (ii + game_nb_rows(g)) % game_nb_rows(g);
    jj = (jj + game_nb_cols(g)) % game_nb_cols(g);
  }

  // check if next square at (ii,jj) is out of grid
  if (ii < 0 || ii >= (int)h) return false;
  if (jj < 0 || jj >= (int)w) return false;

  *pi_next = ii;
  *pj_next = jj;

  return true;
}

/* *************************** GAME HAS HALF EDGE *************************** */
bool game_has_half_edge(cgame g, uint i, uint j, direction d) {
  assert(g);
  assert(i < game_nb_rows(g) && j < game_nb_cols(g));
  assert(d >= 0 && d < NB_DIRS);

  shape s = game_get_piece_shape(g, i, j);
  direction o = game_get_piece_orientation(g, i, j);

  switch (s) {
    case EMPTY:
      return false;
    case ENDPOINT:
      return (d == o);
    case SEGMENT:
      return (d == o || d == OPPOSITE_DIR(o));
    case TEE:
      return (d != OPPOSITE_DIR(o));
    case CORNER:
      return (d == o || d == NEXT_DIR_CW(o));
    case CROSS:
      return true;
    default:
      assert(true);
      return false;
  }
}

/* **************************** GAME CHECK EDGE ***************************** */
edge_status game_check_edge(cgame g, uint i, uint j, direction d) {
  assert(g);
  assert(i < game_nb_rows(g) && j < game_nb_cols(g));

  // Check if the piece (i,j) has half edge in d direction
  bool hasHE1 = game_has_half_edge(g, i, j, d);
  bool hasHE2;
  uint i_next, j_next;

  // If the piece (i,j) has a piece in direction d, check if this piece has half edge in direction of first piece (i,j)
  if (game_get_ajacent_square(g, i, j, d, &i_next, &j_next))
    hasHE2 = game_has_half_edge(g, i_next, j_next, OPPOSITE_DIR(d));
  else
    hasHE2 = false;

  // Return edge status
  if (hasHE1 && hasHE2)
    return MATCH;
  else if (hasHE1 || hasHE2)
    return MISMATCH;
  else
    return NOEDGE;
}

/* ************************** GAME IS WELL PAIRED *************************** */
bool game_is_well_paired(cgame g) {
  assert(g);

  for (int i = 0; i < game_nb_rows(g); i++) {
    for (int j = 0; j < game_nb_cols(g); j++) {
      for (direction d = 0; d < NB_DIRS; d++) {
        // Game is not well paired if has MISMATCH
        if (game_check_edge(g, i, j, d) == MISMATCH) return false;
      }
    }
  }

  return true;
}

/* *************************** GAME IS CONNECTED **************************** */
bool game_is_connected(cgame g) {
  assert(g);

  // Get game dimensions
  uint h = game_nb_rows(g);
  uint w = game_nb_cols(g);
  uint stack_size = h * w;

  int* stack = malloc(2 * stack_size * sizeof(int));
  assert(stack);

  bool* visited = calloc(stack_size, sizeof(bool));
  assert(visited);

  int start_i = -1, start_j = -1;
  for (uint i = 0; i < h; i++) {
    for (uint j = 0; j < w; j++) {
      if (game_get_piece_shape(g, i, j) != EMPTY) {
        start_i = i;
        start_j = j;
        break;
      }
    }
    if (start_i != -1) break;
  }

  if (start_i == -1) {
    free(visited);
    free(stack);
    return true;
  }

  int stack_top = 0;
  stack[stack_top++] = start_i;
  stack[stack_top++] = start_j;

  while (stack_top > 0) {
    int j = stack[--stack_top];
    int i = stack[--stack_top];

    if (visited[i * w + j]) continue;
    visited[i * w + j] = true;

    for (direction d = 0; d < NB_DIRS; d++) {
      uint i_next, j_next;
      if (game_get_ajacent_square(g, i, j, d, &i_next, &j_next)) {
        if (game_has_half_edge(g, i, j, d) && game_check_edge(g, i, j, d) == MATCH && !visited[i_next * w + j_next]) {
          stack[stack_top++] = i_next;
          stack[stack_top++] = j_next;
        }
      }
    }
  }

  for (uint i = 0; i < h; i++) {
    for (uint j = 0; j < w; j++) {
      if (game_get_piece_shape(g, i, j) != EMPTY && !visited[i * w + j]) {
        free(visited);
        free(stack);
        return false;
      }
    }
  }

  free(visited);
  free(stack);
  return true;
}