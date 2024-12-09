#include <fileioc.h>
#include <stdint.h>
#include <graphx.h>
#include <keypadc.h>
#include <string.h>

struct chunk_t {
    bool free;
    unsigned int id;
    uint8_t size;
};

int main(void) {
    gfx_Begin();

    uint8_t slot = ti_Open("Input", "r");
    char *tok = ti_GetDataPtr(slot);
    char *endOfFile = tok + ti_GetSize(slot);
    char *start = tok;
    ti_Close(slot);

    unsigned int chunks = 0;
    struct chunk_t *memory = (struct chunk_t *)0x;

    for (bool free = false; tok < endOfFile; tok++, free = !free) {
        memory[chunks].free = free;
        memory[chunks].id = tok - start;
        memory[chunks].size = *tok - '0';
    }

    for (int i = chunks - 1; i >= 0; i--) {
        if (memory[i].free == false) {
            for (unsigned int j = 0; j < i; j++) {
                if (memory[j].free && memory[j].size >= memory[i].size) {
                    uint8_t newFree = memory[j].size - memory[i].size;
                    memory[j].free = false;
                    memory[j].id = memory[i].id;
                    memory[j].size = memory[i].size;

                    if (newFree) {
                        // Shift everything over up to old thing [i] here but I'm too lazy
                        memory[j + 1].free = true;
                        memory[j + 1].size = newFree;
                    }
                }
            }
        }
    }

    for (unsigned int i = 0, j = 0; i < chunks; i++) {
        if (memory[i].free == false) {
            for (uint8_t k = 0; k < memory[i].size; k++, j++) {
                total += memory[i].id * j;
            }
        }
    }

    gfx_PrintUInt(total, 1);
    while (!kb_AnyKey());
    gfx_End();
}
