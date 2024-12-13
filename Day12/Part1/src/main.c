#include <fileioc.h>
#include <stdint.h>
#include <graphx.h>
#include <keypadc.h>
#include <string.h>

#include <debug.h>

uint8_t columns;
uint8_t rows;

struct plot_t {
    uint8_t type;
    unsigned int area;
    unsigned int perimeter;
} plot;

void getPlot(int x, int y) {
    plot.area += 1;
    gfx_SetColor(224);
    gfx_SetPixel(x, y);

    uint8_t scan = 0;

    if (x) {
        scan = gfx_GetPixel(x - 1, y);

        if (scan == plot.type) {
            getPlot(x - 1, y);
        } else if (scan != 224) {
            plot.perimeter += 1;
        }
    } else {
        plot.perimeter += 1;
    }

    if (y) {
        scan = gfx_GetPixel(x, y - 1);

        if (scan == plot.type) {
            getPlot(x, y - 1);
        } else if (scan != 224) {
            plot.perimeter += 1;
        }
    } else {
        plot.perimeter += 1;
    }

    if (x < columns - 1) {
        scan = gfx_GetPixel(x + 1, y);

        if (scan == plot.type) {
            getPlot(x + 1, y);
        } else if (scan != 224) {
            plot.perimeter += 1;
        }
    } else {
        plot.perimeter += 1;
    }

    if (y < rows - 1) {
        scan = gfx_GetPixel(x, y + 1);

        if (scan == plot.type) {
            getPlot(x, y + 1);
        } else if (scan != 224) {
            plot.perimeter += 1;
        }
    } else  {
        plot.perimeter += 1;
    }
}

int main(void) {
    asm("ld ($D00429 + 200), sp\n\tld sp, $D0B5F6");
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
            gfx_SetPixel(x, rows);
        }
    }

    unsigned int total = 0;

    for (uint8_t y = 0; y < rows; y++) {
        for (uint8_t x = 0; x < columns; x++) {
            if (222 != gfx_GetPixel(x, y)) {
                plot.type = gfx_GetPixel(x, y);
                plot.area = 0;
                plot.perimeter = 0;
                getPlot(x, y);
                dbg_printf("Area: %d | Perimeter: %d\n", plot.area, plot.perimeter);
                dbg_Debugger();
                gfx_FloodFill(x, y, 222);
                total += plot.area * plot.perimeter;
            }
        }
    }

    asm("ld sp, ($D00429 + 200)");

    gfx_SetTextFGColor(85);
    gfx_SetTextXY(0, 230);
    gfx_PrintUInt(total, 1);
    while (!kb_AnyKey());
    gfx_End();
}
