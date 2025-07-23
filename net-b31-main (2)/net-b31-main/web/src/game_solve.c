/**
 * @file game_solver.c
 * @brief Game solver and solution counter.
 * @details This program allows solving a given game or counting the number of possible solutions.
 * @copyright University of Bordeaux. All rights reserved, 2024.
 **/
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_aux.h"
#include "game_tools.h"

/* **************************** COMPUTE SOLUTION **************************** */
int compute_solution(game g, char* option, char* output) {
  if (strcmp(option, "-s") == 0) {
    if (game_solve(g)) {
      printf("> A solution to the game :\n");
      game_print(g);
      if (output) game_save(g, output);
      game_delete(g);
      return EXIT_SUCCESS;
    }
    printf("> The game has no solutions\n");
    game_delete(g);
    return EXIT_FAILURE;
  } else {
    uint nb_sols = game_nb_solutions(g);
    printf("> The game has %u solutions\n", nb_sols);
    if (output) {
      FILE* f = fopen(output, "w");
      assert(f);
      fprintf(f, "%u\n", nb_sols);
      fclose(f);
      printf("> Game was successfully saved as '%s'\n", output);
    }
    return EXIT_SUCCESS;
  }
}

/* ************************************************************************** */
/*                                  USAGE                                     */
/* ************************************************************************** */

void usage(const char* prog_name) {
  fprintf(stderr, "Usage: %s <option> <input> [<output>]\n", prog_name);
  fprintf(stderr, "Example: %s -s default.txt default_sol.txt\n", prog_name);
  exit(EXIT_FAILURE);
}

/* ************************************************************************** */
/*                             MAIN FUNCTION                                  */
/* ************************************************************************** */

int main(int argc, char* argv[]) {
  // This program needs at least 2 arguments (3rd one is facultative)
  if (argc < 3) usage(argv[0]);

  char* option = argv[1];
  char* input = argv[2];
  char* output = argc > 3 ? argv[3] : NULL;

  if (strcmp(option, "-c") != 0 && strcmp(option, "-s") != 0) usage(argv[0]);  // Check valid option
  game g = game_load(input);
  game_print(g);

  return compute_solution(g, option, output);
}
