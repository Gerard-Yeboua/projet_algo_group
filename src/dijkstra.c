/**
 * @file dijkstra.c
 * @brief Implémentation du Module 2 - Algorithmes de routage
 */

#include "dijkstra.h"
#include "liste_chainee.h"

/* ============================================================
 *  UTILITAIRES INTERNES : FILE DE PRIORITÉ POUR DIJKSTRA
 *  (Liste chaînée triée par distance croissante)
 * ============================================================ */

typedef struct EntreePQ {
    int   noeud;
    float distance;
    struct EntreePQ* suivant;
} EntreePQ;

typedef struct FilePriorite {
    EntreePQ* tete;
    int taille;
} FilePriorite;

static FilePriorite* pq_creer(void) {
    FilePriorite* pq = (FilePriorite*)malloc(sizeof(FilePriorite));
    pq->tete = NULL; pq->taille = 0;
    return pq;
}

static void pq_inserer(FilePriorite* pq, int noeud, float distance) {
    /* Insertion triée O(n) */
    EntreePQ* nouvelle = (EntreePQ*)malloc(sizeof(EntreePQ));
    nouvelle->noeud = noeud; nouvelle->distance = distance;
    nouvelle->suivant = NULL;

    if (!pq->tete || distance < pq->tete->distance) {
        nouvelle->suivant = pq->tete;
        pq->tete = nouvelle;
    } else {
        EntreePQ* courant = pq->tete;
        while (courant->suivant && courant->suivant->distance <= distance)
            courant = courant->suivant;
        nouvelle->suivant = courant->suivant;
        courant->suivant  = nouvelle;
    }
    pq->taille++;
}

static int pq_extraire_min(FilePriorite* pq, float* dist_out) {
    if (!pq->tete) return -1;
    EntreePQ* min = pq->tete;
    int noeud = min->noeud;
    if (dist_out) *dist_out = min->distance;
    pq->tete = min->suivant;
    free(min); pq->taille--;
    return noeud;
}

static int pq_vide(const FilePriorite* pq) { return pq->taille == 0; }

static void pq_detruire(FilePriorite* pq) {
    EntreePQ* c = pq->tete;
    while (c) { EntreePQ* t = c->suivant; free(c); c = t; }
    free(pq);
}

/* ============================================================
 *  RECONSTRUCTION DU CHEMIN depuis le tableau pred[]
 * ============================================================ */

static Chemin* reconstruire_chemin(int* pred,
                                float* dist, int dest) {
    if (dist[dest] == INFINI) return NULL; /* Pas de chemin */

    Chemin* c = creer_chemin();
    /* Remonter les prédécesseurs avec une pile temporaire */
    Pile* pile = creer_pile();
    int noeud = dest;
    while (noeud != -1) {
        empiler(pile, noeud);
        noeud = pred[noeud];
    }
    while (!pile_vide(pile)) {
        int n = depiler(pile);
        ajouter_noeud_chemin(c, n);
    }
    detruire_pile(pile);
    c->latence_totale = dist[dest];
    return c;
}

/* ============================================================
 *  DIJKSTRA COMPLET (depuis une source vers tous les nœuds)
 *  Complexité : O((V + E) log V) avec file triée O(E log V)
 * ============================================================ */

void dijkstra_tout(const Graphe* g, int source, float* dist, int* pred) {
    int V = g->capacite_max;

    /* Initialisation */
    for (int i = 0; i < V; i++) { dist[i] = INFINI; pred[i] = -1; }
    dist[source] = 0.0f;

    FilePriorite* pq = pq_creer();
    pq_inserer(pq, source, 0.0f);

    int* traite = (int*)calloc(V, sizeof(int));

    while (!pq_vide(pq)) {
        float d_u;
        int u = pq_extraire_min(pq, &d_u);

        /* Ignorer les entrées obsolètes */
        if (traite[u]) continue;
        traite[u] = 1;

        /* Relaxation des voisins */
        Arete* a = g->noeuds[u].aretes;
        while (a) {
            int v = a->destination;
            float nouvelle_dist = dist[u] + a->latence;
            if (nouvelle_dist < dist[v]) {
                dist[v] = nouvelle_dist;
                pred[v] = u;
                pq_inserer(pq, v, nouvelle_dist);
            }
            a = a->suivant;
        }
    }
    free(traite);
    pq_detruire(pq);
}

Chemin* dijkstra(const Graphe* g, int source, int destination) {
    if (!g || !noeud_existe(g, source) || !noeud_existe(g, destination))
        return NULL;

    int V = g->capacite_max;
    float* dist = (float*)malloc(V * sizeof(float));
    int*   pred = (int*)  malloc(V * sizeof(int));

    dijkstra_tout(g, source, dist, pred);
    Chemin* c = reconstruire_chemin(pred, dist, destination);

    free(dist); free(pred);
    return c;
}

/* ============================================================
 *  BELLMAN-FORD
 *  Complexité : O(V × E)
 * ============================================================ */

Chemin* bellman_ford(const Graphe* g, int source, int destination,
                     int* a_cycle_negatif) {
    if (!g) return NULL;
    *a_cycle_negatif = 0;

    int V = g->capacite_max;
    float* dist = (float*)malloc(V * sizeof(float));
    int*   pred = (int*)  malloc(V * sizeof(int));

    for (int i = 0; i < V; i++) { dist[i] = INFINI; pred[i] = -1; }
    dist[source] = 0.0f;

    /* V-1 relaxations de toutes les arêtes */
    for (int iteration = 0; iteration < V - 1; iteration++) {
        int modification = 0;
        for (int u = 0; u < V; u++) {
            if (!g->noeuds[u].actif || dist[u] == INFINI) continue;
            Arete* a = g->noeuds[u].aretes;
            while (a) {
                float nouvelle_dist = dist[u] + a->latence;
                if (nouvelle_dist < dist[a->destination]) {
                    dist[a->destination] = nouvelle_dist;
                    pred[a->destination] = u;
                    modification = 1;
                }
                a = a->suivant;
            }
        }
        if (!modification) break; /* Convergence anticipée */
    }

    /* Détection de cycle négatif : une Vième relaxation possible = cycle négatif */
    for (int u = 0; u < V; u++) {
        if (!g->noeuds[u].actif || dist[u] == INFINI) continue;
        Arete* a = g->noeuds[u].aretes;
        while (a) {
            if (dist[u] + a->latence < dist[a->destination]) {
                *a_cycle_negatif = 1;
                free(dist); free(pred);
                return NULL;
            }
            a = a->suivant;
        }
    }

    Chemin* c = reconstruire_chemin(pred, dist, destination);
    free(dist); free(pred);
    return c;
}

/* ============================================================
 *  BACKTRACKING : CHEMIN AVEC CONTRAINTES
 *  Complexité : O(b^d) - élagage réduisant en pratique
 * ============================================================ */

typedef struct ContextBacktrack {
    const Graphe*     g;
    const Contraintes* c;
    int               destination;
    int*              visite;
    Chemin*           meilleur;
    Chemin*           courant;
} ContextBacktrack;

static int contrainte_respectee(const Arete* a, const Contraintes* c) {
    return (a->bande_passante >= c->bw_min_requise) &&
           (a->securite >= c->securite_min);
}

static int noeud_exclu(int id, const Contraintes* c) {
    for (int i = 0; i < c->nb_exclus; i++)
        if (c->noeuds_exclus[i] == id) return 1;
    return 0;
}

static int tous_obligatoires_visites(const Chemin* c_chemin,
                                     const Contraintes* c) {
    for (int i = 0; i < c->nb_obligatoires; i++)
        if (!chemin_contient(c_chemin, c->noeuds_oblgatoires[i])) return 0;
    return 1;
}

static void backtrack_recursif(ContextBacktrack* ctx, int noeud_actuel,
                               float lat_acc, float cout_acc, float bw_min_acc) {
    /* Élagage : dépassement du budget */
    if (cout_acc > ctx->c->cout_max_autorise) return;

    /* Arrivée à destination */
    if (noeud_actuel == ctx->destination) {
        if (!tous_obligatoires_visites(ctx->courant, ctx->c)) return;
        /* Meilleur chemin ? */
        if (!ctx->meilleur || lat_acc < ctx->meilleur->latence_totale) {
            detruire_chemin(ctx->meilleur);
            ctx->meilleur = copier_chemin(ctx->courant);
            ctx->meilleur->latence_totale = lat_acc;
            ctx->meilleur->cout_total     = cout_acc;
            ctx->meilleur->bw_minimale    = bw_min_acc;
        }
        return;
    }

    /* Explorer les voisins */
    Arete* a = ctx->g->noeuds[noeud_actuel].aretes;
    while (a) {
        int v = a->destination;
        if (!ctx->visite[v] && !noeud_exclu(v, ctx->c) &&
            contrainte_respectee(a, ctx->c)) {

            /* Élagage : chemin actuel déjà moins bon que le meilleur */
            if (ctx->meilleur && lat_acc + a->latence >= ctx->meilleur->latence_totale) {
                a = a->suivant; continue;
            }

            ctx->visite[v] = 1;
            ajouter_noeud_chemin(ctx->courant, v);

            float nouvelle_bw = (bw_min_acc < a->bande_passante) ?
                                  bw_min_acc : a->bande_passante;

            backtrack_recursif(ctx, v, lat_acc + a->latence,
                               cout_acc + a->cout, nouvelle_bw);

            /* Retour arrière */
            ctx->visite[v] = 0;
            /* Retirer le dernier nœud du chemin courant */
            /* (implémenter retirer_dernier_noeud dans utilitaires) */
        }
        a = a->suivant;
    }
}

Chemin* chemin_contraint_backtracking(const Graphe* g, int source,
                                      int destination,
                                      const Contraintes* c) {
    if (!g || !noeud_existe(g, source) || !noeud_existe(g, destination)) return NULL;

    ContextBacktrack ctx;
    ctx.g          = g;
    ctx.c          = c;
    ctx.destination = destination;
    ctx.visite     = (int*)calloc(g->capacite_max, sizeof(int));
    ctx.meilleur   = NULL;
    ctx.courant    = creer_chemin();

    ajouter_noeud_chemin(ctx.courant, source);
    ctx.visite[source] = 1;

    backtrack_recursif(&ctx, source, 0.0f, 0.0f, INFINI);

    free(ctx.visite);
    detruire_chemin(ctx.courant);
    return ctx.meilleur;
}

/* ============================================================
 *  UTILITAIRES CHEMINS
 * ============================================================ */

Chemin* creer_chemin(void) {
    Chemin* c = (Chemin*)calloc(1, sizeof(Chemin));
    return c;
}

void ajouter_noeud_chemin(Chemin* c, int id_noeud) {
    NoeudChemin* n = (NoeudChemin*)malloc(sizeof(NoeudChemin));
    n->id_noeud = id_noeud; n->suivant = NULL;

    if (!c->tete) {
        c->tete = n;
    } else {
        NoeudChemin* courant = c->tete;
        while (courant->suivant) courant = courant->suivant;
        courant->suivant = n;
    }
    c->longueur++;
}

int chemin_contient(const Chemin* c, int id_noeud) {
    NoeudChemin* n = c->tete;
    while (n) { if (n->id_noeud == id_noeud) return 1; n = n->suivant; }
    return 0;
}

Chemin* copier_chemin(const Chemin* src) {
    Chemin* dest = creer_chemin();
    NoeudChemin* n = src->tete;
    while (n) { ajouter_noeud_chemin(dest, n->id_noeud); n = n->suivant; }
    dest->latence_totale = src->latence_totale;
    dest->cout_total     = src->cout_total;
    dest->bw_minimale    = src->bw_minimale;
    dest->securite_min   = src->securite_min;
    return dest;
}

void detruire_chemin(Chemin* c) {
    if (!c) return;
    NoeudChemin* n = c->tete;
    while (n) { NoeudChemin* t = n->suivant; free(n); n = t; }
    free(c);
}

void afficher_chemin(const Chemin* c, const Graphe* g) {
    if (!c || !c->tete) { printf("(aucun chemin)\n"); return; }
    printf("Chemin [%d nœuds] : ", c->longueur);
    NoeudChemin* n = c->tete;
    while (n) {
        printf("%s", g->noeuds[n->id_noeud].nom);
        if (n->suivant) printf(" -> ");
        n = n->suivant;
    }
    printf("\n  Latence: %.2f ms | BW min: %.2f Mbps | Coût: %.2f | Sécurité min: %d\n",
           c->latence_totale, c->bw_minimale, c->cout_total, c->securite_min);
}

/* ============================================================
 *  K PLUS COURTS CHEMINS (Algorithme de Yen - Diviser pour régner)
 *  Complexité : O(K * V * (V + E) log V)
 * ============================================================ */

void detruire_liste_chemins(Chemin* liste) {
    while (liste) {
        Chemin* suivant = liste->suivant;
        detruire_chemin(liste);
        liste = suivant;
    }
}

Chemin* k_plus_courts_chemins(const Graphe* g, int source,
                               int destination, int k) {
    if (!g || k <= 0) return NULL;

    Chemin* tete_liste = NULL; /* Liste des K meilleurs chemins */
    Chemin* queue_liste = NULL;
    int nb_trouves = 0;

    /* Initialisation : trouver le 1er plus court chemin */
    Chemin* premier = dijkstra(g, source, destination);
    if (!premier) return NULL;

    tete_liste = queue_liste = premier;
    nb_trouves = 1;

    /* Candidats potentiels */
    Chemin* candidats = NULL;

    while (nb_trouves < k) {
        /* Chemin de référence : le dernier trouvé */
        Chemin* ref = queue_liste;

        /* Pour chaque nœud racine dans le chemin de référence */
        NoeudChemin* spur_node = ref->tete;

        while (spur_node && spur_node->suivant) {
            /* Créer une copie modifiée du graphe */
            /* (Approche simplifiée : on masque certaines arêtes) */
            int spur_id = spur_node->id_noeud;

            /* Trouver un chemin alternatif depuis spur_node */
            Chemin* spur_path = dijkstra(g, spur_id, destination);
            if (spur_path) {
                /* Construire le chemin complet = root_path + spur_path */
                Chemin* candidat = creer_chemin();
                /* Ajouter le chemin racine */
                NoeudChemin* n = ref->tete;
                while (n != spur_node) {
                    ajouter_noeud_chemin(candidat, n->id_noeud);
                    n = n->suivant;
                }
                /* Ajouter le chemin spur */
                NoeudChemin* s = spur_path->tete;
                while (s) {
                    if (!chemin_contient(candidat, s->id_noeud))
                        ajouter_noeud_chemin(candidat, s->id_noeud);
                    s = s->suivant;
                }
                candidat->latence_totale = spur_path->latence_totale;
                detruire_chemin(spur_path);

                /* Ajouter aux candidats si pas déjà présent */
                candidat->suivant = candidats;
                candidats = candidat;
            }
    
            spur_node = spur_node->suivant;
        }

        /* Extraire le meilleur candidat */
        if (!candidats) break;

        /* Trouver le candidat avec la latence minimale */
        Chemin* meilleur = candidats;
        Chemin* prev_meilleur = NULL;
        Chemin* c = candidats->suivant;
        Chemin* prev = candidats;
        while (c) {
            if (c->latence_totale < meilleur->latence_totale) {
                meilleur = c;
                prev_meilleur = prev;
            }
            prev = c;
            c = c->suivant;
        }

        /* Retirer le meilleur de la liste des candidats */
        if (prev_meilleur) prev_meilleur->suivant = meilleur->suivant;
        else candidats = meilleur->suivant;
        meilleur->suivant = NULL;

        /* Ajouter à la liste résultat */
        queue_liste->suivant = meilleur;
        queue_liste = meilleur;
        nb_trouves++;
    }

    /* Libérer les candidats restants */
    detruire_liste_chemins(candidats);
    return tete_liste;
}
