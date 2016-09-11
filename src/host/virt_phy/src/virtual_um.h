#pragma once

#include <stdint.h>
#include <osmocom/core/msgb.h>
#include <osmocom/core/select.h>

#define VIRTUAL_UM_ADDR "239.0.47.29" /* Default multicast address */

struct virt_um_inst {
	void *priv; // Will be appended after osmo-fd's data pointer.
	struct osmo_fd ofd; /* Osmocom file descriptor containing mcast sock connection. */
	void (*recv_cb)(struct virt_um_inst *vui, struct msgb *msg); /* Callback function called for incoming messages. */
};

/**
 * @brief Initialise virtual physical layer.
 *
 * @param[in] ctx The talloc context to hang the result off.
 * @param[in] group The multicast group.
 * @param[in] port The port.
 * @param[in] netdev The network device.
 * @param[in] priv The physical link ??? TODO: What is that rly used for?
 * @param[in] recv_cb The callback function called for incoming messages.
 *
 * @return The initialised virtual um instance.
 */
struct virt_um_inst *virt_um_init(
                void *ctx, const char *group, uint16_t port, const char *netdev,
                void *priv,
                void (*recv_cb)(struct virt_um_inst *vui, struct msgb *msg));
/**
 * @brief destroy the virtual um interface.
 *
 * @param[in] vui The virtual um interface to destroy.
 */
void virt_um_destroy(struct virt_um_inst *vui);

/**
 * @brief Write a message to the virtual um interface.
 *
 * @param[in] vui The virtual um interface.
 * @param[in] msg The message to write.
 */
int virt_um_write_msg(struct virt_um_inst *vui, struct msgb *msg);
