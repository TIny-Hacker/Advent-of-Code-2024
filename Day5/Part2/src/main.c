#include "asm/tools.h"

#include <fileioc.h>
#include <stdint.h>
#include <graphx.h>
#include <keypadc.h>
#include <string.h>

int8_t checkRule(uint8_t num1, uint8_t num2, uint8_t *row, uint8_t pageCount) {
    bool num1Found = false;

    for (uint8_t i = 0; i < pageCount; i++) {
        if (row[i] == num1) {
            num1Found = true;
        } else if (row[i] == num2) {
            if (num1Found) {
                return i;
            } else {
                return -1;
            }
        }
    }

    return -1;
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
        bool swapped = false;

        for (int8_t i = 0; i < pageCount; i++) {
            for (unsigned int j = 0; j < rulesCount; j++) {
                if ((uint8_t)getNumber(rulesStart + j * 6) == row[i]) {
                    int8_t ruleAns = checkRule((uint8_t)getNumber(rulesStart + j * 6 + 3), row[i], row, pageCount);

                    if (ruleAns != -1) {
                        swapped = true;
                        uint8_t backup = row[i];
                        row[i] = row[ruleAns];
                        row[ruleAns] = backup;
                        i = -1;
                        break;
                    }
                } else if ((uint8_t)getNumber(rulesStart + j * 6 + 3) == row[i]) {
                    int8_t ruleAns = checkRule(row[i], (uint8_t)getNumber(rulesStart + j * 6), row, pageCount);

                    if (ruleAns != -1) {
                        swapped = true;
                        uint8_t backup = row[i];
                        row[i] = row[ruleAns];
                        row[ruleAns] = backup;
                        i = -1;
                        break;
                    }
                }
            }
        }

        if (swapped) {
            total += row[(pageCount - 1) / 2];
        }
    }

    gfx_Begin();
    gfx_PrintUInt(total, 1);
    while (!kb_AnyKey());
    gfx_End();
}
