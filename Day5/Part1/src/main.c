#include "asm/tools.h"

#include <fileioc.h>
#include <stdint.h>
#include <graphx.h>
#include <keypadc.h>
#include <string.h>

bool checkRule(uint8_t num1, uint8_t num2, uint8_t *row, uint8_t pageCount) {
    bool num1Found = false;

    for (uint8_t i = 0; i < pageCount; i++) {
        if (row[i] == num1) {
            num1Found = true;
        } else if (row[i] == num2 && num1Found) {
            return false;
        }
    }

    return true;
}

int main(void) {
    uint8_t slot = ti_Open("Input", "r");
    char *tok = ti_GetDataPtr(slot);
    char *endOfFile = tok + ti_GetSize(slot);
    ti_Close(slot);

    unsigned int rulesCount = 0;
    char *rulesStart = tok;

    for (; *tok !='\0'; tok += 6) {
        rulesCount++;
    }

    tok++;
    uint8_t row[24];
    unsigned int total = 0;

    while (tok < endOfFile) {
        memset(row, 0, 24);
        uint8_t pageCount = 0;

        for (; tok < endOfFile; tok++) {
            row[pageCount] = (uint8_t)getNumber(tok);
            pageCount++;
            tok += 2;

            if (*tok == '\0') {
                break;
            }
        }

        tok++;
        bool rowValid = true;

        for (uint8_t i = 0; i < pageCount && rowValid; i++) {
            for (unsigned int j = 0; j < rulesCount; j++) {
                if (rowValid) {
                    if (getNumber(rulesStart + j * 6) == row[i]) {
                        rowValid = checkRule(getNumber(rulesStart + j * 6 + 3), row[i], row, pageCount);
                    } else if (getNumber(rulesStart + j * 6 + 3) == row[i]) {
                        rowValid = checkRule(row[i], getNumber(rulesStart + j * 6), row, pageCount);
                    }
                } else {
                    break;
                }
            }
        }

        if (rowValid) {
            total += row[(pageCount - 1) / 2];
        }
    }

    gfx_Begin();
    gfx_PrintUInt(total, 1);
    while (!kb_AnyKey());
    gfx_End();
}
