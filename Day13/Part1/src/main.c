#include <fileioc.h>
#include <stdint.h>
#include <graphx.h>
#include <keypadc.h>
#include <string.h>

int main(void) {
    gfx_Begin();

    uint8_t slot = ti_Open("Input", "r");
    char *tok = ti_GetDataPtr(slot);
    char *endOfFile = tok + ti_GetSize(slot);
    ti_Close(slot);

    unsigned int total = 0;

    while (tok < endOfFile) {
        int AX = strtol(tok + 12, &tok, 10);
        int AY = strtol(tok + 4, &tok, 10);
        int BX = strtol(tok + 13, &tok, 10);
        int BY = strtol(tok + 4, &tok, 10);
        int X = strtol(tok + 10, &tok, 10);
        int Y = strtol(tok + 4, &tok, 10);

        tok += 2;

        int A = (X * BY - Y * BX) / (AX * BY - AY * BX);
        int B = (Y * AX - X * AY)  / (AX * BY - AY * BX);

        if (A * AX + B * BX == X && A * AY + B * BY == Y) {
            total += 3 * A + B;
        }
    }

    gfx_PrintUInt(total, 1);
    while (!kb_AnyKey());
    gfx_End();
}
