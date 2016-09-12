/* GSMTAP layer1 is transmits gsmtap messages over a virtual layer 1.*/

/* (C) 2016 Sebastian Stumpf
 *
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <osmocom/core/gsmtap.h>
#include <osmocom/core/gsmtap_util.h>
#include <osmocom/core/msgb.h>
#include <stddef.h>
#include <stdlib.h>
#include <l1ctl_proto.h>

#include "l1ctl_sap.h"
#include "gsmtapl1_if.h"
#include "virtual_um.h"
#include "logging.h"

static struct virt_um_inst *_vui = NULL;
static struct l1ctl_sock_inst *_lsi = NULL;

void gsmtapl1_init(struct virt_um_inst *vui, struct l1ctl_sock_inst *lsi)
{
	_vui = vui;
	_lsi = lsi;
}

/**
 * Append a gsmtap header to msg and send it over the virt um.
 */
void gsmtapl1_tx_to_virt_um_inst(struct virt_um_inst *vui, struct msgb *msg)
{
	struct l1ctl_hdr *l1hdr = (struct l1ctl_hdr *)msg->l1h;
	struct l1ctl_info_dl *l1dl = (struct l1ctl_info_dl *)msg->data;
	uint8_t ss = 0;
	uint8_t gsmtap_chan;
	struct msgb *outmsg;

	switch (l1hdr->msg_type) {
	case L1CTL_DATA_REQ:
		// TODO: check what data request and set gsmtap_chan depending on that
		gsmtap_chan = 0;
		break;
	}
	outmsg = gsmtap_makemsg(l1dl->band_arfcn, l1dl->chan_nr, gsmtap_chan,
	                ss, l1dl->frame_nr, 0, 0, msgb_l2(msg),
	                msgb_l2len(msg));
	if (outmsg) {
		struct gsmtap_hdr *gh = (struct gsmtap_hdr *)outmsg->l1h;
		virt_um_write_msg(vui, outmsg);
		DEBUGP(DVIRPHY,
		                "Sending gsmtap msg to virt um - (arfcn=%u, type=%u, subtype=%u, timeslot=%u, subslot=%u)\n",
		                gh->arfcn, gh->type, gh->sub_type, gh->timeslot,
		                gh->sub_slot);
	} else {
		LOGP(DVIRPHY, LOGL_ERROR, "Gsmtap msg could not be created!\n");
	}

	/* free message */
	msgb_free(msg);
}

/**
 * @see void gsmtapl1_tx_to_virt_um(struct virt_um_inst *vui, struct msgb *msg).
 */
void gsmtapl1_tx_to_virt_um(struct msgb *msg)
{
	gsmtapl1_tx_to_virt_um_inst(_vui, msg);
}

/**
 * Converts msg to gsmtap and send it overt the virt um.
 */
void gsmtapl1_rx_from_virt_um_inst_cb(struct virt_um_inst *vui,
                                      struct msgb *msg)
{
	if (msg) {
		struct l1ctl_hdr *l1ctlh;
		struct l1ctl_info_dl *l1dl;
		struct gsmtap_hdr *gh = (struct gsmtap_hdr *)msg->l1h;
		struct msgb *l1ctl_msg = NULL;

		DEBUGP(DVIRPHY,
		                "Receiving gsmtap msg from virt um - (arfcn=%u, type=%u, subtype=%u, timeslot=%u, subslot=%u)\n",
		                gh->arfcn, gh->type, gh->sub_type, gh->timeslot,
		                gh->sub_slot);

		switch (gh->sub_type) {
		case GSMTAP_CHANNEL_RACH:
			LOGP(DL1C, LOGL_NOTICE,
			                "Ignoring gsmtap msg from virt um - channel type is uplink only!");
			goto nomessage;
		case GSMTAP_CHANNEL_SDCCH:
		case GSMTAP_CHANNEL_SDCCH4:
		case GSMTAP_CHANNEL_SDCCH8:
			l1ctl_msg = l1ctl_msgb_alloc(L1CTL_DATA_IND);
			// TODO: implement channel handling
			break;
		case GSMTAP_CHANNEL_TCH_F:
			l1ctl_msg = l1ctl_msgb_alloc(L1CTL_DATA_IND);
			// TODO: implement channel handling
			break;
		case GSMTAP_CHANNEL_TCH_H:
			l1ctl_msg = l1ctl_msgb_alloc(L1CTL_DATA_IND);
			// TODO: implement channel handling
			break;
		case GSMTAP_CHANNEL_AGCH:
			l1ctl_msg = l1ctl_msgb_alloc(L1CTL_DATA_IND);
			// TODO: implement channel handling
			break;
		case GSMTAP_CHANNEL_PCH:
			l1ctl_msg = l1ctl_msgb_alloc(L1CTL_DATA_IND);
			// TODO: implement channel handling
			break;
		case GSMTAP_CHANNEL_BCCH:
			l1ctl_msg = l1ctl_msgb_alloc(L1CTL_DATA_IND);
			// TODO: implement channel handling
			break;
		case GSMTAP_CHANNEL_CCCH:
		case GSMTAP_CHANNEL_PACCH:
		case GSMTAP_CHANNEL_PDCH:
		case GSMTAP_CHANNEL_PTCCH:
		case GSMTAP_CHANNEL_CBCH51:
		case GSMTAP_CHANNEL_CBCH52:
			LOGP(DL1C, LOGL_NOTICE,
			                "Ignoring gsmtap msg from virt um - channel type not supported!");
			goto nomessage;
		default:
			LOGP(DL1C, LOGL_NOTICE,
			                "Ignoring gsmtap msg from virt um - channel type unknown.");
			goto nomessage;
		}
		/* forward l1ctl primitive */
		l1ctl_sap_tx_to_l23(l1ctl_msg);
		DEBUGP(DL1C, "Message forwarded to layer 2.");
		return;

		// handle memory deallocation
		nomessage: free(msg);
		free(l1ctl_msg);
	}
}

/**
 * @see void gsmtapl1_rx_from_virt_um_cb(struct virt_um_inst *vui, struct msgb msg).
 */
void gsmtapl1_rx_from_virt_um(struct msgb *msg)
{
	gsmtapl1_rx_from_virt_um_inst_cb(_vui, msg);
}
