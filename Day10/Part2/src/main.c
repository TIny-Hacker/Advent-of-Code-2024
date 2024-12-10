#include <fileioc.h>
#include <stdint.h>
#include <graphx.h>
#include <keypadc.h>
#include <string.h>

uint8_t columns;
uint8_t rows;

int getScore(int x, int y, int8_t previousElevation) {
    int8_t elevation = gfx_GetPixel(x * 2, y * 2) - '0';

    if (elevation - previousElevation != 1) {
        return 0;
    }

    gfx_SetColor(elevation);
    gfx_FillRectangle_NoClip(x * 2 + 160, y * 2, 2, 2);

    if (elevation == 9) {
        return 1;
    }

    int surroundScore = 0;

    if (x) {
        surroundScore += getScore(x - 1, y, elevation);
    }

    if (x < rows - 1) {
        surroundScore += getScore(x + 1, y, elevation);
    }

    if (y) {
        surroundScore += getScore(x, y - 1, elevation);
    }

    if (y < columns - 1) {
        surroundScore += getScore(x, y + 1, elevation);
    }

    return surroundScore;
}

int main(void) {
    gfx_Begin();
    gfx_ZeroScreen();

    uint8_t slot = ti_Open("Input", "r");
    char *tok = ti_GetDataPtr(slot);
    char *endOfFile = tok + ti_GetSize(slot);
    ti_Close(slot);

    columns = strlen(tok);
    rows = 0;

    for (; tok < endOfFile; rows++, tok++) {
        for (uint8_t x = 0; x < columns; x++, tok++) {
            gfx_SetColor(*tok);
            gfx_FillRectangle_NoClip(x * 2, rows * 2, 2, 2);
        }
    }

    unsigned int total = 0;

    for (int x = 0; x < columns; x++) {
        for (int y = 0; y < rows; y++) {
            if ('0' == gfx_GetPixel(x * 2, y * 2)) {
                gfx_BlitScreen();
                total += getScore(x, y, -1);
                gfx_SetColor(0);
                gfx_BlitBuffer();
            }
        }
    }

    gfx_SetTextFGColor(85);
    gfx_SetTextXY(0, 230);
    gfx_PrintUInt(total, 1);
    while (!kb_AnyKey());
    gfx_End();
}
