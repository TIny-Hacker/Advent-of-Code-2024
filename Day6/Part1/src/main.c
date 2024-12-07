#include <fileioc.h>
#include <stdint.h>
#include <graphx.h>
#include <keypadc.h>
#include <string.h>

enum {
    UP,
    RIGHT,
    DOWN,
    LEFT,
};

struct guard_t {
    unsigned int x;
    unsigned int y;
    uint8_t direction;
};

void setColors(void) {
    uint8_t color[2] = {0x43, 0x08};
    gfx_SetPalette(color, 2, 255);
    color[0] = 0xBA;
    color[1] = 0xD6;
    gfx_SetPalette(color, 2, '.');
    color[0] = 0xA2;
    color[1] = 0x99;
    gfx_SetPalette(color, 2, '#');
    color[0] = 0x21;
    color[1] = 0xC0;
    gfx_SetPalette(color, 2, 'X');
}

bool moveGuard(struct guard_t *guard, unsigned int *total, uint8_t rows, uint8_t columns) {
    switch (guard->direction) {
        case UP:
            if ('#' == gfx_GetPixel(guard->x, guard->y - 1)) {
                guard->direction += 1;
            } else {
                guard->y -= 1;
            }

            break;
        case RIGHT:
            if ('#' == gfx_GetPixel(guard->x + 1, guard->y)) {
                guard->direction += 1;
            } else {
                guard->x += 1;
            }

            break;
        case DOWN:
            if ('#' == gfx_GetPixel(guard->x, guard->y + 1)) {
                guard->direction += 1;
            } else {
                guard->y += 1;
            }

            break;
        case LEFT:
            if ('#' == gfx_GetPixel(guard->x - 1, guard->y)) {
                guard->direction = UP;
            } else {
                guard->x -= 1;
            }

            break;
    }

    if ('X' != gfx_GetPixel(guard->x, guard->y)) {
        *total += 1;
        gfx_SetColor('X');
        gfx_SetPixel(guard->x, guard->y);
    }

    return !guard->x || guard->x == columns - 1 || !guard->y || guard->y == rows - 1;
}

int main(void) {
    gfx_Begin();
    setColors(); // Nicer colors for visuals

    uint8_t slot = ti_Open("Input", "r");
    char *tok = ti_GetDataPtr(slot);
    char *endOfFile = tok + ti_GetSize(slot);
    ti_Close(slot);

    unsigned int total = 1;
    uint8_t columns = strlen(tok);
    uint8_t rows = 0;

    struct guard_t guard = {
        0,
        0,
        UP,
    };

    for (; tok < endOfFile; rows++, tok++) {
        for (uint8_t x = 0; x < columns; x++, tok++) {
            gfx_SetColor(*tok);

            if (*tok == '^') {
                guard.x = x;
                guard.y = rows;
                gfx_SetColor('X');
            }

            gfx_SetPixel(x, rows); // Fun for the family with a visualization
        }
    }

    while (!moveGuard(&guard, &total, rows, columns));

    gfx_PrintUInt(total, 1);
    while (!kb_AnyKey());
    gfx_End();
}
