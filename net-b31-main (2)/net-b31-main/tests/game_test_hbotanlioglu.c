#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"

#define TEST_SAVE_FILE "test_game"  // Save file name inside ".saves/"

bool test_game_save(void) {
  uint rows = 7, cols = 6;  // Creatın of the size of the game
  shape shapes[7 * 6] = {ENDPOINT, ENDPOINT, CORNER, ENDPOINT, SEGMENT, TEE,      CORNER, CORNER, ENDPOINT,
                         ENDPOINT, ENDPOINT, TEE,    CORNER,   CORNER,  ENDPOINT, CORNER, TEE,    CORNER,
                         ENDPOINT, ENDPOINT, TEE,    TEE,      TEE,     ENDPOINT, CORNER, TEE,    SEGMENT,
                         TEE,      ENDPOINT, CORNER, ENDPOINT, TEE,     ENDPOINT, TEE,    TEE,    CORNER,
                         ENDPOINT, CORNER,   CORNER, TEE,      TEE,     ENDPOINT};
  direction orientations[7 * 6] = {WEST,  EAST,  WEST,  NORTH, NORTH, EAST,  SOUTH, EAST,  WEST, SOUTH, NORTH,
                                   EAST,  NORTH, WEST,  SOUTH, NORTH, SOUTH, WEST,  SOUTH, EAST, NORTH, SOUTH,
                                   NORTH, WEST,  NORTH, SOUTH, EAST,  WEST,  EAST,  SOUTH, EAST, WEST,  EAST,
                                   NORTH, SOUTH, WEST,  EAST,  WEST,  EAST,  SOUTH, WEST,  SOUTH};
  game g = game_new_ext(rows, cols, shapes, orientations, false);  // Creation of the game
  game_save(g, TEST_SAVE_FILE);                                    // Using the save function
  // char filepath[256];                                              // Creating the file path
  // snprintf(filepath, sizeof(filepath), "./saves/%s", TEST_SAVE_FILE);
  game savedG = game_load(TEST_SAVE_FILE);  // Opening the saved file in the file path
  if (savedG == NULL) return false;
  if (!game_equal(g, savedG, false)) {
    return false;
  }
  game_delete(savedG);
  game_delete(g);
  return true;
}

bool test_game_set_piece_orientation(shape *shapes, direction *orientations) {
  // Create a (7*6) game with shapes and orientations
  uint h = 7;
  uint w = 6;
  game g = game_new_ext(h, w, shapes, orientations, false);

  // For each piece, ...
  for (uint i = 0; i < h; i++) {
    for (uint j = 0; j < w; j++) {
      // change the orientation of the piece (i,j) with set function, ...
      direction d = game_get_piece_orientation(g, i, j);
      d = (d + 2) % NB_DIRS;
      game_set_piece_orientation(g, i, j, d);
      // and test if it succeeded
      if (game_get_piece_orientation(g, i, j) != d) return false;
    }
  }

  game_delete(g);
  return true;
}

bool test_game_get_piece_shape(shape *shapes, direction *orientations) {
  // Create a (7*6) game with shapes and orientations
  uint h = 7;
  uint w = 6;
  game g = game_new_ext(h, w, shapes, orientations, false);

  // For each piece, ...
  for (uint i = 0; i < h; i++) {
    for (uint j = 0; j < w; j++) {
      shape s = game_get_piece_shape(g, i, j);
      s = (s + 2) % NB_SHAPES;
      game_set_piece_shape(g, i, j, s);
      // get the shape of the piece (i,j) and test if it is correct
      if (game_get_piece_shape(g, i, j) != s) return false;
    }
  }

  game_delete(g);
  return true;
}

bool test_game_get_piece_orientation(shape *shapes, direction *orientations) {
  // Create a (7*6) game with shapes and orientations
  uint h = 7;
  uint w = 6;
  game g = game_new_ext(h, w, shapes, orientations, false);

  // For each piece, ...
  for (uint i = 0; i < h; i++) {
    for (uint j = 0; j < w; j++) {
      direction d = game_get_piece_orientation(g, i, j);
      d = (d + 2) % NB_DIRS;
      game_set_piece_orientation(g, i, j, d);
      // get the orientation of the piece (i,j) and test if it is correct
      if (game_get_piece_orientation(g, i, j) != d) return false;
    }
  }

  game_delete(g);
  return true;
}

bool test_game_play_move(shape *shapes, direction *orientations) {
  // Create a (7*6) game with shapes and orientations
  uint h = 7;
  uint w = 6;
  game g = game_new_ext(h, w, shapes, orientations, false);

  // For each piece, ...
  for (uint i = 0; i < h; i++) {
    for (uint j = 0; j < w; j++) {
      // test if game_play_move change the orientation correctly
      direction d = game_get_piece_orientation(g, i, j);
      int r = ((i + j) % 2 == 0) ? 1 : -1;
      game_play_move(g, i, j, r);
      if (game_get_piece_orientation(g, i, j) != (d + r) % NB_DIRS) return false;
    }
  }

  game_delete(g);
  return true;
}

bool test_game_won(shape *shapes, direction *orientations) {
  // Create 2 default games (one solved) and 2 (7*6) games with shapes and orientations
  game g1 = game_default();
  game g2 = game_default_solution();
  game g3 = game_new_ext(7, 6, shapes, orientations, false);
  game g4 = game_new_ext(7, 6, shapes, orientations, true);

  // g1 is not won but g2 is
  if (game_won(g1)) return false;
  if (!game_won(g2)) return false;
  // With wrapping on, g4 is won but not g3
  if (game_won(g3)) return false;
  if (!game_won(g4)) return false;

  game_delete(g1);
  game_delete(g2);
  game_delete(g3);
  game_delete(g4);
  return true;
}

bool test_game_reset_orientation(shape *shapes, direction *orientations) {
  // Create a (7*6) game with shapes and orientations and a default game
  uint h = 7;
  uint w = 6;
  game g1 = game_new_ext(h, w, shapes, orientations, false);
  game g2 = game_default();

  // Should set all orientations to NORTH
  game_reset_orientation(g1);
  game_reset_orientation(g2);

  // Test for each piece (i,j) of g1 that orientation is set to NORTH
  for (uint i = 0; i < h; i++) {
    for (uint j = 0; j < w; j++) {
      if (game_get_piece_orientation(g1, i, j) != NORTH) return false;
    }
  }

  // Test for each piece (i,j) of g2 that orientation is set to NORTH
  for (uint i = 0; i < DEFAULT_SIZE; i++) {
    for (uint j = 0; j < DEFAULT_SIZE; j++) {
      if (game_get_piece_orientation(g2, i, j) != NORTH) return false;
    }
  }

  game_delete(g1);
  game_delete(g2);
  return true;
}

bool test_game_shuffle_orientation(shape *shapes, direction *orientations) {
  // Create a (7*6) game with shapes and orientations and a default game
  game g1 = game_new_ext(7, 6, shapes, orientations, false);
  game g2 = game_default();

  // Reset orientation of both game
  game_shuffle_orientation(g1);
  game_shuffle_orientation(g2);

  game_delete(g1);
  game_delete(g2);
  return true;
}

bool test_game_nb_cols(void) {
  // Create 2 empty games with different sizes and wrapping statue
  game g1 = game_new_empty_ext(5, 6, true);
  game g2 = game_new_empty_ext(7, 4, false);

  // Test that the function game_nb_cols return the correct values
  uint w1 = game_nb_cols(g1);
  uint w2 = game_nb_cols(g2);
  if (w1 != 6 || w2 != 4) return false;

  game_delete(g1);
  game_delete(g2);
  return true;
}

bool test_game_redo(shape *shapes, direction *orientations) {
  // Create a (7*6) game with shapes and orientations
  game g = game_new_ext(7, 6, shapes, orientations, false);

  // Play 3 mooves on the game and make a copy of it
  game_play_move(g, 0, 0, 1);
  game g1 = game_copy(g);
  game_play_move(g, 3, 4, 1);
  game g2 = game_copy(g);
  game_play_move(g, 5, 5, -1);
  game g3 = game_copy(g);

  // Undo the 3 previous mooves played
  for (int i = 0; i < 3; i++) game_undo(g);

  // Call game_redo function and test if g equal g1 (state before last undone moove)
  game_redo(g);
  if (!game_equal(g, g1, false)) return false;
  // Call game_redo function and test if g equal g2 (state before two last undone moove)
  game_redo(g);
  if (!game_equal(g, g2, false)) return false;
  // Call game_redo function and test if g equal g3 (state before three last undone moove)
  game_redo(g);
  if (!game_equal(g, g3, false)) return false;
  // Call game_redo function and test if g is still equal to g1 (nothing to redo)
  game_redo(g);
  if (!game_equal(g, g3, false)) return false;

  game_delete(g);
  game_delete(g1);
  game_delete(g2);
  game_delete(g3);
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

  // Shapes and orientations of a (7*6) game with wrapping. All pieces are connected
  shape any_shape[7 * 6] = {ENDPOINT, ENDPOINT, CORNER, ENDPOINT, SEGMENT, TEE,      CORNER, CORNER, ENDPOINT,
                            ENDPOINT, ENDPOINT, TEE,    CORNER,   CORNER,  ENDPOINT, CORNER, TEE,    CORNER,
                            ENDPOINT, ENDPOINT, TEE,    TEE,      TEE,     ENDPOINT, CORNER, TEE,    SEGMENT,
                            TEE,      ENDPOINT, CORNER, ENDPOINT, TEE,     ENDPOINT, TEE,    TEE,    CORNER,
                            ENDPOINT, CORNER,   CORNER, TEE,      TEE,     ENDPOINT};
  direction any_orientation_solution[7 * 6] = {
      WEST,  EAST,  WEST,  NORTH, NORTH, EAST,  SOUTH, EAST,  WEST,  SOUTH, NORTH, EAST,  NORTH, WEST,
      SOUTH, NORTH, SOUTH, WEST,  SOUTH, EAST,  NORTH, SOUTH, NORTH, WEST,  NORTH, SOUTH, EAST,  WEST,
      EAST,  SOUTH, EAST,  WEST,  EAST,  NORTH, SOUTH, WEST,  EAST,  WEST,  EAST,  SOUTH, WEST,  SOUTH};

  bool ok = false;

  if (strcmp(argv[1], "game_set_piece_orientation") == 0) {
    ok = test_game_set_piece_orientation(any_shape, any_orientation_solution);
  } else if (strcmp(argv[1], "game_get_piece_shape") == 0) {
    ok = test_game_get_piece_shape(any_shape, any_orientation_solution);
  } else if (strcmp(argv[1], "game_get_piece_orientation") == 0) {
    ok = test_game_get_piece_orientation(any_shape, any_orientation_solution);
  } else if (strcmp(argv[1], "game_play_move") == 0) {
    ok = test_game_play_move(any_shape, any_orientation_solution);
  } else if (strcmp(argv[1], "game_won") == 0) {
    ok = test_game_won(any_shape, any_orientation_solution);
  } else if (strcmp(argv[1], "game_reset_orientation") == 0) {
    ok = test_game_reset_orientation(any_shape, any_orientation_solution);
  } else if (strcmp(argv[1], "game_shuffle_orientation") == 0) {
    ok = test_game_shuffle_orientation(any_shape, any_orientation_solution);
  } else if (strcmp(argv[1], "game_nb_cols") == 0) {
    ok = test_game_nb_cols();
  } else if (strcmp(argv[1], "game_redo") == 0) {
    ok = test_game_redo(any_shape, any_orientation_solution);
  } else if (strcmp(argv[1], "game_save") == 0) {
    ok = test_game_save();
  } else {
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