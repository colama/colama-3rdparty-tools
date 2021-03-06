/*

 Spice Virtual Machine Channel (VMC).

 A virtio-serial port used for spice to guest communication, over
 which spice client and a daemon in the guest operating system
 communicate.

 Replaces the old vdi_port PCI device.

*/

#include <stdio.h>
#include <stdbool.h>
#include <spice.h>
#include <spice-experimental.h>

#include "virtio-serial.h"
#include "ui/qemu-spice.h"

#define VMC_GUEST_DEVICE_NAME "com.redhat.spice.0"
#define VMC_DEVICE_NAME       "spicevmc"

/* windows guest driver bug workaround */
#define VMC_MAX_HOST_WRITE    2048

#define dprintf(_svc, _level, _fmt, ...)                                \
    do {                                                                \
        static unsigned __dprintf_counter = 0;                          \
        if (_svc->debug >= _level) {                                    \
            fprintf(stderr, "svc: %3d: " _fmt, ++__dprintf_counter, ## __VA_ARGS__);\
        }                                                               \
    } while (0)

typedef struct SpiceVirtualChannel {
    VirtIOSerialPort         port;
    VMChangeStateEntry       *vmstate;
    SpiceCharDeviceInstance  sin;
    char                     *subtype;
    bool                     active;
    uint8_t                  *buffer;
    uint8_t                  *datapos;
    ssize_t                  bufsize, datalen;
    uint32_t                 debug;
} SpiceVirtualChannel;

static int vmc_write(SpiceCharDeviceInstance *sin, const uint8_t *buf, int len)
{
    SpiceVirtualChannel *svc = container_of(sin, SpiceVirtualChannel, sin);
    ssize_t out = 0;
    ssize_t last_out;
    uint8_t* p = (uint8_t*)buf;

    while (len > 0) {
        last_out = virtio_serial_write(&svc->port, p,
                            MIN(len, VMC_MAX_HOST_WRITE));
        if (last_out > 0) {
            out += last_out;
            len -= last_out;
            p += last_out;
        } else {
            break;
        }
    }

    dprintf(svc, 3, "%s: %lu/%zd\n", __func__, out, len + out);
    return out;
}

static int vmc_read(SpiceCharDeviceInstance *sin, uint8_t *buf, int len)
{
    SpiceVirtualChannel *svc = container_of(sin, SpiceVirtualChannel, sin);
    int bytes = MIN(len, svc->datalen);

    dprintf(svc, 2, "%s: %p %d/%d/%zd\n", __func__, svc->datapos, len, bytes, svc->datalen);
    if (bytes > 0) {
        memcpy(buf, svc->datapos, bytes);
        svc->datapos += bytes;
        svc->datalen -= bytes;
        assert(svc->datalen >= 0);
        if (svc->datalen == 0) {
            svc->datapos = 0;
            virtio_serial_throttle_port(&svc->port, false);
            // ^^^ !!! may call vmc_have_data, so don't touch svc after it!
        }
    }
    return bytes;
}

static SpiceCharDeviceInterface vmc_interface = {
    .base.type          = SPICE_INTERFACE_CHAR_DEVICE,
    .base.description   = "spice virtual channel char device",
    .base.major_version = SPICE_INTERFACE_CHAR_DEVICE_MAJOR,
    .base.minor_version = SPICE_INTERFACE_CHAR_DEVICE_MINOR,
    .write              = vmc_write,
    .read               = vmc_read,
};

static void vmc_register_interface(SpiceVirtualChannel *svc)
{
    if (svc->active) {
        return;
    }
    dprintf(svc, 1, "%s\n", __func__);
    svc->sin.base.sif = &vmc_interface.base;
    qemu_spice_add_interface(&svc->sin.base);
    svc->active = true;
}

static void vmc_unregister_interface(SpiceVirtualChannel *svc)
{
    if (!svc->active) {
        return;
    }
    dprintf(svc, 1, "%s\n", __func__);
    spice_server_remove_interface(&svc->sin.base);
    svc->active = false;
}


static void vmc_change_state_handler(void *opaque, int running, int reason)
{
    SpiceVirtualChannel *svc = opaque;

    if (running && svc->active) {
        spice_server_char_device_wakeup(&svc->sin);
    }
}

/*
 * virtio-serial callbacks
 */

static void vmc_guest_open(VirtIOSerialPort *port)
{
    SpiceVirtualChannel *svc = DO_UPCAST(SpiceVirtualChannel, port, port);

    dprintf(svc, 1, "%s\n", __func__);
    vmc_register_interface(svc);
}

static void vmc_guest_close(VirtIOSerialPort *port)
{
    SpiceVirtualChannel *svc = DO_UPCAST(SpiceVirtualChannel, port, port);

    dprintf(svc, 1, "%s\n", __func__);
    vmc_unregister_interface(svc);
}

static void vmc_guest_ready(VirtIOSerialPort *port)
{
    SpiceVirtualChannel *svc = DO_UPCAST(SpiceVirtualChannel, port, port);

    dprintf(svc, 1, "%s\n", __func__);
    if (svc->active) {
        spice_server_char_device_wakeup(&svc->sin);
    }
}

static void vmc_have_data(VirtIOSerialPort *port, const uint8_t *buf, size_t len)
{
    SpiceVirtualChannel *svc = DO_UPCAST(SpiceVirtualChannel, port, port);

    dprintf(svc, 2, "%s: %zd\n", __func__, len);
    assert(svc->datalen == 0);
    if (svc->bufsize < len) {
        svc->bufsize = len;
        svc->buffer = qemu_realloc(svc->buffer, svc->bufsize);
    }
    memcpy(svc->buffer, buf, len);
    svc->datapos = svc->buffer;
    svc->datalen = len;
    virtio_serial_throttle_port(&svc->port, true);
    spice_server_char_device_wakeup(&svc->sin);
}

static void vmc_print_optional_subtypes(void)
{
    const char** psubtype = spice_server_char_device_recognized_subtypes();
    int i;

    fprintf(stderr, "supported subtypes: ");
    for(i=0; *psubtype != NULL; ++psubtype, ++i) {
        if (i == 0) {
            fprintf(stderr, "%s", *psubtype);
        } else {
            fprintf(stderr, ", %s", *psubtype);
        }
    }
    fprintf(stderr, "\n");
}

static int vmc_initfn(VirtIOSerialDevice *dev)
{
    VirtIOSerialPort *port = DO_UPCAST(VirtIOSerialPort, dev, &dev->qdev);
    SpiceVirtualChannel *svc = DO_UPCAST(SpiceVirtualChannel, port, port);
    const char** psubtype = spice_server_char_device_recognized_subtypes();
    const char *subtype = NULL;

    if (!using_spice) {
        return -1;
    }

    dprintf(svc, 1, "%s\n", __func__);

    if (svc->subtype == NULL) {
        svc->subtype = strdup("vdagent");
    }

    for(;*psubtype != NULL; ++psubtype) {
        if (strcmp(svc->subtype, *psubtype) == 0) {
            subtype = *psubtype;
            break;
        }
    }
    if (subtype == NULL) {
        fprintf(stderr, "spice-vmc: unsupported subtype\n");
        vmc_print_optional_subtypes();
        return -1;
    }
    port->name = qemu_strdup(VMC_GUEST_DEVICE_NAME);
    svc->vmstate = qemu_add_vm_change_state_handler
        (vmc_change_state_handler, svc);
    svc->sin.subtype = svc->subtype;
    virtio_serial_open(port);
    return 0;
}

static int vmc_exitfn(VirtIOSerialDevice *dev)
{
    VirtIOSerialPort *port = DO_UPCAST(VirtIOSerialPort, dev, &dev->qdev);
    SpiceVirtualChannel *svc = DO_UPCAST(SpiceVirtualChannel, port, port);

    dprintf(svc, 1, "%s\n", __func__);
    vmc_unregister_interface(svc);
    qemu_del_vm_change_state_handler(svc->vmstate);
    virtio_serial_close(port);
    return 0;
}

static VirtIOSerialPortInfo vmc_info = {
    .qdev.name     = VMC_DEVICE_NAME,
    .qdev.size     = sizeof(SpiceVirtualChannel),
    .init          = vmc_initfn,
    .exit          = vmc_exitfn,
    .guest_open    = vmc_guest_open,
    .guest_close   = vmc_guest_close,
    .guest_ready   = vmc_guest_ready,
    .have_data     = vmc_have_data,
    .qdev.props = (Property[]) {
        DEFINE_PROP_UINT32("nr", SpiceVirtualChannel, port.id, VIRTIO_CONSOLE_BAD_ID),
        DEFINE_PROP_UINT32("debug", SpiceVirtualChannel, debug, 1),
        DEFINE_PROP_STRING("subtype", SpiceVirtualChannel, subtype),
        DEFINE_PROP_END_OF_LIST(),
    }
};

static void vmc_register(void)
{
    virtio_serial_port_qdev_register(&vmc_info);
}
device_init(vmc_register)
