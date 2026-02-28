/**
 * @file liste_chainee.c
 * @brief Implémentation du Module 4 - Structures chaînées et simulation
 */

#include "liste_chainee.h"

/* ============================================================
 *  FILE DE PRIORITÉ DOUBLEMENT CHAÎNÉE
 * ============================================================ */

FileAttente* creer_file_attente(int capacite_max) {
    FileAttente* f = (FileAttente*)calloc(1, sizeof(FileAttente));
    if (!f) return NULL;
    f->capacite_max    = capacite_max;
    f->tete            = NULL;
    f->queue           = NULL;
    f->taille_actuelle = 0;
    f->total_insere    = 0;
    f->total_perdu     = 0;
    f->temps_attente_cumule = 0.0f;
    return f;
}

void detruire_file_attente(FileAttente* f) {
    if (!f) return;
    Paquet* courant = f->tete;
    while (courant) {
        Paquet* temp = courant->suivant;
        free(courant);
        courant = temp;
    }
    free(f);
}

/**
 * @brief Insère un paquet en maintenant l'ordre décroissant de priorité
 *        O(n) : parcours pour trouver la bonne position
 */
int enqueue(FileAttente* f, int id, int priorite, float taille,
            int source, int destination, float timestamp) {
    f->total_insere++;

    /* File pleine : paquet perdu */
    if (file_pleine(f)) {
        f->total_perdu++;
        fprintf(stderr, "[PERTE] Paquet %d rejeté (file saturée)\n", id);
        return -1;
    }

    /* Créer le nouveau paquet */
    Paquet* nouveau = (Paquet*)malloc(sizeof(Paquet));
    if (!nouveau) return -1;
    nouveau->id                = id;
    nouveau->priorite          = priorite;
    nouveau->taille_Mo         = taille;
    nouveau->source            = source;
    nouveau->destination       = destination;
    nouveau->timestamp_arrivee = timestamp;
    nouveau->precedent         = NULL;
    nouveau->suivant           = NULL;

    /* File vide */
    if (!f->tete) {
        f->tete = f->queue = nouveau;
        f->taille_actuelle++;
        return 0;
    }

    /* Trouver la position (ordre décroissant de priorité) */
    Paquet* courant = f->tete;
    while (courant && courant->priorite >= nouveau->priorite)
        courant = courant->suivant;

    if (!courant) {
        /* Insérer en fin */
        nouveau->precedent = f->queue;
        f->queue->suivant  = nouveau;
        f->queue           = nouveau;
    } else if (!courant->precedent) {
        /* Insérer en tête */
        nouveau->suivant   = f->tete;
        f->tete->precedent = nouveau;
        f->tete            = nouveau;
    } else {
        /* Insérer entre courant->precedent et courant */
        nouveau->suivant            = courant;
        nouveau->precedent          = courant->precedent;
        courant->precedent->suivant = nouveau;
        courant->precedent          = nouveau;
    }
    f->taille_actuelle++;
    return 0;
}

/**
 * @brief Extrait le paquet en tête (priorité max)
 *        O(1) : extraction directe
 */
Paquet* dequeue(FileAttente* f) {
    if (!f || !f->tete) return NULL;

    Paquet* paquet = f->tete;
    f->tete = paquet->suivant;
    if (f->tete) f->tete->precedent = NULL;
    else f->queue = NULL;

    paquet->suivant  = NULL;
    paquet->precedent = NULL;
    f->taille_actuelle--;
    return paquet;
}

Paquet* peek(const FileAttente* f) {
    return (f && f->tete) ? f->tete : NULL;
}

int file_vide(const FileAttente* f)  { return (!f || f->taille_actuelle == 0); }
int file_pleine(const FileAttente* f){ return (f && f->taille_actuelle >= f->capacite_max); }

void afficher_file(const FileAttente* f) {
    if (!f) return;
    printf("\n=== FILE D'ATTENTE (%d/%d paquets) ===\n",
           f->taille_actuelle, f->capacite_max);
    Paquet* courant = f->tete;
    int i = 1;
    while (courant) {
        printf("  [%d] Paquet#%d | Priorité:%d | %.2f Mo | %d->%d\n",
               i++, courant->id, courant->priorite,
               courant->taille_Mo, courant->source, courant->destination);
        courant = courant->suivant;
    }
}

void afficher_statistiques(const FileAttente* f) {
    if (!f) return;
    printf("\n=== STATISTIQUES DE LA FILE ===\n");
    printf("  Paquets insérés    : %d\n", f->total_insere);
    printf("  Paquets perdus     : %d\n", f->total_perdu);
    printf("  Taux de perte      : %.2f%%\n",
           f->total_insere > 0 ? (f->total_perdu * 100.0f / f->total_insere) : 0.0f);
    printf("  En attente         : %d\n", f->taille_actuelle);
    if (f->total_insere - f->total_perdu > 0)
        printf("  Temps attente moy  : %.2f ms\n",
               f->temps_attente_cumule / (f->total_insere - f->total_perdu));
}

/* ============================================================
 *  SIMULATION DE FLUX DE PAQUETS
 * ============================================================ */

void simuler_flux(FileAttente* f, int nb_paquets, int source, int destination) {
    printf("\n[SIMULATION] Envoi de %d paquets de %d vers %d\n",
           nb_paquets, source, destination);

    for (int i = 0; i < nb_paquets; i++) {
        int   prio    = rand() % 10 + 1;
        float taille  = (float)(rand() % 100 + 1) / 10.0f;
        float ts      = (float)i * 0.5f; /* timestamp simulé */
        enqueue(f, i, prio, taille, source, destination, ts);
    }

    /* Traitement des paquets */
    float temps_courant = 0.0f;
    int nb_traites = 0;
    while (!file_vide(f)) {
        Paquet* p = dequeue(f);
        float attente = temps_courant - p->timestamp_arrivee;
        if (attente < 0) attente = 0;
        f->temps_attente_cumule += attente;
        temps_courant += p->taille_Mo * 0.1f; /* Simulation du traitement */
        nb_traites++;
        free(p);
    }
    printf("[SIMULATION] %d paquets traités\n", nb_traites);
    afficher_statistiques(f);
}

/* ============================================================
 *  PILE (LIFO) - pour DFS et backtracking
 * ============================================================ */

Pile* creer_pile(void) {
    Pile* p = (Pile*)malloc(sizeof(Pile));
    p->sommet = NULL; p->taille = 0;
    return p;
}

void detruire_pile(Pile* p) {
    if (!p) return;
    while (p->sommet) {
        ElementPile* t = p->sommet->suivant;
        free(p->sommet); p->sommet = t;
    }
    free(p);
}

void empiler(Pile* p, int valeur) {
    ElementPile* e = (ElementPile*)malloc(sizeof(ElementPile));
    e->valeur   = valeur;
    e->suivant  = p->sommet;
    p->sommet   = e;
    p->taille++;
}

int depiler(Pile* p) {
    if (!p->sommet) return -1;
    ElementPile* e = p->sommet;
    int val = e->valeur;
    p->sommet = e->suivant;
    free(e); p->taille--;
    return val;
}

int pile_vide(const Pile* p)  { return (!p || p->taille == 0); }
int pile_sommet(const Pile* p){ return p->sommet ? p->sommet->valeur : -1; }

/* ============================================================
 *  FILE SIMPLE (FIFO) - pour BFS
 * ============================================================ */

FileSimple* creer_file_simple(void) {
    FileSimple* f = (FileSimple*)malloc(sizeof(FileSimple));
    f->tete = f->queue = NULL; f->taille = 0;
    return f;
}

void detruire_file_simple(FileSimple* f) {
    if (!f) return;
    while (f->tete) {
        ElementFile* t = f->tete->suivant;
        free(f->tete); f->tete = t;
    }
    free(f);
}

void enfiler_simple(FileSimple* f, int valeur) {
    ElementFile* e = (ElementFile*)malloc(sizeof(ElementFile));
    e->valeur  = valeur; e->suivant = NULL;
    if (!f->queue) { f->tete = f->queue = e; }
    else { f->queue->suivant = e; f->queue = e; }
    f->taille++;
}

int defiler_simple(FileSimple* f) {
    if (!f->tete) return -1;
    ElementFile* e = f->tete;
    int val = e->valeur;
    f->tete = e->suivant;
    if (!f->tete) f->queue = NULL;
    free(e); f->taille--;
    return val;
}

int file_simple_vide(const FileSimple* f) { return (!f || f->taille == 0); }
