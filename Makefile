# ============================================================
# Makefile - NetFlow Optimizer & Security Analyzer
# UVCI ALC2101 - 2025-2026
# ============================================================

CC      = gcc
CFLAGS  = -Wall -Wextra -std=gnu11 -Isrc
DBFLAGS = -g -DDEBUG -fsanitize=address
TARGET  = netflow
SRCDIR  = src
TESTDIR = tests
DATADIR = data

# Sources et objets (ordre de la structure imposee par le Dr)
SRCS = $(SRCDIR)/graphe.c        \
       $(SRCDIR)/liste_chainee.c  \
       $(SRCDIR)/dijkstra.c       \
       $(SRCDIR)/backtracking.c   \
       $(SRCDIR)/securite.c       \
       $(SRCDIR)/utils.c          \
       $(SRCDIR)/main.c

OBJS = $(SRCS:.c=.o)

# ============================================================
# Cibles principales
# ============================================================

all: $(TARGET)
	@echo "Compilation reussie : ./$(TARGET)"

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Mode debug avec symboles de debogage
debug: CFLAGS += $(DBFLAGS)
debug: clean $(TARGET)
	@echo "Mode DEBUG active"

# Tests unitaires
test: $(TESTDIR)/tests_unitaires.c $(filter-out $(SRCDIR)/main.c, $(SRCS))
	$(CC) $(CFLAGS) -o test_runner $^
	./test_runner
	@echo "Tests executes"

# Nettoyage
clean:
	rm -f $(SRCDIR)/*.o $(TARGET) test_runner
	@echo "Nettoyage effectue"

# Verification memoire (Linux seulement)
valgrind: $(TARGET)
	valgrind --leak-check=full --track-origins=yes \
	         ./$(TARGET) $(DATADIR)/reseau_test1.txt

help:
	@echo "make         : Compilation normale"
	@echo "make debug   : Compilation avec debogage"
	@echo "make clean   : Suppression fichiers compiles"
	@echo "make test    : Execution des tests unitaires"
	@echo "make valgrind: Verification fuites memoire (Linux)"

.PHONY: all debug test clean valgrind help
