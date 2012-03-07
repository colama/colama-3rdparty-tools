#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "qemu-common.h"
#include "block_int.h"
#include <assert.h>

static void __attribute__((noreturn)) error(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "qemu-img: ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
    va_end(ap);
}

static void help(void)
{
    fprintf(stdout, "Use img_info <image_filename>\n");
    exit(0);
}

static int64_t get_allocated_file_size(const char *filename)
{
    struct stat st;
    if (stat(filename, &st) < 0)
        return -1;
    return (int64_t)st.st_blocks * 512;
}

static void dump_snapshots(BlockDriverState *bs)
{
    QEMUSnapshotInfo *sn_tab, *sn;
    int nb_sns, i;
    char buf[256];

    nb_sns = bdrv_snapshot_list(bs, &sn_tab);
    if (nb_sns <= 0)
        return;
    printf("Snapshot list:\n");
    printf("%s\n", bdrv_snapshot_dump(buf, sizeof(buf), NULL));
    for(i = 0; i < nb_sns; i++) {
        sn = &sn_tab[i];
        printf("%s\n", bdrv_snapshot_dump(buf, sizeof(buf), sn));
    }
    qemu_free(sn_tab);
}

int 
main(int argc, char *argv[])
{
    int c;
    const char *filename, *fmt;
    BlockDriver *drv;
    BlockDriverState *bs;
    char fmt_name[128], size_buf[128], dsize_buf[128];
    uint64_t total_sectors;
    int64_t allocated_size;
    char backing_filename[1024];
    char backing_filename2[1024];
    BlockDriverInfo bdi;

    bdrv_init();

    fmt = NULL;
    for(;;) {
        c = getopt(argc, argv, "f:h");
        if (c == -1)
            break;
        switch(c) {
        case 'h':
           // help();
            break;
        case 'f':
            fmt = optarg;
            break;
        }
    }
    if (optind >= argc)
        help();
    filename = argv[optind++];

    bs = bdrv_new("");
    if (!bs)
        error("Not enough memory");
    if (fmt) {
        drv = bdrv_find_format(fmt);
        if (!drv)
            error("Unknown file format '%s'", fmt);
    } else {
        drv = NULL;
    }
    if (bdrv_open2(bs, filename, 0, drv) < 0) {
        error("Could not open '%s'", filename);
    }
    bdrv_get_format(bs, fmt_name, sizeof(fmt_name));
    bdrv_get_geometry(bs, &total_sectors);
    get_human_readable_size(size_buf, sizeof(size_buf), total_sectors * 512);
    allocated_size = get_allocated_file_size(filename);
    if (allocated_size < 0)
        sprintf(dsize_buf, "unavailable");
    else
        get_human_readable_size(dsize_buf, sizeof(dsize_buf),
                                allocated_size);
    /*
    if (bdrv_is_encrypted(bs))
        fprintf(stderr, "encrypted: yes\n");
    if (bdrv_get_info(bs, &bdi) >= 0) {
        if (bdi.cluster_size != 0)
            fprintf(stderr, "cluster_size: %d\n", bdi.cluster_size);
    }
    */
    bdrv_get_info(bs, &bdi);
    bdrv_get_backing_filename(bs, backing_filename, sizeof(backing_filename));
    if (backing_filename[0] != '\0') {
        path_combine(backing_filename2, sizeof(backing_filename2),
                     filename, backing_filename);
        /*
        fprintf(stderr, "backing file: %s (actual path: %s)\n",
               backing_filename,
               backing_filename2);
        */
    }
    fprintf(stdout, "{'filename' : '%s',"
            " 'format' : '%s',"
            " 'image_disk_size' : '%s',"
            " 'allocated_size' : '%s',"
            " 'total_sectors' : '%"PRId64"',"
            " 'backing_file' : '%s',}",
           filename, fmt_name, size_buf, dsize_buf, total_sectors, 
           backing_filename);
    dump_snapshots(bs);
    bdrv_delete(bs);
    return 0;
}
