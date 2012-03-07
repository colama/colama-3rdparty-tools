/* -*- Mode: C; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
   Copyright (C) 2009,2010 Red Hat, Inc.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdbool.h>
#include <inttypes.h>
#include "red_common.h"
#include "red_memslots.h"
#include "red_parse_qxl.h"

#if 0
static void hexdump_qxl(RedMemSlotInfo *slots, int group_id,
                        QXLPHYSICAL addr, uint8_t bytes)
{
    uint8_t *hex;
    int i;

    hex = (uint8_t*)get_virt(slots, addr, bytes, group_id);
    for (i = 0; i < bytes; i++) {
        if (0 == i % 16) {
            fprintf(stderr, "%lx: ", addr+i);
        }
        if (0 == i % 4) {
            fprintf(stderr, " ");
        }
        fprintf(stderr, " %02x", hex[i]);
        if (15 == i % 16) {
            fprintf(stderr, "\n");
        }
    }
}
#endif

static uint8_t *red_linearize_chunk(RedDataChunk *head, size_t size, bool *free_chunk)
{
    uint8_t *data, *ptr;
    RedDataChunk *chunk;
    uint32_t copy;

    if (head->next_chunk == NULL) {
        ASSERT(size <= head->data_size);
        *free_chunk = false;
        return head->data;
    }

    ptr = data = spice_malloc(size);
    *free_chunk = true;
    for (chunk = head; chunk != NULL && size > 0; chunk = chunk->next_chunk) {
        copy = MIN(chunk->data_size, size);
        memcpy(ptr, chunk->data, copy);
        ptr += copy;
        size -= copy;
    }
    ASSERT(size == 0);
    return data;
}

static size_t red_get_data_chunks_ptr(RedMemSlotInfo *slots, int group_id,
                                      int memslot_id,
                                      RedDataChunk *red, QXLDataChunk *qxl)
{
    RedDataChunk *red_prev;
    size_t data_size = 0;

    red->data_size = qxl->data_size;
    data_size += red->data_size;
    validate_virt(slots, (intptr_t)qxl->data, memslot_id, red->data_size, group_id);
    red->data = qxl->data;
    red->prev_chunk = NULL;

    while (qxl->next_chunk) {
        red_prev = red;
        red = spice_new(RedDataChunk, 1);
        memslot_id = get_memslot_id(slots, qxl->next_chunk);
        qxl = (QXLDataChunk*)get_virt(slots, qxl->next_chunk, sizeof(*qxl), group_id);

        red->data_size = qxl->data_size;
        data_size += red->data_size;
        validate_virt(slots, (intptr_t)qxl->data, memslot_id, red->data_size, group_id);
        red->data = qxl->data;
        red->prev_chunk = red_prev;
        red_prev->next_chunk = red;
    }

    red->next_chunk = NULL;
    return data_size;
}

static size_t red_get_data_chunks(RedMemSlotInfo *slots, int group_id,
                                  RedDataChunk *red, QXLPHYSICAL addr)
{
    QXLDataChunk *qxl;
    int memslot_id = get_memslot_id(slots, addr);

    qxl = (QXLDataChunk*)get_virt(slots, addr, sizeof(*qxl), group_id);
    return red_get_data_chunks_ptr(slots, group_id, memslot_id, red, qxl);
}

static void red_put_data_chunks(RedDataChunk *red)
{
    RedDataChunk *tmp;

    red = red->next_chunk;
    while (red) {
        tmp = red;
        red = red->next_chunk;
        free(tmp);
    }
}

static void red_get_point_ptr(SpicePoint *red, QXLPoint *qxl)
{
    red->x = qxl->x;
    red->y = qxl->y;
}

static void red_get_point16_ptr(SpicePoint16 *red, QXLPoint16 *qxl)
{
    red->x = qxl->x;
    red->y = qxl->y;
}

void red_get_rect_ptr(SpiceRect *red, QXLRect *qxl)
{
    red->top    = qxl->top;
    red->left   = qxl->left;
    red->bottom = qxl->bottom;
    red->right  = qxl->right;
}

static SpicePath *red_get_path(RedMemSlotInfo *slots, int group_id,
                               QXLPHYSICAL addr)
{
    RedDataChunk chunks;
    QXLPathSeg *start, *end;
    SpicePathSeg *seg;
    uint8_t *data;
    bool free_data;
    QXLPath *qxl;
    SpicePath *red;
    size_t size, mem_size, mem_size2, dsize, segment_size;
    int n_segments;
    int i;
    uint32_t count;

    qxl = (QXLPath *)get_virt(slots, addr, sizeof(*qxl), group_id);
    size = red_get_data_chunks_ptr(slots, group_id,
                                   get_memslot_id(slots, addr),
                                   &chunks, &qxl->chunk);
    data = red_linearize_chunk(&chunks, size, &free_data);
    red_put_data_chunks(&chunks);

    n_segments = 0;
    mem_size = sizeof(*red);

    start = (QXLPathSeg*)data;
    end = (QXLPathSeg*)(data + size);
    while (start < end) {
        n_segments++;
        count = start->count;
        segment_size = sizeof(SpicePathSeg) + count * sizeof(SpicePointFix);
        mem_size += sizeof(SpicePathSeg *) + SPICE_ALIGN(segment_size, 4);
        start = (QXLPathSeg*)(&start->points[count]);
    }

    red = spice_malloc(mem_size);
    red->num_segments = n_segments;

    start = (QXLPathSeg*)data;
    end = (QXLPathSeg*)(data + size);
    seg = (SpicePathSeg*)&red->segments[n_segments];
    n_segments = 0;
    mem_size2 = sizeof(*red);
    while (start < end) {
        red->segments[n_segments++] = seg;
        count = start->count;

        /* Protect against overflow in size calculations before
           writing to memory */
        ASSERT(mem_size2 + sizeof(SpicePathSeg) > mem_size2);
        mem_size2  += sizeof(SpicePathSeg);
        ASSERT(count < UINT32_MAX / sizeof(SpicePointFix));
        dsize = count * sizeof(SpicePointFix);
        ASSERT(mem_size2 + dsize > mem_size2);
        mem_size2  += dsize;

        /* Verify that we didn't overflow due to guest changing data */
        ASSERT(mem_size2 <= mem_size);

        seg->flags = start->flags;
        seg->count = count;
        for (i = 0; i < seg->count; i++) {
            seg->points[i].x = start->points[i].x;
            seg->points[i].y = start->points[i].y;
        }
        start = (QXLPathSeg*)(&start->points[i]);
        seg = (SpicePathSeg*)(&seg->points[i]);
    }
    /* Ensure guest didn't tamper with segment count */
    ASSERT(n_segments == red->num_segments);

    if (free_data) {
        free(data);
    }
    return red;
}

static SpiceClipRects *red_get_clip_rects(RedMemSlotInfo *slots, int group_id,
                                          QXLPHYSICAL addr)
{
    RedDataChunk chunks;
    QXLClipRects *qxl;
    SpiceClipRects *red;
    QXLRect *start, *end;
    uint8_t *data;
    bool free_data;
    size_t size;
    int i;

    qxl = (QXLClipRects *)get_virt(slots, addr, sizeof(*qxl), group_id);
    size = red_get_data_chunks_ptr(slots, group_id,
                                   get_memslot_id(slots, addr),
                                   &chunks, &qxl->chunk);
    data = red_linearize_chunk(&chunks, size, &free_data);
    red_put_data_chunks(&chunks);

    ASSERT(qxl->num_rects * sizeof(QXLRect) == size);
    red = spice_malloc(sizeof(*red) + qxl->num_rects * sizeof(SpiceRect));
    red->num_rects = qxl->num_rects;

    start = (QXLRect*)data;
    end = (QXLRect*)(data + size);
    for (i = 0; i < red->num_rects; i++) {
        red_get_rect_ptr(red->rects + i, start++);
    }

    if (free_data) {
        free(data);
    }
    return red;
}

static SpiceChunks *red_get_image_data_flat(RedMemSlotInfo *slots, int group_id,
                                            QXLPHYSICAL addr, size_t size)
{
    SpiceChunks *data;

    data = spice_chunks_new(1);
    data->data_size      = size;
    data->chunk[0].data  = (void*)get_virt(slots, addr, size, group_id);
    data->chunk[0].len   = size;
    return data;
}

static SpiceChunks *red_get_image_data_chunked(RedMemSlotInfo *slots, int group_id,
                                               RedDataChunk *head)
{
    SpiceChunks *data;
    RedDataChunk *chunk;
    int i;

    for (i = 0, chunk = head; chunk != NULL; chunk = chunk->next_chunk) {
        i++;
    }

    data = spice_chunks_new(i);
    data->data_size = 0;
    for (i = 0, chunk = head;
         chunk != NULL && i < data->num_chunks;
         chunk = chunk->next_chunk, i++) {
        data->chunk[i].data  = chunk->data;
        data->chunk[i].len   = chunk->data_size;
        data->data_size     += chunk->data_size;
    }
    ASSERT(i == data->num_chunks);
    return data;
}

static SpiceImage *red_get_image(RedMemSlotInfo *slots, int group_id,
                                 QXLPHYSICAL addr)
{
    RedDataChunk chunks;
    QXLImage *qxl;
    SpiceImage *red;
    size_t bitmap_size, size;
    uint8_t qxl_flags;

    if (addr == 0) {
        return NULL;
    }

    qxl = (QXLImage *)get_virt(slots, addr, sizeof(*qxl), group_id);
    red = spice_new0(SpiceImage, 1);
    red->descriptor.id     = qxl->descriptor.id;
    red->descriptor.type   = qxl->descriptor.type;
    red->descriptor.flags = 0;
    if (qxl->descriptor.flags & QXL_IMAGE_HIGH_BITS_SET) {
        red->descriptor.flags |= SPICE_IMAGE_FLAGS_HIGH_BITS_SET;
    }
    if (qxl->descriptor.flags & QXL_IMAGE_CACHE) {
        red->descriptor.flags |= SPICE_IMAGE_FLAGS_CACHE_ME;
    }
    red->descriptor.width  = qxl->descriptor.width;
    red->descriptor.height = qxl->descriptor.height;

    switch (red->descriptor.type) {
    case SPICE_IMAGE_TYPE_BITMAP:
        red->u.bitmap.format = qxl->bitmap.format;
        qxl_flags = qxl->bitmap.flags;
        if (qxl_flags & QXL_BITMAP_TOP_DOWN) {
            red->u.bitmap.flags = SPICE_BITMAP_FLAGS_TOP_DOWN;
        }
        red->u.bitmap.x      = qxl->bitmap.x;
        red->u.bitmap.y      = qxl->bitmap.y;
        red->u.bitmap.stride = qxl->bitmap.stride;
        if (qxl->bitmap.palette) {
            QXLPalette *qp;
            SpicePalette *rp;
            int i, num_ents;
            qp = (QXLPalette *)get_virt(slots, qxl->bitmap.palette,
                                        sizeof(*qp), group_id);
            num_ents = qp->num_ents;
            validate_virt(slots, (intptr_t)qp->ents,
                          get_memslot_id(slots, qxl->bitmap.palette),
                          num_ents * sizeof(qp->ents[0]), group_id);
            rp = spice_malloc_n_m(num_ents, sizeof(rp->ents[0]), sizeof(*rp));
            rp->unique   = qp->unique;
            rp->num_ents = num_ents;
            for (i = 0; i < num_ents; i++) {
                rp->ents[i] = qp->ents[i];
            }
            red->u.bitmap.palette = rp;
            red->u.bitmap.palette_id = rp->unique;
        }
        bitmap_size = red->u.bitmap.y * abs(red->u.bitmap.stride);
        if (qxl_flags & QXL_BITMAP_DIRECT) {
            red->u.bitmap.data = red_get_image_data_flat(slots, group_id,
                                                         qxl->bitmap.data,
                                                         bitmap_size);
        } else {
            size = red_get_data_chunks(slots, group_id,
                                       &chunks, qxl->bitmap.data);
            ASSERT(size == bitmap_size);
            red->u.bitmap.data = red_get_image_data_chunked(slots, group_id,
                                                            &chunks);
            red_put_data_chunks(&chunks);
        }
        if (qxl_flags & QXL_BITMAP_UNSTABLE) {
            red->u.bitmap.data->flags |= SPICE_CHUNKS_FLAGS_UNSTABLE;
        }
        break;
    case SPICE_IMAGE_TYPE_SURFACE:
        red->u.surface.surface_id = qxl->surface_image.surface_id;
        break;
    case SPICE_IMAGE_TYPE_QUIC:
        red->u.quic.data_size = qxl->quic.data_size;
        size = red_get_data_chunks_ptr(slots, group_id,
                                       get_memslot_id(slots, addr),
                                       &chunks, (QXLDataChunk *)qxl->quic.data);
        ASSERT(size == red->u.quic.data_size);
        red->u.quic.data = red_get_image_data_chunked(slots, group_id,
                                                      &chunks);
        red_put_data_chunks(&chunks);
        break;
    default:
        red_error("%s: unknown type %d", __FUNCTION__, red->descriptor.type);
        abort();
    }
    return red;
}

void red_put_image(SpiceImage *red)
{
    if (red == NULL)
        return;

    switch (red->descriptor.type) {
    case SPICE_IMAGE_TYPE_BITMAP:
        if (red->u.bitmap.palette) {
            free(red->u.bitmap.palette);
        }
        spice_chunks_destroy(red->u.bitmap.data);
        break;
    }
    free(red);
}

static void red_get_brush_ptr(RedMemSlotInfo *slots, int group_id,
                              SpiceBrush *red, QXLBrush *qxl)
{
    red->type = qxl->type;
    switch (red->type) {
    case SPICE_BRUSH_TYPE_SOLID:
        red->u.color = qxl->u.color;
        break;
    case SPICE_BRUSH_TYPE_PATTERN:
        red->u.pattern.pat = red_get_image(slots, group_id, qxl->u.pattern.pat);
        red_get_point_ptr(&red->u.pattern.pos, &qxl->u.pattern.pos);
        break;
    }
}

static void red_put_brush(SpiceBrush *red)
{
    switch (red->type) {
    case SPICE_BRUSH_TYPE_PATTERN:
        red_put_image(red->u.pattern.pat);
        break;
    }
}

static void red_get_qmask_ptr(RedMemSlotInfo *slots, int group_id,
                              SpiceQMask *red, QXLQMask *qxl)
{
    red->flags  = qxl->flags;
    red_get_point_ptr(&red->pos, &qxl->pos);
    red->bitmap = red_get_image(slots, group_id, qxl->bitmap);
}

static void red_put_qmask(SpiceQMask *red)
{
    red_put_image(red->bitmap);
}

static void red_get_fill_ptr(RedMemSlotInfo *slots, int group_id,
                             SpiceFill *red, QXLFill *qxl)
{
    red_get_brush_ptr(slots, group_id, &red->brush, &qxl->brush);
    red->rop_descriptor = qxl->rop_descriptor;
    red_get_qmask_ptr(slots, group_id, &red->mask, &qxl->mask);
}

static void red_put_fill(SpiceFill *red)
{
    red_put_brush(&red->brush);
    red_put_qmask(&red->mask);
}

static void red_get_opaque_ptr(RedMemSlotInfo *slots, int group_id,
                               SpiceOpaque *red, QXLOpaque *qxl)
{
   red->src_bitmap     = red_get_image(slots, group_id, qxl->src_bitmap);
   red_get_rect_ptr(&red->src_area, &qxl->src_area);
   red_get_brush_ptr(slots, group_id, &red->brush, &qxl->brush);
   red->rop_descriptor = qxl->rop_descriptor;
   red->scale_mode     = qxl->scale_mode;
   red_get_qmask_ptr(slots, group_id, &red->mask, &qxl->mask);
}

static void red_put_opaque(SpiceOpaque *red)
{
    red_put_image(red->src_bitmap);
    red_put_brush(&red->brush);
    red_put_qmask(&red->mask);
}

static void red_get_copy_ptr(RedMemSlotInfo *slots, int group_id,
                             SpiceCopy *red, QXLCopy *qxl)
{
   red->src_bitmap      = red_get_image(slots, group_id, qxl->src_bitmap);
   red_get_rect_ptr(&red->src_area, &qxl->src_area);
   red->rop_descriptor  = qxl->rop_descriptor;
   red->scale_mode      = qxl->scale_mode;
   red_get_qmask_ptr(slots, group_id, &red->mask, &qxl->mask);
}

static void red_put_copy(SpiceCopy *red)
{
    red_put_image(red->src_bitmap);
    red_put_qmask(&red->mask);
}

static void red_get_blend_ptr(RedMemSlotInfo *slots, int group_id,
                             SpiceBlend *red, QXLBlend *qxl)
{
   red->src_bitmap      = red_get_image(slots, group_id, qxl->src_bitmap);
   red_get_rect_ptr(&red->src_area, &qxl->src_area);
   red->rop_descriptor  = qxl->rop_descriptor;
   red->scale_mode      = qxl->scale_mode;
   red_get_qmask_ptr(slots, group_id, &red->mask, &qxl->mask);
}

static void red_put_blend(SpiceBlend *red)
{
    red_put_image(red->src_bitmap);
    red_put_qmask(&red->mask);
}

static void red_get_transparent_ptr(RedMemSlotInfo *slots, int group_id,
                                    SpiceTransparent *red, QXLTransparent *qxl)
{
   red->src_bitmap      = red_get_image(slots, group_id, qxl->src_bitmap);
   red_get_rect_ptr(&red->src_area, &qxl->src_area);
   red->src_color       = qxl->src_color;
   red->true_color      = qxl->true_color;
}

static void red_put_transparent(SpiceTransparent *red)
{
    red_put_image(red->src_bitmap);
}

static void red_get_alpha_blend_ptr(RedMemSlotInfo *slots, int group_id,
                                    SpiceAlphaBlend *red, QXLAlphaBlend *qxl)
{
    red->alpha_flags = qxl->alpha_flags;
    red->alpha       = qxl->alpha;
    red->src_bitmap  = red_get_image(slots, group_id, qxl->src_bitmap);
    red_get_rect_ptr(&red->src_area, &qxl->src_area);
}

static void red_get_alpha_blend_ptr_compat(RedMemSlotInfo *slots, int group_id,
                                           SpiceAlphaBlend *red, QXLCompatAlphaBlend *qxl)
{
    red->alpha       = qxl->alpha;
    red->src_bitmap  = red_get_image(slots, group_id, qxl->src_bitmap);
    red_get_rect_ptr(&red->src_area, &qxl->src_area);
}

static void red_put_alpha_blend(SpiceAlphaBlend *red)
{
    red_put_image(red->src_bitmap);
}

static void red_get_rop3_ptr(RedMemSlotInfo *slots, int group_id,
                             SpiceRop3 *red, QXLRop3 *qxl)
{
   red->src_bitmap = red_get_image(slots, group_id, qxl->src_bitmap);
   red_get_rect_ptr(&red->src_area, &qxl->src_area);
   red_get_brush_ptr(slots, group_id, &red->brush, &qxl->brush);
   red->rop3       = qxl->rop3;
   red->scale_mode = qxl->scale_mode;
   red_get_qmask_ptr(slots, group_id, &red->mask, &qxl->mask);
}

static void red_put_rop3(SpiceRop3 *red)
{
    red_put_image(red->src_bitmap);
    red_put_brush(&red->brush);
    red_put_qmask(&red->mask);
}

static void red_get_stroke_ptr(RedMemSlotInfo *slots, int group_id,
                               SpiceStroke *red, QXLStroke *qxl)
{
    red->path = red_get_path(slots, group_id, qxl->path);
    red->attr.flags       = qxl->attr.flags;
    if (red->attr.flags & SPICE_LINE_FLAGS_STYLED) {
        int style_nseg;
        uint8_t *buf;

        style_nseg = qxl->attr.style_nseg;
        red->attr.style = spice_malloc_n(style_nseg, sizeof(SPICE_FIXED28_4));
        red->attr.style_nseg  = style_nseg;
        ASSERT(qxl->attr.style);
        buf = (uint8_t *)get_virt(slots, qxl->attr.style,
                                  style_nseg * sizeof(QXLFIXED), group_id);
        memcpy(red->attr.style, buf, style_nseg * sizeof(QXLFIXED));
    } else {
        red->attr.style_nseg  = 0;
        red->attr.style       = NULL;
    }
    red_get_brush_ptr(slots, group_id, &red->brush, &qxl->brush);
    red->fore_mode        = qxl->fore_mode;
    red->back_mode        = qxl->back_mode;
}

static void red_put_stroke(SpiceStroke *red)
{
    red_put_brush(&red->brush);
    free(red->path);
    if (red->attr.flags & SPICE_LINE_FLAGS_STYLED) {
        free(red->attr.style);
    }
}

static SpiceString *red_get_string(RedMemSlotInfo *slots, int group_id,
                                   QXLPHYSICAL addr)
{
    RedDataChunk chunks;
    QXLString *qxl;
    QXLRasterGlyph *start, *end;
    SpiceString *red;
    SpiceRasterGlyph *glyph;
    uint8_t *data;
    bool free_data;
    size_t chunk_size, qxl_size, red_size, glyph_size;
    int glyphs, bpp = 0, i;

    qxl = (QXLString *)get_virt(slots, addr, sizeof(*qxl), group_id);
    chunk_size = red_get_data_chunks_ptr(slots, group_id,
                                         get_memslot_id(slots, addr),
                                         &chunks, &qxl->chunk);
    data = red_linearize_chunk(&chunks, chunk_size, &free_data);
    red_put_data_chunks(&chunks);

    qxl_size = qxl->data_size;
    ASSERT(chunk_size == qxl_size);

    if (qxl->flags & SPICE_STRING_FLAGS_RASTER_A1) {
        bpp = 1;
    } else if (qxl->flags & SPICE_STRING_FLAGS_RASTER_A4) {
        bpp = 4;
    } else if (qxl->flags & SPICE_STRING_FLAGS_RASTER_A8) {
        bpp = 8;
    }
    ASSERT(bpp != 0);

    start = (QXLRasterGlyph*)data;
    end = (QXLRasterGlyph*)(data + chunk_size);
    red_size = sizeof(SpiceString);
    glyphs = 0;
    while (start < end) {
        ASSERT((QXLRasterGlyph*)(&start->data[0]) <= end);
        glyphs++;
        glyph_size = start->height * ((start->width * bpp + 7) / 8);
        red_size += sizeof(SpiceRasterGlyph *) + SPICE_ALIGN(sizeof(SpiceRasterGlyph) + glyph_size, 4);
        start = (QXLRasterGlyph*)(&start->data[glyph_size]);
    }
    ASSERT(start <= end);
    ASSERT(glyphs == qxl->length);

    red = spice_malloc(red_size);
    red->length = qxl->length;
    red->flags = qxl->flags;

    start = (QXLRasterGlyph*)data;
    end = (QXLRasterGlyph*)(data + chunk_size);
    glyph = (SpiceRasterGlyph *)&red->glyphs[red->length];
    for (i = 0; i < red->length; i++) {
        ASSERT((QXLRasterGlyph*)(&start->data[0]) <= end);
        red->glyphs[i] = glyph;
        glyph->width = start->width;
        glyph->height = start->height;
        red_get_point_ptr(&glyph->render_pos, &start->render_pos);
        red_get_point_ptr(&glyph->glyph_origin, &start->glyph_origin);
        glyph_size = glyph->height * ((glyph->width * bpp + 7) / 8);
        ASSERT((QXLRasterGlyph*)(&start->data[glyph_size]) <= end);
        memcpy(glyph->data, start->data, glyph_size);
        start = (QXLRasterGlyph*)(&start->data[glyph_size]);
        glyph = (SpiceRasterGlyph*)
            (((uint8_t *)glyph) +
             SPICE_ALIGN(sizeof(SpiceRasterGlyph) + glyph_size, 4));
    }

    if (free_data) {
        free(data);
    }
    return red;
}

static void red_get_text_ptr(RedMemSlotInfo *slots, int group_id,
                             SpiceText *red, QXLText *qxl)
{
   red->str = red_get_string(slots, group_id, qxl->str);
   red_get_rect_ptr(&red->back_area, &qxl->back_area);
   red_get_brush_ptr(slots, group_id, &red->fore_brush, &qxl->fore_brush);
   red_get_brush_ptr(slots, group_id, &red->back_brush, &qxl->back_brush);
   red->fore_mode  = qxl->fore_mode;
   red->back_mode  = qxl->back_mode;
}

static void red_put_text_ptr(SpiceText *red)
{
    free(red->str);
    red_put_brush(&red->fore_brush);
    red_put_brush(&red->back_brush);
}

static void red_get_whiteness_ptr(RedMemSlotInfo *slots, int group_id,
                                  SpiceWhiteness *red, QXLWhiteness *qxl)
{
    red_get_qmask_ptr(slots, group_id, &red->mask, &qxl->mask);
}

static void red_put_whiteness(SpiceWhiteness *red)
{
    red_put_qmask(&red->mask);
}

static void red_get_blackness_ptr(RedMemSlotInfo *slots, int group_id,
                                  SpiceBlackness *red, QXLBlackness *qxl)
{
    red_get_qmask_ptr(slots, group_id, &red->mask, &qxl->mask);
}

static void red_put_blackness(SpiceWhiteness *red)
{
    red_put_qmask(&red->mask);
}

static void red_get_invers_ptr(RedMemSlotInfo *slots, int group_id,
                               SpiceInvers *red, QXLInvers *qxl)
{
    red_get_qmask_ptr(slots, group_id, &red->mask, &qxl->mask);
}

static void red_put_invers(SpiceWhiteness *red)
{
    red_put_qmask(&red->mask);
}

static void red_get_clip_ptr(RedMemSlotInfo *slots, int group_id,
                             SpiceClip *red, QXLClip *qxl)
{
    red->type = qxl->type;
    switch (red->type) {
    case SPICE_CLIP_TYPE_RECTS:
        red->rects = red_get_clip_rects(slots, group_id, qxl->data);
        break;
    }
}

static void red_put_clip(SpiceClip *red)
{
    switch (red->type) {
    case SPICE_CLIP_TYPE_RECTS:
        free(red->rects);
        break;
    }
}

void red_get_drawable(RedMemSlotInfo *slots, int group_id,
                      RedDrawable *red, QXLPHYSICAL addr)
{
    QXLDrawable *qxl;
    int i;

    qxl = (QXLDrawable *)get_virt(slots, addr, sizeof(*qxl), group_id);
    red->release_info     = &qxl->release_info;

    red_get_rect_ptr(&red->bbox, &qxl->bbox);
    red_get_clip_ptr(slots, group_id, &red->clip, &qxl->clip);
    red->effect           = qxl->effect;
    red->mm_time          = qxl->mm_time;
    red->self_bitmap      = qxl->self_bitmap;
    red_get_rect_ptr(&red->self_bitmap_area, &qxl->self_bitmap_area);
    red->surface_id       = qxl->surface_id;

    for (i = 0; i < 3; i++) {
        red->surfaces_dest[i] = qxl->surfaces_dest[i];
        red_get_rect_ptr(&red->surfaces_rects[i], &qxl->surfaces_rects[i]);
    }

    red->type = qxl->type;
    switch (red->type) {
    case QXL_DRAW_ALPHA_BLEND:
        red_get_alpha_blend_ptr(slots, group_id,
                                &red->u.alpha_blend, &qxl->u.alpha_blend);
        break;
    case QXL_DRAW_BLACKNESS:
        red_get_blackness_ptr(slots, group_id, &red->u.blackness, &qxl->u.blackness);
        break;
    case QXL_DRAW_BLEND:
        red_get_blend_ptr(slots, group_id, &red->u.blend, &qxl->u.blend);
        break;
    case QXL_DRAW_COPY:
        red_get_copy_ptr(slots, group_id, &red->u.copy, &qxl->u.copy);
        break;
    case QXL_COPY_BITS:
        red_get_point_ptr(&red->u.copy_bits.src_pos, &qxl->u.copy_bits.src_pos);
        break;
    case QXL_DRAW_FILL:
        red_get_fill_ptr(slots, group_id, &red->u.fill, &qxl->u.fill);
        break;
    case QXL_DRAW_OPAQUE:
        red_get_opaque_ptr(slots, group_id, &red->u.opaque, &qxl->u.opaque);
        break;
    case QXL_DRAW_INVERS:
        red_get_invers_ptr(slots, group_id, &red->u.invers, &qxl->u.invers);
        break;
    case QXL_DRAW_NOP:
        break;
    case QXL_DRAW_ROP3:
        red_get_rop3_ptr(slots, group_id, &red->u.rop3, &qxl->u.rop3);
        break;
    case QXL_DRAW_STROKE:
        red_get_stroke_ptr(slots, group_id, &red->u.stroke, &qxl->u.stroke);
        break;
    case QXL_DRAW_TEXT:
        red_get_text_ptr(slots, group_id, &red->u.text, &qxl->u.text);
        break;
    case QXL_DRAW_TRANSPARENT:
        red_get_transparent_ptr(slots, group_id,
                                &red->u.transparent, &qxl->u.transparent);
        break;
    case QXL_DRAW_WHITENESS:
        red_get_whiteness_ptr(slots, group_id, &red->u.whiteness, &qxl->u.whiteness);
        break;
    default:
        red_error("%s: unknown type %d", __FUNCTION__, red->type);
        break;
    };
}

void red_get_compat_drawable(RedMemSlotInfo *slots, int group_id,
                             RedDrawable *red, QXLPHYSICAL addr)
{
    QXLCompatDrawable *qxl;

    qxl = (QXLCompatDrawable *)get_virt(slots, addr, sizeof(*qxl), group_id);
    red->release_info     = &qxl->release_info;

    red_get_rect_ptr(&red->bbox, &qxl->bbox);
    red_get_clip_ptr(slots, group_id, &red->clip, &qxl->clip);
    red->effect           = qxl->effect;
    red->mm_time          = qxl->mm_time;

    red->self_bitmap = (qxl->bitmap_offset != 0);
    red_get_rect_ptr(&red->self_bitmap_area, &qxl->bitmap_area);

    red->surfaces_dest[0] = -1;
    red->surfaces_dest[1] = -1;
    red->surfaces_dest[2] = -1;

    red->type = qxl->type;
    switch (red->type) {
    case QXL_DRAW_ALPHA_BLEND:
        red_get_alpha_blend_ptr_compat(slots, group_id,
                                       &red->u.alpha_blend, &qxl->u.alpha_blend);
        break;
    case QXL_DRAW_BLACKNESS:
        red_get_blackness_ptr(slots, group_id, &red->u.blackness, &qxl->u.blackness);
        break;
    case QXL_DRAW_BLEND:
        red_get_blend_ptr(slots, group_id, &red->u.blend, &qxl->u.blend);
        break;
    case QXL_DRAW_COPY:
        red_get_copy_ptr(slots, group_id, &red->u.copy, &qxl->u.copy);
        break;
    case QXL_COPY_BITS:
        red_get_point_ptr(&red->u.copy_bits.src_pos, &qxl->u.copy_bits.src_pos);
        red->surfaces_dest[0] = 0;
        red->surfaces_rects[0].left   = red->u.copy_bits.src_pos.x;
        red->surfaces_rects[0].right  = red->u.copy_bits.src_pos.x +
            (red->bbox.right - red->bbox.left);
        red->surfaces_rects[0].top    = red->u.copy_bits.src_pos.y;
        red->surfaces_rects[0].bottom = red->u.copy_bits.src_pos.y +
            (red->bbox.bottom - red->bbox.top);
        break;
    case QXL_DRAW_FILL:
        red_get_fill_ptr(slots, group_id, &red->u.fill, &qxl->u.fill);
        break;
    case QXL_DRAW_OPAQUE:
        red_get_opaque_ptr(slots, group_id, &red->u.opaque, &qxl->u.opaque);
        break;
    case QXL_DRAW_INVERS:
        red_get_invers_ptr(slots, group_id, &red->u.invers, &qxl->u.invers);
        break;
    case QXL_DRAW_NOP:
        break;
    case QXL_DRAW_ROP3:
        red_get_rop3_ptr(slots, group_id, &red->u.rop3, &qxl->u.rop3);
        break;
    case QXL_DRAW_STROKE:
        red_get_stroke_ptr(slots, group_id, &red->u.stroke, &qxl->u.stroke);
        break;
    case QXL_DRAW_TEXT:
        red_get_text_ptr(slots, group_id, &red->u.text, &qxl->u.text);
        break;
    case QXL_DRAW_TRANSPARENT:
        red_get_transparent_ptr(slots, group_id,
                                &red->u.transparent, &qxl->u.transparent);
        break;
    case QXL_DRAW_WHITENESS:
        red_get_whiteness_ptr(slots, group_id, &red->u.whiteness, &qxl->u.whiteness);
        break;
    default:
        red_error("%s: unknown type %d", __FUNCTION__, red->type);
        break;
    };
}

void red_put_drawable(RedDrawable *red)
{
    red_put_clip(&red->clip);
    switch (red->type) {
    case QXL_DRAW_ALPHA_BLEND:
        red_put_alpha_blend(&red->u.alpha_blend);
        break;
    case QXL_DRAW_BLACKNESS:
        red_put_blackness(&red->u.blackness);
        break;
    case QXL_DRAW_BLEND:
        red_put_blend(&red->u.blend);
        break;
    case QXL_DRAW_COPY:
        red_put_copy(&red->u.copy);
        break;
    case QXL_DRAW_FILL:
        red_put_fill(&red->u.fill);
        break;
    case QXL_DRAW_OPAQUE:
        red_put_opaque(&red->u.opaque);
        break;
    case QXL_DRAW_INVERS:
        red_put_invers(&red->u.invers);
        break;
    case QXL_DRAW_ROP3:
        red_put_rop3(&red->u.rop3);
        break;
    case QXL_DRAW_STROKE:
        red_put_stroke(&red->u.stroke);
        break;
    case QXL_DRAW_TEXT:
        red_put_text_ptr(&red->u.text);
        break;
    case QXL_DRAW_TRANSPARENT:
        red_put_transparent(&red->u.transparent);
        break;
    case QXL_DRAW_WHITENESS:
        red_put_whiteness(&red->u.whiteness);
        break;
    }
}

void red_get_update_cmd(RedMemSlotInfo *slots, int group_id,
                        RedUpdateCmd *red, QXLPHYSICAL addr)
{
    QXLUpdateCmd *qxl;

    qxl = (QXLUpdateCmd *)get_virt(slots, addr, sizeof(*qxl), group_id);
    red->release_info     = &qxl->release_info;

    red_get_rect_ptr(&red->area, &qxl->area);
    red->update_id  = qxl->update_id;
    red->surface_id = qxl->surface_id;
}

void red_put_update_cmd(RedUpdateCmd *red)
{
    /* nothing yet */
}

void red_get_message(RedMemSlotInfo *slots, int group_id,
                     RedMessage *red, QXLPHYSICAL addr)
{
    QXLMessage *qxl;

    /*
     * security alert:
     *   qxl->data[0] size isn't specified anywhere -> can't verify
     *   luckily this is for debug logging only,
     *   so we can just ignore it by default.
     */
    qxl = (QXLMessage *)get_virt(slots, addr, sizeof(*qxl), group_id);
    red->release_info  = &qxl->release_info;
    red->data          = qxl->data;
}

void red_put_message(RedMessage *red)
{
    /* nothing yet */
}

void red_get_surface_cmd(RedMemSlotInfo *slots, int group_id,
                         RedSurfaceCmd *red, QXLPHYSICAL addr)
{
    QXLSurfaceCmd *qxl;
    size_t size;

    qxl = (QXLSurfaceCmd *)get_virt(slots, addr, sizeof(*qxl), group_id);
    red->release_info     = &qxl->release_info;

    red->surface_id = qxl->surface_id;
    red->type       = qxl->type;
    red->flags      = qxl->flags;

    switch (red->type) {
    case QXL_SURFACE_CMD_CREATE:
        red->u.surface_create.format = qxl->u.surface_create.format;
        red->u.surface_create.width  = qxl->u.surface_create.width;
        red->u.surface_create.height = qxl->u.surface_create.height;
        red->u.surface_create.stride = qxl->u.surface_create.stride;
        size = red->u.surface_create.height * abs(red->u.surface_create.stride);
        red->u.surface_create.data =
            (uint8_t*)get_virt(slots, qxl->u.surface_create.data, size, group_id);
        break;
    }
}

void red_put_surface_cmd(RedSurfaceCmd *red)
{
    /* nothing yet */
}

static void red_get_cursor(RedMemSlotInfo *slots, int group_id,
                           SpiceCursor *red, QXLPHYSICAL addr)
{
    QXLCursor *qxl;
    RedDataChunk chunks;
    size_t size;
    uint8_t *data;
    bool free_data;

    qxl = (QXLCursor *)get_virt(slots, addr, sizeof(*qxl), group_id);

    red->header.unique     = qxl->header.unique;
    red->header.type       = qxl->header.type;
    red->header.width      = qxl->header.width;
    red->header.height     = qxl->header.height;
    red->header.hot_spot_x = qxl->header.hot_spot_x;
    red->header.hot_spot_y = qxl->header.hot_spot_y;

    red->flags = 0;
    red->data_size = qxl->data_size;
    size = red_get_data_chunks_ptr(slots, group_id,
                                   get_memslot_id(slots, addr),
                                   &chunks, &qxl->chunk);
    data = red_linearize_chunk(&chunks, size, &free_data);
    red_put_data_chunks(&chunks);
    if (free_data) {
        red->data = data;
    } else {
        red->data = spice_malloc(size);
        memcpy(red->data, data, size);
    }
}

static void red_put_cursor(SpiceCursor *red)
{
    free(red->data);
}

void red_get_cursor_cmd(RedMemSlotInfo *slots, int group_id,
                        RedCursorCmd *red, QXLPHYSICAL addr)
{
    QXLCursorCmd *qxl;

    qxl = (QXLCursorCmd *)get_virt(slots, addr, sizeof(*qxl), group_id);
    red->release_info     = &qxl->release_info;

    red->type = qxl->type;
    switch (red->type) {
    case QXL_CURSOR_SET:
        red_get_point16_ptr(&red->u.set.position, &qxl->u.set.position);
        red->u.set.visible  = qxl->u.set.visible;
        red_get_cursor(slots, group_id,  &red->u.set.shape, qxl->u.set.shape);
        break;
    case QXL_CURSOR_MOVE:
        red_get_point16_ptr(&red->u.position, &qxl->u.position);
        break;
    case QXL_CURSOR_TRAIL:
        red->u.trail.length    = qxl->u.trail.length;
        red->u.trail.frequency = qxl->u.trail.frequency;
        break;
    }
}

void red_put_cursor_cmd(RedCursorCmd *red)
{
    switch (red->type) {
    case QXL_CURSOR_SET:
        red_put_cursor(&red->u.set.shape);
        break;
    }
}

