#include "asm/tools.h"

#include <fileioc.h>
#include <stdint.h>
#include <graphx.h>
#include <keypadc.h>

#include <debug.h>

int main(void) {
    uint8_t slot = ti_Open("Input", "r");
    char *num = ti_GetDataPtr(slot);
    char *endOfFile = num + ti_GetSize(slot);
    ti_Close(slot);

    unsigned int total = 0;
    uint8_t lastNum = 0;
    bool lessThan;

    while (num < endOfFile) {
        dbg_printf("%s\n", num);
        lastNum = getNumber(num);
        for (; *num != ' '; num++);
        dbg_printf("- ");

        if (getNumber(num + 1) < lastNum) {
            lessThan = true;
        } else {
            lessThan = false;
        }

        total++;

        while (num < endOfFile && *num != '\0') {
            num++;

            if ((getNumber(num) < lastNum && lessThan) || (getNumber(num) > lastNum && !lessThan)) {
                if (abs((int)(getNumber(num) - lastNum)) < 4) {
                    lastNum = getNumber(num);
                    dbg_printf("- ");
                    for (; *num != ' ' && *num != '\0'; num++);
                } else {
                    dbg_printf("X ");
                    total--;
                    for (; *num != '\0'; num++);
                }
            } else {
                dbg_printf("! ");
                total--;
                for (; *num != '\0'; num++);
            }
        }

        num++;

        dbg_printf("\n");
    }

    gfx_Begin();
    gfx_SetTextXY(0, 0);
    gfx_PrintUInt(total, 1);
    while (!kb_AnyKey());
    gfx_End();
}
