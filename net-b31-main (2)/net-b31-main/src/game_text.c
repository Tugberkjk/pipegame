/**
 * @file game_text.c
 * @brief Text-based interface for playing the game in a terminal
 * @details This program is a simple text-based program to play our game in a terminal.
 * @copyright University of Bordeaux. All rights reserved, 2024.
 **/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"

#define GAME_SIZE_MAX 10

/* ************************************************************************** */
/*                            GAME TEXT FUNCTIONS                             */
/* ************************************************************************** */

/* ******************************* HELP MENU ******************************** */
void help_menu() {
  printf("- press 'c <i> <j>' to rotate piece clockwise in square (i,j)\n");
  printf("- press 'a <i> <j>' to rotate piece anti-clockwise in square (i,j)\n");
  printf("- press 'r' to shuffle game\n");
  printf("- press 'z' to undo\n");
  printf("- press 'y' to redo\n");
  printf("- press 'q' to quit\n");
  printf("- press 's <filename>' to save the game\n");
}

/* ****************************** EMPTY BUFFER ****************************** */
void empty_buffer() {
  int c;
  while ((c = getchar()) != '\n' && c != EOF) {
    continue;
  }
}

/* ***************************** PLAYER ACTION ****************************** */
bool player_action(game g) {
  char c;
  uint i, j;
  int rot;

  game_print(g);
  printf("> ? [h for help]\n");
  if (scanf(" %c", &c) != 1) {
    fprintf(stderr, "Error: invalid user input!\n");
  }

  switch (c) {
    case 'h':
      printf("> action: help\n");
      help_menu();
      break;
    case 'r':
      printf("> action: restart\n");
      game_shuffle_orientation(g);
      break;
    case 'c':
      rot = 1;
    case 'a':
      if (c == 'a') rot = -1;
      if (scanf("%d %d", &i, &j) != 2) {
        fprintf(stderr, "Error: invalid user input!\n");
        break;
      }
      printf("> action: play move '%c' into square (%d,%d)\n", c, i, j);
      game_play_move(g, i, j, rot);
      break;
    case 'z':
      printf("> action: undo\n");
      game_undo(g);
      break;
    case 'y':
      printf("> action: redo\n");
      game_redo(g);
      break;
    case 'q':
      printf("> action: quit\n");
      return false;
    case 's': {
      char filename[256];
      if (scanf(" %255[^\n]", filename) != 1) {
        fprintf(stderr, "Error: invalid user input!\n");
      }
      printf("> action: save game as %s\n", filename);
      game_save(g, filename);
      break;
    }
    default:
      fprintf(stderr, "Error: invalid user input!\n");
      break;
  }
  return true;
}

/* ************************************************************************** */
/*                                  USAGE                                     */
/* ************************************************************************** */

void usage(const char* prog_name) {
  fprintf(stderr, "Usage: %s [<filename>]\n", prog_name);
  fprintf(stderr, "Example: %s default.txt\n", prog_name);
  exit(EXIT_FAILURE);
}

/* ************************************************************************** */
/*                             MAIN FUNCTION                                  */
/* ************************************************************************** */

int main(int argc, char* argv[]) {
  // Only one parameter is waited
  if (argc > 2) usage(argv[0]);

  game g = argc == 1 ? game_default() : game_load(argv[1]);

  assert(g);
  assert(game_nb_rows(g) < GAME_SIZE_MAX && game_nb_cols(g) < GAME_SIZE_MAX);

  while (!game_won(g) && player_action(g)) {
    continue;
  }

  game_print(g);
  game_won(g) ? printf("> Congratulation !\n") : printf("> Shame !\n");
  game_delete(g);

  return EXIT_SUCCESS;
}
