WHICH				:= $(shell which which)
PWD					:= $(shell $(WHICH) pwd)

# 
WD						:= $(shell $(PWD))
BUILD_DIR				:= bin
EXE_DIR					:= $(WD)
BLDDIR					:= $(WD)/$(BUILD_DIR)
GADGETLIB3_SRC_DIR		:= tinyram/gadgetlib/gadgetlib
FFTLIB_SRC_DIR			:= algebra/FFT/src

ALGEBRALIB_DIR			:= $(WD)/algebra/algebralib
LIBSTARK_DIR			:= $(WD)/libstark
GADGETLIB3_DIR			:= $(WD)/$(GADGETLIB3_SRC_DIR)
TINYRAM_DIR				:= $(WD)/tinyram/stark-tinyram
DPM_DIR					:= $(WD)/starkdpm
FFTLIB_DIR				:= $(WD)/algebra/FFT

.PHONY: \
		libstark libstark-clean \
		stark-dpm stark-dpm-clean \
		stark-tinyram stark-tinyram-clean \
		fft fft-clean \
		algebralib algebralib-clean \
		gadgetlib gadgetlib-clean \
		clean

default: stark-dpm stark-tinyram

libstark:
	$(MAKE) -C $(LIBSTARK_DIR) \
	BLDDIR=$(BLDDIR)/libstark \
	FFTINC=$(FFTLIB_DIR)/src		\
	ALGEBRAINC=$(ALGEBRALIB_DIR)/headers

libstark-clean:
	$(MAKE) clean -C $(LIBSTARK_DIR) BLDDIR=$(BLDDIR)/libstark

stark-dpm: fft algebralib libstark
	$(MAKE) -C $(DPM_DIR) \
		BLDDIR=$(BLDDIR)/starkdpm                       \
		EXEDIR=$(EXE_DIR) \
		FFTINC=$(FFTLIB_DIR)/src \
		FFTLIBLNKDIR=$(BLDDIR)/fft					\
		ALGEBRAINC=$(ALGEBRALIB_DIR)/headers \
		ALGEBRALNKDIR=$(BLDDIR)/algebralib \
		LIBSTARKINC=$(LIBSTARK_DIR)/src \
		LIBSTARKLINKDIR=$(BLDDIR)/libstark 

stark-dpm-clean:
	$(MAKE) clean -C $(DPM_DIR) \
		BLDDIR=$(BLDDIR)/starkdpm \
		EXEDIR=$(EXE_DIR)

stark-tinyram: gadgetlib fft algebralib libstark
	$(MAKE) -C $(TINYRAM_DIR) \
		BLDDIR=$(BLDDIR)/stark-tinyram                       \
		EXEDIR=$(EXE_DIR) \
		FFTINC=$(FFTLIB_DIR)/src \
		FFTLIBLNKDIR=$(BLDDIR)/fft					\
		ALGEBRAINC=$(ALGEBRALIB_DIR)/headers \
		ALGEBRALNKDIR=$(BLDDIR)/algebralib \
		LIBSTARKINC=$(LIBSTARK_DIR)/src \
		LIBSTARKLINKDIR=$(BLDDIR)/libstark \
		GADGET3INC=$(GADGETLIB3_DIR)/../.			\
		GADGET3LNKDIR=$(BLDDIR)/gadgetlib		

stark-tinyram-clean:
	$(MAKE) clean -C $(TINYRAM_DIR) \
		BLDDIR=$(BLDDIR)/stark-tinyram \
		EXEDIR=$(EXE_DIR)

fft:
	$(MAKE) -C $(FFTLIB_DIR)  BLDDIR=$(BLDDIR)/fft

fft-clean:
	$(MAKE) clean -C $(FFTLIB_DIR)  BLDDIR=$(BLDDIR)/fft

algebralib:
	$(MAKE) -C $(ALGEBRALIB_DIR) \
	BLDDIR=$(BLDDIR)/algebralib FFTINC=$(FFTLIB_DIR)/src

algebralib-clean:
	$(MAKE) clean -C $(ALGEBRALIB_DIR) BLDDIR=$(BLDDIR)/algebralib

gadgetlib: 
	$(MAKE) -C $(GADGETLIB3_DIR)						\
	GADGETINC=$(GADGETLIB3_DIR)/. \
	ALGEBRAINC=$(ALGEBRALIB_DIR)/headers 				\
	ALGEBRALIBLINKDIR=$(BLDDIR)/algebralib				\
	FFTLIBLNKDIR=$(BLDDIR)/fft	\
	FFTINC=$(FFTLIB_DIR) FFTLIBLNKDIR=$(BLDDIR)/fft		\
	BLDDIR=$(BLDDIR)/gadgetlib

gadgetlib-clean:
	$(MAKE) -C $(GADGETLIB3_DIR) BLDDIR=$(BLDDIR)/gadgetlib clean

clean: gadgetlib-clean stark-dpm-clean stark-tinyram-clean libstark-clean fft-clean algebralib-clean
	$(RM) -r $(BLDDIR)
