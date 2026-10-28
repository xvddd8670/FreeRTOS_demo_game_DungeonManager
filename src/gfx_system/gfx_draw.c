#include <stdint.h>
#include <stdbool.h>

#include "dungeon_manager/utils/log.h"

#include "dungeon_manager/display_system/display_manager.h"
#include "dungeon_manager/gfx_system/gfx_draw.h"
#include "dungeon_manager/gfx_system/gfx_resources.h"

static void draw_rect(const gfx_cmd_t *gfx_cmd) {
    display_fill_rect(
        gfx_cmd->payload.rect.x,
        gfx_cmd->payload.rect.y, 
        gfx_cmd->payload.rect.w, 
        gfx_cmd->payload.rect.h, 
        gfx_cmd->color);
}
static void draw_hline(const gfx_cmd_t *gfx_cmd) {
    display_hline(
        gfx_cmd->payload.rect.x, 
        gfx_cmd->payload.rect.y, 
        gfx_cmd->payload.rect.w, 
        gfx_cmd->color);
}
static void draw_vline(const gfx_cmd_t *gfx_cmd) {
    display_vline(
        gfx_cmd->payload.rect.x, 
        gfx_cmd->payload.rect.y, 
        gfx_cmd->payload.rect.h, 
        gfx_cmd->color);
}
static void draw_line(const gfx_cmd_t *gfx_cmd) {
    display_line(
        gfx_cmd->payload.line.x0, 
        gfx_cmd->payload.line.y0, 
        gfx_cmd->payload.line.x1, 
        gfx_cmd->payload.line.y1, 
        gfx_cmd->color);
}
static void draw_pattern(const gfx_cmd_t *gfx_cmd) {
    display_fill_rect_pat(
        gfx_cmd->payload.pattern.x, 
        gfx_cmd->payload.pattern.y, 
        gfx_cmd->payload.pattern.w, 
        gfx_cmd->payload.pattern.h,
        (display_pattern_t)gfx_cmd->payload.pattern.pat,
        gfx_cmd->color);
}

static void draw_sprite(const gfx_cmd_t *gfx_cmd) {
    if (gfx_cmd->payload.sprite.id >= SPR_COUNT) return;
    const sprite_t *s = &gfx_sprites[gfx_cmd->payload.sprite.id];
    uint8_t frame = gfx_cmd->payload.sprite.frame < s->frames ? gfx_cmd->payload.sprite.frame : 0;
    uint16_t off = frame * ((s->w + 7) / 8) * s->h;
    int16_t x = gfx_cmd->payload.sprite.x, y = gfx_cmd->payload.sprite.y;

    if (s->mask) display_blit_mono(x, y, s->w, s->h, s->mask + off, DISPLAY_COLOR_BG);
    display_blit_mono(x, y, s->w, s->h, s->img + off, gfx_cmd->color);
}

static void draw_text(const gfx_cmd_t *gfx_cmd) {
    if (gfx_cmd->payload.text.font >= FONT_COUNT || !gfx_cmd->payload.text.str) return;
    const font_t *f = &gfx_fonts[gfx_cmd->payload.text.font];
    uint8_t glyph_size = ((f->w + 7) / 8) * f->h;
    int16_t x = gfx_cmd->payload.text.x;

    for (const char *s = gfx_cmd->payload.text.str; *s && x < display_width(); s++, x += f->w + 1) {
        uint8_t ch = (uint8_t)*s;
        if (ch < f->first || ch >= f->first + f->count) continue;   // нет глифа, оставляем пробел
        display_blit_mono(x, gfx_cmd->payload.text.y, f->w, f->h, f->data + (ch - f->first) * glyph_size, gfx_cmd->color);
    }
}

static void draw_shape(const gfx_cmd_t *gfx_cmd) {
    if (gfx_cmd->payload.shape.id >= SHAPE_COUNT) return;
    const shape_t *sh = &gfx_shapes[gfx_cmd->payload.shape.id];
    int16_t ox = gfx_cmd->payload.shape.x, oy = gfx_cmd->payload.shape.y;

    for (uint8_t i = 0; i < sh->n; i++) {
        const shape_item_t *it = &sh->items[i];
        if (it->type == CMD_LINE) display_line(ox + it->a, oy + it->b, ox + it->c, oy + it->d, gfx_cmd->color);
        else                      display_fill_rect(ox + it->a, oy + it->b, it->c, it->d, gfx_cmd->color);
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

static void gfx_draw_cmd(const gfx_cmd_t *gfx_cmd) {
    if (gfx_cmd->hidden || gfx_cmd->type >= CMD_COUNT) return;
    draw_fn[gfx_cmd->type](gfx_cmd);
}

void gfx_draw_gfx_obj(const gfx_obj_t *gfx_obj){
    gfx_draw_metasprite(gfx_obj->metasprite, gfx_obj->coords);
}

void gfx_draw_metasprite(const metasprite_t *metasprite, vector2_t ms_coords) {
    // LOG_DEBUG("draw metasprite: %d", metasprite->cmds_count);
    for (uint8_t i = 0; i < metasprite->cmds_count; i++){
        gfx_cmd_t gfx_cmd = metasprite->cmds_arr[i];
        gfx_cmd.payload.rect.x += ms_coords.x;
        gfx_cmd.payload.rect.y += ms_coords.y;
        if (gfx_cmd.type == CMD_LINE){
            gfx_cmd.payload.line.x1 += ms_coords.x;
            gfx_cmd.payload.line.y1 += ms_coords.y;
        }
        gfx_draw_cmd(&gfx_cmd);
    }
}

void gfx_draw_one_cmd(const gfx_cmd_t *gfx_cmd, int16_t x, int16_t y){
    metasprite_t t = {1, gfx_cmd};
    gfx_draw_metasprite(&t, (vector2_t){x, y});
}
