#include "red_common.h"
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <spice/protocol.h>
#include <spice/macros.h>
#include <marshaller.h>

#ifdef _MSC_VER
#pragma warning(disable:4101)
#pragma warning(disable:4018)
#endif

void spice_marshall_msg_migrate(SpiceMarshaller *m, SpiceMsgMigrate *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgMigrate *src;
    src = (SpiceMsgMigrate *)msg;

    spice_marshaller_add_uint32(m, src->flags);
}

void spice_marshall_SpiceMsgData(SpiceMarshaller *m, SpiceMsgData *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgData *src;
    src = (SpiceMsgData *)msg;

    /* Remaining data must be appended manually */
}

void spice_marshall_msg_set_ack(SpiceMarshaller *m, SpiceMsgSetAck *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgSetAck *src;
    src = (SpiceMsgSetAck *)msg;

    spice_marshaller_add_uint32(m, src->generation);
    spice_marshaller_add_uint32(m, src->window);
}

void spice_marshall_msg_ping(SpiceMarshaller *m, SpiceMsgPing *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgPing *src;
    src = (SpiceMsgPing *)msg;

    spice_marshaller_add_uint32(m, src->id);
    spice_marshaller_add_uint64(m, src->timestamp);
    /* Remaining data must be appended manually */
}

void spice_marshall_msg_wait_for_channels(SpiceMarshaller *m, SpiceMsgWaitForChannels *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgWaitForChannels *src;
    SpiceWaitForChannel *wait_list__element;
    uint32_t i;
    src = (SpiceMsgWaitForChannels *)msg;

    spice_marshaller_add_uint8(m, src->wait_count);
    wait_list__element = src->wait_list;
    for (i = 0; i < src->wait_count; i++) {
        SpiceWaitForChannel *src2;
        src2 = (SpiceWaitForChannel *)wait_list__element;

        spice_marshaller_add_uint8(m, src2->channel_type);
        spice_marshaller_add_uint8(m, src2->channel_id);
        spice_marshaller_add_uint64(m, src2->message_serial);
        wait_list__element++;
    }
}

void spice_marshall_msg_disconnecting(SpiceMarshaller *m, SpiceMsgDisconnect *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgDisconnect *src;
    src = (SpiceMsgDisconnect *)msg;

    spice_marshaller_add_uint64(m, src->time_stamp);
    spice_marshaller_add_uint32(m, src->reason);
}

void spice_marshall_msg_notify(SpiceMarshaller *m, SpiceMsgNotify *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgNotify *src;
    src = (SpiceMsgNotify *)msg;

    spice_marshaller_add_uint64(m, src->time_stamp);
    spice_marshaller_add_uint32(m, src->severity);
    spice_marshaller_add_uint32(m, src->visibilty);
    spice_marshaller_add_uint32(m, src->what);
    spice_marshaller_add_uint32(m, src->message_len);
    /* Don't marshall @nomarshal message */
}

SPICE_GNUC_UNUSED static void spice_marshall_array_uint8(SpiceMarshaller *m, uint8_t *ptr, int count)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    uint32_t i;

    for (i = 0; i < count; i++) {
        spice_marshaller_add_uint8(m, *ptr++);
    }
}

void spice_marshall_msg_main_migrate_begin(SpiceMarshaller *m, SpiceMsgMainMigrationBegin *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgMainMigrationBegin *src;
    src = (SpiceMsgMainMigrationBegin *)msg;

    spice_marshaller_add_uint16(m, src->port);
    spice_marshaller_add_uint16(m, src->sport);
    spice_marshaller_add_uint32(m, src->host_size);
    m2 = spice_marshaller_get_ptr_submarshaller(m, 0);
    spice_marshall_array_uint8(m2, src->host_data, src->host_size);
    spice_marshaller_add_uint16(m, src->pub_key_type);
    spice_marshaller_add_uint32(m, src->pub_key_size);
    m2 = spice_marshaller_get_ptr_submarshaller(m, 0);
    spice_marshall_array_uint8(m2, src->pub_key_data, src->pub_key_size);
}

void spice_marshall_SpiceMsgEmpty(SpiceMarshaller *m, SpiceMsgEmpty *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgEmpty *src;
    src = (SpiceMsgEmpty *)msg;

}

void spice_marshall_msg_main_init(SpiceMarshaller *m, SpiceMsgMainInit *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgMainInit *src;
    src = (SpiceMsgMainInit *)msg;

    spice_marshaller_add_uint32(m, src->session_id);
    spice_marshaller_add_uint32(m, src->display_channels_hint);
    spice_marshaller_add_uint32(m, src->supported_mouse_modes);
    spice_marshaller_add_uint32(m, src->current_mouse_mode);
    spice_marshaller_add_uint32(m, src->agent_connected);
    spice_marshaller_add_uint32(m, src->agent_tokens);
    spice_marshaller_add_uint32(m, src->multi_media_time);
    spice_marshaller_add_uint32(m, src->ram_hint);
}

void spice_marshall_msg_main_channels_list(SpiceMarshaller *m, SpiceMsgChannels *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgChannels *src;
    SpiceChannelId *channels__element;
    uint32_t i;
    src = (SpiceMsgChannels *)msg;

    spice_marshaller_add_uint32(m, src->num_of_channels);
    channels__element = src->channels;
    for (i = 0; i < src->num_of_channels; i++) {
        SpiceChannelId *src2;
        src2 = (SpiceChannelId *)channels__element;

        spice_marshaller_add_uint8(m, src2->type);
        spice_marshaller_add_uint8(m, src2->id);
        channels__element++;
    }
}

void spice_marshall_msg_main_mouse_mode(SpiceMarshaller *m, SpiceMsgMainMouseMode *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgMainMouseMode *src;
    src = (SpiceMsgMainMouseMode *)msg;

    spice_marshaller_add_uint16(m, src->supported_modes);
    spice_marshaller_add_uint16(m, src->current_mode);
}

void spice_marshall_msg_main_multi_media_time(SpiceMarshaller *m, SpiceMsgMainMultiMediaTime *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgMainMultiMediaTime *src;
    src = (SpiceMsgMainMultiMediaTime *)msg;

    spice_marshaller_add_uint32(m, src->time);
}

void spice_marshall_msg_main_agent_disconnected(SpiceMarshaller *m, SpiceMsgMainAgentDisconnect *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgMainAgentDisconnect *src;
    src = (SpiceMsgMainAgentDisconnect *)msg;

    spice_marshaller_add_uint32(m, src->error_code);
}

void spice_marshall_msg_main_agent_token(SpiceMarshaller *m, SpiceMsgMainAgentTokens *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgMainAgentTokens *src;
    src = (SpiceMsgMainAgentTokens *)msg;

    spice_marshaller_add_uint32(m, src->num_tokens);
}

void spice_marshall_msg_main_migrate_switch_host(SpiceMarshaller *m, SpiceMsgMainMigrationSwitchHost *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgMainMigrationSwitchHost *src;
    src = (SpiceMsgMainMigrationSwitchHost *)msg;

    spice_marshaller_add_uint16(m, src->port);
    spice_marshaller_add_uint16(m, src->sport);
    spice_marshaller_add_uint32(m, src->host_size);
    m2 = spice_marshaller_get_ptr_submarshaller(m, 0);
    if (src->host_data != NULL) {
        spice_marshall_array_uint8(m2, src->host_data, src->host_size);
    }
    spice_marshaller_add_uint32(m, src->cert_subject_size);
    m2 = spice_marshaller_get_ptr_submarshaller(m, 0);
    if (src->cert_subject_data != NULL) {
        spice_marshall_array_uint8(m2, src->cert_subject_data, src->cert_subject_size);
    }
}

void spice_marshall_msg_display_mode(SpiceMarshaller *m, SpiceMsgDisplayMode *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgDisplayMode *src;
    src = (SpiceMsgDisplayMode *)msg;

    spice_marshaller_add_uint32(m, src->x_res);
    spice_marshaller_add_uint32(m, src->y_res);
    spice_marshaller_add_uint32(m, src->bits);
}

void spice_marshall_msg_display_copy_bits(SpiceMarshaller *m, SpiceMsgDisplayCopyBits *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgDisplayCopyBits *src;
    uint32_t i;
    src = (SpiceMsgDisplayCopyBits *)msg;

    /* base */ {
        spice_marshaller_add_uint32(m, src->base.surface_id);
        /* box */ {
            spice_marshaller_add_int32(m, src->base.box.top);
            spice_marshaller_add_int32(m, src->base.box.left);
            spice_marshaller_add_int32(m, src->base.box.bottom);
            spice_marshaller_add_int32(m, src->base.box.right);
        }
        /* clip */ {
            spice_marshaller_add_uint8(m, src->base.clip.type);
            if (src->base.clip.type == SPICE_CLIP_TYPE_RECTS) {
                SpiceRect *rects__element;
                spice_marshaller_add_uint32(m, src->base.clip.rects->num_rects);
                rects__element = src->base.clip.rects->rects;
                for (i = 0; i < src->base.clip.rects->num_rects; i++) {
                    SpiceRect *src2;
                    src2 = (SpiceRect *)rects__element;

                    spice_marshaller_add_int32(m, src2->top);
                    spice_marshaller_add_int32(m, src2->left);
                    spice_marshaller_add_int32(m, src2->bottom);
                    spice_marshaller_add_int32(m, src2->right);
                    rects__element++;
                }
            }
        }
    }
    /* src_pos */ {
        spice_marshaller_add_int32(m, src->src_pos.x);
        spice_marshaller_add_int32(m, src->src_pos.y);
    }
}

void spice_marshall_msg_display_inval_list(SpiceMarshaller *m, SpiceResourceList *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceResourceList *src;
    SpiceResourceID *resources__element;
    uint32_t i;
    src = (SpiceResourceList *)msg;

    spice_marshaller_add_uint16(m, src->count);
    resources__element = src->resources;
    for (i = 0; i < src->count; i++) {
        SpiceResourceID *src2;
        src2 = (SpiceResourceID *)resources__element;

        spice_marshaller_add_uint8(m, src2->type);
        spice_marshaller_add_uint64(m, src2->id);
        resources__element++;
    }
}

void spice_marshall_msg_display_inval_all_pixmaps(SpiceMarshaller *m, SpiceMsgWaitForChannels *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgWaitForChannels *src;
    SpiceWaitForChannel *wait_list__element;
    uint32_t i;
    src = (SpiceMsgWaitForChannels *)msg;

    spice_marshaller_add_uint8(m, src->wait_count);
    wait_list__element = src->wait_list;
    for (i = 0; i < src->wait_count; i++) {
        SpiceWaitForChannel *src2;
        src2 = (SpiceWaitForChannel *)wait_list__element;

        spice_marshaller_add_uint8(m, src2->channel_type);
        spice_marshaller_add_uint8(m, src2->channel_id);
        spice_marshaller_add_uint64(m, src2->message_serial);
        wait_list__element++;
    }
}

void spice_marshall_msg_display_inval_palette(SpiceMarshaller *m, SpiceMsgDisplayInvalOne *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgDisplayInvalOne *src;
    src = (SpiceMsgDisplayInvalOne *)msg;

    spice_marshaller_add_uint64(m, src->id);
}

void spice_marshall_msg_display_stream_create(SpiceMarshaller *m, SpiceMsgDisplayStreamCreate *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgDisplayStreamCreate *src;
    uint32_t i;
    src = (SpiceMsgDisplayStreamCreate *)msg;

    spice_marshaller_add_uint32(m, src->surface_id);
    spice_marshaller_add_uint32(m, src->id);
    spice_marshaller_add_uint8(m, src->flags);
    spice_marshaller_add_uint8(m, src->codec_type);
    spice_marshaller_add_uint64(m, src->stamp);
    spice_marshaller_add_uint32(m, src->stream_width);
    spice_marshaller_add_uint32(m, src->stream_height);
    spice_marshaller_add_uint32(m, src->src_width);
    spice_marshaller_add_uint32(m, src->src_height);
    /* dest */ {
        spice_marshaller_add_int32(m, src->dest.top);
        spice_marshaller_add_int32(m, src->dest.left);
        spice_marshaller_add_int32(m, src->dest.bottom);
        spice_marshaller_add_int32(m, src->dest.right);
    }
    /* clip */ {
        spice_marshaller_add_uint8(m, src->clip.type);
        if (src->clip.type == SPICE_CLIP_TYPE_RECTS) {
            SpiceRect *rects__element;
            spice_marshaller_add_uint32(m, src->clip.rects->num_rects);
            rects__element = src->clip.rects->rects;
            for (i = 0; i < src->clip.rects->num_rects; i++) {
                SpiceRect *src2;
                src2 = (SpiceRect *)rects__element;

                spice_marshaller_add_int32(m, src2->top);
                spice_marshaller_add_int32(m, src2->left);
                spice_marshaller_add_int32(m, src2->bottom);
                spice_marshaller_add_int32(m, src2->right);
                rects__element++;
            }
        }
    }
}

void spice_marshall_msg_display_stream_data(SpiceMarshaller *m, SpiceMsgDisplayStreamData *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgDisplayStreamData *src;
    src = (SpiceMsgDisplayStreamData *)msg;

    spice_marshaller_add_uint32(m, src->id);
    spice_marshaller_add_uint32(m, src->multi_media_time);
    spice_marshaller_add_uint32(m, src->data_size);
    /* Don't marshall @nomarshal data */
}

void spice_marshall_msg_display_stream_clip(SpiceMarshaller *m, SpiceMsgDisplayStreamClip *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgDisplayStreamClip *src;
    uint32_t i;
    src = (SpiceMsgDisplayStreamClip *)msg;

    spice_marshaller_add_uint32(m, src->id);
    /* clip */ {
        spice_marshaller_add_uint8(m, src->clip.type);
        if (src->clip.type == SPICE_CLIP_TYPE_RECTS) {
            SpiceRect *rects__element;
            spice_marshaller_add_uint32(m, src->clip.rects->num_rects);
            rects__element = src->clip.rects->rects;
            for (i = 0; i < src->clip.rects->num_rects; i++) {
                SpiceRect *src2;
                src2 = (SpiceRect *)rects__element;

                spice_marshaller_add_int32(m, src2->top);
                spice_marshaller_add_int32(m, src2->left);
                spice_marshaller_add_int32(m, src2->bottom);
                spice_marshaller_add_int32(m, src2->right);
                rects__element++;
            }
        }
    }
}

void spice_marshall_msg_display_stream_destroy(SpiceMarshaller *m, SpiceMsgDisplayStreamDestroy *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgDisplayStreamDestroy *src;
    src = (SpiceMsgDisplayStreamDestroy *)msg;

    spice_marshaller_add_uint32(m, src->id);
}

void spice_marshall_Palette(SpiceMarshaller *m, SpicePalette *ptr)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpicePalette *src;
    uint32_t *ents__element;
    uint32_t i;

    src = (SpicePalette *)ptr;

    spice_marshaller_add_uint64(m, src->unique);
    spice_marshaller_add_uint16(m, src->num_ents);
    ents__element = src->ents;
    for (i = 0; i < src->num_ents; i++) {
        spice_marshaller_add_uint32(m, *ents__element);
        ents__element++;
    }
}

void spice_marshall_Image(SpiceMarshaller *m, SpiceImage *ptr, SpiceMarshaller **bitmap_palette_out, SpiceMarshaller **lzplt_palette_out)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceImage *src;
    *bitmap_palette_out = NULL;
    *lzplt_palette_out = NULL;

    src = (SpiceImage *)ptr;

    /* descriptor */ {
        spice_marshaller_add_uint64(m, src->descriptor.id);
        spice_marshaller_add_uint8(m, src->descriptor.type);
        spice_marshaller_add_uint8(m, src->descriptor.flags);
        spice_marshaller_add_uint32(m, src->descriptor.width);
        spice_marshaller_add_uint32(m, src->descriptor.height);
    }
    if (src->descriptor.type == SPICE_IMAGE_TYPE_BITMAP) {
        spice_marshaller_add_uint8(m, src->u.bitmap.format);
        spice_marshaller_add_uint8(m, src->u.bitmap.flags);
        spice_marshaller_add_uint32(m, src->u.bitmap.x);
        spice_marshaller_add_uint32(m, src->u.bitmap.y);
        spice_marshaller_add_uint32(m, src->u.bitmap.stride);
        if ((src->u.bitmap.flags & SPICE_BITMAP_FLAGS_PAL_FROM_CACHE)) {
            spice_marshaller_add_uint64(m, src->u.bitmap.palette_id);
        } else if (1) {
            *bitmap_palette_out = spice_marshaller_get_ptr_submarshaller(m, 0);
        }
        /* Don't marshall @nomarshal data */
    } else if (src->descriptor.type == SPICE_IMAGE_TYPE_QUIC) {
        spice_marshaller_add_uint32(m, src->u.quic.data_size);
        /* Don't marshall @nomarshal data */
    } else if (src->descriptor.type == SPICE_IMAGE_TYPE_LZ_RGB || src->descriptor.type == SPICE_IMAGE_TYPE_GLZ_RGB) {
        spice_marshaller_add_uint32(m, src->u.lz_rgb.data_size);
        /* Don't marshall @nomarshal data */
    } else if (src->descriptor.type == SPICE_IMAGE_TYPE_JPEG) {
        spice_marshaller_add_uint32(m, src->u.jpeg.data_size);
        /* Don't marshall @nomarshal data */
    } else if (src->descriptor.type == SPICE_IMAGE_TYPE_LZ_PLT) {
        spice_marshaller_add_uint8(m, src->u.lz_plt.flags);
        spice_marshaller_add_uint32(m, src->u.lz_plt.data_size);
        if ((src->u.lz_plt.flags & SPICE_BITMAP_FLAGS_PAL_FROM_CACHE)) {
            spice_marshaller_add_uint64(m, src->u.lz_plt.palette_id);
        } else if (1) {
            *lzplt_palette_out = spice_marshaller_get_ptr_submarshaller(m, 0);
        }
        /* Don't marshall @nomarshal data */
    } else if (src->descriptor.type == SPICE_IMAGE_TYPE_ZLIB_GLZ_RGB) {
        spice_marshaller_add_uint32(m, src->u.zlib_glz.glz_data_size);
        spice_marshaller_add_uint32(m, src->u.zlib_glz.data_size);
        /* Don't marshall @nomarshal data */
    } else if (src->descriptor.type == SPICE_IMAGE_TYPE_JPEG_ALPHA) {
        spice_marshaller_add_uint8(m, src->u.jpeg_alpha.flags);
        spice_marshaller_add_uint32(m, src->u.jpeg_alpha.jpeg_size);
        spice_marshaller_add_uint32(m, src->u.jpeg_alpha.data_size);
        /* Don't marshall @nomarshal data */
    } else if (src->descriptor.type == SPICE_IMAGE_TYPE_SURFACE) {
        spice_marshaller_add_uint32(m, src->u.surface.surface_id);
    }
}

void spice_marshall_msg_display_draw_fill(SpiceMarshaller *m, SpiceMsgDisplayDrawFill *msg, SpiceMarshaller **brush_pat_out, SpiceMarshaller **mask_bitmap_out)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgDisplayDrawFill *src;
    uint32_t i;
    *brush_pat_out = NULL;
    *mask_bitmap_out = NULL;
    src = (SpiceMsgDisplayDrawFill *)msg;

    /* base */ {
        spice_marshaller_add_uint32(m, src->base.surface_id);
        /* box */ {
            spice_marshaller_add_int32(m, src->base.box.top);
            spice_marshaller_add_int32(m, src->base.box.left);
            spice_marshaller_add_int32(m, src->base.box.bottom);
            spice_marshaller_add_int32(m, src->base.box.right);
        }
        /* clip */ {
            spice_marshaller_add_uint8(m, src->base.clip.type);
            if (src->base.clip.type == SPICE_CLIP_TYPE_RECTS) {
                SpiceRect *rects__element;
                spice_marshaller_add_uint32(m, src->base.clip.rects->num_rects);
                rects__element = src->base.clip.rects->rects;
                for (i = 0; i < src->base.clip.rects->num_rects; i++) {
                    SpiceRect *src2;
                    src2 = (SpiceRect *)rects__element;

                    spice_marshaller_add_int32(m, src2->top);
                    spice_marshaller_add_int32(m, src2->left);
                    spice_marshaller_add_int32(m, src2->bottom);
                    spice_marshaller_add_int32(m, src2->right);
                    rects__element++;
                }
            }
        }
    }
    /* data */ {
        /* brush */ {
            spice_marshaller_add_uint8(m, src->data.brush.type);
            if (src->data.brush.type == SPICE_BRUSH_TYPE_SOLID) {
                spice_marshaller_add_uint32(m, src->data.brush.u.color);
            } else if (src->data.brush.type == SPICE_BRUSH_TYPE_PATTERN) {
                *brush_pat_out = spice_marshaller_get_ptr_submarshaller(m, 0);
                /* pos */ {
                    spice_marshaller_add_int32(m, src->data.brush.u.pattern.pos.x);
                    spice_marshaller_add_int32(m, src->data.brush.u.pattern.pos.y);
                }
            }
        }
        spice_marshaller_add_uint16(m, src->data.rop_descriptor);
        /* mask */ {
            spice_marshaller_add_uint8(m, src->data.mask.flags);
            /* pos */ {
                spice_marshaller_add_int32(m, src->data.mask.pos.x);
                spice_marshaller_add_int32(m, src->data.mask.pos.y);
            }
            *mask_bitmap_out = spice_marshaller_get_ptr_submarshaller(m, 0);
        }
    }
}

void spice_marshall_msg_display_draw_opaque(SpiceMarshaller *m, SpiceMsgDisplayDrawOpaque *msg, SpiceMarshaller **src_bitmap_out, SpiceMarshaller **pat_out, SpiceMarshaller **mask_bitmap_out)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgDisplayDrawOpaque *src;
    uint32_t i;
    *src_bitmap_out = NULL;
    *pat_out = NULL;
    *mask_bitmap_out = NULL;
    src = (SpiceMsgDisplayDrawOpaque *)msg;

    /* base */ {
        spice_marshaller_add_uint32(m, src->base.surface_id);
        /* box */ {
            spice_marshaller_add_int32(m, src->base.box.top);
            spice_marshaller_add_int32(m, src->base.box.left);
            spice_marshaller_add_int32(m, src->base.box.bottom);
            spice_marshaller_add_int32(m, src->base.box.right);
        }
        /* clip */ {
            spice_marshaller_add_uint8(m, src->base.clip.type);
            if (src->base.clip.type == SPICE_CLIP_TYPE_RECTS) {
                SpiceRect *rects__element;
                spice_marshaller_add_uint32(m, src->base.clip.rects->num_rects);
                rects__element = src->base.clip.rects->rects;
                for (i = 0; i < src->base.clip.rects->num_rects; i++) {
                    SpiceRect *src2;
                    src2 = (SpiceRect *)rects__element;

                    spice_marshaller_add_int32(m, src2->top);
                    spice_marshaller_add_int32(m, src2->left);
                    spice_marshaller_add_int32(m, src2->bottom);
                    spice_marshaller_add_int32(m, src2->right);
                    rects__element++;
                }
            }
        }
    }
    /* data */ {
        *src_bitmap_out = spice_marshaller_get_ptr_submarshaller(m, 0);
        /* src_area */ {
            spice_marshaller_add_int32(m, src->data.src_area.top);
            spice_marshaller_add_int32(m, src->data.src_area.left);
            spice_marshaller_add_int32(m, src->data.src_area.bottom);
            spice_marshaller_add_int32(m, src->data.src_area.right);
        }
        /* brush */ {
            spice_marshaller_add_uint8(m, src->data.brush.type);
            if (src->data.brush.type == SPICE_BRUSH_TYPE_SOLID) {
                spice_marshaller_add_uint32(m, src->data.brush.u.color);
            } else if (src->data.brush.type == SPICE_BRUSH_TYPE_PATTERN) {
                *pat_out = spice_marshaller_get_ptr_submarshaller(m, 0);
                /* pos */ {
                    spice_marshaller_add_int32(m, src->data.brush.u.pattern.pos.x);
                    spice_marshaller_add_int32(m, src->data.brush.u.pattern.pos.y);
                }
            }
        }
        spice_marshaller_add_uint16(m, src->data.rop_descriptor);
        spice_marshaller_add_uint8(m, src->data.scale_mode);
        /* mask */ {
            spice_marshaller_add_uint8(m, src->data.mask.flags);
            /* pos */ {
                spice_marshaller_add_int32(m, src->data.mask.pos.x);
                spice_marshaller_add_int32(m, src->data.mask.pos.y);
            }
            *mask_bitmap_out = spice_marshaller_get_ptr_submarshaller(m, 0);
        }
    }
}

void spice_marshall_msg_display_draw_copy(SpiceMarshaller *m, SpiceMsgDisplayDrawCopy *msg, SpiceMarshaller **src_bitmap_out, SpiceMarshaller **mask_bitmap_out)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgDisplayDrawCopy *src;
    uint32_t i;
    *src_bitmap_out = NULL;
    *mask_bitmap_out = NULL;
    src = (SpiceMsgDisplayDrawCopy *)msg;

    /* base */ {
        spice_marshaller_add_uint32(m, src->base.surface_id);
        /* box */ {
            spice_marshaller_add_int32(m, src->base.box.top);
            spice_marshaller_add_int32(m, src->base.box.left);
            spice_marshaller_add_int32(m, src->base.box.bottom);
            spice_marshaller_add_int32(m, src->base.box.right);
        }
        /* clip */ {
            spice_marshaller_add_uint8(m, src->base.clip.type);
            if (src->base.clip.type == SPICE_CLIP_TYPE_RECTS) {
                SpiceRect *rects__element;
                spice_marshaller_add_uint32(m, src->base.clip.rects->num_rects);
                rects__element = src->base.clip.rects->rects;
                for (i = 0; i < src->base.clip.rects->num_rects; i++) {
                    SpiceRect *src2;
                    src2 = (SpiceRect *)rects__element;

                    spice_marshaller_add_int32(m, src2->top);
                    spice_marshaller_add_int32(m, src2->left);
                    spice_marshaller_add_int32(m, src2->bottom);
                    spice_marshaller_add_int32(m, src2->right);
                    rects__element++;
                }
            }
        }
    }
    /* data */ {
        *src_bitmap_out = spice_marshaller_get_ptr_submarshaller(m, 0);
        /* src_area */ {
            spice_marshaller_add_int32(m, src->data.src_area.top);
            spice_marshaller_add_int32(m, src->data.src_area.left);
            spice_marshaller_add_int32(m, src->data.src_area.bottom);
            spice_marshaller_add_int32(m, src->data.src_area.right);
        }
        spice_marshaller_add_uint16(m, src->data.rop_descriptor);
        spice_marshaller_add_uint8(m, src->data.scale_mode);
        /* mask */ {
            spice_marshaller_add_uint8(m, src->data.mask.flags);
            /* pos */ {
                spice_marshaller_add_int32(m, src->data.mask.pos.x);
                spice_marshaller_add_int32(m, src->data.mask.pos.y);
            }
            *mask_bitmap_out = spice_marshaller_get_ptr_submarshaller(m, 0);
        }
    }
}

void spice_marshall_msg_display_draw_blend(SpiceMarshaller *m, SpiceMsgDisplayDrawBlend *msg, SpiceMarshaller **src_bitmap_out, SpiceMarshaller **mask_bitmap_out)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgDisplayDrawBlend *src;
    uint32_t i;
    *src_bitmap_out = NULL;
    *mask_bitmap_out = NULL;
    src = (SpiceMsgDisplayDrawBlend *)msg;

    /* base */ {
        spice_marshaller_add_uint32(m, src->base.surface_id);
        /* box */ {
            spice_marshaller_add_int32(m, src->base.box.top);
            spice_marshaller_add_int32(m, src->base.box.left);
            spice_marshaller_add_int32(m, src->base.box.bottom);
            spice_marshaller_add_int32(m, src->base.box.right);
        }
        /* clip */ {
            spice_marshaller_add_uint8(m, src->base.clip.type);
            if (src->base.clip.type == SPICE_CLIP_TYPE_RECTS) {
                SpiceRect *rects__element;
                spice_marshaller_add_uint32(m, src->base.clip.rects->num_rects);
                rects__element = src->base.clip.rects->rects;
                for (i = 0; i < src->base.clip.rects->num_rects; i++) {
                    SpiceRect *src2;
                    src2 = (SpiceRect *)rects__element;

                    spice_marshaller_add_int32(m, src2->top);
                    spice_marshaller_add_int32(m, src2->left);
                    spice_marshaller_add_int32(m, src2->bottom);
                    spice_marshaller_add_int32(m, src2->right);
                    rects__element++;
                }
            }
        }
    }
    /* data */ {
        *src_bitmap_out = spice_marshaller_get_ptr_submarshaller(m, 0);
        /* src_area */ {
            spice_marshaller_add_int32(m, src->data.src_area.top);
            spice_marshaller_add_int32(m, src->data.src_area.left);
            spice_marshaller_add_int32(m, src->data.src_area.bottom);
            spice_marshaller_add_int32(m, src->data.src_area.right);
        }
        spice_marshaller_add_uint16(m, src->data.rop_descriptor);
        spice_marshaller_add_uint8(m, src->data.scale_mode);
        /* mask */ {
            spice_marshaller_add_uint8(m, src->data.mask.flags);
            /* pos */ {
                spice_marshaller_add_int32(m, src->data.mask.pos.x);
                spice_marshaller_add_int32(m, src->data.mask.pos.y);
            }
            *mask_bitmap_out = spice_marshaller_get_ptr_submarshaller(m, 0);
        }
    }
}

void spice_marshall_msg_display_draw_blackness(SpiceMarshaller *m, SpiceMsgDisplayDrawBlackness *msg, SpiceMarshaller **mask_bitmap_out)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgDisplayDrawBlackness *src;
    uint32_t i;
    *mask_bitmap_out = NULL;
    src = (SpiceMsgDisplayDrawBlackness *)msg;

    /* base */ {
        spice_marshaller_add_uint32(m, src->base.surface_id);
        /* box */ {
            spice_marshaller_add_int32(m, src->base.box.top);
            spice_marshaller_add_int32(m, src->base.box.left);
            spice_marshaller_add_int32(m, src->base.box.bottom);
            spice_marshaller_add_int32(m, src->base.box.right);
        }
        /* clip */ {
            spice_marshaller_add_uint8(m, src->base.clip.type);
            if (src->base.clip.type == SPICE_CLIP_TYPE_RECTS) {
                SpiceRect *rects__element;
                spice_marshaller_add_uint32(m, src->base.clip.rects->num_rects);
                rects__element = src->base.clip.rects->rects;
                for (i = 0; i < src->base.clip.rects->num_rects; i++) {
                    SpiceRect *src2;
                    src2 = (SpiceRect *)rects__element;

                    spice_marshaller_add_int32(m, src2->top);
                    spice_marshaller_add_int32(m, src2->left);
                    spice_marshaller_add_int32(m, src2->bottom);
                    spice_marshaller_add_int32(m, src2->right);
                    rects__element++;
                }
            }
        }
    }
    /* data */ {
        /* mask */ {
            spice_marshaller_add_uint8(m, src->data.mask.flags);
            /* pos */ {
                spice_marshaller_add_int32(m, src->data.mask.pos.x);
                spice_marshaller_add_int32(m, src->data.mask.pos.y);
            }
            *mask_bitmap_out = spice_marshaller_get_ptr_submarshaller(m, 0);
        }
    }
}

void spice_marshall_msg_display_draw_whiteness(SpiceMarshaller *m, SpiceMsgDisplayDrawWhiteness *msg, SpiceMarshaller **mask_bitmap_out)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgDisplayDrawWhiteness *src;
    uint32_t i;
    *mask_bitmap_out = NULL;
    src = (SpiceMsgDisplayDrawWhiteness *)msg;

    /* base */ {
        spice_marshaller_add_uint32(m, src->base.surface_id);
        /* box */ {
            spice_marshaller_add_int32(m, src->base.box.top);
            spice_marshaller_add_int32(m, src->base.box.left);
            spice_marshaller_add_int32(m, src->base.box.bottom);
            spice_marshaller_add_int32(m, src->base.box.right);
        }
        /* clip */ {
            spice_marshaller_add_uint8(m, src->base.clip.type);
            if (src->base.clip.type == SPICE_CLIP_TYPE_RECTS) {
                SpiceRect *rects__element;
                spice_marshaller_add_uint32(m, src->base.clip.rects->num_rects);
                rects__element = src->base.clip.rects->rects;
                for (i = 0; i < src->base.clip.rects->num_rects; i++) {
                    SpiceRect *src2;
                    src2 = (SpiceRect *)rects__element;

                    spice_marshaller_add_int32(m, src2->top);
                    spice_marshaller_add_int32(m, src2->left);
                    spice_marshaller_add_int32(m, src2->bottom);
                    spice_marshaller_add_int32(m, src2->right);
                    rects__element++;
                }
            }
        }
    }
    /* data */ {
        /* mask */ {
            spice_marshaller_add_uint8(m, src->data.mask.flags);
            /* pos */ {
                spice_marshaller_add_int32(m, src->data.mask.pos.x);
                spice_marshaller_add_int32(m, src->data.mask.pos.y);
            }
            *mask_bitmap_out = spice_marshaller_get_ptr_submarshaller(m, 0);
        }
    }
}

void spice_marshall_msg_display_draw_invers(SpiceMarshaller *m, SpiceMsgDisplayDrawInvers *msg, SpiceMarshaller **mask_bitmap_out)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgDisplayDrawInvers *src;
    uint32_t i;
    *mask_bitmap_out = NULL;
    src = (SpiceMsgDisplayDrawInvers *)msg;

    /* base */ {
        spice_marshaller_add_uint32(m, src->base.surface_id);
        /* box */ {
            spice_marshaller_add_int32(m, src->base.box.top);
            spice_marshaller_add_int32(m, src->base.box.left);
            spice_marshaller_add_int32(m, src->base.box.bottom);
            spice_marshaller_add_int32(m, src->base.box.right);
        }
        /* clip */ {
            spice_marshaller_add_uint8(m, src->base.clip.type);
            if (src->base.clip.type == SPICE_CLIP_TYPE_RECTS) {
                SpiceRect *rects__element;
                spice_marshaller_add_uint32(m, src->base.clip.rects->num_rects);
                rects__element = src->base.clip.rects->rects;
                for (i = 0; i < src->base.clip.rects->num_rects; i++) {
                    SpiceRect *src2;
                    src2 = (SpiceRect *)rects__element;

                    spice_marshaller_add_int32(m, src2->top);
                    spice_marshaller_add_int32(m, src2->left);
                    spice_marshaller_add_int32(m, src2->bottom);
                    spice_marshaller_add_int32(m, src2->right);
                    rects__element++;
                }
            }
        }
    }
    /* data */ {
        /* mask */ {
            spice_marshaller_add_uint8(m, src->data.mask.flags);
            /* pos */ {
                spice_marshaller_add_int32(m, src->data.mask.pos.x);
                spice_marshaller_add_int32(m, src->data.mask.pos.y);
            }
            *mask_bitmap_out = spice_marshaller_get_ptr_submarshaller(m, 0);
        }
    }
}

void spice_marshall_msg_display_draw_rop3(SpiceMarshaller *m, SpiceMsgDisplayDrawRop3 *msg, SpiceMarshaller **src_bitmap_out, SpiceMarshaller **pat_out, SpiceMarshaller **mask_bitmap_out)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgDisplayDrawRop3 *src;
    uint32_t i;
    *src_bitmap_out = NULL;
    *pat_out = NULL;
    *mask_bitmap_out = NULL;
    src = (SpiceMsgDisplayDrawRop3 *)msg;

    /* base */ {
        spice_marshaller_add_uint32(m, src->base.surface_id);
        /* box */ {
            spice_marshaller_add_int32(m, src->base.box.top);
            spice_marshaller_add_int32(m, src->base.box.left);
            spice_marshaller_add_int32(m, src->base.box.bottom);
            spice_marshaller_add_int32(m, src->base.box.right);
        }
        /* clip */ {
            spice_marshaller_add_uint8(m, src->base.clip.type);
            if (src->base.clip.type == SPICE_CLIP_TYPE_RECTS) {
                SpiceRect *rects__element;
                spice_marshaller_add_uint32(m, src->base.clip.rects->num_rects);
                rects__element = src->base.clip.rects->rects;
                for (i = 0; i < src->base.clip.rects->num_rects; i++) {
                    SpiceRect *src2;
                    src2 = (SpiceRect *)rects__element;

                    spice_marshaller_add_int32(m, src2->top);
                    spice_marshaller_add_int32(m, src2->left);
                    spice_marshaller_add_int32(m, src2->bottom);
                    spice_marshaller_add_int32(m, src2->right);
                    rects__element++;
                }
            }
        }
    }
    /* data */ {
        *src_bitmap_out = spice_marshaller_get_ptr_submarshaller(m, 0);
        /* src_area */ {
            spice_marshaller_add_int32(m, src->data.src_area.top);
            spice_marshaller_add_int32(m, src->data.src_area.left);
            spice_marshaller_add_int32(m, src->data.src_area.bottom);
            spice_marshaller_add_int32(m, src->data.src_area.right);
        }
        /* brush */ {
            spice_marshaller_add_uint8(m, src->data.brush.type);
            if (src->data.brush.type == SPICE_BRUSH_TYPE_SOLID) {
                spice_marshaller_add_uint32(m, src->data.brush.u.color);
            } else if (src->data.brush.type == SPICE_BRUSH_TYPE_PATTERN) {
                *pat_out = spice_marshaller_get_ptr_submarshaller(m, 0);
                /* pos */ {
                    spice_marshaller_add_int32(m, src->data.brush.u.pattern.pos.x);
                    spice_marshaller_add_int32(m, src->data.brush.u.pattern.pos.y);
                }
            }
        }
        spice_marshaller_add_uint8(m, src->data.rop3);
        spice_marshaller_add_uint8(m, src->data.scale_mode);
        /* mask */ {
            spice_marshaller_add_uint8(m, src->data.mask.flags);
            /* pos */ {
                spice_marshaller_add_int32(m, src->data.mask.pos.x);
                spice_marshaller_add_int32(m, src->data.mask.pos.y);
            }
            *mask_bitmap_out = spice_marshaller_get_ptr_submarshaller(m, 0);
        }
    }
}

void spice_marshall_Path(SpiceMarshaller *m, SpicePath *ptr)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpicePath *src;
    SpicePathSeg * *segments__element;
    uint32_t i;
    uint32_t j;

    src = (SpicePath *)ptr;

    spice_marshaller_add_uint32(m, src->num_segments);
    segments__element = src->segments;
    for (i = 0; i < src->num_segments; i++) {
        SpicePathSeg *src2;
        SpicePointFix *points__element;
        src2 = (SpicePathSeg *)*segments__element;

        spice_marshaller_add_uint8(m, src2->flags);
        spice_marshaller_add_uint32(m, src2->count);
        points__element = src2->points;
        for (j = 0; j < src2->count; j++) {
            SpicePointFix *src3;
            src3 = (SpicePointFix *)points__element;

            spice_marshaller_add_int32(m, src3->x);
            spice_marshaller_add_int32(m, src3->y);
            points__element++;
        }
        segments__element++;
    }
}

SPICE_GNUC_UNUSED static void spice_marshall_array_int32(SpiceMarshaller *m, int32_t *ptr, int count)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    uint32_t i;

    for (i = 0; i < count; i++) {
        spice_marshaller_add_int32(m, *ptr++);
    }
}

void spice_marshall_msg_display_draw_stroke(SpiceMarshaller *m, SpiceMsgDisplayDrawStroke *msg, SpiceMarshaller **style_out, SpiceMarshaller **pat_out)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgDisplayDrawStroke *src;
    uint32_t i;
    *style_out = NULL;
    *pat_out = NULL;
    src = (SpiceMsgDisplayDrawStroke *)msg;

    /* base */ {
        spice_marshaller_add_uint32(m, src->base.surface_id);
        /* box */ {
            spice_marshaller_add_int32(m, src->base.box.top);
            spice_marshaller_add_int32(m, src->base.box.left);
            spice_marshaller_add_int32(m, src->base.box.bottom);
            spice_marshaller_add_int32(m, src->base.box.right);
        }
        /* clip */ {
            spice_marshaller_add_uint8(m, src->base.clip.type);
            if (src->base.clip.type == SPICE_CLIP_TYPE_RECTS) {
                SpiceRect *rects__element;
                spice_marshaller_add_uint32(m, src->base.clip.rects->num_rects);
                rects__element = src->base.clip.rects->rects;
                for (i = 0; i < src->base.clip.rects->num_rects; i++) {
                    SpiceRect *src2;
                    src2 = (SpiceRect *)rects__element;

                    spice_marshaller_add_int32(m, src2->top);
                    spice_marshaller_add_int32(m, src2->left);
                    spice_marshaller_add_int32(m, src2->bottom);
                    spice_marshaller_add_int32(m, src2->right);
                    rects__element++;
                }
            }
        }
    }
    /* data */ {
        m2 = spice_marshaller_get_ptr_submarshaller(m, 0);
        spice_marshall_Path(m2, src->data.path);
        /* attr */ {
            spice_marshaller_add_uint8(m, src->data.attr.flags);
            if ((src->data.attr.flags & SPICE_LINE_FLAGS_STYLED)) {
                spice_marshaller_add_uint8(m, src->data.attr.style_nseg);
            }
            if ((src->data.attr.flags & SPICE_LINE_FLAGS_STYLED)) {
                *style_out = spice_marshaller_get_ptr_submarshaller(m, 0);
            }
        }
        /* brush */ {
            spice_marshaller_add_uint8(m, src->data.brush.type);
            if (src->data.brush.type == SPICE_BRUSH_TYPE_SOLID) {
                spice_marshaller_add_uint32(m, src->data.brush.u.color);
            } else if (src->data.brush.type == SPICE_BRUSH_TYPE_PATTERN) {
                *pat_out = spice_marshaller_get_ptr_submarshaller(m, 0);
                /* pos */ {
                    spice_marshaller_add_int32(m, src->data.brush.u.pattern.pos.x);
                    spice_marshaller_add_int32(m, src->data.brush.u.pattern.pos.y);
                }
            }
        }
        spice_marshaller_add_uint16(m, src->data.fore_mode);
        spice_marshaller_add_uint16(m, src->data.back_mode);
    }
}

void spice_marshall_String(SpiceMarshaller *m, SpiceString *ptr)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceString *src;
    SpiceRasterGlyph * *glyphs__element;
    uint32_t i;
    uint32_t j;

    src = (SpiceString *)ptr;

    spice_marshaller_add_uint16(m, src->length);
    spice_marshaller_add_uint8(m, src->flags);
    if ((src->flags & SPICE_STRING_FLAGS_RASTER_A1)) {
        glyphs__element = src->glyphs;
        for (i = 0; i < src->length; i++) {
            SpiceRasterGlyph *src2;
            uint8_t *data__element;
            src2 = (SpiceRasterGlyph *)*glyphs__element;

            /* render_pos */ {
                spice_marshaller_add_int32(m, src2->render_pos.x);
                spice_marshaller_add_int32(m, src2->render_pos.y);
            }
            /* glyph_origin */ {
                spice_marshaller_add_int32(m, src2->glyph_origin.x);
                spice_marshaller_add_int32(m, src2->glyph_origin.y);
            }
            spice_marshaller_add_uint16(m, src2->width);
            spice_marshaller_add_uint16(m, src2->height);
            data__element = src2->data;
            for (j = 0; j < (((src2->width + 7) / 8 ) * src2->height); j++) {
                spice_marshaller_add_uint8(m, *data__element);
                data__element++;
            }
            glyphs__element++;
        }
    } else if ((src->flags & SPICE_STRING_FLAGS_RASTER_A4)) {
        glyphs__element = src->glyphs;
        for (i = 0; i < src->length; i++) {
            SpiceRasterGlyph *src2;
            uint8_t *data__element;
            src2 = (SpiceRasterGlyph *)*glyphs__element;

            /* render_pos */ {
                spice_marshaller_add_int32(m, src2->render_pos.x);
                spice_marshaller_add_int32(m, src2->render_pos.y);
            }
            /* glyph_origin */ {
                spice_marshaller_add_int32(m, src2->glyph_origin.x);
                spice_marshaller_add_int32(m, src2->glyph_origin.y);
            }
            spice_marshaller_add_uint16(m, src2->width);
            spice_marshaller_add_uint16(m, src2->height);
            data__element = src2->data;
            for (j = 0; j < (((4 * src2->width + 7) / 8 ) * src2->height); j++) {
                spice_marshaller_add_uint8(m, *data__element);
                data__element++;
            }
            glyphs__element++;
        }
    } else if ((src->flags & SPICE_STRING_FLAGS_RASTER_A8)) {
        glyphs__element = src->glyphs;
        for (i = 0; i < src->length; i++) {
            SpiceRasterGlyph *src2;
            uint8_t *data__element;
            src2 = (SpiceRasterGlyph *)*glyphs__element;

            /* render_pos */ {
                spice_marshaller_add_int32(m, src2->render_pos.x);
                spice_marshaller_add_int32(m, src2->render_pos.y);
            }
            /* glyph_origin */ {
                spice_marshaller_add_int32(m, src2->glyph_origin.x);
                spice_marshaller_add_int32(m, src2->glyph_origin.y);
            }
            spice_marshaller_add_uint16(m, src2->width);
            spice_marshaller_add_uint16(m, src2->height);
            data__element = src2->data;
            for (j = 0; j < (src2->width * src2->height); j++) {
                spice_marshaller_add_uint8(m, *data__element);
                data__element++;
            }
            glyphs__element++;
        }
    }
}

void spice_marshall_msg_display_draw_text(SpiceMarshaller *m, SpiceMsgDisplayDrawText *msg, SpiceMarshaller **fore_brush_pat_out, SpiceMarshaller **back_brush_pat_out)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgDisplayDrawText *src;
    uint32_t i;
    *fore_brush_pat_out = NULL;
    *back_brush_pat_out = NULL;
    src = (SpiceMsgDisplayDrawText *)msg;

    /* base */ {
        spice_marshaller_add_uint32(m, src->base.surface_id);
        /* box */ {
            spice_marshaller_add_int32(m, src->base.box.top);
            spice_marshaller_add_int32(m, src->base.box.left);
            spice_marshaller_add_int32(m, src->base.box.bottom);
            spice_marshaller_add_int32(m, src->base.box.right);
        }
        /* clip */ {
            spice_marshaller_add_uint8(m, src->base.clip.type);
            if (src->base.clip.type == SPICE_CLIP_TYPE_RECTS) {
                SpiceRect *rects__element;
                spice_marshaller_add_uint32(m, src->base.clip.rects->num_rects);
                rects__element = src->base.clip.rects->rects;
                for (i = 0; i < src->base.clip.rects->num_rects; i++) {
                    SpiceRect *src2;
                    src2 = (SpiceRect *)rects__element;

                    spice_marshaller_add_int32(m, src2->top);
                    spice_marshaller_add_int32(m, src2->left);
                    spice_marshaller_add_int32(m, src2->bottom);
                    spice_marshaller_add_int32(m, src2->right);
                    rects__element++;
                }
            }
        }
    }
    /* data */ {
        m2 = spice_marshaller_get_ptr_submarshaller(m, 0);
        spice_marshall_String(m2, src->data.str);
        /* back_area */ {
            spice_marshaller_add_int32(m, src->data.back_area.top);
            spice_marshaller_add_int32(m, src->data.back_area.left);
            spice_marshaller_add_int32(m, src->data.back_area.bottom);
            spice_marshaller_add_int32(m, src->data.back_area.right);
        }
        /* fore_brush */ {
            spice_marshaller_add_uint8(m, src->data.fore_brush.type);
            if (src->data.fore_brush.type == SPICE_BRUSH_TYPE_SOLID) {
                spice_marshaller_add_uint32(m, src->data.fore_brush.u.color);
            } else if (src->data.fore_brush.type == SPICE_BRUSH_TYPE_PATTERN) {
                *fore_brush_pat_out = spice_marshaller_get_ptr_submarshaller(m, 0);
                /* pos */ {
                    spice_marshaller_add_int32(m, src->data.fore_brush.u.pattern.pos.x);
                    spice_marshaller_add_int32(m, src->data.fore_brush.u.pattern.pos.y);
                }
            }
        }
        /* back_brush */ {
            spice_marshaller_add_uint8(m, src->data.back_brush.type);
            if (src->data.back_brush.type == SPICE_BRUSH_TYPE_SOLID) {
                spice_marshaller_add_uint32(m, src->data.back_brush.u.color);
            } else if (src->data.back_brush.type == SPICE_BRUSH_TYPE_PATTERN) {
                *back_brush_pat_out = spice_marshaller_get_ptr_submarshaller(m, 0);
                /* pos */ {
                    spice_marshaller_add_int32(m, src->data.back_brush.u.pattern.pos.x);
                    spice_marshaller_add_int32(m, src->data.back_brush.u.pattern.pos.y);
                }
            }
        }
        spice_marshaller_add_uint16(m, src->data.fore_mode);
        spice_marshaller_add_uint16(m, src->data.back_mode);
    }
}

void spice_marshall_msg_display_draw_transparent(SpiceMarshaller *m, SpiceMsgDisplayDrawTransparent *msg, SpiceMarshaller **src_bitmap_out)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgDisplayDrawTransparent *src;
    uint32_t i;
    *src_bitmap_out = NULL;
    src = (SpiceMsgDisplayDrawTransparent *)msg;

    /* base */ {
        spice_marshaller_add_uint32(m, src->base.surface_id);
        /* box */ {
            spice_marshaller_add_int32(m, src->base.box.top);
            spice_marshaller_add_int32(m, src->base.box.left);
            spice_marshaller_add_int32(m, src->base.box.bottom);
            spice_marshaller_add_int32(m, src->base.box.right);
        }
        /* clip */ {
            spice_marshaller_add_uint8(m, src->base.clip.type);
            if (src->base.clip.type == SPICE_CLIP_TYPE_RECTS) {
                SpiceRect *rects__element;
                spice_marshaller_add_uint32(m, src->base.clip.rects->num_rects);
                rects__element = src->base.clip.rects->rects;
                for (i = 0; i < src->base.clip.rects->num_rects; i++) {
                    SpiceRect *src2;
                    src2 = (SpiceRect *)rects__element;

                    spice_marshaller_add_int32(m, src2->top);
                    spice_marshaller_add_int32(m, src2->left);
                    spice_marshaller_add_int32(m, src2->bottom);
                    spice_marshaller_add_int32(m, src2->right);
                    rects__element++;
                }
            }
        }
    }
    /* data */ {
        *src_bitmap_out = spice_marshaller_get_ptr_submarshaller(m, 0);
        /* src_area */ {
            spice_marshaller_add_int32(m, src->data.src_area.top);
            spice_marshaller_add_int32(m, src->data.src_area.left);
            spice_marshaller_add_int32(m, src->data.src_area.bottom);
            spice_marshaller_add_int32(m, src->data.src_area.right);
        }
        spice_marshaller_add_uint32(m, src->data.src_color);
        spice_marshaller_add_uint32(m, src->data.true_color);
    }
}

void spice_marshall_msg_display_draw_alpha_blend(SpiceMarshaller *m, SpiceMsgDisplayDrawAlphaBlend *msg, SpiceMarshaller **src_bitmap_out)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgDisplayDrawAlphaBlend *src;
    uint32_t i;
    *src_bitmap_out = NULL;
    src = (SpiceMsgDisplayDrawAlphaBlend *)msg;

    /* base */ {
        spice_marshaller_add_uint32(m, src->base.surface_id);
        /* box */ {
            spice_marshaller_add_int32(m, src->base.box.top);
            spice_marshaller_add_int32(m, src->base.box.left);
            spice_marshaller_add_int32(m, src->base.box.bottom);
            spice_marshaller_add_int32(m, src->base.box.right);
        }
        /* clip */ {
            spice_marshaller_add_uint8(m, src->base.clip.type);
            if (src->base.clip.type == SPICE_CLIP_TYPE_RECTS) {
                SpiceRect *rects__element;
                spice_marshaller_add_uint32(m, src->base.clip.rects->num_rects);
                rects__element = src->base.clip.rects->rects;
                for (i = 0; i < src->base.clip.rects->num_rects; i++) {
                    SpiceRect *src2;
                    src2 = (SpiceRect *)rects__element;

                    spice_marshaller_add_int32(m, src2->top);
                    spice_marshaller_add_int32(m, src2->left);
                    spice_marshaller_add_int32(m, src2->bottom);
                    spice_marshaller_add_int32(m, src2->right);
                    rects__element++;
                }
            }
        }
    }
    /* data */ {
        spice_marshaller_add_uint8(m, src->data.alpha_flags);
        spice_marshaller_add_uint8(m, src->data.alpha);
        *src_bitmap_out = spice_marshaller_get_ptr_submarshaller(m, 0);
        /* src_area */ {
            spice_marshaller_add_int32(m, src->data.src_area.top);
            spice_marshaller_add_int32(m, src->data.src_area.left);
            spice_marshaller_add_int32(m, src->data.src_area.bottom);
            spice_marshaller_add_int32(m, src->data.src_area.right);
        }
    }
}

void spice_marshall_msg_display_surface_create(SpiceMarshaller *m, SpiceMsgSurfaceCreate *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgSurfaceCreate *src;
    src = (SpiceMsgSurfaceCreate *)msg;

    spice_marshaller_add_uint32(m, src->surface_id);
    spice_marshaller_add_uint32(m, src->width);
    spice_marshaller_add_uint32(m, src->height);
    spice_marshaller_add_uint32(m, src->format);
    spice_marshaller_add_uint32(m, src->flags);
}

void spice_marshall_msg_display_surface_destroy(SpiceMarshaller *m, SpiceMsgSurfaceDestroy *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgSurfaceDestroy *src;
    src = (SpiceMsgSurfaceDestroy *)msg;

    spice_marshaller_add_uint32(m, src->surface_id);
}

void spice_marshall_msg_inputs_init(SpiceMarshaller *m, SpiceMsgInputsInit *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgInputsInit *src;
    src = (SpiceMsgInputsInit *)msg;

    spice_marshaller_add_uint16(m, src->keyboard_modifiers);
}

void spice_marshall_msg_inputs_key_modifiers(SpiceMarshaller *m, SpiceMsgInputsKeyModifiers *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgInputsKeyModifiers *src;
    src = (SpiceMsgInputsKeyModifiers *)msg;

    spice_marshaller_add_uint16(m, src->modifiers);
}

void spice_marshall_msg_cursor_init(SpiceMarshaller *m, SpiceMsgCursorInit *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgCursorInit *src;
    src = (SpiceMsgCursorInit *)msg;

    /* position */ {
        spice_marshaller_add_int16(m, src->position.x);
        spice_marshaller_add_int16(m, src->position.y);
    }
    spice_marshaller_add_uint16(m, src->trail_length);
    spice_marshaller_add_uint16(m, src->trail_frequency);
    spice_marshaller_add_uint8(m, src->visible);
    /* cursor */ {
        spice_marshaller_add_uint16(m, src->cursor.flags);
        if (!(src->cursor.flags & SPICE_CURSOR_FLAGS_NONE)) {
            spice_marshaller_add_uint64(m, src->cursor.header.unique);
            spice_marshaller_add_uint8(m, src->cursor.header.type);
            spice_marshaller_add_uint16(m, src->cursor.header.width);
            spice_marshaller_add_uint16(m, src->cursor.header.height);
            spice_marshaller_add_uint16(m, src->cursor.header.hot_spot_x);
            spice_marshaller_add_uint16(m, src->cursor.header.hot_spot_y);
        }
        /* Remaining data must be appended manually */
    }
}

void spice_marshall_msg_cursor_set(SpiceMarshaller *m, SpiceMsgCursorSet *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgCursorSet *src;
    src = (SpiceMsgCursorSet *)msg;

    /* position */ {
        spice_marshaller_add_int16(m, src->position.x);
        spice_marshaller_add_int16(m, src->position.y);
    }
    spice_marshaller_add_uint8(m, src->visible);
    /* cursor */ {
        spice_marshaller_add_uint16(m, src->cursor.flags);
        if (!(src->cursor.flags & SPICE_CURSOR_FLAGS_NONE)) {
            spice_marshaller_add_uint64(m, src->cursor.header.unique);
            spice_marshaller_add_uint8(m, src->cursor.header.type);
            spice_marshaller_add_uint16(m, src->cursor.header.width);
            spice_marshaller_add_uint16(m, src->cursor.header.height);
            spice_marshaller_add_uint16(m, src->cursor.header.hot_spot_x);
            spice_marshaller_add_uint16(m, src->cursor.header.hot_spot_y);
        }
        /* Remaining data must be appended manually */
    }
}

void spice_marshall_msg_cursor_move(SpiceMarshaller *m, SpiceMsgCursorMove *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgCursorMove *src;
    src = (SpiceMsgCursorMove *)msg;

    /* position */ {
        spice_marshaller_add_int16(m, src->position.x);
        spice_marshaller_add_int16(m, src->position.y);
    }
}

void spice_marshall_msg_cursor_trail(SpiceMarshaller *m, SpiceMsgCursorTrail *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgCursorTrail *src;
    src = (SpiceMsgCursorTrail *)msg;

    spice_marshaller_add_uint16(m, src->length);
    spice_marshaller_add_uint16(m, src->frequency);
}

void spice_marshall_msg_cursor_inval_one(SpiceMarshaller *m, SpiceMsgDisplayInvalOne *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgDisplayInvalOne *src;
    src = (SpiceMsgDisplayInvalOne *)msg;

    spice_marshaller_add_uint64(m, src->id);
}

void spice_marshall_msg_playback_data(SpiceMarshaller *m, SpiceMsgPlaybackPacket *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgPlaybackPacket *src;
    src = (SpiceMsgPlaybackPacket *)msg;

    spice_marshaller_add_uint32(m, src->time);
    /* Remaining data must be appended manually */
}

void spice_marshall_msg_playback_mode(SpiceMarshaller *m, SpiceMsgPlaybackMode *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgPlaybackMode *src;
    src = (SpiceMsgPlaybackMode *)msg;

    spice_marshaller_add_uint32(m, src->time);
    spice_marshaller_add_uint16(m, src->mode);
    /* Remaining data must be appended manually */
}

void spice_marshall_msg_playback_start(SpiceMarshaller *m, SpiceMsgPlaybackStart *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgPlaybackStart *src;
    src = (SpiceMsgPlaybackStart *)msg;

    spice_marshaller_add_uint32(m, src->channels);
    spice_marshaller_add_uint16(m, src->format);
    spice_marshaller_add_uint32(m, src->frequency);
    spice_marshaller_add_uint32(m, src->time);
}

void spice_marshall_msg_record_start(SpiceMarshaller *m, SpiceMsgRecordStart *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgRecordStart *src;
    src = (SpiceMsgRecordStart *)msg;

    spice_marshaller_add_uint32(m, src->channels);
    spice_marshaller_add_uint16(m, src->format);
    spice_marshaller_add_uint32(m, src->frequency);
}

void spice_marshall_msg_tunnel_init(SpiceMarshaller *m, SpiceMsgTunnelInit *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgTunnelInit *src;
    src = (SpiceMsgTunnelInit *)msg;

    spice_marshaller_add_uint16(m, src->max_num_of_sockets);
    spice_marshaller_add_uint32(m, src->max_socket_data_size);
}

void spice_marshall_msg_tunnel_service_ip_map(SpiceMarshaller *m, SpiceMsgTunnelServiceIpMap *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgTunnelServiceIpMap *src;
    uint32_t i;
    src = (SpiceMsgTunnelServiceIpMap *)msg;

    spice_marshaller_add_uint32(m, src->service_id);
    /* virtual_ip */ {
        uint8_t *ipv4__element;
        spice_marshaller_add_uint16(m, src->virtual_ip.type);
        if (src->virtual_ip.type == SPICE_TUNNEL_IP_TYPE_IPv4) {
            ipv4__element = src->virtual_ip.u.ipv4;
            for (i = 0; i < 4; i++) {
                spice_marshaller_add_uint8(m, *ipv4__element);
                ipv4__element++;
            }
        }
    }
}

void spice_marshall_msg_tunnel_socket_open(SpiceMarshaller *m, SpiceMsgTunnelSocketOpen *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgTunnelSocketOpen *src;
    src = (SpiceMsgTunnelSocketOpen *)msg;

    spice_marshaller_add_uint16(m, src->connection_id);
    spice_marshaller_add_uint32(m, src->service_id);
    spice_marshaller_add_uint32(m, src->tokens);
}

void spice_marshall_msg_tunnel_socket_fin(SpiceMarshaller *m, SpiceMsgTunnelSocketFin *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgTunnelSocketFin *src;
    src = (SpiceMsgTunnelSocketFin *)msg;

    spice_marshaller_add_uint16(m, src->connection_id);
}

void spice_marshall_msg_tunnel_socket_close(SpiceMarshaller *m, SpiceMsgTunnelSocketClose *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgTunnelSocketClose *src;
    src = (SpiceMsgTunnelSocketClose *)msg;

    spice_marshaller_add_uint16(m, src->connection_id);
}

void spice_marshall_msg_tunnel_socket_data(SpiceMarshaller *m, SpiceMsgTunnelSocketData *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgTunnelSocketData *src;
    src = (SpiceMsgTunnelSocketData *)msg;

    spice_marshaller_add_uint16(m, src->connection_id);
    /* Remaining data must be appended manually */
}

void spice_marshall_msg_tunnel_socket_closed_ack(SpiceMarshaller *m, SpiceMsgTunnelSocketClosedAck *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgTunnelSocketClosedAck *src;
    src = (SpiceMsgTunnelSocketClosedAck *)msg;

    spice_marshaller_add_uint16(m, src->connection_id);
}

void spice_marshall_Rect(SpiceMarshaller *m, SpiceRect *ptr)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceRect *src;

    src = (SpiceRect *)ptr;

    spice_marshaller_add_int32(m, src->top);
    spice_marshaller_add_int32(m, src->left);
    spice_marshaller_add_int32(m, src->bottom);
    spice_marshaller_add_int32(m, src->right);
}

void spice_marshall_Point(SpiceMarshaller *m, SpicePoint *ptr)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpicePoint *src;

    src = (SpicePoint *)ptr;

    spice_marshaller_add_int32(m, src->x);
    spice_marshaller_add_int32(m, src->y);
}

void spice_marshall_DisplayBase(SpiceMarshaller *m, SpiceMsgDisplayBase *ptr)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgDisplayBase *src;
    uint32_t i;

    src = (SpiceMsgDisplayBase *)ptr;

    spice_marshaller_add_uint32(m, src->surface_id);
    /* box */ {
        spice_marshaller_add_int32(m, src->box.top);
        spice_marshaller_add_int32(m, src->box.left);
        spice_marshaller_add_int32(m, src->box.bottom);
        spice_marshaller_add_int32(m, src->box.right);
    }
    /* clip */ {
        spice_marshaller_add_uint8(m, src->clip.type);
        if (src->clip.type == SPICE_CLIP_TYPE_RECTS) {
            SpiceRect *rects__element;
            spice_marshaller_add_uint32(m, src->clip.rects->num_rects);
            rects__element = src->clip.rects->rects;
            for (i = 0; i < src->clip.rects->num_rects; i++) {
                SpiceRect *src2;
                src2 = (SpiceRect *)rects__element;

                spice_marshaller_add_int32(m, src2->top);
                spice_marshaller_add_int32(m, src2->left);
                spice_marshaller_add_int32(m, src2->bottom);
                spice_marshaller_add_int32(m, src2->right);
                rects__element++;
            }
        }
    }
}

void spice_marshall_Fill(SpiceMarshaller *m, SpiceFill *ptr, SpiceMarshaller **brush_pat_out, SpiceMarshaller **mask_bitmap_out)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceFill *src;
    *brush_pat_out = NULL;
    *mask_bitmap_out = NULL;

    src = (SpiceFill *)ptr;

    /* brush */ {
        spice_marshaller_add_uint8(m, src->brush.type);
        if (src->brush.type == SPICE_BRUSH_TYPE_SOLID) {
            spice_marshaller_add_uint32(m, src->brush.u.color);
        } else if (src->brush.type == SPICE_BRUSH_TYPE_PATTERN) {
            *brush_pat_out = spice_marshaller_get_ptr_submarshaller(m, 0);
            /* pos */ {
                spice_marshaller_add_int32(m, src->brush.u.pattern.pos.x);
                spice_marshaller_add_int32(m, src->brush.u.pattern.pos.y);
            }
        }
    }
    spice_marshaller_add_uint16(m, src->rop_descriptor);
    /* mask */ {
        spice_marshaller_add_uint8(m, src->mask.flags);
        /* pos */ {
            spice_marshaller_add_int32(m, src->mask.pos.x);
            spice_marshaller_add_int32(m, src->mask.pos.y);
        }
        *mask_bitmap_out = spice_marshaller_get_ptr_submarshaller(m, 0);
    }
}

void spice_marshall_Opaque(SpiceMarshaller *m, SpiceOpaque *ptr, SpiceMarshaller **src_bitmap_out, SpiceMarshaller **pat_out, SpiceMarshaller **mask_bitmap_out)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceOpaque *src;
    *src_bitmap_out = NULL;
    *pat_out = NULL;
    *mask_bitmap_out = NULL;

    src = (SpiceOpaque *)ptr;

    *src_bitmap_out = spice_marshaller_get_ptr_submarshaller(m, 0);
    /* src_area */ {
        spice_marshaller_add_int32(m, src->src_area.top);
        spice_marshaller_add_int32(m, src->src_area.left);
        spice_marshaller_add_int32(m, src->src_area.bottom);
        spice_marshaller_add_int32(m, src->src_area.right);
    }
    /* brush */ {
        spice_marshaller_add_uint8(m, src->brush.type);
        if (src->brush.type == SPICE_BRUSH_TYPE_SOLID) {
            spice_marshaller_add_uint32(m, src->brush.u.color);
        } else if (src->brush.type == SPICE_BRUSH_TYPE_PATTERN) {
            *pat_out = spice_marshaller_get_ptr_submarshaller(m, 0);
            /* pos */ {
                spice_marshaller_add_int32(m, src->brush.u.pattern.pos.x);
                spice_marshaller_add_int32(m, src->brush.u.pattern.pos.y);
            }
        }
    }
    spice_marshaller_add_uint16(m, src->rop_descriptor);
    spice_marshaller_add_uint8(m, src->scale_mode);
    /* mask */ {
        spice_marshaller_add_uint8(m, src->mask.flags);
        /* pos */ {
            spice_marshaller_add_int32(m, src->mask.pos.x);
            spice_marshaller_add_int32(m, src->mask.pos.y);
        }
        *mask_bitmap_out = spice_marshaller_get_ptr_submarshaller(m, 0);
    }
}

void spice_marshall_Copy(SpiceMarshaller *m, SpiceCopy *ptr, SpiceMarshaller **src_bitmap_out, SpiceMarshaller **mask_bitmap_out)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceCopy *src;
    *src_bitmap_out = NULL;
    *mask_bitmap_out = NULL;

    src = (SpiceCopy *)ptr;

    *src_bitmap_out = spice_marshaller_get_ptr_submarshaller(m, 0);
    /* src_area */ {
        spice_marshaller_add_int32(m, src->src_area.top);
        spice_marshaller_add_int32(m, src->src_area.left);
        spice_marshaller_add_int32(m, src->src_area.bottom);
        spice_marshaller_add_int32(m, src->src_area.right);
    }
    spice_marshaller_add_uint16(m, src->rop_descriptor);
    spice_marshaller_add_uint8(m, src->scale_mode);
    /* mask */ {
        spice_marshaller_add_uint8(m, src->mask.flags);
        /* pos */ {
            spice_marshaller_add_int32(m, src->mask.pos.x);
            spice_marshaller_add_int32(m, src->mask.pos.y);
        }
        *mask_bitmap_out = spice_marshaller_get_ptr_submarshaller(m, 0);
    }
}

void spice_marshall_Blend(SpiceMarshaller *m, SpiceCopy *ptr, SpiceMarshaller **src_bitmap_out, SpiceMarshaller **mask_bitmap_out)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceCopy *src;
    *src_bitmap_out = NULL;
    *mask_bitmap_out = NULL;

    src = (SpiceCopy *)ptr;

    *src_bitmap_out = spice_marshaller_get_ptr_submarshaller(m, 0);
    /* src_area */ {
        spice_marshaller_add_int32(m, src->src_area.top);
        spice_marshaller_add_int32(m, src->src_area.left);
        spice_marshaller_add_int32(m, src->src_area.bottom);
        spice_marshaller_add_int32(m, src->src_area.right);
    }
    spice_marshaller_add_uint16(m, src->rop_descriptor);
    spice_marshaller_add_uint8(m, src->scale_mode);
    /* mask */ {
        spice_marshaller_add_uint8(m, src->mask.flags);
        /* pos */ {
            spice_marshaller_add_int32(m, src->mask.pos.x);
            spice_marshaller_add_int32(m, src->mask.pos.y);
        }
        *mask_bitmap_out = spice_marshaller_get_ptr_submarshaller(m, 0);
    }
}

void spice_marshall_Blackness(SpiceMarshaller *m, SpiceBlackness *ptr, SpiceMarshaller **mask_bitmap_out)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceBlackness *src;
    *mask_bitmap_out = NULL;

    src = (SpiceBlackness *)ptr;

    /* mask */ {
        spice_marshaller_add_uint8(m, src->mask.flags);
        /* pos */ {
            spice_marshaller_add_int32(m, src->mask.pos.x);
            spice_marshaller_add_int32(m, src->mask.pos.y);
        }
        *mask_bitmap_out = spice_marshaller_get_ptr_submarshaller(m, 0);
    }
}

void spice_marshall_Whiteness(SpiceMarshaller *m, SpiceWhiteness *ptr, SpiceMarshaller **mask_bitmap_out)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceWhiteness *src;
    *mask_bitmap_out = NULL;

    src = (SpiceWhiteness *)ptr;

    /* mask */ {
        spice_marshaller_add_uint8(m, src->mask.flags);
        /* pos */ {
            spice_marshaller_add_int32(m, src->mask.pos.x);
            spice_marshaller_add_int32(m, src->mask.pos.y);
        }
        *mask_bitmap_out = spice_marshaller_get_ptr_submarshaller(m, 0);
    }
}

void spice_marshall_Invers(SpiceMarshaller *m, SpiceInvers *ptr, SpiceMarshaller **mask_bitmap_out)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceInvers *src;
    *mask_bitmap_out = NULL;

    src = (SpiceInvers *)ptr;

    /* mask */ {
        spice_marshaller_add_uint8(m, src->mask.flags);
        /* pos */ {
            spice_marshaller_add_int32(m, src->mask.pos.x);
            spice_marshaller_add_int32(m, src->mask.pos.y);
        }
        *mask_bitmap_out = spice_marshaller_get_ptr_submarshaller(m, 0);
    }
}

void spice_marshall_Rop3(SpiceMarshaller *m, SpiceRop3 *ptr, SpiceMarshaller **src_bitmap_out, SpiceMarshaller **pat_out, SpiceMarshaller **mask_bitmap_out)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceRop3 *src;
    *src_bitmap_out = NULL;
    *pat_out = NULL;
    *mask_bitmap_out = NULL;

    src = (SpiceRop3 *)ptr;

    *src_bitmap_out = spice_marshaller_get_ptr_submarshaller(m, 0);
    /* src_area */ {
        spice_marshaller_add_int32(m, src->src_area.top);
        spice_marshaller_add_int32(m, src->src_area.left);
        spice_marshaller_add_int32(m, src->src_area.bottom);
        spice_marshaller_add_int32(m, src->src_area.right);
    }
    /* brush */ {
        spice_marshaller_add_uint8(m, src->brush.type);
        if (src->brush.type == SPICE_BRUSH_TYPE_SOLID) {
            spice_marshaller_add_uint32(m, src->brush.u.color);
        } else if (src->brush.type == SPICE_BRUSH_TYPE_PATTERN) {
            *pat_out = spice_marshaller_get_ptr_submarshaller(m, 0);
            /* pos */ {
                spice_marshaller_add_int32(m, src->brush.u.pattern.pos.x);
                spice_marshaller_add_int32(m, src->brush.u.pattern.pos.y);
            }
        }
    }
    spice_marshaller_add_uint8(m, src->rop3);
    spice_marshaller_add_uint8(m, src->scale_mode);
    /* mask */ {
        spice_marshaller_add_uint8(m, src->mask.flags);
        /* pos */ {
            spice_marshaller_add_int32(m, src->mask.pos.x);
            spice_marshaller_add_int32(m, src->mask.pos.y);
        }
        *mask_bitmap_out = spice_marshaller_get_ptr_submarshaller(m, 0);
    }
}

void spice_marshall_Stroke(SpiceMarshaller *m, SpiceStroke *ptr, SpiceMarshaller **style_out, SpiceMarshaller **pat_out)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceStroke *src;
    *style_out = NULL;
    *pat_out = NULL;

    src = (SpiceStroke *)ptr;

    m2 = spice_marshaller_get_ptr_submarshaller(m, 0);
    spice_marshall_Path(m2, src->path);
    /* attr */ {
        spice_marshaller_add_uint8(m, src->attr.flags);
        if ((src->attr.flags & SPICE_LINE_FLAGS_STYLED)) {
            spice_marshaller_add_uint8(m, src->attr.style_nseg);
        }
        if ((src->attr.flags & SPICE_LINE_FLAGS_STYLED)) {
            *style_out = spice_marshaller_get_ptr_submarshaller(m, 0);
        }
    }
    /* brush */ {
        spice_marshaller_add_uint8(m, src->brush.type);
        if (src->brush.type == SPICE_BRUSH_TYPE_SOLID) {
            spice_marshaller_add_uint32(m, src->brush.u.color);
        } else if (src->brush.type == SPICE_BRUSH_TYPE_PATTERN) {
            *pat_out = spice_marshaller_get_ptr_submarshaller(m, 0);
            /* pos */ {
                spice_marshaller_add_int32(m, src->brush.u.pattern.pos.x);
                spice_marshaller_add_int32(m, src->brush.u.pattern.pos.y);
            }
        }
    }
    spice_marshaller_add_uint16(m, src->fore_mode);
    spice_marshaller_add_uint16(m, src->back_mode);
}

void spice_marshall_Text(SpiceMarshaller *m, SpiceText *ptr, SpiceMarshaller **fore_brush_pat_out, SpiceMarshaller **back_brush_pat_out)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceText *src;
    *fore_brush_pat_out = NULL;
    *back_brush_pat_out = NULL;

    src = (SpiceText *)ptr;

    m2 = spice_marshaller_get_ptr_submarshaller(m, 0);
    spice_marshall_String(m2, src->str);
    /* back_area */ {
        spice_marshaller_add_int32(m, src->back_area.top);
        spice_marshaller_add_int32(m, src->back_area.left);
        spice_marshaller_add_int32(m, src->back_area.bottom);
        spice_marshaller_add_int32(m, src->back_area.right);
    }
    /* fore_brush */ {
        spice_marshaller_add_uint8(m, src->fore_brush.type);
        if (src->fore_brush.type == SPICE_BRUSH_TYPE_SOLID) {
            spice_marshaller_add_uint32(m, src->fore_brush.u.color);
        } else if (src->fore_brush.type == SPICE_BRUSH_TYPE_PATTERN) {
            *fore_brush_pat_out = spice_marshaller_get_ptr_submarshaller(m, 0);
            /* pos */ {
                spice_marshaller_add_int32(m, src->fore_brush.u.pattern.pos.x);
                spice_marshaller_add_int32(m, src->fore_brush.u.pattern.pos.y);
            }
        }
    }
    /* back_brush */ {
        spice_marshaller_add_uint8(m, src->back_brush.type);
        if (src->back_brush.type == SPICE_BRUSH_TYPE_SOLID) {
            spice_marshaller_add_uint32(m, src->back_brush.u.color);
        } else if (src->back_brush.type == SPICE_BRUSH_TYPE_PATTERN) {
            *back_brush_pat_out = spice_marshaller_get_ptr_submarshaller(m, 0);
            /* pos */ {
                spice_marshaller_add_int32(m, src->back_brush.u.pattern.pos.x);
                spice_marshaller_add_int32(m, src->back_brush.u.pattern.pos.y);
            }
        }
    }
    spice_marshaller_add_uint16(m, src->fore_mode);
    spice_marshaller_add_uint16(m, src->back_mode);
}

void spice_marshall_Transparent(SpiceMarshaller *m, SpiceTransparent *ptr, SpiceMarshaller **src_bitmap_out)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceTransparent *src;
    *src_bitmap_out = NULL;

    src = (SpiceTransparent *)ptr;

    *src_bitmap_out = spice_marshaller_get_ptr_submarshaller(m, 0);
    /* src_area */ {
        spice_marshaller_add_int32(m, src->src_area.top);
        spice_marshaller_add_int32(m, src->src_area.left);
        spice_marshaller_add_int32(m, src->src_area.bottom);
        spice_marshaller_add_int32(m, src->src_area.right);
    }
    spice_marshaller_add_uint32(m, src->src_color);
    spice_marshaller_add_uint32(m, src->true_color);
}

void spice_marshall_AlphaBlend(SpiceMarshaller *m, SpiceAlphaBlend *ptr, SpiceMarshaller **src_bitmap_out)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceAlphaBlend *src;
    *src_bitmap_out = NULL;

    src = (SpiceAlphaBlend *)ptr;

    spice_marshaller_add_uint8(m, src->alpha_flags);
    spice_marshaller_add_uint8(m, src->alpha);
    *src_bitmap_out = spice_marshaller_get_ptr_submarshaller(m, 0);
    /* src_area */ {
        spice_marshaller_add_int32(m, src->src_area.top);
        spice_marshaller_add_int32(m, src->src_area.left);
        spice_marshaller_add_int32(m, src->src_area.bottom);
        spice_marshaller_add_int32(m, src->src_area.right);
    }
}

void spice_marshall_msg_tunnel_socket_token(SpiceMarshaller *m, SpiceMsgTunnelSocketTokens *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgTunnelSocketTokens *src;
    src = (SpiceMsgTunnelSocketTokens *)msg;

    spice_marshaller_add_uint16(m, src->connection_id);
    spice_marshaller_add_uint32(m, src->num_tokens);
}

