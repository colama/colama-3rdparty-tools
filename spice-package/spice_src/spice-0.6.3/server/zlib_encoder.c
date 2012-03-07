#include "red_common.h"
#include "zlib_encoder.h"
#include <zlib.h>

struct ZlibEncoder {
    ZlibEncoderUsrContext *usr;

    z_stream strm;
    int last_level;
};

ZlibEncoder* zlib_encoder_create(ZlibEncoderUsrContext *usr, int level)
{
    ZlibEncoder *enc;
    int z_ret;

    if (!usr->more_space || !usr->more_input) {
        return NULL;
    }

    enc = spice_new0(ZlibEncoder, 1);

    enc->usr = usr;

    enc->strm.zalloc = Z_NULL;
    enc->strm.zfree = Z_NULL;
    enc->strm.opaque = Z_NULL;

    z_ret = deflateInit(&enc->strm, level);
    enc->last_level = level;
    if (z_ret != Z_OK) {
        red_printf("zlib error");
        free(enc);
        return NULL;
    }

    return enc;
}

void zlib_encoder_destroy(ZlibEncoder *encoder)
{
    deflateEnd(&encoder->strm);
    free(encoder);
}

/* returns the total size of the encoded data */
int zlib_encode(ZlibEncoder *zlib, int level, int input_size,
                uint8_t *io_ptr, unsigned int num_io_bytes)
{
    int flush;
    int enc_size = 0;
    int out_size = 0;
    int z_ret;

    z_ret = deflateReset(&zlib->strm);

    if (z_ret != Z_OK) {
        red_error("deflateReset failed");
    }

    zlib->strm.next_out = io_ptr;
    zlib->strm.avail_out = num_io_bytes;

    if (level != zlib->last_level) {
        if (zlib->strm.avail_out == 0) {
            zlib->strm.avail_out = zlib->usr->more_space(zlib->usr, &zlib->strm.next_out);
            if (zlib->strm.avail_out == 0) {
                red_error("not enough space");
            }
        }
        z_ret = deflateParams(&zlib->strm, level, Z_DEFAULT_STRATEGY);
        if (z_ret != Z_OK) {
            red_error("deflateParams failed");
        }
        zlib->last_level = level;
    }


    do {
        zlib->strm.avail_in = zlib->usr->more_input(zlib->usr, &zlib->strm.next_in);
        if (zlib->strm.avail_in <= 0) {
            red_error("more input failed\n");
        }
        enc_size += zlib->strm.avail_in;
        flush = (enc_size == input_size) ?  Z_FINISH : Z_NO_FLUSH;
        while (1) {
            int deflate_size = zlib->strm.avail_out;
            z_ret = deflate(&zlib->strm, flush);
            ASSERT(z_ret != Z_STREAM_ERROR);
            out_size += deflate_size - zlib->strm.avail_out;
            if (zlib->strm.avail_out) {
                break;
            }

            zlib->strm.avail_out = zlib->usr->more_space(zlib->usr, &zlib->strm.next_out);
            if (zlib->strm.avail_out == 0) {
                red_error("not enough space");
            }
        }
    } while (flush != Z_FINISH);

    ASSERT(z_ret == Z_STREAM_END);
    return out_size;
}
