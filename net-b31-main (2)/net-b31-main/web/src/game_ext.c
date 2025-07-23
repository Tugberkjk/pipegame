/**
 * @file game_ext.c
 * @copyright University of Bordeaux. All rights reserved, 2024.
 **/
#include "game_ext.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_private.h"
#include "game_struct.h"
#include "queue.h"

/* ************************************************************************** */
/*                             GAME EXT FUNCTIONS                             */
/* ************************************************************************** */

/* ****************************** GAME NEW EXT ****************************** */
game game_new_ext(uint nb_rows, uint nb_cols, shape *shapes, direction *orientations, bool wrapping) {
  game g = game_new_empty_ext(nb_rows, nb_cols, wrapping);
  assert(g);

  if (shapes == NULL && orientations == NULL) return g;

  // Initialisation of tab_shape and tab_direction with the function's arguments
  for (uint i = 0; i < nb_rows; i++) {
    for (uint j = 0; j < nb_cols; j++) {
      if (shapes != NULL) {
        // Set game shapes
        assert(shapes[i * nb_cols + j] >= EMPTY && shapes[i * nb_cols + j] < NB_SHAPES);
        game_set_piece_shape(g, i, j, shapes[i * nb_cols + j]);
      }
      if (orientations != NULL) {
        // Set game orientations
        assert(orientations[i * nb_cols + j] >= NORTH && orientations[i * nb_cols + j] < NB_DIRS);
        game_set_piece_orientation(g, i, j, orientations[i * nb_cols + j]);
      }
    }
  }

  return g;
}

/* *************************** GAME NEW EMPTY EXT *************************** */
game game_new_empty_ext(uint nb_rows, uint nb_cols, bool wrapping) {
  game g = (game)malloc(sizeof(struct game_s));
  assert(g);

  uint size = nb_rows * nb_cols;
  g->HEIGHT = nb_rows;
  g->WIDTH = nb_cols;
  g->is_wrapping = wrapping;

  // Shapes and orientations
  g->tab_shape = (shape *)calloc(size, sizeof(shape));
  g->tab_direction = (direction *)calloc(size, sizeof(direction));
  // History
  g->undo_mooves = queue_new();
  g->redo_mooves = queue_new();

  assert(g->tab_shape && g->tab_direction && g->undo_mooves && g->redo_mooves);

  return g;
}

/* ****************************** GAME NB ROWS ****************************** */
uint game_nb_rows(cgame g) {
  assert(g);
  return g->HEIGHT;
}

/* ****************************** GAME NB COLS ****************************** */
uint game_nb_cols(cgame g) {
  assert(g);
  return g->WIDTH;
}

/* **************************** GAME IS WRAPPING **************************** */
bool game_is_wrapping(cgame g) {
  assert(g);
  return g->is_wrapping;
}

/* ******************************* GAME UNDO ******************************** */
void game_undo(game g) {
  assert(g);

  // If no history
  if (queue_is_empty(g->undo_mooves)) {
    printf("Nothing to undo.\n");
    return;
  }

  move m = _stack_pop_move(g->undo_mooves);
  game_set_piece_orientation(g, m.i, m.j, m.old);
  _stack_push_move(g->redo_mooves, m);
}

/* ******************************* GAME REDO ******************************** */
void game_redo(game g) {
  assert(g);

  // If no history
  if (queue_is_empty(g->redo_mooves)) {
    printf("Nothing to redo.\n");
    return;
  }

  move m = _stack_pop_move(g->redo_mooves);
  game_set_piece_orientation(g, m.i, m.j, m.new);
  _stack_push_move(g->undo_mooves, m);
}
