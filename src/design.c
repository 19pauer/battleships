#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#define BOARD_W (10)
#define BOARD_H (10)

typedef struct Pos {
    int x, y;
} Pos;
typedef Pos Dir;

typedef struct Tile {
    enum {
        TileBlank,
        TileShip, // one of your ships
        TileShipHit, // one of your ships is hit
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
    printf("  ");
    for (long w = 0; w < b->w; w++) {
        printf("\e[4m%c\e[0m", 'A' + w);
    } printf("\n");
    for (long h = b->h-1; h >= 0; h--) {
        printf("%c|", '0' + (b->h - h - 1));
        for (long w = 0; w < b->w; w++) {
            Tile *tile = &b->tiles[w + h*b->w];

            switch (tile->status) {
            case TileBlank: {
                printf(" ");
            } break;
            case TileShip: {
                printf("@");
            } break;
            case TileShipHit: {
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
void playerDraw(Player *p) {
    // clear own ships from board
    for (size_t x = 0; x < p->board.w; x++) {
        for (size_t y = 0; y < p->board.h; y++) {
            Tile *t = &p->board.tiles[x + y*p->board.w];
            switch (t->status) {
            case TileShip:
            case TileShipHit:
                t->status = TileBlank;
                break;
            }
        }
    }

    for (size_t i = 0; i < p->ships_num; i++) {
        Ship *s = &p->ships[i];
        for (size_t j = 0; j < s->segments_num; j++) {
            ShipSegment *e = &s->segments[j];
            p->board.tiles[e->pos.x + e->pos.y*p->board.w].status = e->hit ? TileShipHit : TileShip;
        }
    }
}

Pos getInputPos(const char *msg) {
    Pos r;
    printf("%s\n", msg);
    printf("> ");
    scanf("%c:%i", &r.x, &r.y);
    r.x = toupper(r.x)-'A';
    return r;
}

int main() {
    Player player[2];
    playerInit(&player[0], BOARD_W, BOARD_H, 128);
    playerInit(&player[1], BOARD_W, BOARD_H, 128);
    playerAddShip(&player[0], (Pos){0, 0}, (Dir){0, 1}, 5);
    playerAddShip(&player[0], (Pos){9, 9}, (Dir){0, -1}, 5);
    playerDraw(&player[0]); playerDraw(&player[1]);
    boardRender(&player[0].board);

    Pos p = getInputPos("Enter position");
    printf("%i, %i\n", p.x, p.y);
}