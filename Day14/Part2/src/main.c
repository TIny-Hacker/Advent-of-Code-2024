#include <fileioc.h>
#include <stdint.h>
#include <graphx.h>
#include <keypadc.h>
#include <string.h>

struct bot_t {
    int x;
    int y;
    int vx;
    int vy;
};

#define os_PixelShadow ((struct bot_t *)0xD031F6)

#define WIDTH 101
#define HEIGHT 103

int mod(const int n, const int m) {
    return ((n % m) + m) % m;
}

int main(void) {
    uint8_t slot = ti_Open("Input", "r");
    char *tok = ti_GetDataPtr(slot);
    char *endOfFile = tok + ti_GetSize(slot);
    ti_Close(slot);

    gfx_Begin();
    gfx_SetColor(222);
    gfx_SetTextFGColor(222);

    struct bot_t *bots = os_PixelShadow;
    unsigned int botCount = 0;

    for (; tok < endOfFile; botCount++, tok++) {
        bots[botCount].x = strtol(tok + 2, &tok, 10);
        bots[botCount].y = strtol(tok + 1, &tok, 10);
        bots[botCount].vx = strtol(tok + 3, &tok, 10);
        bots[botCount].vy = strtol(tok + 1, &tok, 10);
    }

    unsigned int i = 7858;
    bool redraw = true;

    while (!kb_IsDown(kb_KeyClear)) {
        kb_Scan();

        if (kb_IsDown(kb_KeyLeft)) {
            i--;
            redraw = true;
            while (kb_AnyKey());
        } else if (kb_IsDown(kb_KeyRight)) {
            i++;
            redraw = true;
            while (kb_AnyKey());
        } else if (kb_IsDown(kb_KeyUp)) {
            i++;
            redraw = true;
        } else if (kb_IsDown(kb_KeyDown)) {
            i--;
            redraw = true;
        }

        if (redraw) {
            gfx_ZeroScreen();
            gfx_SetTextXY(0, 230);
            gfx_PrintUInt(i, 1);

            for (unsigned int j = 0; j < botCount; j++) {
                gfx_FillRectangle_NoClip(mod(bots[j].x + bots[j].vx * i, WIDTH) * 2, mod(bots[j].y + bots[j].vy * i, HEIGHT) * 2, 2, 2);
            }

            redraw = false;
        }
    }

    gfx_End();
}
