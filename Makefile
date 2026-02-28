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

# Sources et objets
SRCS = $(SRCDIR)/graphe.c \
       $(SRCDIR)/dijkstra.c \
       $(SRCDIR)/securite.c \
       $(SRCDIR)/liste_chainee.c \
       $(SRCDIR)/main.c

OBJS = $(SRCS:.c=.o)

# ============================================================
# Cibles principales
# ============================================================

all: $(TARGET)
	@echo "✓ Compilation reussie : ./$(TARGET)"

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Mode debug avec AddressSanitizer
debug: CFLAGS += $(DBFLAGS)
debug: clean $(TARGET)
	@echo "✓ Mode DEBUG active"

# Tests unitaires
test: $(TESTDIR)/tests_unitaires.c $(filter-out $(SRCDIR)/main.c, $(SRCS))
	$(CC) $(CFLAGS) -o test_runner $^
	./test_runner
	@echo "✓ Tests exécutés"

# Nettoyage
clean:
	rm -f $(SRCDIR)/*.o $(TARGET) test_runner
	@echo "✓ Nettoyage effectue"

# Vérification mémoire avec Valgrind
valgrind: $(TARGET)
	valgrind --leak-check=full --track-origins=yes --error-exitcode=1 \
	         ./$(TARGET) $(DATADIR)/reseau_test1.txt

# Aide
help:
	@echo "Commandes disponibles :"
	@echo "  make         → Compilation normale"
	@echo "  make debug   → Compilation avec débogage + AddressSanitizer"
	@echo "  make test    → Exécution des tests unitaires"
	@echo "  make clean   → Suppression des fichiers compilés"
	@echo "  make valgrind→ Vérification des fuites mémoire"

.PHONY: all debug test clean valgrind help
