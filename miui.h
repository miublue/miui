#ifndef _MIUI_INCLUDE_H
#define _MIUI_INCLUDE_H

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <raylib.h>

#define MIUI_VERSION "0.0.2"

#ifndef MIUI_MAX_ENTRY_SIZE
#define MIUI_MAX_ENTRY_SIZE 1024
#endif

#define MIUI_RGBA(V) (Color) { \
        (uint8_t)(((uint32_t)(V) >> 24) & 0xff),    \
        (uint8_t)(((uint32_t)(V) >> 16) & 0xff),    \
        (uint8_t)(((uint32_t)(V) >> 8)  & 0xff),    \
        (uint8_t)(((uint32_t)(V) >> 0)  & 0xff)     \
    }

typedef struct MIUI_Theme {
    Color background_color;
    Color text_color;
    Color border_color;
    Color button_normal_color;
    Color button_hover_color;
    Color button_text_color;
    Color entry_normal_color;
    Color entry_hover_color;
    Color entry_text_color;
    Color entry_cursor_color;
    Font font;
    float font_size, font_spacing, spacing, entry_cursor_width;
    int border_width;
} MIUI_Theme;

typedef struct MIUI_Entry {
    size_t cursor, text_size;
    int is_selected;
    char text[MIUI_MAX_ENTRY_SIZE];
} MIUI_Entry;

typedef enum MIUI_TextAlignment {
    MIUI_ALIGN_LEFT,
    MIUI_ALIGN_CENTER,
    MIUI_ALIGN_RIGHT
} MIUI_TextAlignment;

/*
 * TODO: Menu window with the UI elements rendered within.
 * TODO: Auto layouts (position UI elements automatically without overlapping).
 * TODO: Scrollbars (requires menus).
 */

bool MIUI_LoadFont(MIUI_Theme *theme, const char *path, float size, float spacing);
void MIUI_UnloadFont(MIUI_Theme *theme);
Rectangle MIUI_GetAlignment(Rectangle rec, MIUI_TextAlignment align);
Rectangle MIUI_GetTextAlignment(MIUI_Theme *theme, const char *text, Vector2 pos, MIUI_TextAlignment align);
void MIUI_Text(MIUI_Theme *theme, const char *text, Vector2 pos, MIUI_TextAlignment align);
bool MIUI_Button(MIUI_Theme *theme, const char *text, Vector2 pos, MIUI_TextAlignment align);
bool MIUI_TextEntry(MIUI_Theme *theme, MIUI_Entry *entry, Rectangle rec, MIUI_TextAlignment align);

#endif

#ifdef MIUI_IMPLEMENTATION
#undef MIUI_IMPLEMENTATION

bool MIUI_LoadFont(MIUI_Theme *theme, const char *path, float size, float spacing)
{
    theme->font = LoadFontEx(path, size, NULL, 0);
    theme->font_size = size, theme->font_spacing = spacing;
    return IsFontValid(theme->font);
}

void MIUI_UnloadFont(MIUI_Theme *theme)
{
    UnloadFont(theme->font);
}

Rectangle MIUI_GetAlignment(Rectangle rec, MIUI_TextAlignment align)
{
    switch (align) {
    case MIUI_ALIGN_CENTER: rec.x -= rec.width/2; break;
    case MIUI_ALIGN_RIGHT: rec.x -= rec.width; break;
    default: break;
    }
    rec.y -= rec.height/2;
    return rec;
}

Rectangle MIUI_GetTextAlignment(MIUI_Theme *theme, const char *text, Vector2 pos, MIUI_TextAlignment align)
{
    const Vector2 sz = MeasureTextEx(theme->font, text, theme->font_size, theme->font_spacing);
    return MIUI_GetAlignment((Rectangle) { pos.x, pos.y, sz.x, sz.y }, align);
}

void MIUI_Text(MIUI_Theme *theme, const char *text, Vector2 pos, MIUI_TextAlignment align)
{
    const Rectangle rec = MIUI_GetTextAlignment(theme, text, pos, align);
    pos = (Vector2) { rec.x, rec.y };
    DrawTextEx(theme->font, text, pos, theme->font_size, theme->font_spacing, theme->text_color);
}

bool MIUI_Button(MIUI_Theme *theme, const char *text, Vector2 pos, MIUI_TextAlignment align)
{
    const Rectangle rec = MIUI_GetTextAlignment(theme, text, pos, align);
    const Rectangle btn = {
        rec.x - theme->spacing/2,
        rec.y - theme->spacing/2,
        rec.width + theme->spacing,
        rec.height + theme->spacing
    };
    pos = (Vector2) { rec.x, rec.y };
    const int hover = CheckCollisionPointRec(GetMousePosition(), btn);
    DrawRectangleRec(btn, hover? theme->button_hover_color : theme->button_normal_color);
    if (theme->border_width) DrawRectangleLinesEx(btn, theme->border_width, theme->border_color);
    DrawTextEx(theme->font, text, pos, theme->font_size, theme->font_spacing, theme->button_text_color);
    return hover && (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT));
}

bool MIUI_TextEntry(MIUI_Theme *theme, MIUI_Entry *entry, Rectangle rec, MIUI_TextAlignment align)
{
    /* TODO: move cursor to the mouse position if mouse button pressed and is_selected == true */
    rec = MIUI_GetAlignment(rec, align);
    const Rectangle btn = {
        rec.x - theme->spacing/2,
        rec.y - theme->spacing/2,
        rec.width + theme->spacing,
        rec.height + theme->spacing
    };
    const int hover = CheckCollisionPointRec(GetMousePosition(), btn);
    const int click = IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
    DrawRectangleRec(btn, hover? theme->entry_hover_color : theme->entry_normal_color);
    if (theme->border_width) DrawRectangleLinesEx(btn, theme->border_width, theme->border_color);
    if (click) entry->is_selected = hover;
    if (!entry->is_selected) goto end;
    const int chr = GetCharPressed();
    if (!chr) {
        const int key = GetKeyPressed();
        switch (key) {
        case KEY_ENTER:
            entry->is_selected = 0;
            return true;
        case KEY_LEFT:
            if (entry->cursor > 0) --entry->cursor;
            break;
        case KEY_RIGHT:
            if (entry->cursor < entry->text_size) ++entry->cursor;
            break;
        case KEY_UP: case KEY_HOME:
            entry->cursor = 0;
            break;
        case KEY_DOWN: case KEY_END:
            entry->cursor = entry->text_size;
            break;
        case KEY_BACKSPACE:
            if (entry->cursor == 0 || entry->text_size == 0) break;
            --entry->cursor;
            /* FALLTHROUGH */
        case KEY_DELETE:
            if (entry->cursor == entry->text_size) break;
            memmove(entry->text+entry->cursor, entry->text+entry->cursor+1, entry->text_size-entry->cursor);
            --entry->text_size;
            break;
        default: break;
        }
    } else {
        memmove(entry->text+entry->cursor+1, entry->text+entry->cursor, entry->text_size-entry->cursor);
        entry->text[entry->cursor++] = (char)chr;
        entry->text[++entry->text_size] = 0;
    }
end:
    {
        Rectangle cursor_pos = { rec.x, rec.y, theme->entry_cursor_width, rec.height };
        if (entry->text_size) {
            char text[MIUI_MAX_ENTRY_SIZE];
            snprintf(text, sizeof(text), "%.*s", (int)entry->cursor, entry->text);
            cursor_pos.x += MeasureTextEx(theme->font, text, theme->font_size, theme->font_spacing).x;
        }
        if (entry->is_selected) DrawRectangleRec(cursor_pos, theme->entry_cursor_color);
    }
    const Vector2 pos = { rec.x, rec.y };
    DrawTextEx(theme->font, entry->text, pos, theme->font_size, theme->font_spacing, theme->entry_text_color);
    return false;
}

#endif
