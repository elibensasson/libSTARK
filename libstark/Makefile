include ../flags.mk

INCFLAGS=-Isrc -I$(ALGEBRAINC) -I$(FFTINC)

SRCDIR  = src
SRCEXT  = cpp
OBJDIR  = $(BLDDIR)/obj

SRCS    := $(shell find $(SRCDIR) -name '*.$(SRCEXT)')
SRCDIRS := $(shell find . -name '*.$(SRCEXT)' -exec dirname {} \; | uniq)
OBJS    := $(patsubst %.$(SRCEXT),$(OBJDIR)/%.o,$(SRCS))

TARGET=$(BLDDIR)/libstark.a
all: $(TARGET)

$(TARGET): buildrepo $(OBJS)
#	@echo 'Building target: $@'
#	@echo 'Invoking: GCC Linker'
	ar -r  "$@" $(OBJS) $(LIBS)
#	@echo 'Finished building target: $@'

$(OBJDIR)/%.o: %.$(SRCEXT)
#	@echo "$(CC) $(CFLAGS) $(CPPFLAGS) $(INCFLAGS) -c -o "$@" "$<""
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCFLAGS) -c -o "$@" "$<"

clean:
	$(RM) -f $(TARGET) $(OBJS) $(DEPS)

buildrepo:
	$(call make-repo)

define make-repo
for dir in $(SRCDIRS); \
	do \
	mkdir -p $(OBJDIR)/$$dir; \
	done
endef
