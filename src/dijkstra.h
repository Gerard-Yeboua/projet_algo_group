/**
 * @file dijkstra.h
 * @brief Module 2 - Algorithmes de routage optimal (Dijkstra, Bellman-Ford)
 * @details Le backtracking et K-chemins sont dans backtracking.h
 */

#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "interfaces.h"
#include "graphe.h"

/* Dijkstra - O((V+E) log V) */
Chemin* dijkstra(const Graphe* g, int source, int destination);
void    dijkstra_tout(const Graphe* g, int source, float* dist, int* pred);

/* Bellman-Ford - O(V*E) */
Chemin* bellman_ford(const Graphe* g, int source, int destination,
                     int* a_cycle_negatif);

/* Utilitaires chemins */
Chemin* creer_chemin(void);
void    ajouter_noeud_chemin(Chemin* c, int id_noeud);
void    detruire_chemin(Chemin* c);
void    afficher_chemin(const Chemin* c, const Graphe* g);
int     chemin_contient(const Chemin* c, int id_noeud);
Chemin* copier_chemin(const Chemin* c);

#endif /* DIJKSTRA_H */
