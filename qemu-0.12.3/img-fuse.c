/*
 * This file implements the fuse driver for all the images type supported by
 * Qemu. This is just fuse wrapper over Qemu. So that filesystem inside the
 * image can be mounted and image inspection can be done. Makefile of the
 * Qemu is changed to build the important binaries like qemu-img. 
 */

#define _FILE_OFFSET_BITS 64
#define FUSE_USE_VERSION  26
#define IO_BUF_SIZE (64 * 1024)
#define SECTOR_SIZE 512
//#define _GNU_SOURCE
/* Un/comment the following two defines to (dis/en)able debugging messages */
//#define __D_BUG 
#define __FD_BUG  

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/*
 * Qemu related header files.
 */
#include "qemu-common.h"
#include "block_int.h"
#include <assert.h>

/*
 * Its own header file.
 */
#include "img-fuse.h"

struct disk_img_globals {
    u_int64_t       logcount;
    FILE            *flog;
    int             img_fd;
    struct          stat raw_stat;
    char            *img_filename;
    char            *raw_filename;
    char            *img_dir;
    char            *img_format;
    BlockDriverState *img_bs;
} disk_img_data;

static int openlogfile(void)
{
    if((disk_img_data.flog = fopen("/tmp/disk_img_log.log","w" )))
    {
        disk_img_data.logcount = 0;
        return ERR_NON;
    }
    else
    {
        DBG(("Error opening log file"));
        return ERR_FILE;
    }
}

static BlockDriverState *bdrv_new_open(const char *filename,
                                       const char *fmt)
{
    BlockDriverState *bs;
    BlockDriver *drv;

    bs = bdrv_new("");
    if (!bs)
        FDBG("Not enough memory");
    if (fmt) {
        drv = bdrv_find_format(fmt);
        if (!drv)
            FDBG("Unknown file format '%s'", fmt);
    } else {
        drv = NULL;
    }
    if (bdrv_open2(bs, filename, 0, drv) < 0) {
        FDBG("Could not open '%s'", filename);
    }
    return bs;
}

static int disk_img_info(const char *filename)
{
    BlockDriver *drv;
    BlockDriverState *bs;

    bs = bdrv_new("");
    if (!bs)
        FDBG("Not enough memory");
    drv = NULL;
    if (bdrv_open2(bs, filename, 0, drv) < 0) {
        FDBG("Could not open '%s'", filename);
    }
    bdrv_get_format(bs, disk_img_data.img_format, (sizeof(char) * 128));
    bdrv_delete(bs);
    return 0;
}

static int set_disk_img_dir(void)
{
    char * base_filename = NULL, *relative_path=NULL;
    int path_len;
    DBG(("Enter"));
    if(disk_img_data.img_filename) {
        base_filename= rindex( disk_img_data.img_filename , '/');
        if (base_filename) {
            base_filename++;
            path_len = (strlen(disk_img_data.img_filename) - 
                    strlen(base_filename));
            relative_path = (char *)malloc( path_len + 1 );
            strncpy(relative_path, disk_img_data.img_filename, path_len);
            relative_path[path_len] = '\0';
        }
    }
    disk_img_data.img_dir = relative_path;
    return ERR_NON;
}

static int mount_disk_img(char *filename)
{
    char        *base_filename;
    int         err = ERR_NON;
    uint64_t    size = 0;

    DBG(("Enter"));
    
    disk_img_data.img_filename = filename;
    disk_img_data.img_format = calloc(128, sizeof(char));
    disk_img_info(filename);
    disk_img_data.img_bs = bdrv_new_open(filename, disk_img_data.img_format);
    DBG(( "Format : %s \n", disk_img_data.img_format));
    set_disk_img_dir();
    disk_img_data.img_fd = open(filename, O_RDWR);
    
    if (disk_img_data.img_fd == -1) {
        perror(filename);
        return ERR_FILE;
    }
    
    err = fstat(disk_img_data.img_fd, &disk_img_data.raw_stat);
    
    if( err !=  ERR_NON) {
        DBG(("fstat error"));
        goto out;
    }

    bdrv_get_geometry(disk_img_data.img_bs, &size);
    disk_img_data.raw_stat.st_blocks = size;
    disk_img_data.raw_stat.st_size = (size * SECTOR_SIZE);
    
    base_filename= rindex(disk_img_data.img_filename, '/');
    
    if(!base_filename)
        base_filename = disk_img_data.img_filename;
    else 
        base_filename += 1;
    
    disk_img_data.raw_filename = malloc( sizeof(*disk_img_data.img_filename ) * 
                                     strlen( base_filename  ) + 6 );
    sprintf( disk_img_data.raw_filename, "/%s.img" , base_filename );
    DBG( ( "raw file name : %s \n", disk_img_data.raw_filename));
 out:
    DBG(("Exitcode :%d",err));
    return err;            
}

static int disk_img_getattr(const char *path, 
                            struct stat *stbuf)
{
    int err = 0;

    memset(stbuf, 0, sizeof(struct stat));
    if(strcasecmp(path, "/") == 0) 
    {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    }
    else if(strcasecmp(path, disk_img_data.raw_filename) == 0) 
    {
        stbuf->st_mode = S_IFREG | 0666;
        stbuf->st_nlink = 1;

        stbuf->st_size   = disk_img_data.raw_stat.st_size;
        stbuf->st_mtime  = disk_img_data.raw_stat.st_mtime;
        stbuf->st_atime  = disk_img_data.raw_stat.st_atime;
        stbuf->st_ctime  = disk_img_data.raw_stat.st_ctime;
        stbuf->st_blocks = disk_img_data.raw_stat.st_blocks;

        /* stbuf->st_uid = getuid(); */
        /* stbuf->st_gid = getgid(); */
        stbuf->st_uid = fuse_get_context()->uid;
        stbuf->st_gid = fuse_get_context()->gid;
    }
    else
        err = ENOENT;
    FDBG("  ");
    return err;
}

static int disk_img_readdir(const char      *path, 
                            void            *buf, 
                            fuse_fill_dir_t filler,
                            off_t           offset, 
                            struct fuse_file_info *fi)
{
    (void) offset;
    (void) fi;
    
    if(strcasecmp(path, "/") != 0)
        return -ENOENT;
    
    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    filler(buf, disk_img_data.raw_filename + 1, NULL, 0);
    XDBG(" ls on dir ");
    return 0;
}

static int disk_img_open(const char             *path, 
                         struct fuse_file_info  *fi)
{
    if(strcasecmp(path, disk_img_data.raw_filename) != 0)
        return -ENOENT;

    /*if((fi->flags & 3) != O_RDWR)
    {
        XDBG("FS open unsuccessful");
        return -EACCES;
    }
    */
    XDBG("FS open successful");
    return 0;
}

static int disk_img_read(const char *path, 
                         char       *buf, 
                         size_t     size,
                         off_t      offset,
                         struct fuse_file_info *fi)
{
    int     err;
    int     nb_sectors = size >> SHIFT_SECTOR;
    int64_t start_sec = offset >> SHIFT_SECTOR;

    FDBG("offset: %lu --> path %s", (unsigned long)offset, path); 
    if(strcasecmp(path, disk_img_data.raw_filename) != 0)
        return -ENOENT;

    FDBG("offset: %lu --> path %s", (unsigned long)offset, path); 
    FDBG("offset: %lu --> buffer %s", (unsigned long)offset, buf); 
    FDBG("offset: %lu --> size: %lu sec start %lu fh %d", (unsigned long)offset,
            (unsigned long)size, (signed long)start_sec, (int)fi->fh);
    err = bdrv_read(disk_img_data.img_bs, start_sec, (uint8_t*) buf,
            nb_sectors);
    FDBG("DATA : %s", buf);
    FDBG("Exit code : %d", err);

    return size;
}

static int disk_img_write(const char *path, 
                         char       *buf, 
                         size_t     size,
                         off_t      offset,
                         struct fuse_file_info *fi)
{
    int err;
    int nb_sectors = size >> SHIFT_SECTOR;
    int start_sec = offset >> SHIFT_SECTOR;

    FDBG("path %s" , path ); 
    if(strcasecmp(path, disk_img_data.raw_filename) != 0)
        return -ENOENT;

    FDBG("path %s" , path ); 
    FDBG("buffer %s" , buf ); 
    FDBG("size: %d offset %lld fh %lld" ,  size , offset , fi->fh ); 
    err = bdrv_write(disk_img_data.img_bs, start_sec, (uint8_t*) buf,
            nb_sectors);
    //FDBG("DATA : %s", buf);
    FDBG("Exit code : %d", err);

    return size;
}

static struct fuse_operations disk_img_oper = {
    .getattr	= disk_img_getattr,
    .readdir	= disk_img_readdir,
    .open	    = disk_img_open,
    .read	    = disk_img_read,
    .write	    = disk_img_write,
};

static void init(void)
{
    bdrv_init();
}

static void usage(char **argv)
{
    DBG((" "));
    printf("Usage: %s <vmdk_image_file> <mount_point> \n", 
           argv[0]);
}

static int call_fuse_main(int argc, char *argv[])
{
    int i;
    char **nargv = (char **) malloc(argc * sizeof(char *));
    int nargc = argc - 1;
  
    nargv[0] = argv[0];
    for (i = 0; i < nargc; i++) {
        nargv[i + 1] = argv[i + 2];
    };
    
    return fuse_main(nargc, nargv, &disk_img_oper, NULL);
}

int main(int argc, char *argv[])
{
    int err;
    if(argc < 3) {
        usage( argv );
        exit(EINVAL);
    };
    
    err = openlogfile();
    if(err != ERR_NON)
        goto out;

    DBG (("firs argument %s %d", argv[1], argc));
    DBG (("sec argument %s ", argv[2]));
    FDBG ("fir argument %s ", argv[1]);
    FDBG ("sec argument %s ", argv[2]);
    init();
    if( mount_disk_img( argv[1])!= ERR_NON)
        goto out;

    return call_fuse_main(argc, argv);

out:
    return ERR_GEN;
}
