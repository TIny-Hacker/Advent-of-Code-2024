#include <fileioc.h>
#include <stdint.h>
#include <graphx.h>
#include <keypadc.h>
#include <string.h>
#include <sys/timers.h>

#define UP      '^'
#define DOWN    'v'
#define LEFT    '<'
#define RIGHT   '>'

void setColors(void) {
    uint8_t color[2] = {0x7E, 0x6F};
    gfx_SetPalette(color, 2, '#');
    color[0] = 0xE3;
    color[1] = 0x61;
    gfx_SetPalette(color, 2, 'O');
    color[0] = 0x00;
    color[1] = 0x00;
    gfx_SetPalette(color, 2, '.');
    color[0] = 0x20;
    color[1] = 0xB0;
    gfx_SetPalette(color, 2, '@');
}

bool move(unsigned int x, unsigned int y, uint8_t direction) {
    if ('.' == gfx_GetPixel(x * 4, y * 4)) {
        return true;
    }

    if ('#' == gfx_GetPixel(x * 4, y * 4)) {
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

    if (move(newX, newY, direction)) {
        gfx_SetColor(gfx_GetPixel(x * 4, y * 4));
        gfx_FillRectangle_NoClip(newX * 4, newY * 4, 4, 4);
        gfx_SetColor('.');
        gfx_FillRectangle_NoClip(x * 4, y * 4, 4, 4);
        return true;
    }

    return false;
}

int main(void) {
    gfx_Begin();
    gfx_ZeroScreen();
    setColors();

    uint8_t slot = ti_Open("Input", "r");
    char *tok = ti_GetDataPtr(slot);
    char *endOfFile = tok + ti_GetSize(slot);
    ti_Close(slot);

    unsigned int x  = 0;
    unsigned int y  = 0;
    unsigned int total = 0;

    uint8_t columns = strlen(tok);
    uint8_t rows = 0;

    for (; *tok != '\0'; tok++, rows++) {
        for (uint8_t j = 0; j < columns; j++, tok++) {
            gfx_SetColor(*tok);
            gfx_FillRectangle_NoClip(j * 4, rows * 4, 4, 4);

            if (*tok == '@') {
                x = j;
                y = rows;
            }
        }
    }

    tok++;

    for (; tok < endOfFile; tok++) {
        if (*tok != '\0') {
            // delay(2);
            if (move(x, y, *tok)) {
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
        }
    }

    for (uint8_t i = 0; i < columns; i++) {
        for (uint8_t j = 0; j < rows; j++) {
            if (gfx_GetPixel(i * 4, j * 4) == 'O') {
                total += 100 * j + i;
            }
        }
    }

    gfx_SetTextFGColor(222);
    gfx_SetTextXY(0, 230);
    gfx_PrintUInt(total, 1);
    while (!kb_AnyKey());
    gfx_End();
}
