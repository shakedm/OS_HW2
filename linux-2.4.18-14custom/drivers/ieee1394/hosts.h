#ifndef _IEEE1394_HOSTS_H
#define _IEEE1394_HOSTS_H

#include <linux/wait.h>
#include <linux/tqueue.h>
#include <linux/list.h>
#include <asm/semaphore.h>

#include "ieee1394_types.h"
#include "csr.h"


struct hpsb_packet;

struct hpsb_host {
        struct list_head host_list;

        struct hpsb_host_operations *ops;
        void *hostdata;

        atomic_t generation;

        int refcount;

        struct list_head pending_packets;
        spinlock_t pending_pkt_lock;
        struct tq_struct timeout_tq;

        /* A bitmask where a set bit means that this tlabel is in use.
         * FIXME - should be handled per node instead of per bus. */
        u32 tlabel_pool[2];
        struct semaphore tlabel_count;
        spinlock_t tlabel_lock;

        unsigned char iso_listen_count[64];

        int node_count; /* number of identified nodes on this bus */
        int selfid_count; /* total number of SelfIDs received */
	int nodes_active; /* number of nodes that are actually active */

        nodeid_t node_id; /* node ID of this host */
        nodeid_t irm_id; /* ID of this bus' isochronous resource manager */
        nodeid_t busmgr_id; /* ID of this bus' bus manager */

        /* this nodes state */
        unsigned in_bus_reset:1;
        unsigned is_shutdown:1;

        /* this nodes' duties on the bus */
        unsigned is_root:1;
        unsigned is_cycmst:1;
        unsigned is_irm:1;
        unsigned is_busmgr:1;

        int reset_retries;
        quadlet_t *topology_map;
        u8 *speed_map;
        struct csr_control csr;

        struct hpsb_host_driver *driver;
        struct list_head driver_list;

	struct pci_dev *pdev;
};



enum devctl_cmd {
        /* Host is requested to reset its bus and cancel all outstanding async
         * requests.  If arg == 1, it shall also attempt to become root on the
         * bus.  Return void. */
        RESET_BUS,

        /* Arg is void, return value is the hardware cycle counter value. */
        GET_CYCLE_COUNTER,

        /* Set the hardware cycle counter to the value in arg, return void.
         * FIXME - setting is probably not required. */
        SET_CYCLE_COUNTER,

        /* Configure hardware for new bus ID in arg, return void. */
        SET_BUS_ID,

        /* If arg true, start sending cycle start packets, stop if arg == 0.
         * Return void. */
        ACT_CYCLE_MASTER,

        /* Cancel all outstanding async requests without resetting the bus.
         * Return void. */
        CANCEL_REQUESTS,

        /* Decrease host usage count if arg == 0, increase otherwise.  Return
         * 1 for success, 0 for failure.  Increase usage may fail if the driver
         * is in the process of shutting itself down.  Decrease usage can not
         * fail. */
        MODIFY_USAGE,

        /* Start or stop receiving isochronous channel in arg.  Return void.
         * This acts as an optimization hint, hosts are not required not to
         * listen on unrequested channels. */
        ISO_LISTEN_CHANNEL,
        ISO_UNLISTEN_CHANNEL
};

enum reset_types {
        /* 166 microsecond reset -- only type of reset available on
           non-1394a capable IEEE 1394 controllers */
        LONG_RESET,

        /* Short (arbitrated) reset -- only available on 1394a capable
           IEEE 1394 capable controllers */
        SHORT_RESET
};

struct hpsb_host_operations {
        /* This function must store a pointer to the configuration ROM into the
         * location referenced to by pointer and return the size of the ROM. It
         * may not fail.  If any allocation is required, it must be done
         * earlier.
         */
        size_t (*get_rom) (struct hpsb_host *host, const quadlet_t **pointer);

        /* This function shall implement packet transmission based on
         * packet->type.  It shall CRC both parts of the packet (unless
         * packet->type == raw) and do byte-swapping as necessary or instruct
         * the hardware to do so.  It can return immediately after the packet
         * was queued for sending.  After sending, hpsb_sent_packet() has to be
         * called.  Return 0 for failure.
         * NOTE: The function must be callable in interrupt context.
         */
        int (*transmit_packet) (struct hpsb_host *host, 
                                struct hpsb_packet *packet);

        /* This function requests miscellanous services from the driver, see
         * above for command codes and expected actions.  Return -1 for unknown
         * command, though that should never happen.
         */
        int (*devctl) (struct hpsb_host *host, enum devctl_cmd command, int arg);

        /* This function is mainly to redirect local CSR reads/locks to the iso
         * management registers (bus manager id, bandwidth available, channels
         * available) to the hardware registers in OHCI.  reg is 0,1,2,3 for bus
         * mgr, bwdth avail, ch avail hi, ch avail lo respectively (the same ids
         * as OHCI uses).  data and compare are the new data and expected data
         * respectively, return value is the old value.
         */
        quadlet_t (*hw_csr_reg) (struct hpsb_host *host, int reg,
                                 quadlet_t data, quadlet_t compare);
};

struct hpsb_host_driver {
        struct list_head list;

        struct list_head hosts;

        int number_of_hosts;
        const char *name;

        struct hpsb_host_operations *ops;
};


/* core internal use */
void register_builtin_lowlevels(void);

/* high level internal use */
struct hpsb_highlevel;
void hl_all_hosts(void (*function)(struct hpsb_host*));


/*
 * In order to prevent hosts from unloading, use hpsb_ref_host().  This prevents
 * the host from going away (e.g. makes module unloading of the driver
 * impossible), but still can not guarantee it (e.g. PC-Card being pulled by the
 * user).  hpsb_ref_host() returns false if host could not be locked.  If it is
 * successful, host is valid as a pointer until hpsb_unref_host() (not just
 * until after remove_host).
 */
int hpsb_ref_host(struct hpsb_host *host);
void hpsb_unref_host(struct hpsb_host *host);

struct hpsb_host *hpsb_alloc_host(struct hpsb_host_driver *drv, size_t extra);
void hpsb_add_host(struct hpsb_host *host);
void hpsb_remove_host(struct hpsb_host *h);

struct hpsb_host_driver *hpsb_register_lowlevel(struct hpsb_host_operations *op,
                                                const char *name);
void hpsb_unregister_lowlevel(struct hpsb_host_driver *drv);

#endif /* _IEEE1394_HOSTS_H */
