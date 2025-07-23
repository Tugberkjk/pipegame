/**
 * @file game_private.h
 * @brief Private Game Functions.
 * @copyright University of Bordeaux. All rights reserved, 2024.
 **/

#ifndef __GAME_PRIVATE_H__
#define __GAME_PRIVATE_H__

#include <stdbool.h>

#include "game.h"
#include "game_aux.h"
#include "game_struct.h"
#include "queue.h"

/* ************************************************************************** */
/*                             DATA TYPES                                     */
/* ************************************************************************** */

/**
 * @brief Move structure.
 * @details This structure is used to save the game history.
 */
struct move_s {
  uint i, j;      // piece position
  direction old;  // old piece orientation
  direction new;  // new piece orientation
};

typedef struct move_s move;

/* ************************************************************************** */
/*                                MACRO                                       */
/* ************************************************************************** */

#define MAX(x, y) ((x > (y)) ? (x) : (y))

/* ************************************************************************** */
/*                             STACK ROUTINES                                 */
/* ************************************************************************** */

/** push a move in the stack */
void _stack_push_move(queue* q, move m);

/** pop a move from the stack */
move _stack_pop_move(queue* q);

/** test if the stack is empty */
bool _stack_is_empty(queue* q);

/** clear all the stack */
void _stack_clear(queue* q);

/* ************************************************************************** */
/*                                MISC                                        */
/* ************************************************************************** */

/** convert a square into its string representation
 * @details a single utf8 wide char represented by a string
 */
char* _square2str(shape s, direction d);

#endif  // __GAME_PRIVATE_H__

/* ************************************************************************** */
/*                                 ADD_EDGE                                   */
/* ************************************************************************** */

/** encode a shape and an orientation into an integer code */
uint _encode_shape(shape s, direction o);

/** decode an integer code into a shape and an orientation */
bool _decode_shape(uint code, shape* s, direction* o);

/** add an half-edge in the direction d */
void _add_half_edge(game g, uint i, uint j, direction d);

/**
 * @brief Add an edge between two adjacent squares.
 * @details This is done by modifying the pieces of the two adjacent squares.
 * More precisely, we add an half-edge to each adjacent square, so as to build
 * an edge between these two squares.
 * @param g the game
 * @param i row index
 * @param j column index
 * @param d the direction of the adjacent square
 * @pre @p g must be a valid pointer toward a game structure.
 * @pre @p i < game height
 * @pre @p j < game width
 * @return true if an edge can be added, false otherwise
 */
bool _add_edge(game g, uint i, uint j, direction d);
