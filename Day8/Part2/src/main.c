#include <fileioc.h>
#include <stdint.h>
#include <graphx.h>
#include <keypadc.h>
#include <string.h>

#define os_PixelShadow          ((uint8_t *)0xD031F6)

struct antenna_t {
    uint8_t type;
    uint8_t x;
    uint8_t y;
};

uint8_t placeAntinode(struct antenna_t antenna1, struct antenna_t antenna2, uint8_t xMax, uint8_t yMax) {
    int xDiff = (int)(antenna2.x - antenna1.x);
    int yDiff = (int)(antenna2.y - antenna1.y);
    uint8_t placed = 0;

    for (int y = antenna2.y, x = antenna2.x; x >= 0 && x < xMax && y >= 0 && y < yMax; y += yDiff, x += xDiff) {
        if ('#' != gfx_GetPixel(x * 4, y * 4)) {
            gfx_FillRectangle_NoClip(x * 4, y * 4, 4, 4);
            placed++;
        }
    }

    return placed;
}

int main(void) {
    gfx_Begin();

    uint8_t slot = ti_Open("Input", "r");
    char *tok = ti_GetDataPtr(slot);
    char *endOfFile = tok + ti_GetSize(slot);
    ti_Close(slot);

    unsigned int total = 0;
    uint8_t columns = strlen(tok);
    uint8_t rows = 0;

    struct antenna_t *antennas = (struct antenna_t *)os_PixelShadow;
    unsigned int count = 0;

    for (; tok < endOfFile; rows++, tok++) {
        for (uint8_t x = 0; x < columns; x++, tok++) {
            gfx_SetColor(*tok);

            if (*tok != '.') {
                antennas[count].type = *tok;
                antennas[count].x = x;
                antennas[count].y = rows;
                count++;
            }

            gfx_FillRectangle_NoClip(x * 4, rows * 4, 4, 4); // Accidental visualization 2 electric boogaloo
        }
    }

    gfx_SetColor('#');

    for (uint8_t i = 0; i < count - 1; i++) {
        for (uint8_t j = i + 1; j < count; j++) {
            if (antennas[i].type == antennas[j].type) {
                total += placeAntinode(antennas[i], antennas[j], columns, rows);
                total += placeAntinode(antennas[j], antennas[i], columns, rows);
            }
        }
    }

    gfx_PrintUInt(total, 1);
    while (!kb_AnyKey());
    gfx_End();
}
