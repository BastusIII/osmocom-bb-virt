AM_CFLAGS=-Wall $(LIBOSMOCORE_CFLAGS)
AM_CPPFLAGS = -I$(top_srcdir)/include

sbin_PROGRAMS = virtphy
virtphy_SOURCES = virtphy.c l1ctl_sock.c virtual_um.c l1ctl_sap.c l1ctl_rx_routines.c l1ctl_tx_routines.c gsmtapl1_if.c logging.c osmo_mcast_sock.c
virtphy_LDADD = $(LIBOSMOCORE_LIBS)

# debug output
#all:
#	$(info $$AM_CPPFLAGS is [${AM_CPPFLAGS}])
