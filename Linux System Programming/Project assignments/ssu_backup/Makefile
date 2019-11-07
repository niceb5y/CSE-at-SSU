TARGET   = ssu_backup


CC      ?= gcc
CFLAGS   = -W -Wall -std=gnu11 -pthread
LDFLAGS  = -W -Wall -std=gnu11 -lpthread

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
