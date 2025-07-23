/**
 * @file game_tools.c
 * @copyright University of Bordeaux. All rights reserved, 2024.
 **/
#include "game_tools.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_private.h"

/* ************************************************************************** */
/*                          MAPPING SHAPE AND DIRECTION                       */
/* ************************************************************************** */
#define NORTH_B 0b0001
#define EAST_B 0b0010
#define SOUTH_B 0b0100
#define WEST_B 0b1000
uint8_t directions[] = {NORTH_B, EAST_B, SOUTH_B, WEST_B};

static shape shape_map[256] = {
    ['E'] = EMPTY, ['N'] = ENDPOINT, ['S'] = SEGMENT, ['C'] = CORNER, ['T'] = TEE, ['X'] = CROSS};

static direction direction_map[256] = {['N'] = NORTH, ['E'] = EAST, ['S'] = SOUTH, ['W'] = WEST};

static char shapeToChar[NB_SHAPES] = {'E', 'N', 'S', 'C', 'T', 'X'};

static char directionToChar[NB_DIRS] = {'N', 'E', 'S', 'W'};

/* ************************************************************************** */
/*                            GAME TOOLS FUNCTIONS                            */
/* ************************************************************************** */

/* ******************************* GAME LOAD ******************************** */
game game_load(char* filename) {
  assert(filename);

  FILE* f = fopen(filename, "r");
  assert(f);

  // Reading games parameters
  uint nb_rows = 0, nb_cols = 0, wrapping = 0;
  if (fscanf(f, "%u %u %u\n", &nb_rows, &nb_cols, &wrapping) != 3) assert(false);

  shape* shapes = malloc(nb_rows * nb_cols * sizeof(shape));
  direction* directions = malloc(nb_rows * nb_cols * sizeof(direction));
  assert(shapes && directions);

  // Reading games shapes and directions
  for (uint i = 0; i < nb_rows; i++) {
    for (uint j = 0; j < nb_cols; j++) {
      unsigned char shape_char = EMPTY, direction_char = NORTH;
      if (fscanf(f, " %c%c", &shape_char, &direction_char) != 2) assert(false);

      assert(shape_map[shape_char] || shape_char == 'E');
      assert(direction_map[direction_char] || direction_char == 'N');

      shapes[i * nb_cols + j] = shape_map[shape_char];
      directions[i * nb_cols + j] = direction_map[direction_char];
    }
  }

  game g = game_new_ext(nb_rows, nb_cols, shapes, directions, wrapping);

  fclose(f);
  free(shapes);
  free(directions);

  printf("> Game '%s' has been successfully loaded\n", filename);
  return g;
}

/* ******************************* GAME SAVE ******************************** */
void game_save(cgame g, char* filename) {
  assert(g);
  assert(filename);

  FILE* f = fopen(filename, "w");
  assert(f);

  // Writing the game in the "filename" file
  fprintf(f, "%u %u %u\n", game_nb_rows(g), game_nb_cols(g), game_is_wrapping(g));
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      shape s = game_get_piece_shape(g, i, j);
      direction d = game_get_piece_orientation(g, i, j);
      char shape_char = shapeToChar[s];
      char direction_char = directionToChar[d];
      fprintf(f, "%c%c ", shape_char, direction_char);
    }
    fprintf(f, "\n");
  }
  fclose(f);
  printf("> Game was successfully saved as '%s'\n", filename);
}

/* ****************************** GAME RANDOM ******************************* */
game game_random(uint nb_rows, uint nb_cols, bool wrapping, uint nb_empty, uint nb_extra) {
  uint size = nb_rows * nb_cols;
  assert(nb_cols * nb_rows >= 2);
  assert(nb_empty <= size - 2);

  game g = game_new_empty_ext(nb_rows, nb_cols, wrapping);
  if (nb_empty == size) return g;

  uint i, j, d;
  size -= nb_empty;

  // Add the first 2 pieces
  do {
    i = (((double)rand() * nb_rows) / RAND_MAX);
    j = (((double)rand() * nb_cols) / RAND_MAX);
    d = (((double)rand() * NB_DIRS) / RAND_MAX);
  } while (!_add_edge(g, i, j, d));

  // Add the rest
  for (int n = 2; n < size; n++) {
    uint i_next, j_next;
    do {
      do {
        i = (((double)rand() * nb_rows) / RAND_MAX);
        j = (((double)rand() * nb_cols) / RAND_MAX);
        d = (((double)rand() * NB_DIRS) / RAND_MAX);
      } while (game_get_piece_shape(g, i, j) == EMPTY || !game_get_ajacent_square(g, i, j, d, &i_next, &j_next));
    } while (game_get_piece_shape(g, i_next, j_next) != EMPTY);
    _add_edge(g, i, j, d);
  }

  // Add more connections
  for (int n = 0; n < nb_extra; n++) {
    uint i_next, j_next;
    do {
      do {
        i = (((double)rand() * nb_rows) / RAND_MAX);
        j = (((double)rand() * nb_cols) / RAND_MAX);
        d = (((double)rand() * NB_DIRS) / RAND_MAX);
      } while (!game_get_ajacent_square(g, i, j, d, &i_next, &j_next) || game_check_edge(g, i, j, d) == MATCH);
    } while (game_get_piece_shape(g, i, j) == EMPTY || game_get_piece_shape(g, i_next, j_next) == EMPTY);
    _add_edge(g, i, j, d);
  }

  return g;
}

/* ******************************* INIT ARRAY ******************************* */
void init_Array(game g, uint8_t* t_shape) {
  uint nb_rows = game_nb_rows(g);
  uint nb_cols = game_nb_cols(g);

  for (uint i = 0; i < nb_rows; i++) {
    for (uint j = 0; j < nb_cols; j++) {
      shape s = game_get_piece_shape(g, i, j);
      t_shape[i * nb_cols + j] = 0;

      switch (s) {
        case EMPTY:
        case CROSS:
          t_shape[i * nb_cols + j] |= NORTH_B;
          break;
        case SEGMENT:
          t_shape[i * nb_cols + j] |= (NORTH_B | EAST_B);
          break;
        default:
          t_shape[i * nb_cols + j] |= (WEST_B | EAST_B | NORTH_B | SOUTH_B);
          break;
      }
    }
  }
}

/* ****************************** IS MISMATCH ******************************* */
bool isMismatch(game g, uint i, uint j, direction d, uint8_t* t_shape) {
  // Optimisation function, if current game checked is not winnable, no need to continue
  uint nb_cols = game_nb_cols(g);
  uint nb_rows = game_nb_rows(g);
  bool isEndpoint = (game_get_piece_shape(g, i, j) == ENDPOINT);

  bool wrapping = game_is_wrapping(g);
  if (!wrapping) {
    // If a piece is Mismatch with the border, no need to ever test it ever again
    if (i == 0 && game_check_edge(g, i, j, NORTH) == MISMATCH) {
      t_shape[i * nb_cols + j] &= ~directions[d];
      return true;
    }
    if (j == 0 && game_check_edge(g, i, j, WEST) == MISMATCH) {
      t_shape[i * nb_cols + j] &= ~directions[d];
      return true;
    }
    if (i == nb_rows - 1 && game_check_edge(g, i, j, SOUTH) == MISMATCH) {
      t_shape[i * nb_cols + j] &= ~directions[d];
      return true;
    }
    if (j == nb_cols - 1 && game_check_edge(g, i, j, EAST) == MISMATCH) {
      t_shape[i * nb_cols + j] &= ~directions[d];
      return true;
    }
  }

  // If game is not winning so far, don't need to continue
  if (i >= 1 && game_check_edge(g, i, j, NORTH) == MISMATCH) {
    shape s = game_get_piece_shape(g, i - 1, j);
    if (s == EMPTY || s == CROSS) t_shape[i * nb_cols + j] &= ~directions[d];
    return true;
  }
  if (j >= 1 && game_check_edge(g, i, j, WEST) == MISMATCH) {
    shape s = game_get_piece_shape(g, i, j - 1);
    if (s == EMPTY || s == CROSS) t_shape[i * nb_cols + j] &= ~directions[d];
    return true;
  }

  if (game_is_wrapping(g)) {
    if (i == nb_rows - 1 && game_check_edge(g, i, j, SOUTH) == MISMATCH) {
      shape s = game_get_piece_shape(g, 0, j);
      if (s == EMPTY || s == CROSS) t_shape[i * nb_cols + j] &= ~directions[d];
      return true;
    }
    if (j == nb_cols - 1 && game_check_edge(g, i, j, EAST) == MISMATCH) {
      shape s = game_get_piece_shape(g, i, 0);
      if (s == EMPTY || s == CROSS) t_shape[i * nb_cols + j] &= ~directions[d];
      return true;
    }
  }

  // If 2 two endpoints are looking each others can't be winnable
  if (isEndpoint && nb_cols * nb_rows > 2 && game_check_edge(g, i, j, d) == MATCH) {
    uint i_next = 0, j_next = 0;
    game_get_ajacent_square(g, i, j, d, &i_next, &j_next);
    if (game_get_piece_shape(g, i_next, j_next) == ENDPOINT) {
      uint nb_piece = 0;
      for (uint index = 0; index < nb_cols * nb_rows; index++) {
        if (game_get_piece_shape(g, i, j) != EMPTY) nb_piece++;
        if (nb_piece == 3) {
          t_shape[i * nb_cols + j] &= ~directions[d];
          return true;
        }
      }
    }
  }

  return false;
}

/* ******************************* SOLVE REC ******************************** */
bool solve_rec(game g, uint pos, uint size, uint* count, uint8_t* t_shape) {
  // Functions used to brut-force solving games
  // If count is NULL then return first game won
  // Else compute the numbers of solutions in count
  if (pos == size) {
    bool won = game_won(g);
    if (count && won) (*count)++;
    return won;
  }

  uint nb_cols = game_nb_cols(g);
  uint j = pos % game_nb_cols(g);
  uint i = pos / game_nb_cols(g);

  for (int d = 0; d < 4; d++) {
    if (t_shape[i * nb_cols + j] & directions[d]) {
      game_set_piece_orientation(g, i, j, d);

      if (!isMismatch(g, i, j, d, t_shape)) {
        if (solve_rec(g, pos + 1, size, count, t_shape) && !count) return true;
      }
    }
  }

  return false;
}

/* *************************** GAME NB SOLUTIONS **************************** */
uint game_nb_solutions(cgame g) {
  assert(g);
  game g_copy = game_copy(g);
  uint nb_sols = 0;

  uint8_t* t_shape = (uint8_t*)malloc(game_nb_cols(g) * game_nb_rows(g) * sizeof(uint8_t));
  init_Array(g_copy, t_shape);

  solve_rec(g_copy, 0, game_nb_cols(g) * game_nb_rows(g), &nb_sols, t_shape);
  game_delete(g_copy);
  return nb_sols;
}

/* ******************************* GAME SOLVE ******************************* */
bool game_solve(game g) {
  assert(g);
  if (game_won(g)) {
    return true;
  }
  game g_copy = game_copy(g);

  uint8_t* t_shape = (uint8_t*)malloc(game_nb_cols(g) * game_nb_rows(g) * sizeof(uint8_t));
  init_Array(g_copy, t_shape);

  if (solve_rec(g_copy, 0, game_nb_cols(g) * game_nb_rows(g), NULL, t_shape)) {
    for (uint i = 0; i < game_nb_rows(g); i++) {
      for (uint j = 0; j < game_nb_cols(g); j++) {
        // Copy the orientation and shape of piece (i,j) in game g to piece (i,j) in game new_game
        game_set_piece_orientation(g, i, j, game_get_piece_orientation(g_copy, i, j));
        game_set_piece_shape(g, i, j, game_get_piece_shape(g_copy, i, j));
      }
    }
  }

  game_delete(g_copy);
  return game_won(g);
}