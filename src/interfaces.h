/**
 * @file interfaces.h
 * @brief Contrat global entre tous les modules du projet NetFlow Optimizer
 * @details Ce fichier DOIT être inclus par tous les modules.
 *          Il définit toutes les structures partagées et les signatures de fonctions.
 *
 * @authors Groupe NetFlow - UVCI ALC2101 2025-2026
 */

#ifndef INTERFACES_H
#define INTERFACES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <limits.h>

/* ============================================================
 *  CONSTANTES GLOBALES
 * ============================================================ */
#define TAILLE_MAX_NOM       50
#define INFINI               FLT_MAX
#define NON_VISITE           0
#define EN_COURS             1
#define VISITE               2
#define SECURITE_MIN         0
#define SECURITE_MAX         10
#define CAPACITE_FILE_MAX    1000

/* ============================================================
 *  MODULE 1 : STRUCTURES DU GRAPHE
 * ============================================================ */

/**
 * @brief Représente une arête dans la liste d'adjacence
 */
typedef struct Arete {
    int   destination;       /**< ID du nœud destination */
    float latence;           /**< Délai en millisecondes */
    float bande_passante;    /**< Capacité en Mbps */
    float cout;              /**< Coût monétaire */
    int   securite;          /**< Niveau de sécurité : 0 (faible) à 10 (élevé) */
    struct Arete* suivant;   /**< Prochain élément dans la liste chaînée */
} Arete;

/**
 * @brief Représente un nœud du réseau
 */
typedef struct Noeud {
    int   id;                /**< Identifiant unique */
    char  nom[TAILLE_MAX_NOM]; /**< Nom lisible (ex: "Routeur_Paris") */
    int   actif;             /**< 1 = nœud actif, 0 = désactivé */
    Arete* aretes;           /**< Tête de la liste d'adjacence */
} Noeud;

/**
 * @brief Structure principale du graphe (double représentation)
 */
typedef struct Graphe {
    int    nb_noeuds;        /**< Nombre de nœuds actuels */
    int    nb_aretes;        /**< Nombre d'arêtes actuelles */
    int    capacite_max;     /**< Capacité maximale allouée */
    int    est_oriente;      /**< 1 = orienté, 0 = non orienté */
    Noeud* noeuds;           /**< Tableau de nœuds (liste d'adjacence) */
    float** matrice_latence; /**< Matrice d'adjacence - latence */
    float** matrice_bw;      /**< Matrice d'adjacence - bande passante */
    float** matrice_cout;    /**< Matrice d'adjacence - coût */
    int**   matrice_sec;     /**< Matrice d'adjacence - sécurité */
} Graphe;

/* ============================================================
 *  MODULE 2 : STRUCTURES DE ROUTAGE
 * ============================================================ */

/**
 * @brief Nœud d'un chemin (liste chaînée du résultat)
 */
typedef struct NoeudChemin {
    int id_noeud;
    struct NoeudChemin* suivant;
} NoeudChemin;

/**
 * @brief Représente un chemin complet avec ses métriques
 */
typedef struct Chemin {
    NoeudChemin* tete;       /**< Premier nœud du chemin */
    int          longueur;   /**< Nombre de nœuds dans le chemin */
    float        latence_totale;
    float        bw_minimale;     /**< Goulot d'étranglement */
    float        cout_total;
    int          securite_min;    /**< Niveau de sécurité le plus faible */
    struct Chemin* suivant;       /**< Pour la liste des K chemins */
} Chemin;

/**
 * @brief Contraintes pour le routage avec backtracking
 */
typedef struct Contraintes {
    float bw_min_requise;    /**< Bande passante minimale garantie (Mbps) */
    float cout_max_autorise; /**< Budget maximum */
    int   securite_min;      /**< Niveau de sécurité minimum accepté */
    int*  noeuds_oblgatoires;/**< IDs des nœuds à traverser obligatoirement */
    int   nb_obligatoires;   /**< Taille du tableau précédent */
    int*  noeuds_exclus;     /**< IDs des nœuds à éviter */
    int   nb_exclus;         /**< Taille du tableau précédent */
} Contraintes;

/* ============================================================
 *  MODULE 4 : STRUCTURES DE FILES
 * ============================================================ */

/**
 * @brief Paquet réseau avec priorité
 */
typedef struct Paquet {
    int   id;
    int   priorite;          /**< Plus grand = plus prioritaire */
    float taille_Mo;
    int   source;
    int   destination;
    float timestamp_arrivee; /**< En millisecondes */
    struct Paquet* precedent;
    struct Paquet* suivant;
} Paquet;

/**
 * @brief File de priorité doublement chaînée
 */
typedef struct FileAttente {
    Paquet* tete;            /**< Paquet le plus prioritaire */
    Paquet* queue;           /**< Paquet le moins prioritaire */
    int     taille_actuelle;
    int     capacite_max;
    int     total_insere;
    int     total_perdu;
    float   temps_attente_cumule;
} FileAttente;

/* ============================================================
 *  STRUCTURES UTILITAIRES (Pile et File simples pour parcours)
 * ============================================================ */

typedef struct ElementPile {
    int valeur;
    struct ElementPile* suivant;
} ElementPile;

typedef struct Pile {
    ElementPile* sommet;
    int taille;
} Pile;

typedef struct ElementFile {
    int valeur;
    struct ElementFile* suivant;
} ElementFile;

typedef struct FileSimple {
    ElementFile* tete;
    ElementFile* queue;
    int taille;
} FileSimple;

/* ============================================================
 *  STRUCTURE RÉSULTATS D'ANALYSE SÉCURITÉ
 * ============================================================ */

typedef struct ResultatSecurite {
    int*  points_articulation; /**< Tableau des IDs des nœuds critiques */
    int   nb_points_articulation;
    int** ponts;               /**< Tableau de paires [src, dest] */
    int   nb_ponts;
    int*  composantes;         /**< composantes[i] = ID du SCC du nœud i */
    int   nb_composantes;
    int   a_cycle;             /**< 1 si un cycle a été détecté */
} ResultatSecurite;

#endif /* INTERFACES_H */
