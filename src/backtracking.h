/**
 * @file backtracking.h
 * @brief Module Backtracking - Recherche de chemins avec contraintes multiples
 * @details Implemente la recherche exhaustive avec elagage (pruning) pour
 *          trouver des chemins satisfaisant des contraintes multiples simultanees.
 *
 * Technique : Backtracking avec elagage des branches non-prometteuses
 * Complexite : O(b^d) dans le pire cas, reduit en pratique par l'elagage
 *   - b = facteur de branchement (degre moyen des noeuds)
 *   - d = profondeur maximale (longueur du chemin)
 *
 * @authors Groupe NetFlow - UVCI ALC2101 2025-2026
 */

#ifndef BACKTRACKING_H
#define BACKTRACKING_H

#include "interfaces.h"
#include "graphe.h"
#include "dijkstra.h"

/* ============================================================
 *  RECHERCHE DE CHEMIN AVEC CONTRAINTES (BACKTRACKING)
 * ============================================================ */

/**
 * @brief Trouve le meilleur chemin respectant toutes les contraintes
 *
 * Critere d'optimisation : minimisation de la latence totale
 * Contraintes simultanees :
 *   - Bande passante >= bw_min_requise sur chaque arete
 *   - Cout total <= cout_max_autorise
 *   - Securite >= securite_min sur chaque arete
 *   - Passage obligatoire par certains noeuds
 *   - Exclusion de certains noeuds
 *
 * Strategies d'elagage implementees :
 *   1. Depassement du budget cout -> branche abandonnee
 *   2. Arete avec BW insuffisante -> noeud ignore
 *   3. Arete non securisee -> noeud ignore
 *   4. Noeud exclu -> noeud ignore
 *   5. Noeud deja visite -> evite les cycles
 *   6. Latence actuelle >= meilleure connue -> branche abandonnee
 *
 * @param g           Graphe a analyser
 * @param source      ID du noeud source
 * @param destination ID du noeud destination
 * @param c           Contraintes a respecter
 * @return Meilleur chemin valide, NULL si aucun chemin ne satisfait les contraintes
 * @complexity O(b^d) pire cas, O(d) meilleur cas avec elagage efficace
 */
Chemin* chemin_contraint_backtracking(const Graphe* g, int source,
                                      int destination,
                                      const Contraintes* c);

/* ============================================================
 *  K PLUS COURTS CHEMINS (DIVISER POUR REGNER - Algorithme de Yen)
 * ============================================================ */

/**
 * @brief Trouve les K meilleurs chemins alternatifs sans boucle
 *
 * Principe diviser pour regner :
 *   - Diviser : identifier le noeud d'ecart (spur node)
 *   - Regner  : trouver le meilleur chemin depuis ce noeud
 *   - Combiner : concatener chemin racine + chemin ecart
 *
 * @param g           Graphe a analyser
 * @param source      ID du noeud source
 * @param destination ID du noeud destination
 * @param k           Nombre de chemins a trouver (k >= 1)
 * @return Liste chainee de K chemins (du meilleur au moins bon)
 * @complexity O(K * V * (V + E) log V)
 */
Chemin* k_plus_courts_chemins(const Graphe* g, int source,
                               int destination, int k);

/**
 * @brief Libere une liste de chemins (resultat de k_plus_courts_chemins)
 */
void detruire_liste_chemins(Chemin* liste);

/* ============================================================
 *  STATISTIQUES D'ELAGAGE (pour le rapport de complexite)
 * ============================================================ */

/**
 * @brief Structure pour mesurer l'efficacite de l'elagage
 */
typedef struct StatsBacktrack {
    int noeuds_explores;    /**< Nombre total de noeuds visites */
    int branches_elaguees;  /**< Branches abandonnees par elagage */
    int chemins_trouves;    /**< Chemins valides trouves */
    double temps_ms;        /**< Temps d'execution en millisecondes */
} StatsBacktrack;

/**
 * @brief Version avec statistiques d'elagage (pour le rapport)
 */
Chemin* backtracking_avec_stats(const Graphe* g, int source,
                                int destination,
                                const Contraintes* c,
                                StatsBacktrack* stats);

void afficher_stats_backtrack(const StatsBacktrack* stats);

#endif /* BACKTRACKING_H */
