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
#include <osmocom/core/utils.h>
#include <osmocom/core/msgb.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <l1ctl_proto.h>

#include "l1ctl_sap.h"
#include "gsmtapl1_if.h"
#include "virtual_um.h"
#include "logging.h"

static struct virt_um_inst *_vui = NULL;
static struct l1ctl_sock_inst *_lsi = NULL;

// for debugging
static const struct value_string gsmtap_channels [22] = {
	{ GSMTAP_CHANNEL_UNKNOWN,	"UNKNOWN" },
	{ GSMTAP_CHANNEL_BCCH,		"BCCH" },
	{ GSMTAP_CHANNEL_CCCH,		"CCCH" },
	{ GSMTAP_CHANNEL_RACH,		"RACH" },
	{ GSMTAP_CHANNEL_AGCH,		"AGCH" },
	{ GSMTAP_CHANNEL_PCH,		"PCH" },
	{ GSMTAP_CHANNEL_SDCCH,		"SDCCH" },
	{ GSMTAP_CHANNEL_SDCCH4,	"SDCCH/4" },
	{ GSMTAP_CHANNEL_SDCCH8,	"SDCCH/8" },
	{ GSMTAP_CHANNEL_TCH_F,		"FACCH/F" },
	{ GSMTAP_CHANNEL_TCH_H,		"FACCH/H" },
	{ GSMTAP_CHANNEL_PACCH,		"PACCH" },
	{ GSMTAP_CHANNEL_CBCH52,    	"CBCH" },
	{ GSMTAP_CHANNEL_PDCH,      	"PDCH" },
	{ GSMTAP_CHANNEL_PTCCH,    	"PTTCH" },
	{ GSMTAP_CHANNEL_CBCH51,    	"CBCH" },
        { GSMTAP_CHANNEL_ACCH|
	  GSMTAP_CHANNEL_SDCCH,		"LSACCH" },
	{ GSMTAP_CHANNEL_ACCH|
	  GSMTAP_CHANNEL_SDCCH4,	"SACCH/4" },
	{ GSMTAP_CHANNEL_ACCH|
	  GSMTAP_CHANNEL_SDCCH8,	"SACCH/8" },
	{ GSMTAP_CHANNEL_ACCH|
	  GSMTAP_CHANNEL_TCH_F,		"SACCH/F" },
	{ GSMTAP_CHANNEL_ACCH|
	  GSMTAP_CHANNEL_TCH_H,		"SACCH/H" },
	{ 0,				NULL },
};
// for debugging
static const struct value_string gsmtap_types [10] = {
	{ GSMTAP_TYPE_UM,		"GSM Um (MS<->BTS)" },
	{ GSMTAP_TYPE_ABIS,		"GSM Abis (BTS<->BSC)" },
	{ GSMTAP_TYPE_UM_BURST,		"GSM Um burst (MS<->BTS)" },
	{ GSMTAP_TYPE_SIM,		"SIM" },
	{ GSMTAP_TYPE_TETRA_I1, 	"TETRA V+D"},
	{ GSMTAP_TYPE_WMX_BURST,	"WiMAX burst" },
	{ GSMTAP_TYPE_GMR1_UM, 		"GMR-1 air interfeace (MES-MS<->GTS)" },
	{ GSMTAP_TYPE_UMTS_RLC_MAC,	"UMTS RLC/MAC" },
	{ GSMTAP_TYPE_UMTS_RRC,		"UMTS RRC" },
	{ 0,				NULL },
};

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

/* This is the header as it is used by gsmtap peer virtual layer 1.
struct gsmtap_hdr {
	guint8 version;		// version, set to 0x01 currently
	guint8 hdr_len;		// length in number of 32bit words
	guint8 type;		// see GSMTAP_TYPE_*
	guint8 timeslot;	// timeslot (0..7 on Um)
	guint16 arfcn;		// ARFCN (frequency)
	gint8 signal_dbm;	// signal level in dBm
	gint8 snr_db;		// signal/noise ratio in dB
	guint32 frame_number;	// GSM Frame Number (FN)
	guint8 sub_type;	// Type of burst/channel, see above
	guint8 antenna_nr;	// Antenna Number
	guint8 sub_slot;	// sub-slot within timeslot
	guint8 res;		// reserved for future use (RFU)
}
 */

/**
 * Receive a gsmtap message from the virt um.
 */
void gsmtapl1_rx_from_virt_um_inst_cb(struct virt_um_inst *vui,
                                      struct msgb *msg)
{
	if (msg) {
		struct gsmtap_hdr *gh = msgb_l1(msg);
		uint8_t gsmtap_chan_type = gh->type; // the logical channel type
		uint8_t timeslot = gh->timeslot; // indicates the physical channel
		uint8_t subslot = gh->sub_slot; // indicates the logical channel subslot on the physical channel FIXME: calculate

		struct l1ctl_hdr *l1ctlh;
		struct l1ctl_info_dl *l1dl;
		struct msgb *l1ctl_msg;

		msg->l2h = (uint8_t *) gh + sizeof(*gh);

		DEBUGP(DVIRPHY,
		                "Receiving gsmtap msg from virt um - (arfcn=%u, type=%s, subtype=%s, timeslot=%u, subslot=%u)\n",
		                gh->arfcn, get_value_string(gsmtap_types, gh->type), get_value_string(gsmtap_channels, gh->sub_type), gh->timeslot,
		                gh->sub_slot);

		// compose the l1ctl header for layer 2
		switch (gh->sub_type) {
		case GSMTAP_CHANNEL_RACH:
			LOGP(DL1C, LOGL_NOTICE,
			                "Ignoring gsmtap msg from virt um - channel type is uplink only!\n");
			goto nomessage;
		case GSMTAP_CHANNEL_SDCCH:
		case GSMTAP_CHANNEL_SDCCH4:
		case GSMTAP_CHANNEL_SDCCH8:
			l1ctl_msg = l1ctl_msgb_alloc(L1CTL_DATA_IND);
			// TODO: implement channel handling
			break;
		case GSMTAP_CHANNEL_TCH_F:
			l1ctl_msg = l1ctl_msgb_alloc(L1CTL_TRAFFIC_IND);
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
		case GSMTAP_CHANNEL_TCH_H:
		case GSMTAP_CHANNEL_PACCH:
		case GSMTAP_CHANNEL_PDCH:
		case GSMTAP_CHANNEL_PTCCH:
		case GSMTAP_CHANNEL_CBCH51:
		case GSMTAP_CHANNEL_CBCH52:
			LOGP(DL1C, LOGL_NOTICE,
			                "Ignoring gsmtap msg from virt um - channel type not supported!\n");
			goto nomessage;
		default:
			LOGP(DL1C, LOGL_NOTICE,
			                "Ignoring gsmtap msg from virt um - channel type unknown.\n");
			goto nomessage;
		}

		// fill l1ctl message with received l2 data
		l1ctl_msg->l2h = msgb_put(l1ctl_msg, msgb_l2len(msg));
		memcpy(l1ctl_msg->l2h, msgb_l2(msg), msgb_l2len(msg));
		/* forward l1ctl primitive */
		l1ctl_sap_tx_to_l23(l1ctl_msg);
		//DEBUGP(DL1C, "Message forwarded to layer 2.\n");
		return;

		// handle memory deallocation
		nomessage: free(msg);
	}
}

/**
 * @see void gsmtapl1_rx_from_virt_um_cb(struct virt_um_inst *vui, struct msgb msg).
 */
void gsmtapl1_rx_from_virt_um(struct msgb *msg)
{
	gsmtapl1_rx_from_virt_um_inst_cb(_vui, msg);
}

