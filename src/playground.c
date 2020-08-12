#include "playground.h"
#include <ncurses.h>
#include <stdlib.h>

plg_playground plg_new() {
  plg_playground pl;
  pl.turn = TEAM_WHITE;
  pl.table[0][0] = WHITE_ROOK;
  pl.table[1][0] = WHITE_KNIGHT;
  pl.table[2][0] = WHITE_BISHOP;
  pl.table[3][0] = WHITE_QUEEN;
  pl.table[4][0] = WHITE_KING;
  pl.table[5][0] = WHITE_BISHOP;
  pl.table[6][0] = WHITE_KNIGHT;
  pl.table[7][0] = WHITE_ROOK;

  pl.table[0][1] = WHITE_PAWN;
  pl.table[1][1] = WHITE_PAWN;
  pl.table[2][1] = WHITE_PAWN;
  pl.table[3][1] = WHITE_PAWN;
  pl.table[4][1] = WHITE_PAWN;
  pl.table[5][1] = WHITE_PAWN;
  pl.table[6][1] = WHITE_PAWN;
  pl.table[7][1] = WHITE_PAWN;

  int x, y;
  for (x = 0; x < 8; x++) {
    for (y = 2; y < 6; y++) {
      pl.table[x][y] = EMPTY;
    }
  }
  pl.table[0][6] = BLACK_PAWN;
  pl.table[1][6] = BLACK_PAWN;
  pl.table[2][6] = BLACK_PAWN;
  pl.table[3][6] = BLACK_PAWN;
  pl.table[4][6] = BLACK_PAWN;
  pl.table[5][6] = BLACK_PAWN;
  pl.table[6][6] = BLACK_PAWN;
  pl.table[7][6] = BLACK_PAWN;

  pl.table[0][7] = BLACK_ROOK;
  pl.table[1][7] = BLACK_KNIGHT;
  pl.table[2][7] = BLACK_BISHOP;
  pl.table[3][7] = BLACK_QUEEN;
  pl.table[4][7] = BLACK_KING;
  pl.table[5][7] = BLACK_BISHOP;
  pl.table[6][7] = BLACK_KNIGHT;
  pl.table[7][7] = BLACK_ROOK;
  pl.possibilities.size = 0;

  pl.turn = TEAM_WHITE;
  plg_pos p = {
      .x = -1,
      .y = -1,
  };
  pl.selection = p;
  return pl;
}

static const char *plg_get_symbol(char id) {
  switch (id) {
  case WHITE_KING:
    return "\xe2\x99\x94";
  case WHITE_QUEEN:
    return "\xe2\x99\x95";
  case WHITE_ROOK:
    return "\xe2\x99\x96";
  case WHITE_BISHOP:
    return "\xe2\x99\x97";
  case WHITE_KNIGHT:
    return "\xe2\x99\x98";
  case WHITE_PAWN:
    return "\xe2\x99\x99";
  case BLACK_KING:
    return "\xe2\x99\x9a";
  case BLACK_QUEEN:
    return "\xe2\x99\x9b";
  case BLACK_ROOK:
    return "\xe2\x99\x9c";
  case BLACK_BISHOP:
    return "\xe2\x99\x9d";
  case BLACK_KNIGHT:
    return "\xe2\x99\x9e";
  case BLACK_PAWN:
    return "\xe2\x99\x9f";
  case EMPTY:
    return " ";
  default:
    return "\xE2\x9D\x8C";
  }
}

char plg_get_team(char id) {
  if (id & TEAM_WHITE)
    return TEAM_WHITE;
  if (id & TEAM_BLACK)
    return TEAM_BLACK;
  return EMPTY;
}

void plg_draw(plg_playground *plg) {
  int x, y;
  move(LINES / 2 - 4 - 2, COLS / 2 - 4 * 3);
  printw(plg->turn == TEAM_WHITE ? "TEAM WHITE      " : "TEAM BLACK      ");
  for (y = 0; y < 8; y++) {
    move(LINES / 2 - 4 + y, COLS / 2 - 4 * 3);
    // move(LINES / 2 + 4 - y, COLS / 2 - 4 * 3);
    for (x = 0; x < 8; x++) {
      char color;
      if (y == plg->selection.y && x == plg->selection.x) {
        color = SELECTED_TILE;
      } else if ((y % 2 == 1 && x % 2 == 0) || (y % 2 == 0 && x % 2 == 1)) {
        color = WHITE_TILE;
      } else {
        color = BLACK_TILE;
      }
      int i;
      if (plg->possibilities.size > 0) {

        for (i = 0; i < plg->possibilities.size; i++) {
          if (x == plg->possibilities.list[i].x &&
              y == plg->possibilities.list[i].y) {
            color = POSSIBILITY;
            break;
          }
        }
      }
      attron(COLOR_PAIR(color));
      printw(" ");
      printw(plg_get_symbol(plg->table[x][y]));
      printw(" ");
      attroff(COLOR_PAIR(color));
    }
  }
}

plg_possibilities *plg_possibilities_new() {
  plg_possibilities *possibilities =
      (plg_possibilities *)malloc(sizeof(plg_possibilities));
  possibilities->size = 0;
  possibilities->list = NULL;
  return possibilities;
}
/* return 0: no move, 1 move on emty, 2 move & eat. */
int plg_position_is_valid(plg_playground *plg, char team, plg_pos target,
                          int mvt) {
  if (target.x >= 0 && target.y >= 0 && target.x < 8 && target.y < 8) {
    switch (mvt) {
    case MVT_CAN_EAT:
      if (plg->table[target.x][target.y] == EMPTY) {
        return 1;
      } else if (team != plg_get_team(plg->table[target.x][target.y])) {
        return 2;
      }

      break;
    case MVT_MUST_EAT:
      if ((team & 0x0F != plg->table[target.x][target.y] & 0x0F) &&
          (plg->table[target.x][target.y] != EMPTY)) {
        return 2;
      }
      break;
    case MVT_CANT_EAT:
      if ((plg->table[target.x][target.y] == EMPTY)) {
        return 1;
      }
      break;
    default:
      break;
    }

    return 0;
  }
}

int plg_possibilities_add(plg_playground *plg, plg_possibilities *possibilities,
                          int x, int y, int mvt) {
  if (plg->turn == TEAM_BLACK) {
    x = -x;
    y = -y;
  }
  plg_pos p = {
      .x = x,
      .y = y,
  };
  p.x += plg->selection.x;
  p.y += plg->selection.y;
  int ret = plg_position_is_valid(
      plg, plg_get_team(plg->table[plg->selection.x][plg->selection.y]), p,
      mvt);
  if (!ret)
    return 0;
  possibilities->size++;
  if (possibilities->size == 1) {
    possibilities->list = malloc(sizeof(plg_pos));

  } else {
    possibilities->list = (plg_pos *)realloc(
        possibilities->list, possibilities->size * sizeof(plg_pos));
  }
  possibilities->list[possibilities->size - 1] = p;
  return ret;
}

void plg_possibilities_get_rook(plg_playground *plg) {
  int x = 0, y = 0;
  int i;
  for (i = x + 1; i < 8; i++) {
    if (plg_possibilities_add(plg, &plg->possibilities, x + i, y,
                              MVT_CAN_EAT) != 1) {
      break;
    }
  }
  for (i = x - 1; i > -8; i--) {
    if (!plg_possibilities_add(plg, &plg->possibilities, x + i, y,
                               MVT_CAN_EAT) != 1) {
      break;
    }
  }
  for (i = y - 1; i > -8; i--) {
    if (!plg_possibilities_add(plg, &plg->possibilities, x, y + i,
                               MVT_CAN_EAT) != 1) {
      break;
    }
  }
  for (i = y + 1; i < 8; i++) {
    if (plg_possibilities_add(plg, &plg->possibilities, x, y + i,
                              MVT_CAN_EAT) != 1) {
      break;
    }
  }
}

void plg_possibilities_get_bishop(plg_playground *plg) {

  int x = 1, y = 1;
  for (y = 1; y < 8; y++) {
    int ret =
        plg_possibilities_add(plg, &plg->possibilities, x, y, MVT_CAN_EAT);
    if (ret == 0 || ret == 2) {
      break;
    }
    x++;
  }
  x = 1;
  for (y = -1; y > -8; y--) {
    int ret =
        plg_possibilities_add(plg, &plg->possibilities, x, y, MVT_CAN_EAT);
    if (ret == 0 || ret == 2) {
      break;
    }
    x++;
  }
  y = 1;
  for (x = -1; x > -8; x--) {
    int ret =
        plg_possibilities_add(plg, &plg->possibilities, x, y, MVT_CAN_EAT);
    if (ret == 0 || ret == 2) {
      break;
    }
    y++;
  }
  y = -1;
  for (x = -1; x > -8; x--) {
    int ret =
        plg_possibilities_add(plg, &plg->possibilities, x, y, MVT_CAN_EAT);
    if (ret == 0 || ret == 2) {
      break;
    }
    y--;
  }
}

void plg_possibilities_get_at(plg_playground *plg) {
  if (plg->selection.x != -1 && plg->selection.y != -1) {
    switch (plg->table[plg->selection.x][plg->selection.y] & 0xF0) {
    case PAWN: {
      if (plg_possibilities_add(plg, &plg->possibilities, 0, 1, MVT_CANT_EAT))
        plg_possibilities_add(plg, &plg->possibilities, 0, 2, MVT_CANT_EAT);
      plg_possibilities_add(plg, &plg->possibilities, 1, 1, MVT_MUST_EAT);
      plg_possibilities_add(plg, &plg->possibilities, -1, 1, MVT_MUST_EAT);
    } break;
    case KNIGHT:
      plg_possibilities_add(plg, &plg->possibilities, -2, 1, MVT_CAN_EAT);
      plg_possibilities_add(plg, &plg->possibilities, -2, -1, MVT_CAN_EAT);
      plg_possibilities_add(plg, &plg->possibilities, 2, 1, MVT_CAN_EAT);
      plg_possibilities_add(plg, &plg->possibilities, 2, -1, MVT_CAN_EAT);
      plg_possibilities_add(plg, &plg->possibilities, 1, 2, MVT_CAN_EAT);
      plg_possibilities_add(plg, &plg->possibilities, -1, 2, MVT_CAN_EAT);
      plg_possibilities_add(plg, &plg->possibilities, 1, -2, MVT_CAN_EAT);
      plg_possibilities_add(plg, &plg->possibilities, -1, -2, MVT_CAN_EAT);
      break;
    case KING:
      plg_possibilities_add(plg, &plg->possibilities, -1, 1, MVT_CAN_EAT);
      plg_possibilities_add(plg, &plg->possibilities, -1, -1, MVT_CAN_EAT);
      plg_possibilities_add(plg, &plg->possibilities, 1, -1, MVT_CAN_EAT);
      plg_possibilities_add(plg, &plg->possibilities, 1, 1, MVT_CAN_EAT);
      plg_possibilities_add(plg, &plg->possibilities, 0, 1, MVT_CAN_EAT);
      plg_possibilities_add(plg, &plg->possibilities, 1, 0, MVT_CAN_EAT);
      plg_possibilities_add(plg, &plg->possibilities, 0, -1, MVT_CAN_EAT);
      plg_possibilities_add(plg, &plg->possibilities, -1, 0, MVT_CAN_EAT);
      break;
    case BISHOP:
      plg_possibilities_get_bishop(plg);
      break;
    case ROOK:
      plg_possibilities_get_rook(plg);
      break;
    case QUEEN:
      plg_possibilities_get_rook(plg);
      plg_possibilities_get_bishop(plg);
      break;
    default:
      break;
    }
  }
}
void plg_possibilities_free(plg_possibilities *possibilities) {
  if (possibilities->size > 0)
    free(possibilities->list);
  possibilities->size = 0;
}

void plg_select(plg_playground *plg, plg_pos p) {
  plg_possibilities_free(&plg->possibilities);
  if (p.x >= 0 && p.y >= 0 && p.x < 8 && p.y < 8) {
    if (plg_get_team(plg->table[p.x][p.y]) == plg->turn) {
      plg->selection = p;

      plg_possibilities_get_at(plg);
      int i;

      return;
    }
  }
  plg->selection.x = -1;
  plg->selection.y = -1;
}
void plg_move(plg_playground *plg, plg_pos from, plg_pos to) {
  plg->selection.x = -1;
  plg->selection.y = -1;
  plg_possibilities_free(&plg->possibilities);
  plg->table[to.x][to.y] = plg->table[from.x][from.y];
  plg->table[from.x][from.y] = EMPTY;
  plg->turn = plg->turn == TEAM_BLACK ? TEAM_WHITE : TEAM_BLACK;
}