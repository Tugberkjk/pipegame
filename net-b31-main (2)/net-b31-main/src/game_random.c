/**
 * @file game_random.c
 * @brief Random game generator
 * @details This program generates a random game based on few arguments.
 * @copyright University of Bordeaux. All rights reserved, 2024.
 **/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "game_aux.h"
#include "game_tools.h"

/* ************************************************************************** */
/*                                  USAGE                                     */
/* ************************************************************************** */

void usage(const char* prog_name) {
  fprintf(stderr, "Usage: %s <nb_rows> <nb_cols> <wrapping> <nb_empty> <nb_extra> <shuffle> [<filename>]\n", prog_name);
  fprintf(stderr, "Example: %s 4 4 0 0 0 0 random.sol\n", prog_name);
  exit(EXIT_FAILURE);
}

/* ************************************************************************** */
/*                             MAIN FUNCTION                                  */
/* ************************************************************************** */

int main(int argc, char* argv[]) {
  // Ensure at least 6 arguments are provided (7th is facultative)
  if (argc < 7) usage(argv[0]);

  srand(time(NULL));

  uint nb_rows = atoi(argv[1]);
  uint nb_cols = atoi(argv[2]);
  uint wrapping = atoi(argv[3]);
  uint nb_empty = atoi(argv[4]);
  uint nb_extra = atoi(argv[5]);
  uint shuffle = atoi(argv[6]);

  game g = game_random(nb_rows, nb_cols, wrapping, nb_empty, nb_extra);
  if (shuffle) game_shuffle_orientation(g);

  printf("> nb_rows = %u nb_cols = %u wrapping = %u\n", nb_rows, nb_cols, wrapping);
  printf("> nb_empty = %u nb_extra = %u shuffle = %u\n", nb_empty, nb_extra, shuffle);
  game_print(g);

  // Save the game if filename is given
  if (argc > 7) game_save(g, argv[7]);

  return EXIT_SUCCESS;
}
