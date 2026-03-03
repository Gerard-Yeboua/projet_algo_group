/**
 * @file utils.h
 * @brief Structures utilitaires partagees : Pile, File simple, affichage
 * @details Ce module fournit les structures de base utilisees par
 *          tous les autres modules (DFS, BFS, backtracking, Dijkstra).
 *
 * @authors Groupe NetFlow - UVCI ALC2101 2025-2026
 */

#ifndef UTILS_H
#define UTILS_H

#include "interfaces.h"

/* ============================================================
 *  PILE (LIFO) - utilisee par DFS, backtracking, Tarjan
 * ============================================================ */

/**
 * @brief Cree une pile vide allouee dynamiquement
 * @return Pointeur vers la pile, NULL si echec
 * @complexity O(1)
 */
Pile* creer_pile(void);

/**
 * @brief Libere toute la memoire de la pile
 * @complexity O(n)
 */
void detruire_pile(Pile* p);

/**
 * @brief Empile une valeur entiere
 * @complexity O(1)
 */
void empiler(Pile* p, int valeur);

/**
 * @brief Depile et retourne la valeur au sommet
 * @return Valeur depilee, -1 si pile vide
 * @complexity O(1)
 */
int depiler(Pile* p);

/**
 * @brief Consulte le sommet sans depiler
 * @return Valeur au sommet, -1 si pile vide
 * @complexity O(1)
 */
int pile_sommet(const Pile* p);

/**
 * @brief Teste si la pile est vide
 * @complexity O(1)
 */
int pile_vide(const Pile* p);

/* ============================================================
 *  FILE SIMPLE (FIFO) - utilisee par BFS
 * ============================================================ */

/**
 * @brief Cree une file simple vide
 * @complexity O(1)
 */
FileSimple* creer_file_simple(void);

/**
 * @brief Libere toute la memoire de la file
 * @complexity O(n)
 */
void detruire_file_simple(FileSimple* f);

/**
 * @brief Enfile une valeur entiere en queue
 * @complexity O(1)
 */
void enfiler_simple(FileSimple* f, int valeur);

/**
 * @brief Defile et retourne la valeur en tete
 * @return Valeur defiilee, -1 si file vide
 * @complexity O(1)
 */
int defiler_simple(FileSimple* f);

/**
 * @brief Teste si la file est vide
 * @complexity O(1)
 */
int file_simple_vide(const FileSimple* f);

/* ============================================================
 *  UTILITAIRES D'AFFICHAGE ET MESURE
 * ============================================================ */

/**
 * @brief Affiche un separateur de section
 */
void afficher_separateur(const char* titre);

/**
 * @brief Retourne le temps courant en millisecondes (pour benchmarks)
 */
double temps_ms(void);

/**
 * @brief Genere un entier aleatoire dans [min, max]
 */
int rand_entre(int min, int max);

/**
 * @brief Genere un float aleatoire dans [min, max]
 */
float rand_float(float min, float max);

#endif /* UTILS_H */
