/* L1CTL receive routines.  */

#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <osmocom/core/msgb.h>
#include <l1ctl_proto.h>

#include "logging.h"
#include "l1ctl_sap.h"

/**
 * @brief Handler for received L1CTL_FBSB_REQ from L23.
 *
 * -- frequency burst synchronisation burst request --
 *
 * @param [in] msg the received message.
 *
 * Transmit frequency control and synchronisation bursts on FCCH and SCH to calibrate transceiver and search for base stations.
 *
 * Note: Not needed for virtual physical layer.
 */
void l1ctl_rx_fbsb_req(struct msgb *msg)
{
	struct l1ctl_hdr *l1h = (struct l1ctl_hdr *)msg->data;
	struct l1ctl_fbsb_req *sync_req = (struct l1ctl_fbsb_req *)l1h->data;

	DEBUGP(DL1C,
	                "Received and ignored from l23 - L1CTL_FBSB_REQ (arfcn=%u, flags=0x%x)\n",
	                ntohs(sync_req->band_arfcn), sync_req->flags);
}

/**
 * @brief Handler for received L1CTL_DM_EST_REQ from L23.
 *
 * -- dedicated mode established request --
 *
 * @param [in] msg the received message.
 *
 * Handle state change from idle to dedicated mode.
 *
 * TODO: Implement this handler routine!
 */
void l1ctl_rx_dm_est_req(struct msgb *msg)
{
	struct l1ctl_hdr *l1h = (struct l1ctl_hdr *)msg->data;
	struct l1ctl_info_ul *ul = (struct l1ctl_info_ul *)l1h->data;
	struct l1ctl_dm_est_req *est_req =
	                (struct l1ctl_dm_est_req *)ul->payload;

	DEBUGP(DL1C,
	                "Received and handled from l23 - L1CTL_DM_EST_REQ (arfcn=%u, chan_nr=0x%02x, tsc=%u)\n",
	                ntohs(est_req->h0.band_arfcn), ul->chan_nr,
	                est_req->tsc);

//	/* disable neighbour cell measurement of C0 TS 0 */
//	mframe_disable(MF_TASK_NEIGH_PM51_C0T0);
//
//	/* configure dedicated channel state */
//	l1s.dedicated.type = chan_nr2dchan_type(ul->chan_nr);
//	l1s.dedicated.tsc  = est_req->tsc;
//	l1s.dedicated.tn   = ul->chan_nr & 0x7;
//	l1s.dedicated.h    = est_req->h;
//
//	if (est_req->h) {
//		int i;
//		l1s.dedicated.h1.hsn  = est_req->h1.hsn;
//		l1s.dedicated.h1.maio = est_req->h1.maio;
//		l1s.dedicated.h1.n    = est_req->h1.n;
//		for (i=0; i<est_req->h1.n; i++)
//			l1s.dedicated.h1.ma[i] = ntohs(est_req->h1.ma[i]);
//	} else {
//		l1s.dedicated.h0.arfcn = ntohs(est_req->h0.band_arfcn);
//	}
//
//	/* TCH config */
//	if (chan_nr_is_tch(ul->chan_nr)) {
//		/* Mode */
//		l1a_tch_mode_set(est_req->tch_mode);
//		l1a_audio_mode_set(est_req->audio_mode);
//
//		/* Sync */
//		l1s.tch_sync = 1;	/* can be set without locking */
//
//		/* Audio path */
//		audio_set_enabled(est_req->tch_mode, est_req->audio_mode);
//	}
//
//	/* figure out which MF tasks to enable */
//	l1a_mftask_set(chan_nr2mf_task_mask(ul->chan_nr, NEIGH_MODE_PM));
}

/**
 * @brief Handler for received L1CTL_DM_FREQ_REQ from L23.
 *
 * -- dedicated mode frequency request --
 *
 * @param [in] msg the received message.
 *
 * Handle frequency change in dedicated mode. E.g. used for frequency hopping.
 *
 * Note: Not needed for virtual physical layer.
 */
void l1ctl_rx_dm_freq_req(struct msgb *msg)
{
	struct l1ctl_hdr *l1h = (struct l1ctl_hdr *)msg->data;
	struct l1ctl_info_ul *ul = (struct l1ctl_info_ul *)l1h->data;
	struct l1ctl_dm_freq_req *freq_req =
	                (struct l1ctl_dm_freq_req *)ul->payload;

	DEBUGP(DL1C,
	                "Received and ignored from l23 - L1CTL_DM_FREQ_REQ (arfcn=%u, tsc=%u)\n",
	                ntohs(freq_req->h0.band_arfcn), freq_req->tsc);
}

/**
 * @brief Handler for received L1CTL_CRYPTO_REQ from L23.
 *
 * -- cryptographic request --
 *
 * @param [in] msg the received message.
 *
 * Configure the key and algorithm used for cryptographic operations in the DSP (Digital Signal Processor).
 *
 * Note: in the virtual physical layer the cryptographic operations are not handled in the DSP.
 *
 * TODO: Implement cryptographic operations for virtual um!
 * TODO: Implement this handler routine!
 */
void l1ctl_rx_crypto_req(struct msgb *msg)
{
	struct l1ctl_hdr *l1h = (struct l1ctl_hdr *)msg->data;
	struct l1ctl_info_ul *ul = (struct l1ctl_info_ul *)l1h->data;
	struct l1ctl_crypto_req *cr = (struct l1ctl_crypto_req *)ul->payload;
	uint8_t key_len = msg->len - sizeof(*l1h) - sizeof(*ul) - sizeof(*cr);

	DEBUGP(DL1C,
	                "Received and handled from l23 - L1CTL_CRYPTO_REQ (algo=A5/%u, len=%u)\n",
	                cr->algo, key_len);

//	if (cr->algo && key_len != 8) {
//		DEBUGP(DL1C, "L1CTL_CRYPTO_REQ -> Invalid key\n");
//		return;
//	}
//
//	dsp_load_ciph_param(cr->algo, cr->key);
}

/**
 * @brief Handler for received L1CTL_DM_REL_REQ from L23.
 *
 * -- dedicated mode release request --
 *
 * @param [in] msg the received message.
 *
 * Handle state change from dedicated to idle mode. Flush message buffers of dedicated channel.
 *
 * TODO: Implement this handler routine!
 */
void l1ctl_rx_dm_rel_req(struct msgb *msg)
{
//	struct l1ctl_hdr *l1h = (struct l1ctl_hdr *)msg->data;

	DEBUGP(DL1C, "Received and ignored from l23 - L1CTL_DM_REL_REQ\n");
//	l1a_mftask_set(0);
//	l1s.dedicated.type = GSM_DCHAN_NONE;
//	l1a_txq_msgb_flush(&l1s.tx_queue[L1S_CHAN_MAIN]);
//	l1a_txq_msgb_flush(&l1s.tx_queue[L1S_CHAN_SACCH]);
//	l1a_txq_msgb_flush(&l1s.tx_queue[L1S_CHAN_TRAFFIC]);
//	l1a_meas_msgb_set(NULL);
//	dsp_load_ciph_param(0, NULL);
//	l1a_tch_mode_set(GSM48_CMODE_SIGN);
//	audio_set_enabled(GSM48_CMODE_SIGN, 0);
//	l1s.neigh_pm.n = 0;
}

/**
 * @brief Handler for received L1CTL_PARAM_REQ from L23.
 *
 * -- parameter request --
 *
 * @param [in] msg the received message.
 *
 * Configure transceiver parameters timing advance value and sending power.
 *
 * Note: Not needed for virtual physical layer.
 */
void l1ctl_rx_param_req(struct msgb *msg)
{
	struct l1ctl_hdr *l1h = (struct l1ctl_hdr *)msg->data;
	struct l1ctl_info_ul *ul = (struct l1ctl_info_ul *)l1h->data;
	struct l1ctl_par_req *par_req = (struct l1ctl_par_req *)ul->payload;

	DEBUGP(DL1C,
	                "Received and ignored from l23 - L1CTL_PARAM_REQ (ta=%d, tx_power=%d)\n",
	                par_req->ta, par_req->tx_power);
}

/**
 * @brief Handler for received L1CTL_RACH_REQ from L23.
 *
 * -- random access channel request --
 *
 * @param [in] msg the received message.
 *
 * Transmit RACH request on RACH.
 *
 * TODO: Implement this handler routine!
 */
void l1ctl_rx_rach_req(struct msgb *msg)
{
	struct l1ctl_hdr *l1h = (struct l1ctl_hdr *)msg->data;
	struct l1ctl_info_ul *ul = (struct l1ctl_info_ul *)l1h->data;
	struct l1ctl_rach_req *rach_req = (struct l1ctl_rach_req *)ul->payload;

	DEBUGP(DL1C,
	                "Received and handled from l23 - L1CTL_RACH_REQ (ra=0x%02x, offset=%d combined=%d)\n",
	                rach_req->ra, ntohs(rach_req->offset),
	                rach_req->combined);

//	l1a_rach_req(ntohs(rach_req->offset), rach_req->combined,
//		rach_req->ra);
}

/**
 * @brief Handler for received L1CTL_DATA_REQ from L23.
 *
 * -- data request --
 *
 * @param [in] msg the received message.
 *
 * Transmit message on a signalling channel. FACCH/SDCCH or SACCH depending on the headers set link id (TS 8.58 - 9.3.2).
 *
 * TODO: Implement this handler routine!
 */
void l1ctl_rx_data_req(struct msgb *msg)
{
	struct l1ctl_hdr *l1h = (struct l1ctl_hdr *)msg->data;
	struct l1ctl_info_ul *ul = (struct l1ctl_info_ul *)l1h->data;
	struct l1ctl_data_ind *data_ind = (struct l1ctl_data_ind *)ul->payload;
	struct llist_head *tx_queue;

	DEBUGP(DL1C,
	                "Received and handled from l23 - L1CTL_DATA_REQ (link_id=0x%02x)\n",
	                ul->link_id);

//	msg->l3h = data_ind->data;
//	if (ul->link_id & 0x40) {
//		struct gsm48_hdr *gh = (struct gsm48_hdr *)(data_ind->data + 5);
//		if (gh->proto_discr == GSM48_PDISC_RR
//		 && gh->msg_type == GSM48_MT_RR_MEAS_REP) {
//			DEBUGP(DL1C, "updating measurement report\n");
//			l1a_meas_msgb_set(msg);
//			return;
//		}
//		tx_queue = &l1s.tx_queue[L1S_CHAN_SACCH];
//	} else
//		tx_queue = &l1s.tx_queue[L1S_CHAN_MAIN];
//
//	DEBUGP(DL1C, "ul=%p, ul->payload=%p, data_ind=%p, data_ind->data=%p l3h=%p\n",
//		ul, ul->payload, data_ind, data_ind->data, msg->l3h);
//
//	l1a_txq_msgb_enq(tx_queue, msg);
}

/**
 * @brief Handler for received L1CTL_PM_REQ from L23.
 *
 * -- power measurement request --
 *
 * @param [in] msg the received message.
 *
 * Process power measurement to calculate and adjust optimal sending power.
 *
 * Note: Not needed for virtual physical layer.
 */
void l1ctl_rx_pm_req(struct msgb *msg)
{
	struct l1ctl_hdr *l1h = (struct l1ctl_hdr *)msg->data;
	struct l1ctl_pm_req *pm_req = (struct l1ctl_pm_req *)l1h->data;

	DEBUGP(DL1C, "Received and ignored from l23 - L1CTL_PM_REQ TYPE=%u\n",
	                pm_req->type);
}

/**
 * @brief Handler for received L1CTL_RESET_REQ from L23.
 *
 * -- reset request --
 *
 * @param [in] msg the received message.
 *
 * Reset layer 1 (state machine, scheduler, transceiver) depending on the reset type.
 *
 * TODO: Implement this handler routine!
 */
void l1ctl_rx_reset_req(struct msgb *msg)
{
	struct l1ctl_hdr *l1h = (struct l1ctl_hdr *)msg->data;
	struct l1ctl_reset *reset_req = (struct l1ctl_reset *)l1h->data;

	switch (reset_req->type) {
	case L1CTL_RES_T_FULL:
		DEBUGP(DL1C,
		                "Received and handled from l23 - L1CTL_RESET_REQ (type=FULL)\n");
//		l1s_reset();
//		l1s_reset_hw();
//		audio_set_enabled(GSM48_CMODE_SIGN, 0);
		l1ctl_tx_reset(L1CTL_RESET_CONF, reset_req->type);
		break;
	case L1CTL_RES_T_SCHED:
		DEBUGP(DL1C,
		                "Received and handled from l23 - L1CTL_RESET_REQ (type=SCHED)\n");
//		l1ctl_tx_reset(L1CTL_RESET_CONF, reset_req->type);
//		sched_gsmtime_reset();
		break;
	default:
		LOGP(DL1C, LOGL_ERROR,
		                "Received and ignored from l23 - L1CTL_RESET_REQ (type=unknown)\n");
		break;
	}
}

/**
 * @brief Handler for received L1CTL_CCCH_MODE_REQ from L23.
 *
 * -- common control channel mode request --
 *
 * @param [in] msg the received message.
 *
 * Configure CCCH combined / non-combined mode.
 *
 * TODO: Implement this handler routine!
 */
void l1ctl_rx_ccch_mode_req(struct msgb *msg)
{
	struct l1ctl_hdr *l1h = (struct l1ctl_hdr *)msg->data;
	struct l1ctl_ccch_mode_req *ccch_mode_req =
	                (struct l1ctl_ccch_mode_req *)l1h->data;
	uint8_t ccch_mode = ccch_mode_req->ccch_mode;

	DEBUGP(DL1C, "Received and handled from l23 - L1CTL_CCCH_MODE_REQ\n");

//	/* pre-set the CCCH mode */
//	l1s.serving_cell.ccch_mode = ccch_mode;
//
//	/* Update task */
//	mframe_disable(MF_TASK_CCCH_COMB);
//	mframe_disable(MF_TASK_CCCH);
//
//	if (ccch_mode == CCCH_MODE_COMBINED)
//		mframe_enable(MF_TASK_CCCH_COMB);
//	else if (ccch_mode == CCCH_MODE_NON_COMBINED)
//		mframe_enable(MF_TASK_CCCH);
//
//	l1ctl_tx_ccch_mode_conf(ccch_mode);
}

/**
 * @brief Handler for received L1CTL_TCH_MODE_REQ from L23.
 *
 * -- traffic channel mode request --
 *
 * @param [in] msg the received message.
 *
 * Configure TCH mode and audio mode.
 *
 * TODO: Implement this handler routine!
 */
void l1ctl_rx_tch_mode_req(struct msgb *msg)
{
	struct l1ctl_hdr *l1h = (struct l1ctl_hdr *)msg->data;
	struct l1ctl_tch_mode_req *tch_mode_req =
	                (struct l1ctl_tch_mode_req *)l1h->data;
	uint8_t tch_mode = tch_mode_req->tch_mode;
	uint8_t audio_mode = tch_mode_req->audio_mode;

	DEBUGP(DL1C,
	                "Received and handled from l23 - L1CTL_TCH_MODE_REQ (tch_mode=0x%02x audio_mode=0x%02x)\n",
	                tch_mode, audio_mode);
//	tch_mode = l1a_tch_mode_set(tch_mode);
//	audio_mode = l1a_audio_mode_set(audio_mode);
//
//	audio_set_enabled(tch_mode, audio_mode);
//
//	l1s.tch_sync = 1; /* Needed for audio to work */
//
//	l1ctl_tx_tch_mode_conf(tch_mode, audio_mode);
}

/**
 * @brief Handler for received L1CTL_NEIGH_PM_REQ from L23.
 *
 * -- neighbor power measurement request --
 *
 * @param [in] msg the received message.
 *
 * Update the maintained list of neighbor cells used in neighbor cell power measurement.
 * The neighbor cell description is one of the info messages sent by the BTS on BCCH.
 * This method will also enable neighbor measurement in the multiframe scheduler.
 *
 * Note: Not needed for virtual physical layer.
 */
void l1ctl_rx_neigh_pm_req(struct msgb *msg)
{
	struct l1ctl_hdr *l1h = (struct l1ctl_hdr *)msg->data;
	struct l1ctl_neigh_pm_req *pm_req =
	                (struct l1ctl_neigh_pm_req *)l1h->data;

	DEBUGP(DL1C,
	                "Received and ignored from l23 - L1CTL_NEIGH_PM_REQ new list with %u entries\n",
	                pm_req->n);
}

/**
 * @brief Handler for received L1CTL_TRAFFIC_REQ from L23.
 *
 * -- traffic request --
 *
 * @param [in] msg the received message.
 *
 * Enqueue the message (traffic frame) to the L1 state machine's transmit queue.
 * Will drop the traffic frame at queue sizes >= 4.
 *
 * TODO: Implement this handler routine!
 */
void l1ctl_rx_traffic_req(struct msgb *msg)
{
	struct l1ctl_hdr *l1h = (struct l1ctl_hdr *)msg->data;
	struct l1ctl_info_ul *ul = (struct l1ctl_info_ul *)l1h->data;
	struct l1ctl_traffic_req *tr = (struct l1ctl_traffic_req *)ul->payload;
	int num = 0;

	DEBUGP(DL1C, "Received and handled from l23 - L1CTL_TRAFFIC_REQ\n");

//	msg->l2h = tr->data;

//	num = l1a_txq_msgb_count(&l1s.tx_queue[L1S_CHAN_TRAFFIC]);
//	if (num >= 4) {
//		DEBUGP(DL1C, "dropping traffic frame\n");
//		msgb_free(msg);
//		return;
//	}
//
//	l1a_txq_msgb_enq(&l1s.tx_queue[L1S_CHAN_TRAFFIC], msg);
}

/**
 * @brief Handler for received L1CTL_SIM_REQ from L23.
 *
 * -- sim request --
 *
 * @param [in] msg the received message.
 *
 * Forward and a sim request to the SIM APDU.
 *
 * Note: Not needed for virtual layer. Please configure layer23 application to use test-sim implementation.
 * ms <x>
 * --------
 * sim test
 * test-sim
 *  imsi <xxxxxxxxxxxxxxx>
 *  ki comp128 <xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx>
 * --------
 */
void l1ctl_rx_sim_req(struct msgb *msg)
{
	uint16_t len = msg->len - sizeof(struct l1ctl_hdr);
	uint8_t *data = msg->data + sizeof(struct l1ctl_hdr);

	DEBUGP(DL1C,
	                "Received and ignored from l23 - SIM Request length: %u, data: %s: ",
	                len, osmo_hexdump(data, sizeof(data)));

}
