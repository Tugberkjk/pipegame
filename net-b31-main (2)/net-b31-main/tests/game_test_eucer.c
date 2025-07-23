#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"

bool test_game_new_empty(void) {
  // Create an empty game and check if it is correctly created
  game g = game_new_empty();
  if (g == NULL) return false;

  // Test that the game g has the rigth values
  uint h = game_nb_rows(g);
  uint w = game_nb_cols(g);
  if (h != DEFAULT_SIZE || w != DEFAULT_SIZE) return false;
  if (game_is_wrapping(g)) return false;

  // Test that shape and orientation is correct in each piece
  for (uint i = 0; i < h; i++) {
    for (uint j = 0; j < w; j++) {
      if (game_get_piece_orientation(g, i, j) != NORTH) return false;
      if (game_get_piece_shape(g, i, j) != EMPTY) return false;
    }
  }

  // Check if both queue of the game struct are created
  game_undo(g);
  game_redo(g);

  game_delete(g);
  return true;
}

bool test_game_new(shape *shapes, direction *orientations) {
  // Create the game with shapes and orientations and check if it is correctly created
  game g = game_new(shapes, orientations);
  if (g == NULL) return false;

  // Test that the game g has the rigth values
  uint h = game_nb_rows(g);
  uint w = game_nb_cols(g);
  if (h != DEFAULT_SIZE || w != DEFAULT_SIZE) return false;
  if (game_is_wrapping(g)) return false;

  // Test that shape and orientation is correct in each piece
  for (uint i = 0; i < h; i++) {
    for (uint j = 0; j < w; j++) {
      if (game_get_piece_orientation(g, i, j) != orientations[w * i + j]) return false;
      if (game_get_piece_shape(g, i, j) != shapes[w * i + j]) return false;
    }
  }

  // Check if both queue of the game struct are created
  game_undo(g);
  game_redo(g);

  game_delete(g);
  return true;
}

bool test_game_copy(shape *shapes, direction *orientations) {
  // Create a game, make a copy of if and test if it succeeded
  game g = game_new_ext(7, 6, shapes, orientations, false);
  game g_copy = game_copy(g);
  if (g_copy == NULL) return false;

  // Test that the game g_copy has the same dimension and wrapping status than g
  uint h = game_nb_rows(g_copy);
  uint w = game_nb_cols(g_copy);
  if (h != game_nb_rows(g) || w != game_nb_cols(g)) return false;
  if (game_is_wrapping(g_copy) != game_is_wrapping(g)) return false;

  // Test that shape and orientation is correct in each piece of the copy
  for (uint i = 0; i < h; i++) {
    for (uint j = 0; j < w; j++) {
      if (game_get_piece_orientation(g_copy, i, j) != game_get_piece_orientation(g, i, j)) return false;
      if (game_get_piece_shape(g_copy, i, j) != game_get_piece_shape(g, i, j)) return false;
    }
  }

  // Test copy wrapping
  game gw = game_new_empty_ext(5, 5, true);
  game gw_copy = game_copy(gw);
  if (game_is_wrapping(gw_copy) != game_is_wrapping(gw)) return false;

  game_delete(g);
  game_delete(g_copy);
  game_delete(gw);
  game_delete(gw_copy);
  return true;
}

bool test_game_equal(shape *shapes, direction *orientations) {
  // Create a game and make a copy of if
  game g1 = game_new_ext(7, 6, shapes, orientations, false);
  game g2 = game_copy(g1);
  // Create a game with different size
  direction new_orientations[7] = {NORTH, EAST, WEST, NORTH, SOUTH, WEST, WEST};
  game g3 = game_new_ext(7, 1, shapes, new_orientations, false);
  // Create a game with different wrapping statue
  game g4 = game_new_ext(7, 6, shapes, orientations, true);
  // Create a game with different pieces orientations
  for (int i = 0; i < 7; i++) orientations[i] = new_orientations[i];
  game g5 = game_new_ext(7, 6, shapes, orientations, false);

  // Test if game are equal when they should and different when they should not
  if (!game_equal(g1, g2, true)) return false;
  if (game_equal(g1, g3, true)) return false;
  if (game_equal(g1, g4, true)) return false;
  if (!game_equal(g1, g5, true)) return false;
  if (game_equal(g1, g5, false)) return false;

  game_delete(g1);
  game_delete(g2);
  game_delete(g3);
  game_delete(g4);
  game_delete(g5);
  return true;
}

bool test_game_delete(void) {
  // Create a game and test if the delete function don't provoke error
  game g = game_new_empty();
  game_delete(g);

  return true;
}

bool test_game_set_piece_shape(shape *shapes, direction *orientations) {
  // Create a (7*6) game with shapes and orientations
  uint h = 7;
  uint w = 6;
  game g = game_new_ext(h, w, shapes, orientations, false);

  // For each piece, ...
  for (uint i = 0; i < h; i++) {
    for (uint j = 0; j < w; j++) {
      // change the shape of the piece (i,j) with set function, ...
      shape s = game_get_piece_shape(g, i, j);
      s = (s + 2) % NB_SHAPES;
      game_set_piece_shape(g, i, j, s);
      // and test if it succeeded
      if (game_get_piece_shape(g, i, j) != s) return false;
    }
  }

  game_delete(g);
  return true;
}

bool test_game_new_empty_ext() {
  // Create 2 empty games with different sizes and wrapping statue and check if it succeeded
  game g1 = game_new_empty_ext(5, 6, true);
  game g2 = game_new_empty_ext(7, 4, false);
  if (g1 == NULL || g2 == NULL) return false;

  // Test that games sizes are correct
  uint h1 = game_nb_rows(g1);
  uint w1 = game_nb_cols(g1);
  uint h2 = game_nb_rows(g2);
  uint w2 = game_nb_cols(g2);
  if (h1 != 5 || w1 != 6 || h2 != 7 || w2 != 4) return false;

  // Test the wrappings statues for both games are correct
  if (!game_is_wrapping(g1) || game_is_wrapping(g2)) return false;

  // Test for each piece (i,j) of g1 that orientation and shape is correct
  for (uint i = 0; i < h1; i++) {
    for (uint j = 0; j < w1; j++) {
      if (game_get_piece_orientation(g1, i, j) != NORTH || game_get_piece_shape(g1, i, j) != EMPTY) return false;
    }
  }

  // Test for each piece (i,j) of g2 that orientation and shape is correct
  for (uint i = 0; i < h2; i++) {
    for (uint j = 0; j < w2; j++) {
      if (game_get_piece_orientation(g2, i, j) != NORTH || game_get_piece_shape(g2, i, j) != EMPTY) return false;
    }
  }

  // Check if both queue of both game struct are created
  game_undo(g1);
  game_redo(g1);
  game_undo(g2);
  game_redo(g2);

  game_delete(g1);
  game_delete(g2);
  return true;
}

bool test_game_nb_rows() {
  // Create 2 empty games with different sizes and wrapping statue
  game g1 = game_new_empty_ext(5, 6, true);
  game g2 = game_new_empty_ext(7, 4, false);

  // Test that the function game_nb_rows return the correct values
  uint h1 = game_nb_rows(g1);
  uint h2 = game_nb_rows(g2);
  if (h1 != 5 || h2 != 7) return false;

  game_delete(g1);
  game_delete(g2);
  return true;
}

bool test_game_undo(shape *shapes, direction *orientations) {
  // Create a (7*6) game with shapes and orientations
  game g = game_new_ext(7, 6, shapes, orientations, false);

  // Play 3 mooves on the game but make a copy of it before
  game g1 = game_copy(g);
  game_play_move(g, 0, 0, 1);
  game g2 = game_copy(g);
  game_play_move(g, 3, 4, 1);
  game g3 = game_copy(g);
  game_play_move(g, 5, 5, -1);

  // Make sure last moove is played
  if (game_equal(g, g3, false)) return false;

  // Call game_undo function and test if g equal g3 (state before last played moove)
  game_undo(g);
  if (!game_equal(g, g3, false)) return false;
  // Call game_undo function and test if g equal g2 (state before two last played moove)
  game_undo(g);
  if (!game_equal(g, g2, false)) return false;
  // Call game_undo function and test if g equal g1 (state before three last played moove)
  game_undo(g);
  if (!game_equal(g, g1, false)) return false;
  // Call game_undo function and test if g is still equal to g1 (nothing to undo)
  game_undo(g);
  if (!game_equal(g, g1, false)) return false;

  game_delete(g);
  game_delete(g1);
  game_delete(g2);
  game_delete(g3);
  return true;
}
bool test_game_load() {
  // Load the game from default.txt by using game_load
  game g = game_load("default.txt");
  if (g == NULL) {
    game_delete(g);
    return false;
  }

  // Check game dimensions and wrapping option
  if (game_nb_rows(g) != 5 || game_nb_cols(g) != 5 || game_is_wrapping(g)) {
    game_delete(g);
    return false;
  }

  // Check specific pieces
  if (game_get_piece_shape(g, 0, 0) != CORNER || game_get_piece_orientation(g, 0, 0) != WEST) {
    game_delete(g);
    return false;
  }
  if (game_get_piece_shape(g, 1, 1) != TEE || game_get_piece_orientation(g, 1, 1) != WEST) {
    game_delete(g);
    return false;
  }
  // Clean up
  game_delete(g);
  return true;
}

/* ********** USAGE ********** */
void usage(int argc, char *argv[]) {
  fprintf(stderr, "Usage: %s <testname> [<...>]\n", argv[0]);
  exit(EXIT_FAILURE);
}

/* ********** MAIN ROUTINE ********** */
int main(int argc, char *argv[]) {
  if (argc == 1) usage(argc, argv);

  // Shapes and orientations of a (7*6) game. All pieces are connected
  shape any_shape[7 * 6] = {ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, CORNER,   TEE,     TEE,    TEE,
                            SEGMENT,  ENDPOINT, TEE,      ENDPOINT, ENDPOINT, TEE,      TEE,     CORNER, TEE,
                            ENDPOINT, CORNER,   ENDPOINT, TEE,      TEE,      ENDPOINT, TEE,     TEE,    TEE,
                            TEE,      TEE,      ENDPOINT, SEGMENT,  CORNER,   TEE,      SEGMENT, TEE,    ENDPOINT,
                            ENDPOINT, ENDPOINT, ENDPOINT, ENDPOINT, CORNER,   ENDPOINT};
  direction any_orientation_solution[7 * 6] = {
      SOUTH, SOUTH, SOUTH, SOUTH, EAST,  SOUTH, EAST, NORTH, WEST,  NORTH, EAST,  WEST,  NORTH, EAST,
      NORTH, WEST,  EAST,  WEST,  SOUTH, EAST,  WEST, EAST,  WEST,  NORTH, EAST,  NORTH, SOUTH, WEST,
      EAST,  WEST,  NORTH, EAST,  WEST,  NORTH, EAST, WEST,  NORTH, NORTH, NORTH, NORTH, NORTH, WEST};

  bool ok = false;

  if (strcmp("game_new_empty", argv[1]) == 0)
    ok = test_game_new_empty();
  else if (strcmp("game_new", argv[1]) == 0)
    ok = test_game_new(any_shape, any_orientation_solution);
  else if (strcmp("game_copy", argv[1]) == 0)
    ok = test_game_copy(any_shape, any_orientation_solution);
  else if (strcmp("game_equal", argv[1]) == 0)
    ok = test_game_equal(any_shape, any_orientation_solution);
  else if (strcmp("game_delete", argv[1]) == 0)
    ok = test_game_delete();
  else if (strcmp("game_set_piece_shape", argv[1]) == 0)
    ok = test_game_set_piece_shape(any_shape, any_orientation_solution);
  else if (strcmp("game_new_empty_ext", argv[1]) == 0)
    ok = test_game_new_empty_ext();
  else if (strcmp("game_nb_rows", argv[1]) == 0)
    ok = test_game_nb_rows();
  else if (strcmp("game_undo", argv[1]) == 0)
    ok = test_game_undo(any_shape, any_orientation_solution);
  else if (strcmp("game_load", argv[1]) == 0)
    ok = test_game_load();
  else {
    fprintf(stderr, "Error: test \"%s\" not found!\n", argv[1]);
    return EXIT_FAILURE;
  }

  if (ok) {
    fprintf(stderr, "Test \"%s\" finished: SUCCESS\n", argv[1]);
    return EXIT_SUCCESS;
  } else {
    fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[1]);
    return EXIT_FAILURE;
  }
}