CC       = gcc
CFLAGS   = -std=gnu11 -W -Wall -pthread

SOURCES  := $(wildcard ./*.c)
TARGETS  := $(SOURCES:./%.c=./%)

all: $(TARGETS)

$(TARGETS): ./% : ./%.c
	@ $(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	@ rm -f $(TARGETS)

.PHONY: format
format:
	@ clang-format -style=file -i **/*.c -verbose

.PHONY: fmt
fmt: format

.PHONY: test
test:
	make docker
	make docker-run

.PHONY: docker
docker:
	make clean
	docker build -t lsp .

.PHONY: docker-run
docker-run:
	docker run -i -t --rm --name="lsp" --hostname="lsp" lsp
