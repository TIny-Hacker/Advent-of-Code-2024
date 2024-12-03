#include "asm/tools.h"

#include <fileioc.h>
#include <stdint.h>
#include <graphx.h>
#include <keypadc.h>
#include <string.h>
#include <stdio.h>

int main(void) {
    uint8_t slot = ti_Open("Input", "r");
    char *tok = ti_GetDataPtr(slot);
    char *endOfFile = tok + ti_GetSize(slot);
    ti_Close(slot);

    unsigned long total = 0;
    bool enabled = true;

    while (tok < endOfFile) {
        for (; *tok != 'm' && *tok != 'd'; tok++);

        if (!strncmp(tok, "do()", 4)) {
            tok += 4;
            enabled = true;
        }

        if (!strncmp(tok, "don\'t()", 7)) {
            tok += 7;
            enabled = false;
        }

        if (enabled) {
            if (strncmp(tok, "mul(", 4)) {
                tok++;
                continue;
            }

            tok += 4;

            uint8_t i = 0;

            for (; tok[i] >= '0' && tok[i] <= '9' && tok + i < endOfFile; i++);

            if (i > 4 || !i || tok[i] != ',') {
                continue;
            }

            unsigned int term1 = getNumber(tok);
            tok += i + 1;

            for (i = 0; tok[i] >= '0' && tok[i] <= '9' && tok + i < endOfFile; i++);

            if (i > 4 || !i || tok[i] != ')') {
                continue;
            }

            total += term1 * getNumber(tok);
            tok += i;
        } else {
            tok++;
        }
    }

    printf("%lu", total);
    while (!kb_AnyKey());
}
