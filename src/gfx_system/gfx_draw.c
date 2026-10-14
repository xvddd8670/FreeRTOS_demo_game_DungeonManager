#include <stdint.h>
#include <stdbool.h>

#include "dungeon_manager/display_system/display_manager.h"
#include "dungeon_manager/gfx_system/gfx_draw.h"
#include "dungeon_manager/gfx_system/gfx_resources.h"


// обрезка прямоугольника по экрану, false = ничего не осталось
static bool clip_rect(int16_t *x, int16_t *y, int16_t *w, int16_t *h) {
    int16_t x1 = *x + *w, y1 = *y + *h;
    if (*x < 0) *x = 0;
    if (*y < 0) *y = 0;
    if (x1 > GFX_W) x1 = GFX_W;
    if (y1 > GFX_H) y1 = GFX_H;
    if (*x >= x1 || *y >= y1) return false;
    *w = x1 - *x;
    *h = y1 - *y;
    return true;
}

static void rect_i(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t on) {
    if (clip_rect(&x, &y, &w, &h)) display_fill_rect(x, y, w, h, on);
}


static void draw_rect(const gfx_cmd_t *c) {
    rect_i(c->p.rect.x, c->p.rect.y, c->p.rect.w, c->p.rect.h, c->on);
}

static void draw_hline(const gfx_cmd_t *c) {
    int16_t x = c->p.rect.x, y = c->p.rect.y, w = c->p.rect.w, h = 1;
    if (clip_rect(&x, &y, &w, &h)) display_hline(x, y, w, c->on);
}

static void draw_vline(const gfx_cmd_t *c) {
    int16_t x = c->p.rect.x, y = c->p.rect.y, w = 1, h = c->p.rect.h;
    if (clip_rect(&x, &y, &w, &h)) display_vline(x, y, h, c->on);
}

static void draw_line(const gfx_cmd_t *c) {
    display_line(c->p.line.x0, c->p.line.y0, c->p.line.x1, c->p.line.y1, c->on);   // клиппинг внутри
}

static void draw_pattern(const gfx_cmd_t *c) {
    int16_t x = c->p.pattern.x, y = c->p.pattern.y, w = c->p.pattern.w, h = c->p.pattern.h;
    if (clip_rect(&x, &y, &w, &h))
        display_fill_rect_pat(x, y, w, h, (display_pattern_t)c->p.pattern.pat, c->on);   // узор привязан к экрану
}


static void draw_sprite(const gfx_cmd_t *c) {
    if (c->p.sprite.id >= SPR_COUNT) return;
    const sprite_t *s = &gfx_sprites[c->p.sprite.id];
    uint8_t frame = c->p.sprite.frame < s->frames ? c->p.sprite.frame : 0;
    uint16_t off = frame * s->w * ((s->h + 7) / 8);
    int16_t x = c->p.sprite.x, y = c->p.sprite.y;

    if (s->mask) display_blit(x, y, s->w, s->h, s->mask + off, DISPLAY_BLIT_CLEAR);   // вырезали "дыру"
    display_blit(x, y, s->w, s->h, s->img + off, DISPLAY_BLIT_OR);
}

static void draw_text(const gfx_cmd_t *c) {
    if (c->p.text.font >= FONT_COUNT || !c->p.text.str) return;
    const font_t *f = &gfx_fonts[c->p.text.font];
    uint8_t glyph_size = f->w * ((f->h + 7) / 8);
    display_blit_mode_t mode = c->on ? DISPLAY_BLIT_OR : DISPLAY_BLIT_CLEAR;
    int16_t x = c->p.text.x;

    for (const char *s = c->p.text.str; *s && x < GFX_W; s++, x += f->w + 1) {
        uint8_t ch = (uint8_t)*s;
        if (ch < f->first || ch >= f->first + f->count) continue;   // нет глифа, оставляем пробел
        display_blit(x, c->p.text.y, f->w, f->h, f->data + (ch - f->first) * glyph_size, mode);
    }
}

static void draw_shape(const gfx_cmd_t *c) {
    if (c->p.shape.id >= SHAPE_COUNT) return;
    const shape_t *sh = &gfx_shapes[c->p.shape.id];
    int16_t ox = c->p.shape.x, oy = c->p.shape.y;

    for (uint8_t i = 0; i < sh->n; i++) {
        const shape_item_t *it = &sh->items[i];
        if (it->kind == CMD_LINE) display_line(ox + it->a, oy + it->b, ox + it->c, oy + it->d, c->on);
        else                      rect_i(ox + it->a, oy + it->b, it->c, it->d, c->on);
    }
}


static void (*const draw_fn[CMD_COUNT])(const gfx_cmd_t *) = {
    [CMD_RECT]    = draw_rect,
    [CMD_HLINE]   = draw_hline,
    [CMD_VLINE]   = draw_vline,
    [CMD_LINE]    = draw_line,
    [CMD_PATTERN] = draw_pattern,
    [CMD_SPRITE]  = draw_sprite,
    [CMD_TEXT]    = draw_text,
    [CMD_SHAPE]   = draw_shape,
};

void gfx_draw_cmd(const gfx_cmd_t *c) {
    if (c->hidden || c->kind >= CMD_COUNT) return;
    draw_fn[c->kind](c);
}
