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
    bool safe = true;

    while (num < endOfFile) {
        char *backupNum = num;
        dbg_printf("%s\n", num);

        for (uint8_t i = 0; i < 8; i++) {
            uint8_t j = 0;
            safe = true;
            num = backupNum;

            lastNum = getNumber(num) + 1;

            while (num < endOfFile && *num != '\0') {
                if (j == i) {
                    for (; *num != ' ' && *num != '\0'; num++);

                    if (!j) {
                        lastNum = getNumber(num + 1) + 1;
                    }
                } else if ((getNumber(num) < lastNum) && abs((int)(getNumber(num) - lastNum)) < 4) {
                    lastNum = getNumber(num);
                    dbg_printf("- ");
                    for (; *num != ' ' && *num != '\0'; num++);
                } else {
                    safe = false;
                    dbg_printf("X ");
                    for (; *num != '\0'; num++);
                }

                if (*num != '\0') {
                    num++;
                }

                j++;
            }

            if (safe) {
                num++;
                total++;
                break;
            }

            safe = true;
            num = backupNum;
            lastNum = getNumber(num) - 1;
            j = 0;

            while (num < endOfFile && *num != '\0') {
                if (j == i) {
                    for (; *num != ' ' && *num != '\0'; num++);

                    if (!j) {
                        lastNum = getNumber(num + 1) - 1;
                    }
                } else if ((getNumber(num) > lastNum) && abs((int)(getNumber(num) - lastNum)) < 4) {
                    lastNum = getNumber(num);
                    dbg_printf("- ");
                    for (; *num != ' ' && *num != '\0'; num++);
                } else {
                    safe = false;
                    dbg_printf("X ");
                    for (; *num != '\0'; num++);
                }

                if (*num != '\0') {
                    num++;
                }

                j++;
            }

            num++;

            if (safe) {
                total++;
                break;
            }

        dbg_printf("\n");
        }

        dbg_printf("\n");
    }

    gfx_Begin();
    gfx_SetTextXY(0, 0);
    gfx_PrintUInt(total, 1);
    while (!kb_AnyKey());
    gfx_End();
}
