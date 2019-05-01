include ../../flags.mk

WHICH				:= $(shell which which)
MKDIR				:= $(shell $(WHICH) mkdir)
DIRNAME				:= $(shell $(WHICH) dirname)

CFLAGS+=
CPPFLAGS+=
INCFLAGS=-I$(FFTINC) -Iheaders
TARGET=$(BLDDIR)/libalgebralib.a

SRCS:= $(shell ls src/*.cpp)
OBJS=$(addprefix $(BLDDIR)/, $(SRCS:.cpp=.o))

ifeq ($(AR),)
AR = ar -r
endif

$(BLDDIR)/%.o: %.cpp
#	@echo 'Building file: $@ ($<)'
	@$(MKDIR) -p $(shell $(DIRNAME) $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCFLAGS) -c -o "$@" "$<"

all: $(TARGET)

clean:
	$(RM) -f $(TARGET) $(OBJS) $(DEPS)

$(TARGET): $(OBJS)
#	@echo 'Building target: $@'
	$(AR) "$@" $(OBJS)
