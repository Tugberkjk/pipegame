/**
 * @file game_private.c
 * @copyright University of Bordeaux. All rights reserved, 2024.
 **/

#include "game_private.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_ext.h"
#include "game_struct.h"
#include "queue.h"

/* ************************************************************************** */
/*                             STACK ROUTINES                                 */
/* ************************************************************************** */

/* **************************** STACK PUSH MOVE ***************************** */
void _stack_push_move(queue* q, move m) {
  assert(q);
  move* pm = malloc(sizeof(move));
  assert(pm);
  *pm = m;
  queue_push_head(q, pm);
}

/* ***************************** STACK POP MOVE ***************************** */
move _stack_pop_move(queue* q) {
  assert(q);
  move* pm = queue_pop_head(q);
  assert(pm);
  move m = *pm;
  free(pm);
  return m;
}

/* ***************************** STACK IS EMPTY ***************************** */
bool _stack_is_empty(queue* q) {
  assert(q);
  return queue_is_empty(q);
}

/* ****************************** STACK CLEAR ******************************* */
void _stack_clear(queue* q) {
  assert(q);
  queue_clear_full(q, free);
  assert(queue_is_empty(q));
}

/* ************************************************************************** */
/*                                  MISC                                      */
/* ************************************************************************** */
#define OPPOSITE_DIR(d) ((d + 2) % NB_DIRS)

char* square2str[NB_SHAPES][NB_DIRS] = {
    {" ", " ", " ", " "},  // empty
    {"^", ">", "v", "<"},  // endpoint
    {"|", "-", "|", "-"},  // segment
    {"└", "┌", "┐", "┘"},  // corner
    {"┴", "├", "┬", "┤"},  // tee
    {"+", "+", "+", "+"},  // cross
};

char* _square2str(shape s, direction d) {
  assert(s < NB_SHAPES);
  assert(d < NB_DIRS);
  return square2str[s][d];
}

/* ************************************************************************** */
/*                                 ADD_EDGE                                   */
/* ************************************************************************** */

/** @brief Hard-coding of pieces (shape & orientation) in an integer array.
 * @details The 4 least significant bits encode the presence of an half-edge in
 * the N-E-S-W directions (in that order). Thus, binary coding 1100 represents
 * the piece "└" (a corner in north orientation).
 */
static uint _code[NB_SHAPES][NB_DIRS] = {
    {0b0000, 0b0000, 0b0000, 0b0000},  // EMPTY {" ", " ", " ", " "}
    {0b1000, 0b0100, 0b0010, 0b0001},  // ENDPOINT {"^", ">", "v", "<"},
    {0b1010, 0b0101, 0b1010, 0b0101},  // SEGMENT {"|", "-", "|", "-"},
    {0b1100, 0b0110, 0b0011, 0b1001},  // CORNER {"└", "┌", "┐", "┘"}
    {0b1101, 0b1110, 0b0111, 0b1011},  // TEE {"┴", "├", "┬", "┤"}
    {0b1111, 0b1111, 0b1111, 0b1111}   // CROSS {"+", "+", "+", "+"}
};

/* ****************************** ENCODE SHAPE ****************************** */
uint _encode_shape(shape s, direction o) { return _code[s][o]; }

/* ****************************** DECODE SHAPE ****************************** */
bool _decode_shape(uint code, shape* s, direction* o) {
  assert(code >= 0 && code < 16);
  assert(s);
  assert(o);
  for (int i = 0; i < NB_SHAPES; i++)
    for (int j = 0; j < NB_DIRS; j++)
      if (code == _code[i][j]) {
        *s = i;
        *o = j;
        return true;
      }
  return false;
}

/* ***************************** ADD HALF EDGE ****************************** */
void _add_half_edge(game g, uint i, uint j, direction d) {
  assert(g);
  assert(i < game_nb_rows(g));
  assert(j < game_nb_cols(g));
  assert(d < NB_DIRS);

  shape s = game_get_piece_shape(g, i, j);
  direction o = game_get_piece_orientation(g, i, j);
  uint code = _encode_shape(s, o);
  uint mask = 0b1000 >> d;     // mask with half-edge in the direction d
  assert((code & mask) == 0);  // check there is no half-edge in the direction d
  uint newcode = code | mask;  // add the half-edge in the direction d
  shape news = EMPTY;
  direction newo = NORTH;
  if (!_decode_shape(newcode, &news, &newo)) {
    assert(false);
  }
  game_set_piece_shape(g, i, j, news);
  game_set_piece_orientation(g, i, j, newo);
}

/* ******************************** ADD EDGE ******************************** */
bool _add_edge(game g, uint i, uint j, direction d) {
  assert(g);
  assert(i < game_nb_rows(g));
  assert(j < game_nb_cols(g));
  assert(d < NB_DIRS);

  uint nexti, nextj;
  bool next = game_get_ajacent_square(g, i, j, d, &nexti, &nextj);
  if (!next) return false;

  // check if the two half-edges are free
  bool he = game_has_half_edge(g, i, j, d);
  if (he) return false;
  bool next_he = game_has_half_edge(g, nexti, nextj, OPPOSITE_DIR(d));
  if (next_he) return false;

  _add_half_edge(g, i, j, d);
  _add_half_edge(g, nexti, nextj, OPPOSITE_DIR(d));

  return true;
}