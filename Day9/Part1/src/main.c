#include <fileioc.h>
#include <stdint.h>
#include <graphx.h>
#include <keypadc.h>
#include <string.h>

#include <debug.h>

struct block_t {
    bool free;
    unsigned int id;
};

int main(void) {
    gfx_Begin();

    uint8_t slot = ti_Open("Input", "r");
    char *tok = ti_GetDataPtr(slot);
    char *endOfFile = tok + ti_GetSize(slot);
    ti_Close(slot);

    unsigned int blocks = 0;
    struct block_t *memory = (struct block_t *)gfx_vram;
    uint8_t id = 0;

    for (bool free = false; tok < endOfFile; tok++, free = !free) {
        for (uint8_t i = 0; i < *tok - '0'; i++, blocks++) {
            memory[blocks].free = free;
            memory[blocks].id = id;
        }

        id += !free;
    }

    unsigned int freeStart = 0;
    int fileStart = blocks - 1;

    for (; fileStart >= 0 && (int)freeStart < fileStart; fileStart--) {
        if (memory[fileStart].free == false) {
            for (unsigned int i = freeStart; (int)i < fileStart; i++) {
                if (memory[i].free) {
                    memory[i].free = false;
                    memory[i].id = memory[fileStart].id;
                    memory[fileStart].free = true;
                    freeStart = i + 1;
                    break;
                }
            }
        }
    }

    unsigned int total = 0;

    for (unsigned int i = 0; i < blocks && memory[i].free == false; i++) {
        total += memory[i].id * i;
        dbg_printf("%d * %d | %d\n", memory[i].id, i, total);
    }

    gfx_PrintUInt(total, 1);
    while (!kb_AnyKey());
    gfx_End();
}
