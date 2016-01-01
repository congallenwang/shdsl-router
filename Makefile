#CFLAGS =$(IFX_CFLAGS) -fPIC -Wall
CC = /home/alan/disk2/TPLink/V5/UGW-6.1.1/staging_dir/toolchain-mips_r2_gcc-4.8-linaro_uClibc-0.9.33.2/bin/mips-openwrt-linux-uclibc-gcc
CFLAGS= -I. \
	-I./include \
	-I/home/alan/disk2/TPLink/V5/UGW-6.1.1/staging_dir/toolchain-mips_r2_gcc-4.8-linaro_uClibc-0.9.33.2/usr/include \
	-I/home/alan/disk2/TPLink/V5/UGW-6.1.1/staging_dir/toolchain-mips_r2_gcc-4.8-linaro_uClibc-0.9.33.2/include \
	-I/home/alan/disk2/TPLink/V5/UGW-6.1.1/build_dir/linux-lantiq_xrx200_grx288_gw_he_ethwan_lte_sample/linux-3.10.12/include/ \
	-I/home/alan/disk2/TPLink/V5/UGW-6.1.1/staging_dir/target-mips_r2_uClibc-0.9.33.2_vrx288_gw_he_vdsl_lte/usr/include/ifxos \
	-DLINUX
SRC = $(wildcard *.c)
DEPEND = $(patsubst %.c, %.d, $(SRC))

CFLAGS += -I. -I$(MTD_UTIL_DIR)/include $(IFX_CFLAGS) -DBUILD_FROM_IFX_UTILITIES
LDFLAGS += $(IFX_LDFLAGS)
OBJS = main.o sys_noOSLib.o drv_pef24628e_common.o drv_pef24628e_noOS.o dummy.o

TARGET_BIN += shdsl 

all: $(TARGET_BIN) 

%.o: %.c
	$(CC) $(CFLAGS) -c  $< -o $@

%.d: %.c
	$(CC) -MM $(CFLAGS) $< > $@
	$(CC) -MM $(CFLAGS) $< | sed s/\\.o/.d/ >> $@

shdsl: $(OBJS) 
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $@  

clean:
	-rm -f *.o *.d shdsl 

include $(DEPEND)
