#include <fileioc.h>
#include <stdint.h>
#include <graphx.h>
#include <keypadc.h>
#include <string.h>
#include <stdio.h>

#include <debug.h>

enum {
    MULT,
    CAT,
    ADD,
};

uint64_t cat(uint64_t i, uint64_t j) {
    uint64_t k = 10;

    while (j >= k) {
        k *= 10;
    }

    return i * k + j;
}

void incTernary(uint8_t *map) {
    if (*map == 2) {
        *map = 0;
        incTernary(map + 1);
    } else {
        *map += 1;
    }
}

bool checkRow(char *tok, char *endOfFile) {
    unsigned long long target = strtoull(tok, &tok, 10);
    tok++;
    char *backup = tok;
    bool lastrun = false;
    uint8_t map[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    for (; !lastrun; incTernary(map)) {
        lastrun = true;
        tok = backup;
        unsigned long long checkNum = 0;

        for (uint8_t i = 0; tok < endOfFile && *tok != '\0'; i++) {
            if (map[i] == ADD || !checkNum || checkNum * strtoul(tok + 1, NULL, 10) > target) {
                checkNum += strtoul(tok + 1, &tok, 10);
            } else if (map[i] == MULT) {
                checkNum *= strtoul(tok + 1, &tok, 10);
                lastrun = false;
            } else if (map[i] == CAT) {
                checkNum = cat(checkNum, strtoul(tok + 1, &tok, 10));
                lastrun = false;
            }
        }

        if (checkNum == target) {
            return true;
        }
    }

    return false;
}

int main(void) {
    uint8_t slot = ti_Open("Input", "r");
    char *tok = ti_GetDataPtr(slot);
    char *endOfFile = tok + ti_GetSize(slot);
    ti_Close(slot);

    uint64_t total = 0;

    while (tok < endOfFile) {
        if (checkRow(tok, endOfFile)) {
            dbg_printf("%llu\n", strtoull(tok, NULL, 10));
            total += strtoull(tok, &tok, 10);
        }

        for (; tok < endOfFile && *tok != '\0'; tok++);
        tok++;
    }

    printf("%llu", total);
    while (!kb_AnyKey());
}
