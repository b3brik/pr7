CC = gcc
CFLAGS = -Wall -Wextra -O2
GEN_SCRIPT = ./gen_structure.sh

TARGET_BASIC = z1
TARGET_IGNORE = z2
TARGET_MMAP = z3

SRC_BASIC = z1.c
SRC_IGNORE = z2.c
SRC_MMAP = z3.c

.PHONY: all clean setup run

all: $(TARGET_BASIC) $(TARGET_IGNORE) $(TARGET_MMAP)

$(TARGET_BASIC): $(SRC_BASIC)
	$(CC) $(CFLAGS) -o $@ $<

$(TARGET_IGNORE): $(SRC_IGNORE)
	$(CC) $(CFLAGS) -o $@ $<

$(TARGET_MMAP): $(SRC_MMAP)
	$(CC) $(CFLAGS) -o $@ $<

setup:
	@$(GEN_SCRIPT)

run: all setup
	@echo "[search_basic] поиск слова"
	./$(TARGET_BASIC) test
	@echo "[search_ignore] поиск слова с -i"
	./$(TARGET_IGNORE) -i test
	@echo "[search_mmap] поиск слова через mmap"
	./$(TARGET_MMAP) -i test

clean:
	rm -f $(TARGET_BASIC) $(TARGET_IGNORE) $(TARGET_MMAP)