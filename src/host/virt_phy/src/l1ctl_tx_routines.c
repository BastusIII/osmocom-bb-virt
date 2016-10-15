/* L1CTL transmit routines.  */

#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <osmocom/core/msgb.h>
#include <l1ctl_proto.h>

#include "l1ctl_sap.h"
#include "logging.h"

/**
 * @brief Transmit L1CTL_RESET_IND or L1CTL_RESET_CONF to layer 23.
 *
 * -- reset indication / confirm --
 *
 * @param [in] msg_type L1CTL primitive message type.
 * @param [in] reset_type reset type (full, boot or just scheduler reset).
 */
void l1ctl_tx_reset(uint8_t msg_type, uint8_t reset_type)
{
	struct msgb *msg = l1ctl_msgb_alloc(msg_type);
	struct l1ctl_reset *reset_resp;
	reset_resp = (struct l1ctl_reset *)msgb_put(msg, sizeof(*reset_resp));
	reset_resp->type = reset_type;

	DEBUGP(DL1C, "Sending to l23 - %s (reset_type: %u)\n",
	       	       getL1ctlPrimName(msg_type), reset_type);
	l1ctl_sap_tx_to_l23(msg);
}

/**
 * @brief Transmit L1CTL msg of a given type to layer 23.
 *
 * @param [in] msg_type L1CTL primitive message type.
 */
void l1ctl_tx_msg(uint8_t msg_type)
{
	struct msgb *msg = l1ctl_msgb_alloc(msg_type);
	DEBUGP(DL1C, "Sending to l23 - %s\n", getL1ctlPrimName(msg_type));
	l1ctl_sap_tx_to_l23(msg);
}

/**
 * @brief Transmit L1CTL_FBSB_CONF to l23.
 *
 * -- frequency burst synchronisation burst confirm --
 *
 * @param [in] res 0 -> success, 255 -> error.
 * @param [in] arfcn the arfcn we are synced to.
 *
 * No calculation needed for virtual pyh -> uses default values for a good link quality.
 */
void l1ctl_tx_fbsb_conf(uint8_t res, uint16_t arfcn)
{
	struct msgb *msg;
	struct l1ctl_fbsb_conf *resp;
	uint32_t fn = 0; // 0 should be okay here
	uint16_t snr = 40; // signal noise ratio > 40db is best signal.
	int16_t initial_freq_err = 0; // 0 means no error.
	uint8_t bsic = 0;

	msg = l1ctl_create_l2_msg(L1CTL_FBSB_CONF, fn,
			snr,
			arfcn);

	resp = (struct l1ctl_fbsb_conf *) msgb_put(msg, sizeof(*resp));
	resp->initial_freq_err = htons(initial_freq_err);
	resp->result = res;
	resp->bsic = bsic;

	DEBUGP(DL1C, "Sending to l23 - %s (res: %u)\n",
	                getL1ctlPrimName(L1CTL_FBSB_CONF), res);

	l1ctl_sap_tx_to_l23(msg);
}

/**
 * @brief Transmit L1CTL_CCCH_MODE_CONF to layer 23.
 *
 * -- common control channel mode confirm --
 *
 * @param [in] ccch_mode the new configured ccch mode. Combined or non-combined, see l1ctl_proto.
 *
 * Called by layer 1 to inform layer 2 that the ccch mode was successfully changed.
 */
void l1ctl_tx_ccch_mode_conf(uint8_t ccch_mode)
{
	struct msgb *msg = l1ctl_msgb_alloc(L1CTL_CCCH_MODE_CONF);
	struct l1ctl_ccch_mode_conf *mode_conf;
	mode_conf = (struct l1ctl_ccch_mode_conf *)msgb_put(msg,
	                sizeof(*mode_conf));
	mode_conf->ccch_mode = ccch_mode;

	DEBUGP(DL1C, "Sending to l23 - L1CTL_CCCH_MODE_CONF (mode: %u)\n",
	                ccch_mode);
	l1ctl_sap_tx_to_l23(msg);
}

/**
 * @brief Transmit L1CTL_TCH_MODE_CONF to layer 23.
 *
 * -- traffic channel mode confirm --
 *
 * @param [in] tch_mode the new configured traffic channel mode, see gsm48_chan_mode in gsm_04_08.h.
 * @param [in] audio_mode the new configured audio mode(s), see l1ctl_tch_mode_req in l1ctl_proto.h.
 *
 * Called by layer 1 to inform layer 23 that the traffic channel mode was successfully changed.
 */
void l1ctl_tx_tch_mode_conf(uint8_t tch_mode, uint8_t audio_mode)
{
	struct msgb *msg = l1ctl_msgb_alloc(L1CTL_TCH_MODE_CONF);
	struct l1ctl_tch_mode_conf *mode_conf;
	mode_conf = (struct l1ctl_tch_mode_conf *)msgb_put(msg,
	                sizeof(*mode_conf));
	mode_conf->tch_mode = tch_mode;
	mode_conf->audio_mode = audio_mode;

	DEBUGP(DL1C,
	                "Sending to l23 - L1CTL_TCH_MODE_CONF (tch_mode: %u, audio_mode: %u)\n", tch_mode,
	                audio_mode);
	l1ctl_sap_tx_to_l23(msg);
}
