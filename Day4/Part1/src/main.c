#include <fileioc.h>
#include <stdint.h>
#include <graphx.h>
#include <keypadc.h>
#include <string.h>

bool searchOthers(char *tok, char *search, uint8_t advance) {
    for (uint8_t i = 0; i < 4; i++) {
        if (*tok != search[i]) {
            return false;
        }

        tok += advance;
    }

    return true;
}

int main(void) {
    uint8_t slot = ti_Open("Input", "r");
    char *tok = ti_GetDataPtr(slot);
    char *endOfFile = tok + ti_GetSize(slot);
    ti_Close(slot);
    uint8_t columns = strlen(tok);
    uint8_t rows = (endOfFile - tok + 1) / columns;
    unsigned int total = 0;

    for (uint8_t i = 0; i < rows; i++) {
        for (uint8_t j = 0; j < columns; j++) {
            if (j < columns - 3) {
                if (!strncmp(tok + j + i * (columns + 1), "XMAS", 4)) {
                    total++;
                } else if (!strncmp(tok + j + i * (columns + 1), "SAMX", 4)) {
                    total++;
                }
            }

            if (i < rows - 3) {
                if (j < columns - 3) {
                    total += searchOthers(tok + j + i * (columns + 1), "XMAS", columns + 2);
                    total += searchOthers(tok + j + i * (columns + 1), "SAMX", columns + 2);
                }

                if (j > 2) {
                    total += searchOthers(tok + j + i * (columns + 1), "XMAS", columns);
                    total += searchOthers(tok + j + i * (columns + 1), "SAMX", columns);
                }

                total += searchOthers(tok + j + i * (columns + 1), "XMAS", columns + 1);
                total += searchOthers(tok + j + i * (columns + 1), "SAMX", columns + 1);
            }
        }
    }

    gfx_Begin();
    gfx_PrintInt(total, 1);
    while (!kb_AnyKey());
    gfx_End();
}
