SRC_DIR  := src
BIN_DIR  := scripts
SRCS     := $(wildcard $(SRC_DIR)/*.c)
BINS     := $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%,$(SRCS))
CMDS     := $(patsubst $(SRC_DIR)/%.c,%,$(SRCS))
CC       := gcc
CFLAGS   := -nostartfiles -Os -s -fno-ident -fno-asynchronous-unwind-tables -fno-unwind-tables -fno-stack-protector -ffunction-sections -fdata-sections -Wl,--gc-sections,-z,norelro,--build-id=none
POST     := strip --remove-section=.note* --remove-section=.comment --remove-section=.eh_frame* --remove-section=.sframe

.PHONY: all clean $(CMDS)

all: $(BINS)

# One phony target per command, e.g. make color-scheme
$(CMDS): %: $(BIN_DIR)/%

$(BIN_DIR)/%: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $<
	-$(POST) $@ 2>/dev/null

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -f $(BINS)
