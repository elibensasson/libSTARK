include ../../../flags.mk

WHICH				:= $(shell which which)
MKDIR				:= $(shell $(WHICH) mkdir)
DIRNAME				:= $(shell $(WHICH) dirname)

SRCDIR=.

CFLAGS+=
CPPFLAGS+=
INCFLAGS=-I./..  -I$(ALGEBRAINC) -I$(FFTINC) -I$(FFTINC)/.. -I$(FFTINC)/src
TARGET=$(BLDDIR)/libgadgetlib.a

SRCS=					\
	common_use.cpp  	\
	constraint.cpp  	\
	gadget.cpp  		\
	infrastructure.cpp  \
	protoboard.cpp

OBJS=$(addprefix $(BLDDIR)/, $(SRCS:.cpp=.o))

$(BLDDIR)/%.o: %.cpp
#	@echo 'Building file: $@ ($<)'
	@$(MKDIR) -p $(shell $(DIRNAME) $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCFLAGS) -c -o "$@" "$<"

all: $(TARGET)

clean:
	$(RM) -f $(TARGET) $(OBJS) $(DEPS)

$(TARGET): $(OBJS)
#	@echo 'Building target: $@'
#	@echo 'Invoking: GCC Archiver'
	ar -r  "$@" $(OBJS) $(LIBS)
#	@echo 'Finished building target: $@'
#	@echo ' '
