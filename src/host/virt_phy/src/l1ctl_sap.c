/* L1CTL SAP implementation.  */

#include <osmocom/core/linuxlist.h>
#include <osmocom/core/msgb.h>
#include <osmocom/core/utils.h>
#include <stdio.h>
#include <pthread.h>
#include <l1ctl_proto.h>
#include <netinet/in.h>

#include "l1ctl_sap.h"
#include "logging.h"

/* Buffer for incoming L1CTL messages from layer 2 */
//static struct llist_head l1ctl_rx_queue = LLIST_HEAD_INIT(l1ctl_rx_queue);
//static pthread_mutex_t l1ctl_rx_queue_mutex;

static struct virt_um_inst *_vui = NULL;
static struct l1ctl_sock_inst *_lsi = NULL;

/**
 * @brief Init the SAP.
 */
void l1ctl_sap_init(struct virt_um_inst *vui, struct l1ctl_sock_inst *lsi)
{
	_vui = vui;
	_lsi = lsi;
}

/**
 * @brief L1CTL handler called for received messages from L23.
 *
 * Enqueues the message into the rx queue.
 */
void l1ctl_sap_rx_from_l23_inst_cb(struct l1ctl_sock_inst *lsi, struct msgb *msg)
{
	if (msg) {
		DEBUGP(DL1C, "Message incoming from layer 2: %s\n",
		                osmo_hexdump(msg->data, sizeof(msg->data)));
		l1ctl_sap_handler(msg);
//		pthread_mutex_lock(&l1ctl_rx_queue_mutex);
//		msgb_enqueue(&l1ctl_rx_queue, msg);
//		pthread_mutex_unlock(&l1ctl_rx_queue_mutex);
	}
}
/**
 * @see l1ctl_sap_rx_from_l23_cb(struct l1ctl_sock_inst *lsi, struct msgb *msg).
 */
void l1ctl_sap_rx_from_l23(struct msgb *msg)
{
	l1ctl_sap_rx_from_l23_inst_cb(_lsi, msg);
}

/**
 * @brief Send a l1ctl message to layer 23.
 *
 * This will forward the message as it is to the upper layer.
 */
void l1ctl_sap_tx_to_l23_inst(struct l1ctl_sock_inst *lsi, struct msgb *msg)
{
	uint16_t *len;
	/* prepend 16bit length before sending */
	len = (uint16_t *) msgb_push(msg, sizeof(*len));
	*len = htons(msg->len - sizeof(*len));

	if(l1ctl_sock_write_msg(lsi, msg) == -1 ) {
		perror("Error writing to layer2 socket");
	}
}

/**
 * @see void l1ctl_sap_tx_to_l23(struct l1ctl_sock_inst *lsi, struct msgb *msg).
 */
void l1ctl_sap_tx_to_l23(struct msgb *msg)
{
	l1ctl_sap_tx_to_l23_inst(_lsi, msg);
}

/**
 * @brief Allocates a msgb with set l1ctl header and room for a l3 header.
 *
 * @param [in] msg_type L1CTL primitive message type set to l1ctl_hdr.
 * @return the allocated message.
 *
 * The message looks as follows:
 * # headers
 * [l1ctl_hdr]		: initialized. msgb->l1h points here
 * [spare-bytes]	: L3_MSG_HEAD bytes reserved for l3 header
 * # data
 * [spare-bytes]	: L3_MSG_DATA bytes reserved for data. msgb->tail points here. msgb->data points here.
 */
struct msgb *l1ctl_msgb_alloc(uint8_t msg_type)
{
	struct msgb *msg;
	struct l1ctl_hdr *l1h;

	msg = msgb_alloc_headroom(L3_MSG_SIZE, L3_MSG_HEAD, "l1ctl");
	if (!msg) {
		while (1) {
			puts("OOPS. Out of buffers...\n");
		}

		return NULL;
	}
	l1h = (struct l1ctl_hdr *)msgb_put(msg, sizeof(*l1h));
	l1h->msg_type = msg_type;
	l1h->flags = 0;

	msg->l1h = (uint8_t *)l1h;

	return msg;
}

/**
 * @brief Allocates a msgb with set l1ctl header and room for a l3 header and puts l1ctl_info_dl to the msgb data.
 *
 * @param [in] msg_type L1CTL primitive message type set to l1ctl_hdr.
 * @param [in] fn framenumber put into l1ctl_info_dl.
 * @param [in] snr time slot number put into l1ctl_info_dl.
 * @param [in] arfcn arfcn put into l1ctl_info_dl.
 * @return the allocated message.
 *
 * The message looks as follows:
 * # headers
 * [l1ctl_hdr]		: initialized. msgb->l1h points here
 * [spare-bytes]	: L3_MSG_HEAD bytes reserved for l3 header
 * # data
 * [l1ctl_info_dl]	: initialized with params. msgb->data points here.
 * [spare-bytes]	: L3_MSG_DATA bytes reserved for data. msgb->tail points here.
 */
struct msgb *l1ctl_create_l2_msg(int msg_type, uint32_t fn, uint16_t snr,
                                 uint16_t arfcn)
{
	struct l1ctl_info_dl *dl;
	struct msgb *msg = l1ctl_msgb_alloc(msg_type);

	dl = (struct l1ctl_info_dl *)msgb_put(msg, sizeof(*dl));
	dl->frame_nr = htonl(fn);
	dl->snr = snr;
	dl->band_arfcn = htons(arfcn);

	return msg;
}

/**
 * @brief General handler for incoming L1CTL messages from layer 2/3.
 *
 * This handler will dequeue the rx queue (if !empty) and call the specific routine for the dequeued l1ctl message.
 *
 */
void l1ctl_sap_handler(struct msgb *msg)
{
//	struct msgb *msg;
	struct l1ctl_hdr *l1h;
	unsigned long flags;
//	pthread_mutex_lock(&l1ctl_rx_queue_mutex);
//	msg = msgb_dequeue(&l1ctl_rx_queue);
//	pthread_mutex_unlock(&l1ctl_rx_queue_mutex);

	if (!msg)
		return;

	l1h = (struct l1ctl_hdr *)msg->data;

	if (sizeof(*l1h) > msg->len) {
		LOGP(DL1C, LOGL_NOTICE, "Short message. %u\n", msg->len);
		goto exit_msgbfree;
	}

	switch (l1h->msg_type) {
	case L1CTL_FBSB_REQ:
		l1ctl_rx_fbsb_req(msg);
		break;
	case L1CTL_DM_EST_REQ:
		l1ctl_rx_dm_est_req(msg);
		break;
	case L1CTL_DM_REL_REQ:
		l1ctl_rx_dm_rel_req(msg);
		break;
	case L1CTL_PARAM_REQ:
		l1ctl_rx_param_req(msg);
		break;
	case L1CTL_DM_FREQ_REQ:
		l1ctl_rx_dm_freq_req(msg);
		break;
	case L1CTL_CRYPTO_REQ:
		l1ctl_rx_crypto_req(msg);
		break;
	case L1CTL_RACH_REQ:
		l1ctl_rx_rach_req(msg);
		break;
	case L1CTL_DATA_REQ:
		l1ctl_rx_data_req(msg);
		/* we have to keep the msgb, not free it! */
		goto exit_nofree;
	case L1CTL_PM_REQ:
		l1ctl_rx_pm_req(msg);
		break;
	case L1CTL_RESET_REQ:
		l1ctl_rx_reset_req(msg);
		break;
	case L1CTL_CCCH_MODE_REQ:
		l1ctl_rx_ccch_mode_req(msg);
		break;
	case L1CTL_TCH_MODE_REQ:
		l1ctl_rx_tch_mode_req(msg);
		break;
	case L1CTL_NEIGH_PM_REQ:
		l1ctl_rx_neigh_pm_req(msg);
		break;
	case L1CTL_TRAFFIC_REQ:
		l1ctl_rx_traffic_req(msg);
		/* we have to keep the msgb, not free it! */
		goto exit_nofree;
	case L1CTL_SIM_REQ:
		l1ctl_rx_sim_req(msg);
		break;
	}

	exit_msgbfree: msgb_free(msg);
	exit_nofree: return;
}

