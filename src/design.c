#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#define BOARD_W (10)
#define BOARD_H (10)

typedef struct Pos {
    int x, y;
} Pos;
typedef Pos Dir;

typedef struct Tile {
    enum {
        TileBlank,
        TileTried,
        TileHit,
    } status;
} Tile;
typedef struct Board {
    Tile  *tiles;
    size_t w, h;
} Board;
void boardInit(Board *b, size_t w, size_t h) {
    b->tiles = malloc(sizeof(Tile)*w*h);
    b->w     = w;
    b->h     = h;
}
void boardRender(Board *b) {
    for (size_t h = 0; h < b->h; h++) {
        for (size_t w = 0; w < b->w; w++) {
            Tile *tile = &b->tiles[w + h*b->w];

            switch (tile->status) {
            case TileBlank: {
                printf("+");
            } break;
            case TileTried: {
                printf("O");
            } break;
            case TileHit: {
                printf("X");
            } break;
            }
        }
        printf("\n");
    }
}

typedef struct ShipSegment {
    Pos  pos; // position of the segment
    bool hit; // is the ship segment destroyed
} ShipSegment;
typedef struct Ship {
    ShipSegment *segments;
    size_t       segments_num;
} Ship;
bool shipInit(Ship *s, Board *b, Pos pos, Dir dir, size_t length) {
    s->segments     = malloc(sizeof(ShipSegment)*length);
    s->segments_num = length;

    for (size_t i = 0; i < length; i++) {
        if (pos.x < 0 && pos.x >= b->w &&
            pos.y < 0 && pos.y >= b->h) return false;

        s->segments[i] = (ShipSegment){
            .pos = pos,
            .hit = false,
        };
        // TODO: replace with function
        pos.x += dir.x;
        pos.y += dir.y;
    }

    return true;
}

typedef struct Player {
    Board board;

    Ship  *ships;
    size_t ships_num;
} Player;
void playerInit(Player *p, size_t board_w, size_t board_h, size_t max_ships) {
    boardInit(&p->board, board_w, board_h);

    p->ships     = malloc(sizeof(Ship)*max_ships);
    p->ships_num = 0;
}
bool playerAddShip(Player *p, Pos pos, Dir dir, size_t length) {
    Ship *cur = &p->ships[p->ships_num++];

    return shipInit(cur, &p->board, pos, dir, length);
}

int main() {
    Player player[2];
    playerInit(&player[0], BOARD_W, BOARD_H, 128);
    playerInit(&player[1], BOARD_W, BOARD_H, 128);
    boardRender(&player[0].board);
}