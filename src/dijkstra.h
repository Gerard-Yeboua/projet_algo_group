/**
 * @file dijkstra.h
 * @brief Module 2 - Algorithmes de routage optimal
 */

#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "interfaces.h"
#include "graphe.h"

/* ============================================================
 *  ALGORITHME DE DIJKSTRA
 *  Complexité : O((V + E) log V) avec file de priorité
 * ============================================================ */

/**
 * @brief Calcule le plus court chemin (latence minimale) entre source et destination
 * @param g           Graphe à analyser
 * @param source      ID du nœud source
 * @param destination ID du nœud destination
 * @return Chemin optimal, NULL si aucun chemin n'existe
 * @complexity O((V + E) log V)
 */
Chemin* dijkstra(const Graphe* g, int source, int destination);

/**
 * @brief Calcule tous les plus courts chemins depuis une source (tableau de distances)
 * @param g       Graphe à analyser
 * @param source  ID du nœud source
 * @param dist    Tableau de distances résultat (alloué par l'appelant, taille V)
 * @param pred    Tableau de prédécesseurs résultat (alloué par l'appelant, taille V)
 * @complexity O((V + E) log V)
 */
void dijkstra_tout(const Graphe* g, int source, float* dist, int* pred);

/* ============================================================
 *  ALGORITHME DE BELLMAN-FORD
 *  Complexité : O(V × E) — Gère les poids négatifs
 * ============================================================ */

/**
 * @brief Calcule le plus court chemin avec Bellman-Ford
 * @param g           Graphe à analyser
 * @param source      ID du nœud source
 * @param destination ID du nœud destination
 * @param a_cycle_negatif Sortie : 1 si un cycle négatif est détecté
 * @return Chemin optimal, NULL si aucun chemin ou cycle négatif
 * @complexity O(V × E)
 */
Chemin* bellman_ford(const Graphe* g, int source, int destination,
                     int* a_cycle_negatif);

/* ============================================================
 *  ROUTAGE AVEC CONTRAINTES (BACKTRACKING)
 *  Complexité : O(b^d) dans le pire cas, élagage en pratique
 * ============================================================ */

/**
 * @brief Trouve un chemin respectant toutes les contraintes par backtracking
 * @param g           Graphe à analyser
 * @param source      ID du nœud source
 * @param destination ID du nœud destination
 * @param c           Contraintes à respecter
 * @return Meilleur chemin trouvé, NULL si aucun chemin valide
 */
Chemin* chemin_contraint_backtracking(const Graphe* g, int source,
                                      int destination,
                                      const Contraintes* c);

/* ============================================================
 *  K PLUS COURTS CHEMINS (DIVISER POUR RÉGNER)
 * ============================================================ */

/**
 * @brief Trouve les K meilleurs chemins alternatifs (algorithme de Yen)
 * @param g           Graphe à analyser
 * @param source      ID du nœud source
 * @param destination ID du nœud destination
 * @param k           Nombre de chemins à trouver
 * @return Liste chaînée de K chemins (du meilleur au moins bon)
 */
Chemin* k_plus_courts_chemins(const Graphe* g, int source,
                               int destination, int k);

/* ============================================================
 *  FONCTIONS UTILITAIRES CHEMINS
 * ============================================================ */

Chemin*      creer_chemin(void);
void         ajouter_noeud_chemin(Chemin* c, int id_noeud);
void         detruire_chemin(Chemin* c);
void         detruire_liste_chemins(Chemin* liste);
void         afficher_chemin(const Chemin* c, const Graphe* g);
int          chemin_contient(const Chemin* c, int id_noeud);
Chemin*      copier_chemin(const Chemin* c);

#endif /* DIJKSTRA_H */
