#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/notifier.h>

/* Policy Configuration: v1 only authorizes eth0 for XDP attachment */
#define AUTHORIZED_IFNAME "eth0"

/**
 * sentinel_validate_interface - Core enforcement logic
 * @dev: Pointer to the network device being monitored
 */
static void sentinel_validate_interface(struct net_device *dev) {
    if (strcmp(dev->name, AUTHORIZED_IFNAME) == 0) {
        pr_info("KBPF-Sentinel: [PASS] Authorized interface %s detected.\n", dev->name);
    } else {
        /* In v1, we log the unauthorized attempt for future audit features */
        pr_warn("KBPF-Sentinel: [BLOCK] Unauthorized interface %s detected. Enforcing policy.\n", dev->name);
    }
}

/**
 * sentinel_netdev_event - Notifier callback for network device status changes
 */
static int sentinel_netdev_event(struct notifier_block *nb, unsigned long event, void *ptr) {
    struct net_device *dev = netdev_notifier_info_to_dev(ptr);

    /* Intercept NETDEV_UP event (triggered by 'ip link set dev <name> up') */
    if (event == NETDEV_UP) {
        sentinel_validate_interface(dev);
    }
    return NOTIFY_DONE;
}

/* Define the sentinel notifier block */
static struct notifier_block sentinel_nb = {
    .notifier_call = sentinel_netdev_event,
};

/* Module Initialization */
static int __init sentinel_init(void) {
    pr_info("KBPF-Sentinel: Sentinel LKM engine initialized. Enforcement active.\n");
    register_netdevice_notifier(&sentinel_nb);
    return 0;
}

/* 1. Change 'int' to 'void' */
static void __exit sentinel_exit(void) {
    unregister_netdevice_notifier(&sentinel_nb);
    pr_info("KBPF-Sentinel: Sentinel LKM engine shutdown.\n");
    /* 2. No return statement needed for void functions */
}

/* This stays the same */
/* Ensure these 2 lines are at the very end of kmod/sentinel_main.c */
module_init(sentinel_init);
module_exit(sentinel_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Denny-Lin");
MODULE_DESCRIPTION("eBPF Enforcement Sentinel"); // Add this line!