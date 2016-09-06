AM_CFLAGS=-Wall $(LIBOSMOCORE_CFLAGS)
AM_CPPFLAGS = -I$(top_srcdir)/include

sbin_PROGRAMS = virtphy
virtphy_SOURCES = virtphy.c l23_api.c l1ctl_sock.c virtual_um.c logging.c
virtphy_LDADD = $(LIBOSMOCORE_LIBS)

# debug output
#all:
#	$(info $$AM_CPPFLAGS is [${AM_CPPFLAGS}])
