#include "asm/tools.h"

#include <fileioc.h>
#include <stdint.h>
#include <graphx.h>
#include <keypadc.h>
#include <string.h>

#define os_PixelShadow      ((uint8_t *)0xD031F6)

int main(void) {
    uint8_t slot = ti_Open("Input", "r");
    char *num = ti_GetDataPtr(slot);
    char *endOfFile = num + ti_GetSize(slot);
    ti_Close(slot);

    unsigned int *col1 = (unsigned int *)os_PixelShadow;
    unsigned int *col2 = (unsigned int *)os_PixelShadow + 4200;

    unsigned int i;

    for (i = 0; num < endOfFile; i++) {
        col1[i] = getNumber(num);

        for (; *num != ' '; num++);
        num += 3;

        col2[i] = getNumber(num);

        for (; *num != '\0'; num++);
        num++;

        unsigned int backup = 0; // Time for a switcheroo

        if (i) {
            for (unsigned int j = i; j; j--) {
                if (col1[j] < col1[j - 1]) {
                    backup = col1[j - 1];
                    col1[j - 1] = col1[j];
                    col1[j] = backup;
                }

                if (col2[j] < col2[j - 1]) {
                    backup = col2[j - 1];
                    col2[j - 1] = col2[j];
                    col2[j] = backup;
                }
            }
        }
    }

    unsigned int total = 0;

    for (i = 0; col1[i]; i++) {
        int diff = col2[i] - col1[i];

        if (diff < 0) {
            diff = -diff;
        }

        total += diff;
    }


    gfx_Begin();
    gfx_SetTextXY(0, 0);
    gfx_PrintUInt(total, 1);
    while (!kb_AnyKey());
    gfx_End();
}
