/* osmocom includes */

#include "logging.h"
#include <osmocom/core/msgb.h>
#include <osmocom/core/select.h>
#include <osmo-bts/scheduler.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "virtual_um.h"
#include "l1ctl_sock.h"
#include "gsmtapl1_if.h"
#include "l1ctl_sap.h"

/**
 * Send a message over the virtual um interface.
 * This will at first wrap the msg with a gsmtap header and then write it to the declared multicast socket.
 * TODO Fix dependencies
 */
//static void tx_to_virt_um(struct l1sched_trx *l1t, uint8_t tn, uint32_t fn,
//                          enum trx_chan_type chan, struct msgb *msg)
//{
////	const struct trx_chan_desc *chdesc = &trx_chan_desc[chan];
////	uint8_t ss = 0; //FIXME(chdesc);
////	uint8_t gsmtap_chan;
////	struct msgb *outmsg;
////
////	gsmtap_chan = chantype_rsl2gsmtap(chdesc->chan_nr, chdesc->link_id);
////	outmsg = gsmtap_makemsg(l1t->trx->arfcn, tn, gsmtap_chan, ss, fn, 0, 0,
////			msgb_l2(msg), msgb_l2len(msg));
////	if (outmsg) {
////		struct phy_instance *pinst = trx_phy_instance(l1t->trx);
////		struct virt_um_inst *virt_um = pinst->phy_link->u.virt.virt_um;
////		virt_um_write_msg(virt_um, outmsg);
////	}
////
////	/* free message */
////	msgb_free(msg);
//}
/**
 * Receive GSMTAP msg over virtual um, extract it and call a handler dependent of the logical channel.
 */
//static void virt_um_rcv_cb(struct virt_um_inst *vui, struct msgb *msg)
//{
//	struct msgb *msg_dec = NULL;
//	msg_dec = gsmtap_decode_l1ctl(L1CTL_DATA_IND, msg);
//	// convert and forward incoming gsmtap messages as l1ctl
//	l1ctl_sap_tx_to_l23(msg_dec);
//	LOGP(DLGLOBAL, LOGL_ERROR, "Message incoming on virtual um: %s\n",
//	                msg->data);
//}

int main(void)
{

	// init loginfo
	static struct virt_um_inst *vui;
	static struct l1ctl_sock_inst *lsi;
	ms_log_init("DL1C,1:DVIRPHY,1");

	LOGP(DVIRPHY, LOGL_INFO, "Virtual physical layer starting up...\n");

	// TODO: make this configurable
	vui = virt_um_init(NULL, DEFAULT_BTS_MCAST_GROUP, DEFAULT_BTS_MCAST_PORT, DEFAULT_MS_MCAST_GROUP, DEFAULT_MS_MCAST_PORT, gsmtapl1_rx_from_virt_um_inst_cb);
	lsi = l1ctl_sock_init(NULL, l1ctl_sap_rx_from_l23_inst_cb, NULL);
	gsmtapl1_init(vui, lsi);
	l1ctl_sap_init(vui, lsi);

	/* inform l2 and upwards that we have booted and are ready for orders */
	l1ctl_tx_reset(L1CTL_RESET_IND, L1CTL_RES_T_BOOT);

	LOGP(DVIRPHY, LOGL_INFO, "Virtual physical layer ready...\n");

	while (1) {
		// handle osmocom fd READ events (l1ctl-unix-socket, virtual-um-mcast-socket)
		osmo_select_main(1);
		// handle incoming l1ctl primitives from l2
		l1ctl_sap_handler();
		// handle outgoing l1ctl primitives to l2
		// TODO implement scheduler for uplink messages
	}

	// not reached
	return EXIT_FAILURE;
}
