/* osmocom includes */

#include <osmocom/core/msgb.h>
#include <osmocom/core/select.h>
#include <stdlib.h>
#include <layer1/l23_api.h>
#include <osmocom/core/logging.h>
#include "virtual_um.h"
#include <osmo-bts/scheduler.h>

/**
 * Send a message over the virtual um interface.
 * This will at first wrap the msg with a gsmtap header and then write it to the declared multicast socket.
 * TODO Fix dependencies
 */
static void tx_to_virt_um(struct l1sched_trx *l1t, uint8_t tn, uint32_t fn,
		enum trx_chan_type chan, struct msgb *msg) {
//	const struct trx_chan_desc *chdesc = &trx_chan_desc[chan];
//	uint8_t ss = 0; //FIXME(chdesc);
//	uint8_t gsmtap_chan;
//	struct msgb *outmsg;
//
//	gsmtap_chan = chantype_rsl2gsmtap(chdesc->chan_nr, chdesc->link_id);
//	outmsg = gsmtap_makemsg(l1t->trx->arfcn, tn, gsmtap_chan, ss, fn, 0, 0,
//			msgb_l2(msg), msgb_l2len(msg));
//	if (outmsg) {
//		struct phy_instance *pinst = trx_phy_instance(l1t->trx);
//		struct virt_um_inst *virt_um = pinst->phy_link->u.virt.virt_um;
//		virt_um_write_msg(virt_um, outmsg);
//	}
//
//	/* free message */
//	msgb_free(msg);
}
/**
 * Receive GSMTAP msg over virtual um, extract it and call a handler dependent of the logical channel.
 */
static void virt_um_rcv_cb(struct virt_um_inst *vui, struct msgb *msg) {

	// TODO: Fix logging which is currently not working
	LOGP(DLGLOBAL, LOGL_ERROR, "Message incoming on virtual om: %s\n",
			msg->data);
	printf("Message incoming on virtual om: %s was logged\n", msg->data);
	// TODO: replace by valid handler that enqueues the incoming messages for a l1_primitive handler
}

static void tx_to_l23(struct msgb *msg) {
	// TODO: Fix logging which is currently not working
	printf("Message outgoing to l2: %s was logged\n", msg->data);
	// TODO: replace by valid handler that enqueues the incoming messages for a l1_primitive handler
}

static void l23_rcv_cb(struct msgb *msg) {
	// TODO: Fix logging which is currently not working
	printf("Message outgoing to l2: %s was logged\n", msg->data);
	// TODO: replace by valid handler that enqueues the incoming messages for a l1_primitive handler
}

static void tx_l1ctl_to_l1() {

}

static void dummy_l1a_l23_tx_cb() {
// TODO woot?
}

static void dummy_virtual_um_rcv_cb() {
// TODO woot?
}

int main(void) {

	// init loginfo
	static const struct log_info log_info = { };
	log_init(&log_info, NULL);

	printf("%s\n", "Virtual physical layer starting up...");

	virt_um_init(NULL, "224.0.0.1", 6666, "wlan0", NULL,
			dummy_virtual_um_rcv_cb);
	l1a_l23api_init();

	/* register callback method for transmitted messages to l2 */
	l1a_l23_tx_cb = dummy_l1a_l23_tx_cb;

	/* inform l2 and upwards that we are ready for orders */
	l1ctl_tx_reset(L1CTL_RESET_IND, L1CTL_RES_T_BOOT);

	printf("%s\n", "Virtual physical layer ready...");

	while (1) {
		// handle osmocom fd READ events (l1ctl-unix-socket, virtual-um-mcast-socket)
		osmo_select_main(1);
		// handle incoming l1ctl primitives from l2
		l1a_l23_handler();
		// handle outgoing l1ctl primitives to l2
		// TODO
		// handle outgoing gsmtap messages on virtual um (uplink)
		// TODO
	}

	// not reached
	return EXIT_FAILURE;
}
