/**
 * @file securite.h
 * @brief Module 3 - Détection d'anomalies et analyse de sécurité
 */

#ifndef SECURITE_H
#define SECURITE_H

#include "interfaces.h"
#include "graphe.h"

/* --- Parcours --- */

/**
 * @brief Parcours en profondeur (DFS)
 * @param g       Graphe
 * @param depart  Nœud de départ
 * @param ordre   Tableau de sortie : ordre de visite (alloué par appelant)
 * @complexity O(V + E)
 */
void dfs(const Graphe* g, int depart, int* ordre, int* nb_visites);

/**
 * @brief Parcours en largeur (BFS)
 * @complexity O(V + E)
 */
void bfs(const Graphe* g, int depart, int* ordre, int* nb_visites);

/* --- Détection de cycles --- */

/**
 * @brief Détecte si le graphe contient un cycle
 * @return 1 si cycle détecté, 0 sinon
 * @complexity O(V + E)
 */
int detecter_cycle(const Graphe* g);

/* --- Points d'articulation & Ponts --- */

/**
 * @brief Trouve tous les points d'articulation (nœuds critiques)
 * @param g   Graphe
 * @param res Structure résultat à remplir
 * @complexity O(V + E) - DFS modifié avec tableau low[]
 */
void trouver_points_articulation(const Graphe* g, ResultatSecurite* res);

/**
 * @brief Trouve tous les ponts (arêtes critiques)
 * @complexity O(V + E)
 */
void trouver_ponts(const Graphe* g, ResultatSecurite* res);

/* --- Composantes Fortement Connexes (Tarjan) --- */

/**
 * @brief Algorithme de Tarjan pour les SCC
 * @param g   Graphe orienté
 * @param res Structure résultat à remplir
 * @complexity O(V + E)
 */
void tarjan_scc(const Graphe* g, ResultatSecurite* res);

/* --- Analyse globale --- */

/**
 * @brief Lance une analyse complète de sécurité du graphe
 * @return Structure ResultatSecurite allouée (à libérer par l'appelant)
 */
ResultatSecurite* analyser_securite(const Graphe* g);

void afficher_resultats_securite(const ResultatSecurite* res, const Graphe* g);
void detruire_resultat_securite(ResultatSecurite* res);

#endif /* SECURITE_H */
