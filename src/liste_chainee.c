/**
 * @file liste_chainee.c
 * @brief Implementation du Module 4 - File de priorite et simulation de flux
 */

#include "liste_chainee.h"
#include "utils.h"

FileAttente* creer_file_attente(int capacite_max) {
    FileAttente* f = (FileAttente*)calloc(1, sizeof(FileAttente));
    if (!f) return NULL;
    f->capacite_max = capacite_max;
    return f;
}

void detruire_file_attente(FileAttente* f) {
    if (!f) return;
    Paquet* c = f->tete;
    while (c) { Paquet* t = c->suivant; free(c); c = t; }
    free(f);
}

int enqueue(FileAttente* f, int id, int priorite, float taille,
            int source, int destination, float timestamp) {
    f->total_insere++;
    if (file_pleine(f)) { f->total_perdu++; return -1; }

    Paquet* p = (Paquet*)malloc(sizeof(Paquet));
    if (!p) return -1;
    p->id = id; p->priorite = priorite; p->taille_Mo = taille;
    p->source = source; p->destination = destination;
    p->timestamp_arrivee = timestamp;
    p->precedent = p->suivant = NULL;

    if (!f->tete) { f->tete = f->queue = p; }
    else {
        /* Insertion triee par priorite decroissante - O(n) */
        Paquet* c = f->tete;
        while (c && c->priorite >= p->priorite) c = c->suivant;
        if (!c) {
            p->precedent = f->queue; f->queue->suivant = p; f->queue = p;
        } else if (!c->precedent) {
            p->suivant = f->tete; f->tete->precedent = p; f->tete = p;
        } else {
            p->suivant = c; p->precedent = c->precedent;
            c->precedent->suivant = p; c->precedent = p;
        }
    }
    f->taille_actuelle++;
    return 0;
}

Paquet* dequeue(FileAttente* f) {
    if (!f || !f->tete) return NULL;
    Paquet* p = f->tete;
    f->tete = p->suivant;
    if (f->tete) f->tete->precedent = NULL; else f->queue = NULL;
    p->suivant = p->precedent = NULL;
    f->taille_actuelle--;
    return p;
}

Paquet* peek(const FileAttente* f) { return f ? f->tete : NULL; }
int file_vide(const FileAttente* f)  { return !f || f->taille_actuelle == 0; }
int file_pleine(const FileAttente* f){ return f && f->taille_actuelle >= f->capacite_max; }

void afficher_statistiques(const FileAttente* f) {
    if (!f) return;
    printf("\n=== STATISTIQUES FILE D'ATTENTE ===\n");
    printf("  Inseres  : %d\n", f->total_insere);
    printf("  Perdus   : %d (%.1f%%)\n", f->total_perdu,
           f->total_insere > 0 ? f->total_perdu*100.0f/f->total_insere : 0.0f);
    printf("  En attente : %d/%d\n", f->taille_actuelle, f->capacite_max);
    int traites = f->total_insere - f->total_perdu - f->taille_actuelle;
    if (traites > 0)
        printf("  Temps moy attente : %.2f ms\n", f->temps_attente_cumule / traites);
}

void simuler_flux(FileAttente* f, int nb_paquets, int source, int destination) {
    printf("\n[SIMULATION] %d paquets de %d vers %d\n", nb_paquets, source, destination);
    for (int i = 0; i < nb_paquets; i++) {
        enqueue(f, i, rand_entre(1,10), rand_float(0.1f,10.0f),
                source, destination, (float)i * 0.5f);
    }
    float t = 0.0f;
    while (!file_vide(f)) {
        Paquet* p = dequeue(f);
        float att = t - p->timestamp_arrivee;
        if (att < 0) att = 0;
        f->temps_attente_cumule += att;
        t += p->taille_Mo * 0.1f;
        free(p);
    }
    afficher_statistiques(f);
}
