/**
 * @file liste_chainee.h
 * @brief Module 4 - File de priorité et structures chaînées utilitaires
 */

#ifndef LISTE_CHAINEE_H
#define LISTE_CHAINEE_H

#include "interfaces.h"

/* ============================================================
 *  FILE DE PRIORITÉ (Module 4 principal)
 * ============================================================ */

FileAttente* creer_file_attente(int capacite_max);
void         detruire_file_attente(FileAttente* f);

/**
 * @brief Insère un paquet avec priorité maintenue
 * @complexity O(n) - insertion triée
 */
int  enqueue(FileAttente* f, int id, int priorite, float taille,
             int source, int destination, float timestamp);

/**
 * @brief Extrait le paquet le plus prioritaire
 * @complexity O(1)
 */
Paquet* dequeue(FileAttente* f);

/**
 * @brief Consulte le paquet en tête sans l'extraire
 * @complexity O(1)
 */
Paquet* peek(const FileAttente* f);

int  file_vide(const FileAttente* f);
int  file_pleine(const FileAttente* f);
void afficher_file(const FileAttente* f);
void afficher_statistiques(const FileAttente* f);

/* ============================================================
 *  SIMULATION DE FLUX
 * ============================================================ */

/**
 * @brief Simule l'envoi de paquets sur un chemin et calcule les stats
 */
void simuler_flux(FileAttente* f, int nb_paquets, int source, int destination);

/* ============================================================
 *  PILE (pour DFS et backtracking)
 * ============================================================ */

Pile*  creer_pile(void);
void   detruire_pile(Pile* p);
void   empiler(Pile* p, int valeur);
int    depiler(Pile* p);
int    pile_vide(const Pile* p);
int    pile_sommet(const Pile* p);

/* ============================================================
 *  FILE SIMPLE (pour BFS)
 * ============================================================ */

FileSimple* creer_file_simple(void);
void        detruire_file_simple(FileSimple* f);
void        enfiler_simple(FileSimple* f, int valeur);
int         defiler_simple(FileSimple* f);
int         file_simple_vide(const FileSimple* f);

#endif /* LISTE_CHAINEE_H */
