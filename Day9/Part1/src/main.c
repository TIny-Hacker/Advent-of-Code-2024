#include <fileioc.h>
#include <stdint.h>
#include <graphx.h>
#include <keypadc.h>
#include <string.h>
#include <stdio.h>

#include <debug.h>

unsigned long fillSpace(unsigned long position, unsigned int *reverse) {
    while (gfx_vram[*reverse] == '0') {
        gfx_vram[*reverse - 1] = '0';
        *reverse -= 2;
    }

    gfx_vram[*reverse] -= 1;

    unsigned int checksum = position * ((*reverse + 1) / 2);
    dbg_printf("%u | %u\n", ((*reverse + 1) / 2), *reverse);

    return checksum;
}

bool allFree(unsigned int c, unsigned int blocks) {
    for (; c < blocks; c++) {
        if (gfx_vram[c] > '0') {
            return false;
        }
    }

    return true;
}

int main(void) {
    gfx_Begin();

    uint8_t slot = ti_Open("Input", "r");
    char *tok = ti_GetDataPtr(slot);
    ti_Close(slot);

    unsigned int blocks = strlen(tok);
    gfx_ZeroScreen();
    memcpy(gfx_vram, tok, blocks);

    unsigned long long total = 0;
    unsigned long position = 0;
    unsigned int reverse = blocks - 1;
    bool free = false;

    for (unsigned int id = 0, c = 0; !(free && allFree(c + 1, blocks)); id += !free, free = !free, c++) {
        dbg_printf("%u | %u | %u\n", gfx_vram[c] - '0', c, reverse);
        for (; gfx_vram[c] - '0'; position++) {
            if (free && !allFree(c + 1, blocks)) {
                total += fillSpace(position, &reverse);
            } else if (!free) {
                // dbg_printf ("%u * %lu = %lu\n", id, position, id * position);
                total += id * position;
            }

            gfx_vram[c] -= 1;
        }
    }

    dbg_printf("%llu\n", total);
    while (!kb_AnyKey());
    gfx_End();
    while (kb_AnyKey());
    printf("%llu\n", total);
    while (!kb_AnyKey());
}
