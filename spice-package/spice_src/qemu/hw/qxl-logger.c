/*
 * qxl command logging -- for debug purposes
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "qxl.h"

static const char *qxl_type[] = {
    [ QXL_CMD_NOP ]     = "nop",
    [ QXL_CMD_DRAW ]    = "draw",
    [ QXL_CMD_UPDATE ]  = "update",
    [ QXL_CMD_CURSOR ]  = "cursor",
    [ QXL_CMD_MESSAGE ] = "message",
    [ QXL_CMD_SURFACE ] = "surface",
};

static const char *qxl_draw_type[] = {
    [ QXL_DRAW_NOP         ] = "nop",
    [ QXL_DRAW_FILL        ] = "fill",
    [ QXL_DRAW_OPAQUE      ] = "opaque",
    [ QXL_DRAW_COPY        ] = "copy",
    [ QXL_COPY_BITS        ] = "copy-bits",
    [ QXL_DRAW_BLEND       ] = "blend",
    [ QXL_DRAW_BLACKNESS   ] = "blackness",
    [ QXL_DRAW_WHITENESS   ] = "whitemess",
    [ QXL_DRAW_INVERS      ] = "invers",
    [ QXL_DRAW_ROP3        ] = "rop3",
    [ QXL_DRAW_STROKE      ] = "stroke",
    [ QXL_DRAW_TEXT        ] = "text",
    [ QXL_DRAW_TRANSPARENT ] = "transparent",
    [ QXL_DRAW_ALPHA_BLEND ] = "alpha-blend",
};

static const char *qxl_draw_effect[] = {
    [ QXL_EFFECT_BLEND            ] = "blend",
    [ QXL_EFFECT_OPAQUE           ] = "opaque",
    [ QXL_EFFECT_REVERT_ON_DUP    ] = "revert-on-dup",
    [ QXL_EFFECT_BLACKNESS_ON_DUP ] = "blackness-on-dup",
    [ QXL_EFFECT_WHITENESS_ON_DUP ] = "whiteness-on-dup",
    [ QXL_EFFECT_NOP_ON_DUP       ] = "nop-on-dup",
    [ QXL_EFFECT_NOP              ] = "nop",
    [ QXL_EFFECT_OPAQUE_BRUSH     ] = "opaque-brush",
};

static const char *qxl_surface_cmd[] = {
   [ QXL_SURFACE_CMD_CREATE  ] = "create",
   [ QXL_SURFACE_CMD_DESTROY ] = "destroy",
};

static const char *spice_surface_fmt[] = {
   [ SPICE_SURFACE_FMT_INVALID  ] = "invalid",
   [ SPICE_SURFACE_FMT_1_A      ] = "alpha/1",
   [ SPICE_SURFACE_FMT_8_A      ] = "alpha/8",
   [ SPICE_SURFACE_FMT_16_555   ] = "555/16",
   [ SPICE_SURFACE_FMT_16_565   ] = "565/16",
   [ SPICE_SURFACE_FMT_32_xRGB  ] = "xRGB/32",
   [ SPICE_SURFACE_FMT_32_ARGB  ] = "ARGB/32",
};

static const char *qxl_cursor_cmd[] = {
   [ QXL_CURSOR_SET   ] = "set",
   [ QXL_CURSOR_MOVE  ] = "move",
   [ QXL_CURSOR_HIDE  ] = "hide",
   [ QXL_CURSOR_TRAIL ] = "trail",
};

static const char *spice_cursor_type[] = {
   [ SPICE_CURSOR_TYPE_ALPHA   ] = "alpha",
   [ SPICE_CURSOR_TYPE_MONO    ] = "mono",
   [ SPICE_CURSOR_TYPE_COLOR4  ] = "color4",
   [ SPICE_CURSOR_TYPE_COLOR8  ] = "color8",
   [ SPICE_CURSOR_TYPE_COLOR16 ] = "color16",
   [ SPICE_CURSOR_TYPE_COLOR24 ] = "color24",
   [ SPICE_CURSOR_TYPE_COLOR32 ] = "color32",
};

static const char *qxl_v2n(const char *n[], size_t l, int v)
{
    if (v >= l || !n[v]) {
        return "???";
    }
    return n[v];
}
#define qxl_name(_list, _value) qxl_v2n(_list, ARRAY_SIZE(_list), _value)

static void qxl_log_cmd_draw(PCIQXLDevice *qxl, QXLDrawable *draw)
{
    fprintf(stderr, ": surface_id %d type %s effect %s",
            draw->surface_id,
            qxl_name(qxl_draw_type, draw->type),
            qxl_name(qxl_draw_effect, draw->effect));
}

static void qxl_log_cmd_draw_compat(PCIQXLDevice *qxl, QXLCompatDrawable *draw)
{
    fprintf(stderr, ": type %s effect %s",
            qxl_name(qxl_draw_type, draw->type),
            qxl_name(qxl_draw_effect, draw->effect));
}

static void qxl_log_cmd_surface(PCIQXLDevice *qxl, QXLSurfaceCmd *cmd)
{
    fprintf(stderr, ": %s id %d",
            qxl_name(qxl_surface_cmd, cmd->type),
            cmd->surface_id);
    if (cmd->type == QXL_SURFACE_CMD_CREATE) {
        fprintf(stderr, " size %dx%d stride %d format %s (count %d, max %d)",
                cmd->u.surface_create.width,
                cmd->u.surface_create.height,
                cmd->u.surface_create.stride,
                qxl_name(spice_surface_fmt, cmd->u.surface_create.format),
                qxl->guest_surfaces.count, qxl->guest_surfaces.max);
    }
    if (cmd->type == QXL_SURFACE_CMD_DESTROY) {
        fprintf(stderr, " (count %d)", qxl->guest_surfaces.count);
    }
}

void qxl_log_cmd_cursor(PCIQXLDevice *qxl, QXLCursorCmd *cmd, int group_id)
{
    QXLCursor *cursor;

    fprintf(stderr, ": %s",
            qxl_name(qxl_cursor_cmd, cmd->type));
    switch (cmd->type) {
    case QXL_CURSOR_SET:
        fprintf(stderr, " +%d+%d visible %s, shape @ 0x%" PRIx64,
                cmd->u.set.position.x,
                cmd->u.set.position.y,
                cmd->u.set.visible ? "yes" : "no",
                cmd->u.set.shape);
        cursor = qxl_phys2virt(qxl, cmd->u.set.shape, group_id);
        fprintf(stderr, " type %s size %dx%d hot-spot +%d+%d"
                " unique 0x%" PRIx64 " data-size %d",
                qxl_name(spice_cursor_type, cursor->header.type),
                cursor->header.width, cursor->header.height,
                cursor->header.hot_spot_x, cursor->header.hot_spot_y,
                cursor->header.unique, cursor->data_size);
        break;
    case QXL_CURSOR_MOVE:
        fprintf(stderr, " +%d+%d", cmd->u.position.x, cmd->u.position.y);
        break;
    }
}

void qxl_log_command(PCIQXLDevice *qxl, const char *ring, QXLCommandExt *ext)
{
    bool compat = ext->flags & QXL_COMMAND_FLAG_COMPAT;
    void *data;

    if (!qxl->cmdlog) {
        return;
    }
    fprintf(stderr, "qxl-%d/%s:", qxl->id, ring);
    fprintf(stderr, " cmd @ 0x%" PRIx64 " %s%s", ext->cmd.data,
            qxl_name(qxl_type, ext->cmd.type),
            compat ? "(compat)" : "");

    data = qxl_phys2virt(qxl, ext->cmd.data, ext->group_id);
    switch (ext->cmd.type) {
    case QXL_CMD_DRAW:
        if (!compat) {
            qxl_log_cmd_draw(qxl, data);
        } else {
            qxl_log_cmd_draw_compat(qxl, data);
        }
        break;
    case QXL_CMD_SURFACE:
        qxl_log_cmd_surface(qxl, data);
        break;
    case QXL_CMD_CURSOR:
        qxl_log_cmd_cursor(qxl, data, ext->group_id);
        break;
    }
    fprintf(stderr, "\n");
}
