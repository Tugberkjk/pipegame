/**
 * @file game_test_ddausse.c
 * @brief Unit tests for the game functions.
 * @details Contains test for the following functions
 *
 * @fn game_print
 * @fn game_default
 * @fn game_default_solution
 * @fn game_get_ajacent_square
 * @fn game_has_half_edge
 * @fn game_check_edge
 * @fn game_is_well_paired
 * @fn game_is_connected
 * @fn game_is_wrapping
 * @fn game_new_ext
 *
 * @copyright University of Bordeaux. All rights reserved, 2024.
 **/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"

/* ************************************************************************** */
/*                                MACRO                                       */
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

/* ************************************************************************** */
/*                            Static Variables                                */
/* ************************************************************************** */

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

/* ****************************** ANY 7×6 GAME ****************************** */
static shape any_s[7 * 6] = {
    SN, SN, SN, SN, SN, SC, /* row 0 */
    ST, ST, ST, SS, SN, ST, /* row 1 */
    SN, SN, ST, ST, SC, ST, /* row 2 */
    SN, SC, SN, ST, ST, SN, /* row 3 */
    ST, ST, ST, ST, ST, SN, /* row 4 */
    SS, SC, ST, SS, ST, SN, /* row 5 */
    SN, SN, SN, SN, SC, SN  /* row 6 */
};

static direction any_o[7 * 6] = {
    DS, DS, DS, DS, DE, DS, /* row 0 */
    DE, DN, DW, DN, DE, DW, /* row 1 */
    DN, DE, DN, DW, DE, DW, /* row 2 */
    DS, DE, DW, DE, DW, DN, /* row 3 */
    DE, DN, DS, DW, DE, DW, /* row 4 */
    DN, DE, DW, DN, DE, DW, /* row 5 */
    DN, DN, DN, DN, DN, DW  /* row 6 */
};

/* *********************** ANY 7×6 GAME WITH WRAPPING *********************** */
static shape any_sw[7 * 6] = {
    SN, SN, SC, SN, SS, ST, /* row 0 */
    SC, SC, SN, SN, SN, ST, /* row 1 */
    SC, SC, SN, SC, ST, SC, /* row 2 */
    SN, SN, ST, ST, ST, SN, /* row 3 */
    SC, ST, SS, ST, SN, SC, /* row 4 */
    SN, ST, SN, ST, ST, SC, /* row 5 */
    SN, SC, SC, ST, ST, SN  /* row 6 */
};

static direction any_ow[7 * 6] = {
    DW, DE, DW, DN, DN, DE, /* row 0 */
    DS, DE, DW, DS, DN, DE, /* row 1 */
    DN, DW, DS, DN, DS, DW, /* row 2 */
    DS, DE, DN, DS, DN, DW, /* row 3 */
    DN, DS, DE, DW, DE, DS, /* row 4 */
    DE, DW, DE, DN, DS, DW, /* row 5 */
    DE, DW, DE, DS, DW, DS  /* row 6 */
};

/* ************************************************************************** */
/*                             Test Functions                                 */
/* ************************************************************************** */

/* **************************** TEST GAME PRINT ***************************** */
bool test_game_print() {
  // Creates 3 differents games and print them
  game g1 = game_default();
  game g2 = game_new_empty_ext(10, 8, false);
  game g3 = game_new_ext(7, 6, any_s, any_o, true);

  // If no fatal error the function return true
  game_print(g1);
  game_print(g2);
  game_print(g3);

  game_delete(g1);
  game_delete(g2);
  game_delete(g3);

  return true;
}

/* *************************** TEST GAME DEFAULT **************************** */
bool test_game_default() {
  // Create the default game and check if it is correctly created
  game g = game_default();
  if (g == NULL) return false;

  // Game must be of DEFAULT_SIZE and wrapping false
  uint h = game_nb_rows(g);
  uint w = game_nb_cols(g);
  if (h != DEFAULT_SIZE || w != DEFAULT_SIZE) return false;
  if (game_is_wrapping(g)) return false;

  // Test that shape and orientation is correct in each piece
  for (uint i = 0; i < h; i++) {
    for (uint j = 0; j < w; j++) {
      if (game_get_piece_orientation(g, i, j) != default_o[w * i + j]) return false;
      if (game_get_piece_shape(g, i, j) != default_p[w * i + j]) return false;
    }
  }

  // Check history
  game_undo(g);
  game_redo(g);

  game_delete(g);
  return true;
}

/* *********************** TEST GAME DEFAULT SOLUTION *********************** */
bool test_game_default_solution() {
  // Create the default game solution and check if it is correctly created
  game g = game_default_solution();
  if (g == NULL) return false;

  // Game must be of DEFAULT_SIZE and wrapping false
  uint h = game_nb_rows(g);
  uint w = game_nb_cols(g);
  if (h != DEFAULT_SIZE || w != DEFAULT_SIZE) return false;
  if (game_is_wrapping(g)) return false;

  // Test that shape is correct in each piece
  for (uint i = 0; i < h; i++) {
    for (uint j = 0; j < w; j++) {
      if (game_get_piece_shape(g, i, j) != default_p[w * i + j]) return false;
    }
  }
  // Test if the piece are well oriented
  if (!game_won(g)) return false;

  // Check history
  game_undo(g);
  game_redo(g);

  game_delete(g);
  return true;
}

/* ********************* TEST GAME GET ADJACENT SQUARE ********************** */
bool test_game_get_ajacent_square() {
  // Creation of 3 games, 1 default and 2 with any size
  game g1 = game_default();
  game g2 = game_new_empty_ext(6, 7, true);
  game g3 = game_new_empty_ext(6, 7, false);

  uint i_next, j_next;

  // Test for default game
  if (!game_get_ajacent_square(g1, 0, 0, EAST, &i_next, &j_next)) return false;
  if (i_next != 0 || j_next != 1) return false;
  if (game_get_ajacent_square(g1, 4, 4, EAST, &i_next, &j_next)) return false;

  // Test for any size games, with wrapping on and off
  if (!game_get_ajacent_square(g2, 0, 3, NORTH, &i_next, &j_next)) return false;
  if (i_next != 5 || j_next != 3) return false;
  if (game_get_ajacent_square(g3, 0, 3, NORTH, &i_next, &j_next)) return false;

  game_delete(g1);
  game_delete(g2);
  game_delete(g3);
  return true;
}

/* ************************ TEST GAME HAS HALF EDGE ************************* */
bool test_game_has_half_edge() {
  // Creation of an empty game (5*5)
  game g = game_new_empty();

  // Test for ENDPOINT
  game_set_piece_shape(g, 0, 0, ENDPOINT);
  if (!game_has_half_edge(g, 0, 0, NORTH)) return false;
  if (game_has_half_edge(g, 0, 0, EAST)) return false;
  if (game_has_half_edge(g, 0, 0, SOUTH)) return false;
  if (game_has_half_edge(g, 0, 0, WEST)) return false;

  // Test for SEGMENT
  game_set_piece_shape(g, 0, 0, SEGMENT);
  if (!game_has_half_edge(g, 0, 0, NORTH)) return false;
  if (game_has_half_edge(g, 0, 0, EAST)) return false;
  if (!game_has_half_edge(g, 0, 0, SOUTH)) return false;
  if (game_has_half_edge(g, 0, 0, WEST)) return false;

  // Test for CORNER
  game_set_piece_shape(g, 0, 0, CORNER);
  if (!game_has_half_edge(g, 0, 0, NORTH)) return false;
  if (!game_has_half_edge(g, 0, 0, EAST)) return false;
  if (game_has_half_edge(g, 0, 0, SOUTH)) return false;
  if (game_has_half_edge(g, 0, 0, WEST)) return false;

  // Test for TEE
  game_set_piece_shape(g, 0, 0, TEE);
  if (!game_has_half_edge(g, 0, 0, NORTH)) return false;
  if (!game_has_half_edge(g, 0, 0, EAST)) return false;
  if (game_has_half_edge(g, 0, 0, SOUTH)) return false;
  if (!game_has_half_edge(g, 0, 0, WEST)) return false;

  // Test for CROSS
  game_set_piece_shape(g, 0, 0, CROSS);
  if (!game_has_half_edge(g, 0, 0, NORTH)) return false;
  if (!game_has_half_edge(g, 0, 0, EAST)) return false;
  if (!game_has_half_edge(g, 0, 0, SOUTH)) return false;
  if (!game_has_half_edge(g, 0, 0, WEST)) return false;

  game_delete(g);

  return true;
}

/* ************************** TEST GAME CHECK EDGE ************************** */
bool test_game_check_edge() {
  // Creation of 3 games, g3 has wrapping on
  game g1 = game_default();
  game g2 = game_new_ext(7, 6, any_s, any_o, false);
  game g3 = game_new_ext(7, 6, any_s, any_o, true);

  // Test on default game
  if (game_check_edge(g1, 0, 0, WEST) != MISMATCH) return false;
  if (game_check_edge(g1, 1, 1, NORTH) != MISMATCH) return false;
  if (game_check_edge(g1, 2, 2, SOUTH) != MATCH) return false;
  if (game_check_edge(g1, 4, 1, EAST) != NOEDGE) return false;

  // Test on
  if (game_check_edge(g2, 0, 0, SOUTH) != MATCH) return false;
  if (game_check_edge(g2, 5, 4, EAST) != MATCH) return false;
  if (game_check_edge(g2, 6, 4, WEST) != NOEDGE) return false;

  // Modification of g2 and g3 so piece (0,0) and (0,5) are now facing each other by wrapping
  game_set_piece_orientation(g2, 0, 0, WEST);
  game_set_piece_orientation(g3, 0, 0, WEST);
  game_set_piece_orientation(g2, 0, 5, NORTH);
  game_set_piece_orientation(g3, 0, 5, NORTH);

  // Test of wrapping
  if (game_check_edge(g2, 0, 0, WEST) != MISMATCH) return false;
  if (game_check_edge(g3, 0, 0, WEST) != MATCH) return false;

  game_delete(g1);
  game_delete(g2);
  game_delete(g3);
  return true;
}

/* ************************ TEST GAME IS WELL PAIRED ************************ */
bool test_game_is_well_paired() {
  // Game default and game default solved
  game g1 = game_default();
  game g2 = game_default_solution();
  // Creation of 4 games (7*6), g3 and g6 should be well paired
  game g3 = game_new_ext(7, 6, any_s, any_o, false);
  game g4 = game_new_ext(7, 6, any_s, NULL, true);
  game g5 = game_new_ext(7, 6, any_sw, any_ow, false);
  game g6 = game_new_ext(7, 6, any_sw, any_ow, true);
  // Empty game
  game g7 = game_new_empty();

  // Test on game default
  if (game_is_well_paired(g1)) return false;
  if (!game_is_well_paired(g2)) return false;

  // Test on any game without wrapping (g3 is solved and g4 is not)
  if (!game_is_well_paired(g3)) return false;
  if (game_is_well_paired(g4)) return false;

  // Test on any game both solved with wrapping on, wrapping disabled on g5
  if (game_is_well_paired(g5)) return false;
  if (!game_is_well_paired(g6)) return false;

  // Test on an empty game
  if (!game_is_well_paired(g7)) return false;

  game_delete(g1);
  game_delete(g2);
  game_delete(g3);
  game_delete(g4);
  game_delete(g5);
  game_delete(g6);
  game_delete(g7);

  return true;
}

/* ************************* TEST GAME IS CONNECTED ************************* */
bool test_game_is_connected() {
  // Game default and game default solved
  game g1 = game_default();
  game g2 = game_default_solution();
  // Creation of 4 games (7*6), g3 and g6 should be well paired
  game g3 = game_new_ext(7, 6, any_s, any_o, false);
  game g4 = game_new_ext(7, 6, any_s, NULL, true);
  game g5 = game_new_ext(7, 6, any_sw, any_ow, false);
  game g6 = game_new_ext(7, 6, any_sw, any_ow, true);
  // Empty game
  game g7 = game_new_empty();

  // Test on game default
  if (game_is_connected(g1)) return false;
  if (!game_is_connected(g2)) return false;

  // Test on any game without wrapping (g3 is solved and g4 is not)
  if (!game_is_connected(g3)) return false;
  if (game_is_connected(g4)) return false;

  // Test on any game both solved with wrapping on, wrapping disabled on g5
  if (game_is_connected(g5)) return false;
  if (!game_is_connected(g6)) return false;

  // Test on an empty game
  if (!game_is_connected(g7)) return false;

  game_delete(g1);
  game_delete(g2);
  game_delete(g3);
  game_delete(g4);
  game_delete(g5);
  game_delete(g6);
  game_delete(g7);

  return true;
}

/* ************************* TEST GAME IS WRAPPING ************************** */
bool test_game_is_wrapping() {
  // Creation of two game without wrapping,...
  game g1 = game_default();
  game g2 = game_new_empty_ext(2, 4, false);
  // and one with wrapping
  game g3 = game_new_empty_ext(5, 5, true);

  // Test of wrapping state for the 3 games
  if (game_is_wrapping(g1)) return false;
  if (game_is_wrapping(g2)) return false;
  if (!game_is_wrapping(g3)) return false;

  game_delete(g1);
  game_delete(g2);
  game_delete(g3);
  return true;
}

/* *************************** TEST GAME NEW EXT **************************** */
bool test_game_new_ext() {
  // Creation of 2 games to test the function, one with defined shape and orientation
  game g1 = game_new_ext(7, 6, any_s, any_o, true);
  game g2 = game_new_ext(5, 9, NULL, NULL, false);

  // Test if the games are correctly created
  if (g1 == NULL || g2 == NULL) return false;

  // Test that the game g1 has the rigth values
  uint h = game_nb_rows(g1);
  uint w = game_nb_cols(g1);
  if (h != 7 || w != 6) return false;
  if (!game_is_wrapping(g1)) return false;

  // Test that shape and orientation is correct in each piece of g1
  for (uint i = 0; i < h; i++) {
    for (uint j = 0; j < w; j++) {
      if (game_get_piece_orientation(g1, i, j) != any_o[w * i + j]) return false;
      if (game_get_piece_shape(g1, i, j) != any_s[w * i + j]) return false;
    }
  }

  // Test that the game g2 has the rigth values
  h = game_nb_rows(g2);
  w = game_nb_cols(g2);
  if (h != 5 || w != 9) return false;
  if (game_is_wrapping(g2)) return false;

  // Test that shape and orientation is correct in each piece of g1
  for (uint i = 0; i < h; i++) {
    for (uint j = 0; j < w; j++) {
      if (game_get_piece_orientation(g2, i, j) != NORTH) return false;
      if (game_get_piece_shape(g2, i, j) != EMPTY) return false;
    }
  }

  // Check if both queue of both games struct are created
  game_undo(g1);
  game_redo(g1);
  game_undo(g2);
  game_redo(g2);

  game_delete(g1);
  game_delete(g2);
  return true;
}

/* ************************************************************************** */
/*                             Test Function Mapping                          */
/* ************************************************************************** */
typedef struct {
  const char *name;
  bool (*func)();
} TestEntry;

TestEntry test_functions[] = {
    {"game_print", test_game_print},
    {"game_default", test_game_default},
    {"game_default_solution", test_game_default_solution},
    {"game_get_ajacent_square", test_game_get_ajacent_square},
    {"game_has_half_edge", test_game_has_half_edge},
    {"game_check_edge", test_game_check_edge},
    {"game_is_well_paired", test_game_is_well_paired},
    {"game_is_connected", test_game_is_connected},
    {"game_is_wrapping", test_game_is_wrapping},
    {"game_new_ext", test_game_new_ext},
};

#define NUM_TESTS (sizeof(test_functions) / sizeof(TestEntry))

/* ************************************************************************** */
/*                                  Usage                                     */
/* ************************************************************************** */

void usage(const char *prog_name) {
  fprintf(stderr, "Usage: %s <testname>\n", prog_name);
  exit(EXIT_FAILURE);
}

/* ************************************************************************** */
/*                             Main Function                                  */
/* ************************************************************************** */

int main(int argc, char *argv[]) {
  if (argc != 2) usage(argv[0]);

  for (uint i = 0; i < NUM_TESTS; i++) {
    if (strcmp(test_functions[i].name, argv[1]) == 0) {
      bool ok = test_functions[i].func();
      fprintf(stderr, "Test \"%s\" finished: %s\n", argv[1], ok ? "SUCCESS" : "FAILURE");
      return ok ? EXIT_SUCCESS : EXIT_FAILURE;
    }
  }

  fprintf(stderr, "Error: test \"%s\" not found!\n", argv[1]);
  return EXIT_FAILURE;
}