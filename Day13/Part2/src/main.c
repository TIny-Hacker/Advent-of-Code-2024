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

    unsigned long long total = 0;

    while (tok < endOfFile) {
        tok += 12;
        long long AX = strtol(tok, &tok, 10);
        tok += 4;
        long long AY = strtol(tok, &tok, 10);
        tok += 13;
        long long BX = strtol(tok, &tok, 10);
        tok += 4;
        long long BY = strtol(tok, &tok, 10);

        tok += 10;
        long long X = strtol(tok, &tok, 10) + 10000000000000;
        tok += 4;
        long long Y = strtol(tok, &tok, 10) + 10000000000000;

        tok += 2;

        long long A = (X * BY - Y * BX) / (AX * BY - AY * BX);
        long long B = (Y * AX - X * AY)  / (AX * BY - AY * BX);

        if (A * AX + B * BX == X && A * AY + B * BY == Y) {
            total += 3 * A + B;
        }
    }

    printf("%lld\n", total);
    while (!kb_AnyKey());
}
