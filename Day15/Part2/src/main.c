#include <fileioc.h>
#include <stdint.h>
#include <graphx.h>
#include <keypadc.h>
#include <string.h>

#define UP      '^'
#define DOWN    'v'
#define LEFT    '<'
#define RIGHT   '>'

bool move(unsigned int x, unsigned int y, uint8_t direction) {
    uint8_t type = gfx_GetPixel(x, y);

    if (type == '.') {
        return true;
    }

    if (type == '#') {
        return false;
    }

    unsigned int newX = x;
    unsigned int newY = y;

    switch (direction) {
        case UP:
            newY--;
            break;
        case DOWN:
            newY++;
            break;
        case LEFT:
            newX--;
            break;
        case RIGHT:
            newX++;
            break;
        default:
            break;
    }

    gfx_SetColor('.');
    gfx_SetPixel(x, y);

    if (type == '[' && (direction == UP || direction == DOWN)) {
        if (move(x, newY, direction) && move(x + 1, y, direction)) {
            gfx_SetColor(type);
            gfx_SetPixel(newX, newY);
            return true;
        }
    } else if (type == ']' && (direction == UP || direction == DOWN)) {
        if (move(x, newY, direction) && move(x - 1, y, direction)) {
            gfx_SetColor(type);
            gfx_SetPixel(newX, newY);
            return true;
        }
    } else if (move(newX, newY, direction)) {
        gfx_SetColor(type);
        gfx_SetPixel(newX, newY);
        return true;
    }

    return false;
}

int main(void) {
    gfx_Begin();

    uint8_t slot = ti_Open("Input", "r");
    char *tok = ti_GetDataPtr(slot);
    char *endOfFile = tok + ti_GetSize(slot);
    ti_Close(slot);

    unsigned int x  = 0;
    unsigned int y  = 0;
    unsigned int total = 0;

    uint8_t columns = strlen(tok) * 2;
    uint8_t rows = 0;

    for (; *tok != '\0'; tok++, rows++) {
        for (uint8_t j = 0; j < columns; j++, tok++) {
            gfx_SetColor(*tok);

            if (*tok == '#' || *tok == '.') {
                gfx_SetPixel(j++, rows);
                gfx_SetPixel(j, rows);
            } else if (*tok == '@') {
                x = j;
                y = rows;

                gfx_SetPixel(j++, rows);
                gfx_SetColor('.');
                gfx_SetPixel(j, rows);
            } else {
                gfx_SetColor('[');
                gfx_SetPixel(j++, rows);
                gfx_SetColor(']');
                gfx_SetPixel(j, rows);
            }
        }
    }

    tok++;

    for (; tok < endOfFile; tok++) {
        if (*tok != '\0') {
            gfx_BlitScreen();
            if (move(x, y, *tok)) {
                gfx_BlitScreen();
                switch (*tok) {
                    case UP:
                        y--;
                        break;
                    case DOWN:
                        y++;
                        break;
                    case LEFT:
                        x--;
                        break;
                    case RIGHT:
                        x++;
                        break;
                    default:
                        break;
                }
            }
            gfx_BlitBuffer();
        }
    }

    for (uint8_t i = 0; i < columns; i++) {
        for (uint8_t j = 0; j < rows; j++) {
            if (gfx_GetPixel(i, j) == '[') {
                total += 100 * j + i;
            }
        }
    }

    gfx_SetTextXY(0, 230);
    gfx_PrintUInt(total, 1);
    while (!kb_AnyKey());
    gfx_End();
}
