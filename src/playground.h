#ifndef PLAYGROUND_H
#define PLAYGROUND_H

#define WHITE_KING 0
#define WHITE_QUEEN 1
#define WHITE_ROOK 2
#define WHITE_BISHOP 3
#define WHITE_KNIGHT 4
#define WHITE_PAWN 5
#define BLACK_KING 6
#define BLACK_QUEEN 7
#define BLACK_ROOK 8
#define BLACK_BISHOP 9
#define BLACK_KNIGHT 10
#define BLACK_PAWN 11
#define EMPTY 12

#define TEAM_WHITE 0
#define TEAM_BLACK 1

#define BLACK_TILE 1
#define WHITE_TILE 2

typedef struct playground plg_playground;
struct playground {
    char table [8][8];
    char turn;
};
plg_playground plg_new();
void plg_draw(plg_playground *);


#endif