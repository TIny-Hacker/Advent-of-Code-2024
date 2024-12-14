#include <fileioc.h>
#include <stdint.h>
#include <graphx.h>
#include <keypadc.h>
#include <string.h>

#include <stdio.h>

#define WIDTH 101
#define HEIGHT 103

#define MIDX (WIDTH - 1) / 2
#define MIDY (HEIGHT - 1) / 2

int mod(const int n, const int m) {
    return ((n % m) + m) % m;
}

uint8_t getQuadrant(uint8_t x, uint8_t y) {
    if (x < MIDX && y < MIDY) {
        return 0;
    }

    if (x < MIDX && y > MIDY) {
        return 2;
    }

    if (x > MIDX && y < MIDY) {
        return 1;
    }

    if (x > MIDX && y > MIDY) {
        return 3;
    }

    return 4;
}

uint8_t runBot(char *botData, int cycles) {
    botData += 2;
    int x = strtol(botData, &botData, 10);
    int y = strtol(botData + 1, &botData, 10);
    int vx = strtol(botData + 3, &botData, 10);
    int vy = strtol(botData + 1, &botData, 10);

    return getQuadrant(mod(x + vx * cycles, WIDTH), mod(y + vy * cycles, HEIGHT));
}

int main(void) {
    uint8_t slot = ti_Open("Input", "r");
    char *tok = ti_GetDataPtr(slot);
    char *endOfFile = tok + ti_GetSize(slot);
    ti_Close(slot);

    unsigned int quadrant[5] = {0, 0, 0, 0, 0}; // 5th is the middle which isn't counted

    while (tok < endOfFile) {
        quadrant[runBot(tok, 100)] += 1;
        tok += strlen(tok) + 1;
    }

    printf("%llu\n", (uint64_t)quadrant[0] * (uint64_t)quadrant[1] * (uint64_t)quadrant[2] * (uint64_t)quadrant[3]);
    while (!kb_AnyKey());
}
