#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
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
    for (size_t w = 0; w < b->w; w++) {
        printf("\e[4m%c\e[0m", (char)('0' + w));
    } printf("\n");
    for (size_t h = 0; h < b->h; h++) {
        printf("%c|", (char)('0' + h));
        for (size_t w = 0; w < b->w; w++) {
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
        if (pos.x < 0 || pos.x >= (int)b->w ||
            pos.y < 0 || pos.y >= (int)b->h) return false;

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
void playerDraw(Player *p);
void playerInit(Player *p, size_t board_w, size_t board_h, size_t max_ships) {
    boardInit(&p->board, board_w, board_h);

    p->ships     = malloc(sizeof(Ship)*max_ships);
    p->ships_num = 0;
}
bool playerAddShip(Player *p, Pos pos, Dir dir, size_t length) {
    Ship cur = {};
    if (!shipInit(&cur, &p->board, pos, dir, length)) return false;
    playerDraw(p);

    for (size_t i = 0; i < cur.segments_num; i++) {
        if (p->board.tiles[cur.segments->pos.x + cur.segments->pos.y*p->board.w].status == TileShip) return false;
    }

    p->ships[p->ships_num++] = cur;
    return true;
}
size_t playerNonHitShipTiles(Player *p) {
    size_t count = 0;
    for (size_t i = 0; i < p->ships_num; i++) {
        for (size_t s = 0; s < p->ships[i].segments_num; s++) {
            if (!p->ships[i].segments[s].hit) count++;
        }
    }
    return count;
}
void playerTryHit(Player *p, Board *b, Pos pos) { // this player attempted to hit another
    bool hit = false;
    for (size_t i = 0; i < p->ships_num; i++) {
        for (size_t s = 0; s < p->ships[i].segments_num; s++) {
            if (p->ships[i].segments[s].pos.x == pos.x &&
                p->ships[i].segments[s].pos.y == pos.y) {
                p->ships[i].segments[s].hit = true;
                hit = true;
                break;
            }
        }
    }

    if (!hit) {
        b->tiles[pos.x + pos.y*b->w].status = TileTried;
    }
}
void playerHit(Player *p, Board *b, Pos pos) { // this player was hit by another
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
            default: break;
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

Pos getInputPos() {
    Pos r;
    printf("> ");
    scanf("%i:%i", &r.x, &r.y);
    return r;
}
Dir getInputDir() {
    printf("> ");
    char str[128+1];
    scanf("%s", str);

    if (strcmp("up",    str) == 0) return (Dir){ 0, -1};
    if (strcmp("down",  str) == 0) return (Dir){ 0,  1};
    if (strcmp("left",  str) == 0) return (Dir){-1,  0};
    if (strcmp("right", str) == 0) return (Dir){ 1,  0};

    return (Dir){0, 0};
}
void setupPlayer(Player *p) {
    struct {
        const char *name;
        size_t      len;
    } ships[] = {
        { .name = "carrier",     .len = 5 },
        { .name = "battleship",  .len = 4 },
        { .name = "destroyer",   .len = 3 },
        { .name = "submarine",   .len = 3 },
        { .name = "patrol boat", .len = 2 },
    }; size_t num_ships = sizeof(ships)/sizeof(ships[0]);

    // ask the player to input the positions of each ship
    for (size_t i = 0; i < num_ships; i++) {
        playerDraw(p);
        boardRender(&p->board);
        // ensure the input is valid
        goto after_redo;
    redo:
        printf("You entered invalid data, TRY AGAIN\n");
    after_redo:
        printf("enter the position of the %s\n", ships[i].name);
        Pos pos = getInputPos();
        printf("\n");
        printf("enter the direction of the %s\n", ships[i].name);
        Dir dir = getInputDir();
        printf("\n");
        if (dir.x == 0 && dir.y == 0) goto redo;
        if (!playerAddShip(p, pos, dir, ships[i].len)) goto redo;
    }
    playerDraw(p);
    boardRender(&p->board);
}

int main() {
    Player player[2];
    playerInit(&player[0], BOARD_W, BOARD_H, 128);
    playerInit(&player[1], BOARD_W, BOARD_H, 128);
    printf("-- Setup player 1 --\n");
    setupPlayer(&player[0]);
    printf("-- Setup player 2 --\n");
    setupPlayer(&player[1]);

    // main loop
    while (1) {
        if (playerNonHitShipTiles(&player[0]) <= 0) {
            printf("Player 1 won\n");
        }
        if (playerNonHitShipTiles(&player[1]) <= 0) {
            printf("Player 2 won\n");
        }
    }
}
