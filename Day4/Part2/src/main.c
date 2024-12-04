#include <fileioc.h>
#include <stdint.h>
#include <graphx.h>
#include <keypadc.h>
#include <string.h>

bool search(char *tok, char *search, uint8_t advance) {
    for (uint8_t i = 0; i < 3; i++) {
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
            if (j < columns - 2) {
                if (search(tok + j + i * (columns + 1), "MAS", columns + 2) || search(tok + j + i * (columns + 1), "SAM", columns + 2)) { // What is this
                    if (search(tok + j + 2 + i * (columns + 1), "MAS", columns) || search(tok + j + 2 + i * (columns + 1), "SAM", columns)) {
                        total++;
                    }
                }
            }
        }
    }

    gfx_Begin();
    gfx_PrintInt(total, 1);
    while (!kb_AnyKey());
    gfx_End();
}
