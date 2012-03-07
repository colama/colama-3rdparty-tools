#ifndef __IMG_FUSE_H__
#define __IMG_FUSE_H__

#include <sys/types.h>
#include <linux/limits.h>

#define ERR_NON 0
#define ERR_GEN -1
#define ERR_NOMEM -2
#define ERR_FILE -3
#define ERR_PARSE -4
#define SHIFT_SECTOR 9

#ifdef __D_BUG

#define DBG(_x) \
    ( printf("DBG: %s %d %s(): ", __FILE__, __LINE__, __FUNCTION__),printf _x ,printf("\n") )
#else

#define DBG(_x)    ( (void)0)

#endif /* __D_BUG */

#ifdef __FD_BUG


#define  FDBG( format, args...)                 \
    fprintf(disk_img_data.flog,"\n%6lu DBG: %s %d %s(): ", (unsigned long)disk_img_data.logcount++, __FILE__, __LINE__, __FUNCTION__), \
    fprintf (disk_img_data.flog, format, ## args), \
    fflush(disk_img_data.flog)
#else
#define FDBG(_x,args...)    ( (void)0)
#endif /* __FD_BUG */

#define XDBG(_x,args...)    ( (void)0)

#endif/* __IMG_FUSE_H__*/
