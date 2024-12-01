#include "asm/tools.h"

#include <fileioc.h>
#include <stdint.h>
#include <graphx.h>
#include <keypadc.h>
#include <stdio.h>
#include <string.h>

#define os_PixelShadow      ((uint8_t *)0xD031F6)

int main(void) {
    uint8_t slot = ti_Open("Input", "r");
    char *num = ti_GetDataPtr(slot);
    char *endOfFile = num + ti_GetSize(slot);
    ti_Close(slot);

    unsigned int *col1 = (unsigned int *)os_PixelShadow;
    unsigned int *col2 = (unsigned int *)os_PixelShadow + 4200;

    unsigned int i = 0;

    for (i = 0; num < endOfFile; i++) {
        col1[i] = getNumber(num);
        for (; *num != ' '; num++);
        num += 3;

        col2[i] = getNumber(num);
        for (; *num != '\0'; num++);
        num++;
    }

    unsigned int j;
    unsigned long total = 0;

    for (i = 0; col1[i]; i++) {
        unsigned int sim = 0;
        j = 0;

        for (j = 0; col2[j]; j++) {
            if (col1[i] == col2[j]) {
                sim++;
            }
        }

        total += col1[i] * sim;
    }


    printf("%lu\n", total); // I should really write something to display longs so I don't need to add all the space for printf
    while (!kb_AnyKey());
}
