#include <fileioc.h>
#include <stdint.h>
#include <graphx.h>
#include <keypadc.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#include <debug.h>

unsigned int cacheSize = 0;
struct cache_t *cache = (struct cache_t *)gfx_vram;

struct cache_t {
    uint64_t stone;
    uint64_t children;
};

uint64_t updateStone(uint64_t *stone) {
    dbg_printf("%llu\n", *stone);
    if (!*stone) {
        *stone = 1;
        return (uint64_t)-1;
    }

    uint8_t digits = floor(log10(*stone)) + 1;

    if (digits & 1) {
        dbg_printf("Mult %llu\n", *stone);
        *stone *= 2024;
        return (uint64_t)-1;
    } else {
        uint64_t new = *stone % (uint64_t)ceil(pow(10, digits / 2));
        dbg_printf("Test: %llu | ", ((uint64_t)ceil(pow(10, digits / 2))));
        *stone /= (uint64_t)ceil(pow(10, digits / 2));
        dbg_printf("Split: %llu | %llu\n", *stone, new);
        return new;
    }

    return (uint64_t)-1;
}

uint64_t checkCache(uint64_t stone) {
    for (unsigned int i = 0; i < cacheSize; i++) {
        if (cache[i].stone == stone) {
            return cache[i].children;
        }
    }

    return 0;
}

uint64_t count(uint64_t stone, uint8_t depth) {
    uint64_t new = updateStone(&stone);

    if (depth == 1) {
        return 1 + (new != (uint64_t)-1);
    }

    uint64_t children = checkCache(stone);

    if (!children) {
        children = count(stone, depth - 1);
        cache[cacheSize].stone = stone;
        cache[cacheSize++].children = children;
    }

    uint64_t otherChildren = 0;

    if (new != (uint64_t)-1) {
        otherChildren += checkCache(new);

        if (!otherChildren) {
            otherChildren += count(new, depth - 1);
            cache[cacheSize].stone = new;
            cache[cacheSize++].children = otherChildren;
        }
    }

    return children + otherChildren;
}

int main(void) {
    gfx_Begin();

    uint8_t slot = ti_Open("Input", "r");
    char *tok = ti_GetDataPtr(slot);
    char *endOfFile = tok + ti_GetSize(slot);
    ti_Close(slot);

    uint64_t total = 0;

    for (; tok < endOfFile; tok++) {
        total += count(strtoull(tok, &tok, 10), 7);
    }

    gfx_End();
    printf("%llu", total);
    while (!kb_AnyKey());
}
