/**
 * @file utils.c
 * @brief Implementation des structures utilitaires
 */

#include "utils.h"
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

/* ============================================================
 *  PILE (LIFO)
 * ============================================================ */

Pile* creer_pile(void) {
    Pile* p = (Pile*)malloc(sizeof(Pile));
    if (!p) return NULL;
    p->sommet = NULL;
    p->taille = 0;
    return p;
}

void detruire_pile(Pile* p) {
    if (!p) return;
    ElementPile* courant = p->sommet;
    while (courant) {
        ElementPile* temp = courant->suivant;
        free(courant);
        courant = temp;
    }
    free(p);
}

void empiler(Pile* p, int valeur) {
    if (!p) return;
    ElementPile* e = (ElementPile*)malloc(sizeof(ElementPile));
    if (!e) { fprintf(stderr, "Erreur malloc pile\n"); return; }
    e->valeur  = valeur;
    e->suivant = p->sommet;
    p->sommet  = e;
    p->taille++;
}

int depiler(Pile* p) {
    if (!p || !p->sommet) return -1;
    ElementPile* e = p->sommet;
    int val = e->valeur;
    p->sommet = e->suivant;
    free(e);
    p->taille--;
    return val;
}

int pile_sommet(const Pile* p) {
    if (!p || !p->sommet) return -1;
    return p->sommet->valeur;
}

int pile_vide(const Pile* p) {
    return (!p || p->taille == 0);
}

/* ============================================================
 *  FILE SIMPLE (FIFO)
 * ============================================================ */

FileSimple* creer_file_simple(void) {
    FileSimple* f = (FileSimple*)malloc(sizeof(FileSimple));
    if (!f) return NULL;
    f->tete   = NULL;
    f->queue  = NULL;
    f->taille = 0;
    return f;
}

void detruire_file_simple(FileSimple* f) {
    if (!f) return;
    ElementFile* courant = f->tete;
    while (courant) {
        ElementFile* temp = courant->suivant;
        free(courant);
        courant = temp;
    }
    free(f);
}

void enfiler_simple(FileSimple* f, int valeur) {
    if (!f) return;
    ElementFile* e = (ElementFile*)malloc(sizeof(ElementFile));
    if (!e) return;
    e->valeur  = valeur;
    e->suivant = NULL;
    if (!f->queue) {
        f->tete = f->queue = e;
    } else {
        f->queue->suivant = e;
        f->queue = e;
    }
    f->taille++;
}

int defiler_simple(FileSimple* f) {
    if (!f || !f->tete) return -1;
    ElementFile* e = f->tete;
    int val = e->valeur;
    f->tete = e->suivant;
    if (!f->tete) f->queue = NULL;
    free(e);
    f->taille--;
    return val;
}

int file_simple_vide(const FileSimple* f) {
    return (!f || f->taille == 0);
}

/* ============================================================
 *  UTILITAIRES D'AFFICHAGE ET MESURE
 * ============================================================ */

void afficher_separateur(const char* titre) {
    printf("\n=== %s ===\n", titre);
}

double temps_ms(void) {
#ifdef _WIN32
    LARGE_INTEGER freq, count;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&count);
    return (double)count.QuadPart * 1000.0 / (double)freq.QuadPart;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec * 1000.0 + (double)tv.tv_usec / 1000.0;
#endif
}

int rand_entre(int min, int max) {
    if (max <= min) return min;
    return min + rand() % (max - min + 1);
}

float rand_float(float min, float max) {
    return min + ((float)rand() / RAND_MAX) * (max - min);
}
