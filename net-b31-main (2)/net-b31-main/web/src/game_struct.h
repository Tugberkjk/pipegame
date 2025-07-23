#ifndef __GAME_STRUCT_H__
#define __GAME_STRUCT_H__

#include "queue.h"

struct game_s {
  uint HEIGHT;
  uint WIDTH;
  shape *tab_shape;
  direction *tab_direction;
  bool is_wrapping;
  queue *undo_mooves;
  queue *redo_mooves;
};

#endif /*__GAME_STRUCT_H__*/