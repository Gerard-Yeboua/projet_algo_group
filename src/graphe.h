/**
 * @file graphe.h
 * @brief Module 1 - Modélisation du réseau sous forme de graphe pondéré
 * @complexity Voir chaque fonction
 */

#ifndef GRAPHE_H
#define GRAPHE_H

#include "interfaces.h"

/* --- Création / Destruction --- */

/**
 * @brief Crée un graphe vide alloué dynamiquement
 * @param capacite_max Nombre maximum de nœuds
 * @param est_oriente  1 = orienté, 0 = non orienté
 * @return Pointeur vers le graphe, NULL si échec
 * @complexity O(V²) pour l'allocation des matrices
 */
Graphe* creer_graphe(int capacite_max, int est_oriente);

/**
 * @brief Libère toute la mémoire allouée par le graphe
 * @complexity O(V + E)
 */
void detruire_graphe(Graphe* g);

/* --- Manipulation des nœuds et arêtes --- */

/**
 * @brief Ajoute un nœud au graphe
 * @return 0 = succès, -1 = erreur (capacité pleine)
 * @complexity O(1)
 */
int ajouter_noeud(Graphe* g, int id, const char* nom);

/**
 * @brief Supprime un nœud et toutes ses arêtes
 * @complexity O(V + E)
 */
int supprimer_noeud(Graphe* g, int id);

/**
 * @brief Ajoute une arête entre deux nœuds
 * @return 0 = succès, -1 = erreur
 * @complexity O(1) pour la liste, O(1) pour la matrice
 */
int ajouter_arete(Graphe* g, int src, int dest,
                  float latence, float bande_passante,
                  float cout, int securite);

/**
 * @brief Supprime une arête
 * @complexity O(degré du nœud source)
 */
int supprimer_arete(Graphe* g, int src, int dest);

/* --- Chargement / Sauvegarde --- */

/**
 * @brief Charge un graphe depuis un fichier texte
 * @details Format attendu : voir data/format_reseau.txt
 * @complexity O(V + E)
 */
Graphe* charger_graphe(const char* nom_fichier);

/**
 * @brief Sauvegarde le graphe dans un fichier texte
 * @complexity O(V + E)
 */
int sauvegarder_graphe(const Graphe* g, const char* nom_fichier);

/* --- Affichage et utilitaires --- */

void afficher_graphe(const Graphe* g);
void afficher_matrice(const Graphe* g);
int  graphe_est_connexe(const Graphe* g);
int  noeud_existe(const Graphe* g, int id);
Arete* trouver_arete(const Graphe* g, int src, int dest);

#endif /* GRAPHE_H */
