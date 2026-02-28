/**
 * @file graphe.c
 * @brief Implémentation du Module 1 - Modélisation du réseau
 */

#include "graphe.h"
#include "liste_chainee.h"

/* ============================================================
 *  CRÉATION / DESTRUCTION
 * ============================================================ */

Graphe* creer_graphe(int capacite_max, int est_oriente) {
    Graphe* g = (Graphe*)malloc(sizeof(Graphe));
    if (!g) { fprintf(stderr, "Erreur malloc Graphe\n"); return NULL; }

    g->nb_noeuds   = 0;
    g->nb_aretes   = 0;
    g->capacite_max = capacite_max;
    g->est_oriente  = est_oriente;

    /* Allocation du tableau de nœuds */
    g->noeuds = (Noeud*)calloc(capacite_max, sizeof(Noeud));
    if (!g->noeuds) { free(g); return NULL; }

    /* Allocation des matrices d'adjacence */
    g->matrice_latence = (float**)calloc(capacite_max, sizeof(float*));
    g->matrice_bw      = (float**)calloc(capacite_max, sizeof(float*));
    g->matrice_cout    = (float**)calloc(capacite_max, sizeof(float*));
    g->matrice_sec     = (int**)  calloc(capacite_max, sizeof(int*));

    for (int i = 0; i < capacite_max; i++) {
        g->matrice_latence[i] = (float*)malloc(capacite_max * sizeof(float));
        g->matrice_bw[i]      = (float*)malloc(capacite_max * sizeof(float));
        g->matrice_cout[i]    = (float*)malloc(capacite_max * sizeof(float));
        g->matrice_sec[i]     = (int*)  malloc(capacite_max * sizeof(int));

        for (int j = 0; j < capacite_max; j++) {
            g->matrice_latence[i][j] = INFINI;
            g->matrice_bw[i][j]      = 0.0f;
            g->matrice_cout[i][j]    = INFINI;
            g->matrice_sec[i][j]     = -1;
        }
    }
    return g;
}

void detruire_graphe(Graphe* g) {
    if (!g) return;

    /* Libérer les listes d'adjacence */
    for (int i = 0; i < g->capacite_max; i++) {
        Arete* courant = g->noeuds[i].aretes;
        while (courant) {
            Arete* temp = courant->suivant;
            free(courant);
            courant = temp;
        }
        /* Libérer les matrices ligne par ligne */
        free(g->matrice_latence[i]);
        free(g->matrice_bw[i]);
        free(g->matrice_cout[i]);
        free(g->matrice_sec[i]);
    }

    free(g->noeuds);
    free(g->matrice_latence);
    free(g->matrice_bw);
    free(g->matrice_cout);
    free(g->matrice_sec);
    free(g);
}

/* ============================================================
 *  MANIPULATION DES NŒUDS
 * ============================================================ */

int ajouter_noeud(Graphe* g, int id, const char* nom) {
    if (!g || id < 0 || id >= g->capacite_max) return -1;
    if (noeud_existe(g, id)) return -1; /* Déjà présent */

    g->noeuds[id].id = id;
    strncpy(g->noeuds[id].nom, nom, TAILLE_MAX_NOM - 1);
    g->noeuds[id].nom[TAILLE_MAX_NOM - 1] = '\0';
    g->noeuds[id].actif  = 1;
    g->noeuds[id].aretes = NULL;
    g->nb_noeuds++;
    return 0;
}

int supprimer_noeud(Graphe* g, int id) {
    if (!g || !noeud_existe(g, id)) return -1;

    /* Supprimer toutes les arêtes sortantes */
    Arete* courant = g->noeuds[id].aretes;
    while (courant) {
        Arete* temp = courant->suivant;
        /* Mettre à jour la matrice */
        g->matrice_latence[id][courant->destination] = INFINI;
        free(courant);
        courant = temp;
        g->nb_aretes--;
    }
    g->noeuds[id].aretes = NULL;

    /* Supprimer les arêtes entrantes (depuis les autres nœuds) */
    for (int i = 0; i < g->capacite_max; i++) {
        if (g->noeuds[i].actif && i != id) {
            supprimer_arete(g, i, id);
        }
    }

    g->noeuds[id].actif = 0;
    g->nb_noeuds--;
    return 0;
}

int noeud_existe(const Graphe* g, int id) {
    if (!g || id < 0 || id >= g->capacite_max) return 0;
    return g->noeuds[id].actif;
}

/* ============================================================
 *  MANIPULATION DES ARÊTES
 * ============================================================ */

int ajouter_arete(Graphe* g, int src, int dest,
                  float latence, float bande_passante,
                  float cout, int securite) {
    if (!g || !noeud_existe(g, src) || !noeud_existe(g, dest)) return -1;

    /* --- Liste d'adjacence --- */
    Arete* nouvelle = (Arete*)malloc(sizeof(Arete));
    if (!nouvelle) return -1;

    nouvelle->destination  = dest;
    nouvelle->latence      = latence;
    nouvelle->bande_passante = bande_passante;
    nouvelle->cout         = cout;
    nouvelle->securite     = securite;
    nouvelle->suivant      = g->noeuds[src].aretes; /* Insertion en tête */
    g->noeuds[src].aretes  = nouvelle;

    /* --- Matrice d'adjacence --- */
    g->matrice_latence[src][dest] = latence;
    g->matrice_bw[src][dest]      = bande_passante;
    g->matrice_cout[src][dest]    = cout;
    g->matrice_sec[src][dest]     = securite;

    /* Si non orienté, ajouter l'arête inverse */
    if (!g->est_oriente) {
        ajouter_arete(g, dest, src, latence, bande_passante, cout, securite);
    }

    g->nb_aretes++;
    return 0;
}

int supprimer_arete(Graphe* g, int src, int dest) {
    if (!g || !noeud_existe(g, src)) return -1;

    Arete* courant  = g->noeuds[src].aretes;
    Arete* precedent = NULL;

    while (courant) {
        if (courant->destination == dest) {
            if (precedent) precedent->suivant = courant->suivant;
            else g->noeuds[src].aretes = courant->suivant;
            free(courant);
            g->matrice_latence[src][dest] = INFINI;
            g->nb_aretes--;
            return 0;
        }
        precedent = courant;
        courant = courant->suivant;
    }
    return -1; /* Arête non trouvée */
}

Arete* trouver_arete(const Graphe* g, int src, int dest) {
    if (!g || !noeud_existe(g, src)) return NULL;
    Arete* courant = g->noeuds[src].aretes;
    while (courant) {
        if (courant->destination == dest) return courant;
        courant = courant->suivant;
    }
    return NULL;
}

/* ============================================================
 *  CHARGEMENT / SAUVEGARDE
 * ============================================================ */

/**
 * @brief Format du fichier réseau :
 *   NOEUDS <n>
 *   <id> <nom>
 *   ...
 *   ARETES <m>
 *   <src> <dest> <latence> <bw> <cout> <securite>
 *   ...
 */
Graphe* charger_graphe(const char* nom_fichier) {
    FILE* f = fopen(nom_fichier, "r");
    if (!f) { fprintf(stderr, "Erreur ouverture : %s\n", nom_fichier); return NULL; }

    int nb_noeuds, nb_aretes;
    fscanf(f, "NOEUDS %d\n", &nb_noeuds);

    Graphe* g = creer_graphe(nb_noeuds, 1);
    if (!g) { fclose(f); return NULL; }

    /* Lecture des nœuds */
    for (int i = 0; i < nb_noeuds; i++) {
        int id; char nom[TAILLE_MAX_NOM];
        fscanf(f, "%d %s\n", &id, nom);
        ajouter_noeud(g, id, nom);
    }

    /* Lecture des arêtes */
    fscanf(f, "ARETES %d\n", &nb_aretes);
    for (int i = 0; i < nb_aretes; i++) {
        int src, dest, securite;
        float lat, bw, cout;
        fscanf(f, "%d %d %f %f %f %d\n", &src, &dest, &lat, &bw, &cout, &securite);
        ajouter_arete(g, src, dest, lat, bw, cout, securite);
    }

    fclose(f);
    printf("[OK] Graphe chargé : %d nœuds, %d arêtes\n", g->nb_noeuds, g->nb_aretes);
    return g;
}

int sauvegarder_graphe(const Graphe* g, const char* nom_fichier) {
    FILE* f = fopen(nom_fichier, "w");
    if (!f) return -1;

    fprintf(f, "NOEUDS %d\n", g->nb_noeuds);
    for (int i = 0; i < g->capacite_max; i++) {
        if (g->noeuds[i].actif)
            fprintf(f, "%d %s\n", g->noeuds[i].id, g->noeuds[i].nom);
    }

    fprintf(f, "ARETES %d\n", g->nb_aretes);
    for (int i = 0; i < g->capacite_max; i++) {
        if (!g->noeuds[i].actif) continue;
        Arete* a = g->noeuds[i].aretes;
        while (a) {
            fprintf(f, "%d %d %.2f %.2f %.2f %d\n",
                    i, a->destination,
                    a->latence, a->bande_passante, a->cout, a->securite);
            a = a->suivant;
        }
    }
    fclose(f);
    return 0;
}

/* ============================================================
 *  AFFICHAGE
 * ============================================================ */

void afficher_graphe(const Graphe* g) {
    if (!g) return;
    printf("\n=== GRAPHE (%d nœuds, %d arêtes) ===\n", g->nb_noeuds, g->nb_aretes);
    for (int i = 0; i < g->capacite_max; i++) {
        if (!g->noeuds[i].actif) continue;
        printf("[%d] %s -> ", i, g->noeuds[i].nom);
        Arete* a = g->noeuds[i].aretes;
        if (!a) { printf("(aucune arête)\n"); continue; }
        while (a) {
            printf("[%d](lat:%.1f bw:%.1f coût:%.1f sec:%d) ",
                   a->destination, a->latence,
                   a->bande_passante, a->cout, a->securite);
            a = a->suivant;
        }
        printf("\n");
    }
}

int graphe_est_connexe(const Graphe* g) {
    /* Utilise un BFS depuis le premier nœud actif */
    if (!g || g->nb_noeuds == 0) return 1;
    int* visite = (int*)calloc(g->capacite_max, sizeof(int));
    if (!visite) return 0;

    /* Trouver le premier nœud actif */
    int depart = -1;
    for (int i = 0; i < g->capacite_max; i++) {
        if (g->noeuds[i].actif) { depart = i; break; }
    }

    /* BFS simple */
    FileSimple* file = creer_file_simple(); /* déclarée dans utils.c */
    enfiler_simple(file, depart);
    visite[depart] = 1;
    int nb_visites = 1;

    while (!file_simple_vide(file)) {
        int u = defiler_simple(file);
        Arete* a = g->noeuds[u].aretes;
        while (a) {
            if (!visite[a->destination]) {
                visite[a->destination] = 1;
                enfiler_simple(file, a->destination);
                nb_visites++;
            }
            a = a->suivant;
        }
    }
    detruire_file_simple(file);
    free(visite);
    return (nb_visites == g->nb_noeuds);
}
