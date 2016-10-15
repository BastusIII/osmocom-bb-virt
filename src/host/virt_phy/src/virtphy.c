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

int main(void)
{

	// init loginfo
	static struct virt_um_inst *vui;
	static struct l1ctl_sock_inst *lsi;
	ms_log_init("DL1C,1:DVIRPHY,1");
	//ms_log_init("DL1C,8:DVIRPHY,8");

	LOGP(DVIRPHY, LOGL_INFO, "Virtual physical layer starting up...\n");

	// TODO: make this configurable
	vui = virt_um_init(NULL, DEFAULT_BTS_MCAST_GROUP, DEFAULT_BTS_MCAST_PORT, DEFAULT_MS_MCAST_GROUP, DEFAULT_MS_MCAST_PORT, gsmtapl1_rx_from_virt_um_inst_cb);
	lsi = l1ctl_sock_init(NULL, l1ctl_sap_rx_from_l23_inst_cb, NULL);
	gsmtapl1_init(vui, lsi);
	l1ctl_sap_init(vui, lsi);

	LOGP(DVIRPHY, LOGL_INFO, "Virtual physical layer ready...\n");

	while (1) {
		// handle osmocom fd READ events (l1ctl-unix-socket, virtual-um-mcast-socket)
		osmo_select_main(0);
		// handle queued l1ctl requests from l2
		//l1ctl_sap_handler();
		// handle outgoing l1ctl primitives to l2
		// TODO implement scheduler for uplink messages
	}

	// not reached
	return EXIT_FAILURE;
}
