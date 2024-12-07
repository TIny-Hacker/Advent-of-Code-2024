#include <fileioc.h>
#include <stdint.h>
#include <graphx.h>
#include <keypadc.h>
#include <string.h>

#include <debug.h>

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

bool moveGuard(struct guard_t *guard, uint8_t rows, uint8_t columns, unsigned int *checkCount) {
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

    if (checkCount != NULL && 'X' != gfx_GetPixel(guard->x, guard->y)) {
        *checkCount += 1;
        gfx_SetColor('X');
        gfx_SetPixel(guard->x, guard->y);
    }

    return !guard->x || guard->x == columns - 1 || !guard->y || guard->y == rows - 1;
}

int main(void) {
    gfx_Begin();
    uint8_t slot = ti_Open("Input", "r");
    char *tok = ti_GetDataPtr(slot);
    char *endOfFile = tok + ti_GetSize(slot);
    ti_Close(slot);

    unsigned int checkCount = 1;
    unsigned int total = 0;
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

            gfx_SetPixel(x, rows);
        }
    }

    struct guard_t backup = guard;

    while (!moveGuard(&guard, rows, columns, &checkCount));

    unsigned int checked = 0;

    for (uint8_t x = 0; x < columns; x++) {
        for (uint8_t y = 0; y < rows; y++) {
            if ('X' == gfx_GetPixel(x, y)) {
                gfx_SetColor('#');
                gfx_SetPixel(x, y);
                unsigned int i = 0;
                guard = backup;

                for (; i < 16000 && !moveGuard(&guard, rows, columns, NULL); i++); // Bootleg infinite loop detection

                if (i == 16000) { // See above
                    total++;
                }

                gfx_SetColor('.');
                gfx_SetPixel(x, y);
                dbg_printf("Checked %d / %d\n", ++checked, checkCount); // For those who are impatient
            }
        }
    }

    gfx_PrintUInt(total, 1);
    while (!kb_AnyKey());
    gfx_End();
}
