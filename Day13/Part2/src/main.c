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
        long long AX = strtol(tok + 12, &tok, 10);
        long long AY = strtol(tok + 4, &tok, 10);
        long long BX = strtol(tok + 13, &tok, 10);
        long long BY = strtol(tok + 4, &tok, 10);
        long long X = strtol(tok + 10, &tok, 10);
        long long Y = strtol(tok + 4, &tok, 10);
        long long X = strtol(tok + 10, &tok, 10) + 10000000000000;
        long long Y = strtol(tok + 4, &tok, 10) + 10000000000000;

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
