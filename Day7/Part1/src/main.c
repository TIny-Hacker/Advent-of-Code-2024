#include <fileioc.h>
#include <stdint.h>
#include <graphx.h>
#include <keypadc.h>
#include <string.h>
#include <stdio.h>

bool checkRow(char *tok, char *endOfFile) {
    unsigned long long target = strtoull(tok, &tok, 10);
    tok++;
    char *backup = tok;
    bool lastrun = false;

    for (unsigned int operations = 0; !lastrun; operations++) {
        lastrun = true;
        tok = backup;
        unsigned long long checkNum = 0;

        for (uint8_t i = 0; tok < endOfFile && *tok != '\0'; i++) {
            if (operations & (1 << i) || !checkNum || checkNum * strtoul(tok + 1, NULL, 10) > target) {
                checkNum += strtoul(tok + 1, &tok, 10);
            } else {
                checkNum *= strtoul(tok + 1, &tok, 10);
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
            total += strtoull(tok, &tok, 10);
        }

        for (; tok < endOfFile && *tok != '\0'; tok++);
        tok++;
    }

    printf("%llu", total);
    while (!kb_AnyKey());
}
