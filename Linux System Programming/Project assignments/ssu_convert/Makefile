TARGET   = ssu_convert


CC      ?= gcc
CFLAGS   = -W -Wall -std=gnu11
LDFLAGS  = -W -Wall -std=gnu11

SRCDIR   = src
OBJDIR   = obj
BINDIR   = .

SOURCES  := $(wildcard $(SRCDIR)/*.c)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

$(BINDIR)/$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(OBJECTS) $(BINDIR)/$(TARGET)
