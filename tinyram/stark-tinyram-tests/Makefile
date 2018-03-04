include ../../flags.mk

WHICH				:= $(shell which which)
MKDIR				:= $(shell $(WHICH) mkdir)
DIRNAME				:= $(shell $(WHICH) dirname)

INCFLAGS=-I$(TINYRAMINC_DIR) -I$(GADGET3INC) -I$(LIBSTARKINC) -I$(ALGEBRAINC) -I$(FFTINC)
LIBS=						\
	gtest					\
	gadgetlib				\
	stark					\
	algebralib				\
	FFT

LIBFLAGS=$(addprefix -l, $(LIBS))
LNKFLAGS=-L"$(ALGEBRALNKDIR)" -L"$(FFTLIBLNKDIR)" -L"$(GADGET3LNKDIR)" -L"$(LIBSTARKLINKDIR)" -lgomp

SRCS:=	$(shell ls *.cpp)
OBJS=$(addprefix $(BLDDIR)/, $(SRCS:.cpp=.o))
TINYRAMOBJS := $(shell find $(TINYRAMOBJ_DIR) -name '*.o' ! -name 'main.o')

TARGET=$(BLDDIR)/stark-tinyram-tests
all: $(TARGET)

$(TARGET): $(OBJS)
#	@echo 'Building target: $@'
#	@echo 'Invoking: GCC Linker'
	$(CC) -o "$@" $(OBJS) $(TINYRAMOBJS) -fopenmp $(LNKFLAGS) $(LIBFLAGS)
#	@echo 'Finished building target: $@'

$(BLDDIR)/%.o: %.cpp
#	@echo 'Building file: $@ ($<)'
	@$(MKDIR) -p $(shell $(DIRNAME) $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCFLAGS) -c -o "$@" "$<"

clean:
	rm -f $(TARGET) $(OBJS) $(DEPS)
