#include "model.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_private.h"
#include "game_struct.h"
#include "game_tools.h"
#include "queue.h"

/* ************************************************************************** */
/*                                  MISC                                      */
/* ************************************************************************** */

#define DIR_PATH "res/"
#define FONT DIR_PATH "Quicksand-Regular.ttf"
#define BACKGROUND DIR_PATH "background.png"
#define IMAGE_SEGMENT DIR_PATH "segment.png"
#define IMAGE_CORNER DIR_PATH "corner.png"
#define IMAGE_ENDPOINT DIR_PATH "endpoint.png"
#define IMAGE_TEE DIR_PATH "tee.png"
#define IMAGE_CROSS DIR_PATH "cross.png"

#define FONT_SIZE 35
#define SPACE_BUTTONS 20
#define SPACE_BLOCKS 15
#define MAX_LOGS 3

/* ************************************************************************** */

/* ************************************************************************** */
/*                                  GAME ENV                                  */
/* ************************************************************************** */
typedef struct {
  SDL_Rect rect;
  SDL_Texture *texture;
  bool (*action)(SDL_Renderer *ren, Env *env);
} Button;

struct Env_t {
  /* Images */
  SDL_Texture *background;
  SDL_Texture *endpoint;
  SDL_Texture *corner;
  SDL_Texture *tee;
  SDL_Texture *segment;
  SDL_Texture *cross;
  /* Game */
  cgame save_g;
  game g;
  uint game_x, game_y;
  uint cell_size;
  /* Buttons */
  Button *buttons;
  uint nb_buttons;
  /* Logs */
  SDL_Texture *logs[MAX_LOGS];
  char log_messages[MAX_LOGS][256];
  SDL_Rect rect_logs[MAX_LOGS];
  TTF_Font *font;
  SDL_Color color_font;
};

/* ************************************************************************** */
/*                              USEFUL FUNCTIONS                              */
/* ************************************************************************** */

/* ******************************** ADD LOG ********************************* */
void add_log(SDL_Renderer *ren, Env *env, const char *message) {
  for (int i = 0; i < MAX_LOGS; i++) {
    if (env->logs[i]) {
      if (i == 0) {
        SDL_DestroyTexture(env->logs[0]);
      }
      if (i > 0) {
        strcpy(env->log_messages[i - 1], env->log_messages[i]);
        env->logs[i - 1] = env->logs[i];
        env->rect_logs[i - 1].w = env->rect_logs[i].w;
      }
    }
  }
  strcpy(env->log_messages[MAX_LOGS - 1], message);

  SDL_Surface *surf = TTF_RenderText_Blended(env->font, message, env->color_font);
  env->logs[MAX_LOGS - 1] = SDL_CreateTextureFromSurface(ren, surf);
  env->rect_logs[MAX_LOGS - 1].w = surf->w;
  SDL_FreeSurface(surf);
}

/* ***************************** BUTTON SHUFFLE ***************************** */
bool button_shuffle(SDL_Renderer *ren, Env *env) {
  if (!game_equal(env->g, env->save_g, false)) {
    env->g = game_copy(env->save_g);
    add_log(ren, env, "> Game reset ");
  }
  return false;
}

/* ****************************** BUTTON UNDO ******************************* */
bool button_undo(SDL_Renderer *ren, Env *env) {
  if (queue_is_empty(env->g->undo_mooves)) {
    add_log(ren, env, "> Nothing to undo");
  } else {
    add_log(ren, env, "> Move undone");
  }
  game_undo(env->g);
  return false;
}

/* ****************************** BUTTON REDO ******************************* */
bool button_redo(SDL_Renderer *ren, Env *env) {
  if (queue_is_empty(env->g->redo_mooves)) {
    add_log(ren, env, "> Nothing to redo");
  } else {
    add_log(ren, env, "> Move redone");
  }
  game_redo(env->g);
  return false;
}

/* ****************************** BUTTON SOLVE ****************************** */
bool button_solve(SDL_Renderer *ren, Env *env) {
  if (game_won(env->g)) return false;
  if (game_solve(env->g)) add_log(ren, env, "> Game solved ");
  return false;
}

/* ****************************** BUTTON QUIT ******************************* */
bool button_quit(SDL_Renderer *ren, Env *env) { return true; }

/* ******************************* DRAW FRAME ******************************* */
void draw_frame(SDL_Renderer *renderer, int x1, int y1, int width, int height) {
  // Draw frame for game with wrapping off
  SDL_Rect borders[] = {{x1 - 3, y1 - 3, width + 7, 4},
                        {x1 - 3, y1 + height, width + 7, 4},
                        {x1 - 3, y1, 4, height},
                        {x1 + width, y1, 4, height}};

  SDL_Rect fill[] = {{x1 - 2, y1 - 2, width + 5, 2},
                     {x1 - 2, y1 + height + 1, width + 5, 2},
                     {x1 - 2, y1, 2, height + 1},
                     {x1 + width + 1, y1, 2, height + 1}};

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  for (int i = 0; i < 4; i++) {
    SDL_RenderFillRect(renderer, &borders[i]);
  }

  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  for (int i = 0; i < 4; i++) {
    SDL_RenderFillRect(renderer, &fill[i]);
  }
}

/* ******************************* DRAW GRID ******************************** */
void draw_grid(SDL_Renderer *renderer, int x1, int y1, int cell_size, int nb_cols, int nb_rows, SDL_Color color) {
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  for (int col = 0; col <= nb_cols; ++col) {
    int x = x1 + col * (cell_size - 1);
    SDL_RenderDrawLine(renderer, x, y1, x, y1 + nb_rows * (cell_size - 1));
  }
  for (int row = 0; row <= nb_rows; ++row) {
    int y = y1 + row * (cell_size - 1);
    SDL_RenderDrawLine(renderer, x1, y, x1 + nb_cols * (cell_size - 1), y);
  }
}

/* ************************************************************************** */
/*                              MODEL FUNCTIONS                               */
/* ************************************************************************** */

/* ********************************** INIT ********************************** */
Env *init(SDL_Window *win, SDL_Renderer *ren, int argc, char *argv[]) {
  Env *env = malloc(sizeof(struct Env_t));
  assert(env);

  int w, h;
  SDL_GetWindowSize(win, &w, &h);

  PRINT("Welcome in the game : NET\n");
  PRINT("--- HELP MENU ---\n");
  PRINT("Left click to rotate clockwise and right click anti-clockwise\n");
  PRINT("Press 'r' to reset game\n");
  PRINT("Press 'z' to undo\n");
  PRINT("Press 'y' to redo\n");
  PRINT("Press 's' to solve game\n");
  PRINT("Press ESC to quit\n");
  PRINT("You can also use the buttons\n");
  PRINT("Enjoy the game!\n");

  /* init images texture from PNG images */
  env->background = IMG_LoadTexture(ren, BACKGROUND);
  if (!env->background) ERROR("IMG_LoadTexture: %s\n", BACKGROUND);

  env->segment = IMG_LoadTexture(ren, IMAGE_SEGMENT);
  if (!env->segment) ERROR("IMG_LoadTexture: %s\n", IMAGE_SEGMENT);

  env->corner = IMG_LoadTexture(ren, IMAGE_CORNER);
  if (!env->corner) ERROR("IMG_LoadTexture: %s\n", IMAGE_CORNER);

  env->endpoint = IMG_LoadTexture(ren, IMAGE_ENDPOINT);
  if (!env->endpoint) ERROR("IMG_LoadTexture: %s\n", IMAGE_ENDPOINT);

  env->tee = IMG_LoadTexture(ren, IMAGE_TEE);
  if (!env->tee) ERROR("IMG_LoadTexture: %s\n", IMAGE_TEE);

  env->cross = IMG_LoadTexture(ren, IMAGE_CROSS);
  if (!env->cross) ERROR("IMG_LoadTexture: %s\n", IMAGE_CROSS);

  /* init text font and color */
  env->font = TTF_OpenFont(FONT, FONT_SIZE);
  if (!env->font) ERROR("TTF_OpenFont: %s\n", FONT);
  SDL_Color color = {255, 255, 255, 255};
  env->color_font = color;

  /* Init buttons*/
  env->nb_buttons = 5;
  env->buttons = malloc(env->nb_buttons * sizeof(Button));
  assert(env->buttons);

  const char *labels[] = {"Reset", "Undo", "Redo", "Solve", "Quit"};
  bool (*actions[])(SDL_Renderer *, Env *) = {button_shuffle, button_undo, button_redo, button_solve, button_quit};

  uint pos_y = SPACE_BLOCKS;
  int menu_width = 0;
  for (int i = 0; i < env->nb_buttons; i++) {
    SDL_Surface *surface_button = TTF_RenderText_Blended(env->font, labels[i], env->color_font);
    env->buttons[i].texture = SDL_CreateTextureFromSurface(ren, surface_button);
    if (!env->buttons[i].texture) ERROR("Failed to create button texture: %s\n", labels[i]);
    env->buttons[i].action = actions[i];
    env->buttons[i].rect.w = surface_button->w;
    env->buttons[i].rect.h = surface_button->h;
    env->buttons[i].rect.y = pos_y;
    menu_width += (surface_button->w + SPACE_BUTTONS);
    SDL_FreeSurface(surface_button);
  }

  uint pos_x = (w - menu_width) / 2;
  for (int i = 0; i < env->nb_buttons; i++) {
    env->buttons[i].rect.x = pos_x;
    pos_x += env->buttons[i].rect.w + SPACE_BUTTONS;
  }

  /* Init game board */
  env->save_g = argc == 1 ? game_default() : game_load(argv[1]);
  env->g = game_copy(env->save_g);
  uint nb_cols = game_nb_cols(env->g);
  uint nb_rows = game_nb_rows(env->g);

  uint logs_height = (FONT_SIZE + 5) * MAX_LOGS;
  uint start_y = env->buttons[0].rect.y + env->buttons[0].rect.h + SPACE_BLOCKS;
  env->cell_size = fmin(w / nb_cols, (h - start_y - logs_height - SPACE_BLOCKS) / nb_rows);

  env->game_x = (w - (nb_cols * env->cell_size - nb_cols + 1)) / 2;
  env->game_y = start_y;

  /* init logs texture from created surface */
  uint log_height = FONT_SIZE + 5;
  start_y = h - (log_height * MAX_LOGS) - SPACE_BLOCKS;
  for (int i = 0; i < MAX_LOGS; i++) {
    env->rect_logs[i].x = env->game_x;
    env->rect_logs[i].y = start_y + i * log_height + SPACE_BLOCKS;
    env->rect_logs[i].w = w - 2 * SPACE_BLOCKS;
    env->rect_logs[i].h = log_height;
    env->logs[i] = NULL;
  }

  return env;
}

/* ********************************* RENDER ********************************* */
void render(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  /* Render background texture */
  SDL_RenderCopy(ren, env->background, NULL, NULL);

  /* Render buttons texture */
  for (int i = 0; i < env->nb_buttons; i++) {
    SDL_RenderCopy(ren, env->buttons[i].texture, NULL, &env->buttons[i].rect);
  }

  uint nb_cols = game_nb_cols(env->g);
  uint nb_rows = game_nb_rows(env->g);

  SDL_Color color = {127, 127, 127, 0};
  draw_grid(ren, env->game_x, env->game_y, env->cell_size, nb_cols, nb_rows, color);

  /* Render game */
  for (uint i = 0; i < nb_rows; i++) {
    for (uint j = 0; j < nb_cols; j++) {
      shape s = game_get_piece_shape(env->g, i, j);
      int orientation = game_get_piece_orientation(env->g, i, j) * 90;
      SDL_Rect rect;
      SDL_QueryTexture(env->corner, NULL, NULL, &rect.w, &rect.h);

      SDL_Texture *texture = NULL;
      switch (s) {
        case ENDPOINT:
          texture = env->endpoint;
          break;
        case SEGMENT:
          texture = env->segment;
          break;
        case CORNER:
          texture = env->corner;
          break;
        case TEE:
          texture = env->tee;
          break;
        case CROSS:
          texture = env->cross;
          break;
        case EMPTY:

          break;
        default:
          break;
      }

      if (texture) {
        rect.x = env->game_x + j * (env->cell_size - 1);
        rect.y = env->game_y + i * (env->cell_size - 1);
        rect.w = rect.h = env->cell_size;
        SDL_RenderCopyEx(ren, texture, NULL, &rect, orientation, NULL, SDL_FLIP_NONE);
      }
    }
  }

  /* Render frame */
  if (!game_is_wrapping(env->g)) {
    draw_frame(ren, env->game_x, env->game_y, (env->cell_size - 1) * nb_cols, (env->cell_size - 1) * nb_rows);
  }

  /* Render logs */
  for (int i = 0; i < MAX_LOGS; i++) {
    if (env->logs[i]) {
      SDL_RenderCopy(ren, env->logs[i], NULL, &env->rect_logs[i]);
    }
  }

  /* Render victory on game won */
  if (game_won(env->g)) {
    TTF_Font *font = TTF_OpenFont(FONT, 64);
    assert(font);
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *surf = TTF_RenderText_Solid(font, "VICTORY !", color);
    assert(surf);

    SDL_Texture *texture = SDL_CreateTextureFromSurface(ren, surf);
    assert(texture);

    int w, h;
    SDL_GetWindowSize(win, &w, &h);

    SDL_Rect dstRect = {(w - surf->w) / 2, env->game_y + (nb_rows * env->cell_size - surf->h) / 2, surf->w, surf->h};
    SDL_RenderCopy(ren, texture, NULL, &dstRect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surf);
    TTF_CloseFont(font);
  }
}

/* ******************************** PROCESS ********************************* */
bool process(SDL_Window *win, SDL_Renderer *ren, Env *env, SDL_Event *e) {
  if (e->type == SDL_QUIT) {
    return true;
  } else if (e->type == SDL_KEYDOWN) {
    switch (e->key.keysym.sym) {
      case SDLK_ESCAPE:
        return true;
        break;
      case SDLK_r:
        return button_shuffle(ren, env);
        break;
      case SDLK_z:
        return button_undo(ren, env);
        break;
      case SDLK_y:
        return button_redo(ren, env);
        break;
      case SDLK_s:
        return button_solve(ren, env);
        break;
      default:
        break;
    }
    return false;
  } else if (e->type == SDL_WINDOWEVENT) {
    if (e->window.event == SDL_WINDOWEVENT_RESIZED) {
      int w, h;
      SDL_GetWindowSize(win, &w, &h);

      /* Calcul new buttons pos*/
      int pos_x = 0;
      for (int i = 0; i < env->nb_buttons; i++) {
        int width, height;
        SDL_QueryTexture(env->buttons[i].texture, NULL, NULL, &width, &height);
        if (w > SCREEN_WIDTH) {
          env->buttons[i].rect.w = fmin(width * w / SCREEN_WIDTH, 1.5 * width);
          env->buttons[i].rect.h = fmin(height * w / SCREEN_WIDTH, 1.5 * height);
        } else {
          env->buttons[i].rect.w = fmax(width * w / SCREEN_WIDTH, 0.5 * width);
          env->buttons[i].rect.h = fmax(height * w / SCREEN_WIDTH, 0.5 * height);
        }
        env->buttons[i].rect.y = SPACE_BLOCKS * w / SCREEN_WIDTH;
        pos_x += env->buttons[i].rect.w + (SPACE_BUTTONS * w / SCREEN_WIDTH);
      }

      pos_x = (w - pos_x) / 2;

      for (int i = 0; i < env->nb_buttons; i++) {
        env->buttons[i].rect.x = pos_x;
        pos_x += env->buttons[i].rect.w + (SPACE_BUTTONS * w / SCREEN_WIDTH);
      }

      /* Define dynamically the size of each cells */
      uint nb_cols = game_nb_cols(env->g);
      uint nb_rows = game_nb_rows(env->g);
      uint logs_height = (FONT_SIZE + 5) * MAX_LOGS;
      uint start_y = env->buttons[0].rect.y + env->buttons[0].rect.h + SPACE_BLOCKS;
      env->cell_size = fmin(w / nb_cols, (h - start_y - logs_height - SPACE_BLOCKS) / nb_rows);

      env->game_x = (w - (nb_cols * env->cell_size)) / 2;
      env->game_y = start_y;

      uint log_height = FONT_SIZE + 5;
      start_y = h - (log_height * MAX_LOGS) - SPACE_BLOCKS;
      for (int i = 0; i < MAX_LOGS; i++) {
        env->rect_logs[i].x = env->game_x;
        env->rect_logs[i].y = start_y + i * log_height;
      }
    }
  } else if (e->type == SDL_MOUSEBUTTONDOWN) {
    SDL_Point mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);

    for (int i = 0; i < env->nb_buttons; i++) {
      if (SDL_PointInRect(&mouse, &env->buttons[i].rect)) {
        return env->buttons[i].action(ren, env);
      }
    }

    SDL_Rect rect;
    SDL_QueryTexture(env->corner, NULL, NULL, &rect.w, &rect.h);

    uint nb_cols = game_nb_cols(env->g);
    uint nb_rows = game_nb_rows(env->g);

    int i = (mouse.y - env->game_y) / env->cell_size;
    int j = (mouse.x - env->game_x) / env->cell_size;

    if (i >= 0 && i < nb_rows && j >= 0 && j < nb_cols) {
      game_play_move(env->g, i, j, 1);
      char message[256];
      sprintf(message, "> Played moove in (%d,%d)", i, j);
      add_log(ren, env, message);
    }
  }

  return false;
}

/* ********************************* CLEAN ********************************** */
void clean(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  SDL_DestroyTexture(env->background);
  SDL_DestroyTexture(env->endpoint);
  SDL_DestroyTexture(env->tee);
  SDL_DestroyTexture(env->segment);
  SDL_DestroyTexture(env->corner);
  SDL_DestroyTexture(env->cross);
  TTF_CloseFont(env->font);

  game_delete(env->g);
  for (int i = 0; i < env->nb_buttons; i++) {
    SDL_DestroyTexture(env->buttons[i].texture);
  }
  free(env->buttons);
  if (env) free(env);
}

/* ************************************************************************** */