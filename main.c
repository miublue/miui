#include <stdio.h>
#include <raylib.h>
#define MIUI_IMPLEMENTATION
#include "miui.h"

#define FONT_PATH "Myna-Regular.otf"
#define FONT_SIZE 30
#define FONT_SPACING 0

int main(void)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1600, 900, "MIUI Test.");
    SetTargetFPS(60);
    MIUI_Theme theme = {
        .background_color = MIUI_RGBA(0x262626ff),
        .text_color = MIUI_RGBA(0xbababaff),
        .border_color = MIUI_RGBA(0xffffffff),
        .button_normal_color = MIUI_RGBA(0x484848ff),
        .button_hover_color = MIUI_RGBA(0xff5fafff),
        .button_text_color = MIUI_RGBA(0xffffffff),
        .entry_normal_color = MIUI_RGBA(0x484848ff),
        .entry_hover_color = MIUI_RGBA(0x606060ff),
        .entry_text_color = MIUI_RGBA(0xffffffff),
        .entry_cursor_color = MIUI_RGBA(0xff5fafff),
        .spacing = 30,
        .entry_cursor_width = 2,
        .border_width = 1,
    };

    MIUI_Entry entry = {0};

    if (MIUI_LoadFont(&theme, FONT_PATH, FONT_SIZE, FONT_SPACING) != 0) {
        fprintf(stderr, "ERROR: Failed to load font: %s\n", FONT_PATH);
        goto end;
    }

    char text[100] = "No clicks yet...";
    int num_clicks = 0;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(theme.background_color);

        const Vector2 center = { GetScreenWidth()/2, GetScreenHeight()/2 };
        const Vector2 txt_pos = { center.x + theme.spacing/2, center.y };
        const Vector2 btn_pos = { center.x - theme.spacing/2, center.y };
        const Rectangle ent_pos = { center.x, center.y + theme.font_size + theme.spacing*2, 300, theme.font_size };

        MIUI_Text(&theme, text, txt_pos, MIUI_ALIGN_LEFT);

        if (MIUI_Button(&theme, "Click me!", btn_pos, MIUI_ALIGN_RIGHT)) {
            snprintf(text, sizeof(text), "Clicked %d times!", ++num_clicks);
        }

        if (MIUI_TextEntry(&theme, &entry, ent_pos, MIUI_ALIGN_CENTER)) {
            snprintf(text, sizeof(text), "Entry text: \"%s\"", entry.text);
        }

        EndDrawing();
    }

end:
    MIUI_UnloadFont(&theme);
    CloseWindow();
    return 0;
}
