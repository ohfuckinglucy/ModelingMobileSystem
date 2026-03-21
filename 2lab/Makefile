SRCDIR := src
INCDIR := include
BINDIR := bin

FUNC_SRC := $(SRCDIR)/bits_operation.cpp $(SRCDIR)/error_detector.cpp

BINS := $(BINDIR)/main.elf

CC := g++
CFLAGS := -I$(INCDIR) -Wall -Wextra

.PHONY: all clean run help

all: $(BINDIR) $(BINS)

$(BINDIR):
	@mkdir -p $(BINDIR)

$(BINDIR)/main.elf: $(SRCDIR)/main.cpp $(FUNC_SRC) $(INCDIR)/header.hpp
	@$(CC) $(CFLAGS) $(SRCDIR)/main.cpp $(FUNC_SRC) -o $@

clean:
	@rm -rf $(BINDIR)

run: all
	@./$(BINDIR)/main.elf

help:
	@echo "Доступные команды:"
	@echo "  make        - компиляция всех программ в папку bin/"
	@echo "  make clean  - удаление папки bin/"
	@echo "  make run    - показать инструкцию по запуску"
	@echo "  make help   - показать эту справку"