SHELL           = /bin/bash
CF_ALL	        = -g -O2  -MD -I. -ldl
LDFLAGS         = 
LDLIBS          =
CC		= gcc

.SUFFIXES:
.SUFFIXES: .c .o .tap .t 

COMP            = $(CC) $(CF_ALL) $(CF_TGT) -o $@ -c $<
LINK            = $(CC) $(CF_ALL) $(LF_TGT) -o $@ $^ $(LL_TGT) $(LL_DIR)
COMPLINK        = $(CC) $(CF_ALL) $(CF_TGT) $(LF_DIR) $(LF_TGT) -o $@ $^ $(LL_TGT) 
ARCH		= $(AR) $(ARFLAGS) $@ $^


all:

AUTOCONF_LEFTOVERS=autom4te.cache autoscan.log config.h config.h.in~ config.log config.status */Makefile

distclean:
	rm -f $(AUTOCONF_LEFTOVERS) Makefile


Makefile: Makefile.in configure.ac 
	./config.status

check: 	$(CHECK)

all:	targets

include Rules.mk
