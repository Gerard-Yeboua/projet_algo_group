/**
 * @file dijkstra.c
 * @brief Implementation du Module 2 - Algorithmes de routage optimal
 */

#include "dijkstra.h"
#include "utils.h"

/* File de priorite interne */
typedef struct EntreePQ { int noeud; float distance; struct EntreePQ* suivant; } EntreePQ;
typedef struct FilePriorite { EntreePQ* tete; int taille; } FilePriorite;

static FilePriorite* pq_creer(void) {
    FilePriorite* pq = (FilePriorite*)malloc(sizeof(FilePriorite));
    pq->tete = NULL; pq->taille = 0; return pq;
}
static void pq_inserer(FilePriorite* pq, int noeud, float distance) {
    EntreePQ* e = (EntreePQ*)malloc(sizeof(EntreePQ));
    e->noeud = noeud; e->distance = distance; e->suivant = NULL;
    if (!pq->tete || distance < pq->tete->distance) { e->suivant = pq->tete; pq->tete = e; }
    else {
        EntreePQ* c = pq->tete;
        while (c->suivant && c->suivant->distance <= distance) c = c->suivant;
        e->suivant = c->suivant; c->suivant = e;
    }
    pq->taille++;
}
static int pq_extraire_min(FilePriorite* pq, float* d) {
    if (!pq->tete) return -1;
    EntreePQ* m = pq->tete; int n = m->noeud;
    if (d) *d = m->distance;
    pq->tete = m->suivant; free(m); pq->taille--; return n;
}
static int pq_vide(const FilePriorite* pq) { return pq->taille == 0; }
static void pq_detruire(FilePriorite* pq) {
    EntreePQ* c = pq->tete; while (c) { EntreePQ* t = c->suivant; free(c); c = t; } free(pq);
}

static Chemin* reconstruire_chemin(int* pred, float* dist, int dest) {
    if (dist[dest] == INFINI) return NULL;
    Chemin* c = creer_chemin();
    Pile* p = creer_pile();
    int n = dest;
    while (n != -1) { empiler(p, n); n = pred[n]; }
    while (!pile_vide(p)) ajouter_noeud_chemin(c, depiler(p));
    detruire_pile(p);
    c->latence_totale = dist[dest];
    return c;
}

void dijkstra_tout(const Graphe* g, int source, float* dist, int* pred) {
    int V = g->capacite_max;
    for (int i = 0; i < V; i++) { dist[i] = INFINI; pred[i] = -1; }
    dist[source] = 0.0f;
    FilePriorite* pq = pq_creer();
    pq_inserer(pq, source, 0.0f);
    int* traite = (int*)calloc(V, sizeof(int));
    while (!pq_vide(pq)) {
        float d_u; int u = pq_extraire_min(pq, &d_u);
        if (traite[u]) continue;
        traite[u] = 1;
        Arete* a = g->noeuds[u].aretes;
        while (a) {
            float nd = dist[u] + a->latence;
            if (nd < dist[a->destination]) {
                dist[a->destination] = nd; pred[a->destination] = u;
                pq_inserer(pq, a->destination, nd);
            }
            a = a->suivant;
        }
    }
    free(traite); pq_detruire(pq);
}

Chemin* dijkstra(const Graphe* g, int source, int destination) {
    if (!g || !noeud_existe(g, source) || !noeud_existe(g, destination)) return NULL;
    int V = g->capacite_max;
    float* dist = (float*)malloc(V * sizeof(float));
    int*   pred = (int*)malloc(V * sizeof(int));
    dijkstra_tout(g, source, dist, pred);
    Chemin* c = reconstruire_chemin(pred, dist, destination);
    free(dist); free(pred); return c;
}

Chemin* bellman_ford(const Graphe* g, int source, int destination, int* a_cycle_negatif) {
    if (!g || !a_cycle_negatif) return NULL;
    *a_cycle_negatif = 0;
    int V = g->capacite_max;
    float* dist = (float*)malloc(V * sizeof(float));
    int*   pred = (int*)malloc(V * sizeof(int));
    for (int i = 0; i < V; i++) { dist[i] = INFINI; pred[i] = -1; }
    dist[source] = 0.0f;
    for (int iter = 0; iter < V - 1; iter++) {
        int modif = 0;
        for (int u = 0; u < V; u++) {
            if (!g->noeuds[u].actif || dist[u] == INFINI) continue;
            Arete* a = g->noeuds[u].aretes;
            while (a) {
                float nd = dist[u] + a->latence;
                if (nd < dist[a->destination]) { dist[a->destination] = nd; pred[a->destination] = u; modif = 1; }
                a = a->suivant;
            }
        }
        if (!modif) break;
    }
    for (int u = 0; u < V; u++) {
        if (!g->noeuds[u].actif || dist[u] == INFINI) continue;
        Arete* a = g->noeuds[u].aretes;
        while (a) {
            if (dist[u] + a->latence < dist[a->destination]) {
                *a_cycle_negatif = 1; free(dist); free(pred); return NULL;
            }
            a = a->suivant;
        }
    }
    Chemin* c = reconstruire_chemin(pred, dist, destination);
    free(dist); free(pred); return c;
}

/* Utilitaires chemins */
Chemin* creer_chemin(void) { return (Chemin*)calloc(1, sizeof(Chemin)); }

void ajouter_noeud_chemin(Chemin* c, int id) {
    if (!c) return;
    NoeudChemin* n = (NoeudChemin*)malloc(sizeof(NoeudChemin));
    n->id_noeud = id; n->suivant = NULL;
    if (!c->tete) c->tete = n;
    else { NoeudChemin* cur = c->tete; while (cur->suivant) cur = cur->suivant; cur->suivant = n; }
    c->longueur++;
}

int chemin_contient(const Chemin* c, int id) {
    NoeudChemin* n = c ? c->tete : NULL;
    while (n) { if (n->id_noeud == id) return 1; n = n->suivant; }
    return 0;
}

Chemin* copier_chemin(const Chemin* src) {
    if (!src) return NULL;
    Chemin* d = creer_chemin();
    NoeudChemin* n = src->tete;
    while (n) { ajouter_noeud_chemin(d, n->id_noeud); n = n->suivant; }
    d->latence_totale = src->latence_totale; d->cout_total = src->cout_total;
    d->bw_minimale = src->bw_minimale; d->securite_min = src->securite_min;
    return d;
}

void detruire_chemin(Chemin* c) {
    if (!c) return;
    NoeudChemin* n = c->tete;
    while (n) { NoeudChemin* t = n->suivant; free(n); n = t; }
    free(c);
}

void afficher_chemin(const Chemin* c, const Graphe* g) {
    if (!c || !c->tete) { printf("(aucun chemin)\n"); return; }
    printf("Chemin [%d noeuds] : ", c->longueur);
    NoeudChemin* n = c->tete;
    while (n) { printf("%s%s", g->noeuds[n->id_noeud].nom, n->suivant ? " -> " : ""); n = n->suivant; }
    printf("\n  Latence: %.2f ms | BW min: %.2f Mbps | Cout: %.2f | Securite: %d/10\n",
           c->latence_totale, c->bw_minimale, c->cout_total, c->securite_min);
}
