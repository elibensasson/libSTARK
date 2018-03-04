include ../../flags.mk

WHICH				:= $(shell which which)
MKDIR				:= $(shell $(WHICH) mkdir)
DIRNAME				:= $(shell $(WHICH) dirname)

SRCDIR=.

CFLAGS+=
CPPFLAGS+=
INCFLAGS=-I$(ALGEBRAINC) -I$(FFTINC)
LIBS=						\
	algebralib				\
	gtest					\
	FFT

LIBFLAGS=$(addprefix -l, $(LIBS))
LNKFLAGS=-L"$(ALGEBRALNKDIR)" -L"$(FFTLIBLNKDIR)" -lgomp

TARGET=$(BLDDIR)/algebralib_tests

SRCS:=	$(shell ls *.cpp)
OBJS=$(addprefix $(BLDDIR)/, $(SRCS:.cpp=.o))

$(BLDDIR)/%.o: %.cpp
#	@echo 'Building file: $@ ($<)'
	@$(MKDIR) -p $(shell $(DIRNAME) $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCFLAGS) -c -o "$@" "$<"

all: $(TARGET)

clean:
	$(RM) -f $(TARGET) $(OBJS) $(DEPS)

$(TARGET): $(OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC Linker'
	$(CC) -o "$@" $(OBJS) -fopenmp $(LNKFLAGS) $(LIBFLAGS)
	@echo 'Finished building target: $@'
