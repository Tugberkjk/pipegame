/**
 * @file game.c
 * @copyright University of Bordeaux. All rights reserved, 2024.
 **/
#include "game.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game_aux.h"
#include "game_ext.h"
#include "game_private.h"
#include "game_struct.h"

/* ************************************************************************** */
/*                               GAME FUNCTIONS                               */
/* ************************************************************************** */

/* ***************************** GAME NEW EMPTY ***************************** */
game game_new_empty(void) { return game_new_empty_ext(DEFAULT_SIZE, DEFAULT_SIZE, false); }

/* ******************************** GAME NEW ******************************** */
game game_new(shape *shapes, direction *orientations) {
  return game_new_ext(DEFAULT_SIZE, DEFAULT_SIZE, shapes, orientations, false);
}

/* ******************************* GAME COPY ******************************** */
game game_copy(cgame g) {
  assert(g);

  game game_c = game_new_empty_ext(game_nb_rows(g), game_nb_cols(g), game_is_wrapping(g));

  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      // Copy the orientation and shape of piece (i,j) in game g to piece (i,j) in game new_game
      game_set_piece_orientation(game_c, i, j, game_get_piece_orientation(g, i, j));
      game_set_piece_shape(game_c, i, j, game_get_piece_shape(g, i, j));
    }
  }
  return game_c;
}

/* ******************************* GAME EQUAL ******************************* */
bool game_equal(cgame g1, cgame g2, bool ignore_orientation) {
  assert(g1 && g2);

  if (game_nb_cols(g1) != game_nb_cols(g2) || game_nb_rows(g1) != game_nb_rows(g2)) return false;
  if (game_is_wrapping(g1) != game_is_wrapping(g2)) return false;

  for (uint i = 0; i < game_nb_rows(g1); i++) {
    for (uint j = 0; j < game_nb_cols(g1); j++) {
      if (!ignore_orientation)  // Only if we want to check orientation
        if (game_get_piece_orientation(g1, i, j) != game_get_piece_orientation(g2, i, j)) return false;
      if (game_get_piece_shape(g1, i, j) != game_get_piece_shape(g2, i, j)) return false;
    }
  }

  return true;
}

/* ****************************** GAME DELETE ******************************* */
void game_delete(game g) {
  if (g != NULL) {
    // If memory is allocated, we free
    if (g->tab_shape != NULL) free(g->tab_shape);
    if (g->tab_direction != NULL) free(g->tab_direction);
    if (g->undo_mooves != NULL) queue_free_full(g->undo_mooves, free);
    if (g->redo_mooves != NULL) queue_free_full(g->redo_mooves, free);

    free(g);
  }
}

/* ************************** GAME SET PIECE SHAPE ************************** */
void game_set_piece_shape(game g, uint i, uint j, shape s) {
  assert(g && g->tab_shape);
  assert(i < game_nb_rows(g) && j < game_nb_cols(g));
  assert(s >= 0 && s < NB_SHAPES);

  g->tab_shape[game_nb_cols(g) * i + j] = s;
}

/* *********************** GAME SET PIECE ORIENTATION *********************** */
void game_set_piece_orientation(game g, uint i, uint j, direction o) {
  assert(g && g->tab_direction);
  assert(i < game_nb_rows(g) && j < game_nb_cols(g));
  assert(o >= 0 && o < NB_DIRS);

  g->tab_direction[game_nb_cols(g) * i + j] = o;
}

/* ************************** GAME GET PIECE SHAPE ************************** */
shape game_get_piece_shape(cgame g, uint i, uint j) {
  assert(g && g->tab_shape);
  assert(i < game_nb_rows(g) && j < game_nb_cols(g));

  return g->tab_shape[game_nb_cols(g) * i + j];
}

/* *********************** GAME GET PIECE ORIENTATION *********************** */
direction game_get_piece_orientation(cgame g, uint i, uint j) {
  assert(g && g->tab_direction);
  assert(i < game_nb_rows(g) && j < game_nb_cols(g));

  return g->tab_direction[game_nb_cols(g) * i + j];
}

/* ***************************** GAME PLAY MOVE ***************************** */
void game_play_move(game g, uint i, uint j, int nb_quarter_turns) {
  assert(g);
  assert(i < game_nb_rows(g) && j < game_nb_cols(g));

  direction old = game_get_piece_orientation(g, i, j);
  direction new = (old + nb_quarter_turns + NB_DIRS) % NB_DIRS;
  game_set_piece_orientation(g, i, j, new);

  // save history
  _stack_clear(g->redo_mooves);
  move m = {i, j, old, new};
  _stack_push_move(g->undo_mooves, m);
}

/* ******************************** GAME WON ******************************** */
bool game_won(cgame g) {
  assert(g);
  return game_is_connected(g) && game_is_well_paired(g);
}

/* ************************* GAME RESET ORIENTATION ************************* */
void game_reset_orientation(game g) {
  assert(g);

  // Set all piece to NORTH
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      game_set_piece_orientation(g, i, j, NORTH);
    }
  }

  // reset history
  _stack_clear(g->undo_mooves);
  _stack_clear(g->redo_mooves);
}

/* ************************ GAME SHUFFLE ORIENTATION ************************ */
void game_shuffle_orientation(game g) {
  assert(g);

  // Set all piece to random direction
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      direction o = rand() % NB_DIRS;
      game_set_piece_orientation(g, i, j, o);
    }
  }

  // reset history
  _stack_clear(g->undo_mooves);
  _stack_clear(g->redo_mooves);
}
