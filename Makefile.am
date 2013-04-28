OSNAME := $(shell uname)
LDLIBS=

ifeq ($(OSNAME),Darwin)
    LDLIBS += -liconv -lgcc_eh
endif

