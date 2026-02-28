/**
 * @file securite.c
 * @brief Implémentation du Module 3 - Détection d'anomalies et sécurité
 */

#include "securite.h"
#include "liste_chainee.h"

/* ============================================================
 *  DFS et BFS
 * ============================================================ */

static void dfs_recursif(const Graphe* g, int u, int* visite,
                          int* ordre, int* idx) {
    visite[u] = 1;
    ordre[(*idx)++] = u;
    Arete* a = g->noeuds[u].aretes;
    while (a) {
        if (!visite[a->destination])
            dfs_recursif(g, a->destination, visite, ordre, idx);
        a = a->suivant;
    }
}

void dfs(const Graphe* g, int depart, int* ordre, int* nb_visites) {
    int* visite = (int*)calloc(g->capacite_max, sizeof(int));
    *nb_visites = 0;
    dfs_recursif(g, depart, visite, ordre, nb_visites);
    free(visite);
}

void bfs(const Graphe* g, int depart, int* ordre, int* nb_visites) {
    int* visite = (int*)calloc(g->capacite_max, sizeof(int));
    *nb_visites = 0;

    FileSimple* file = creer_file_simple();
    enfiler_simple(file, depart);
    visite[depart] = 1;

    while (!file_simple_vide(file)) {
        int u = defiler_simple(file);
        ordre[(*nb_visites)++] = u;
        Arete* a = g->noeuds[u].aretes;
        while (a) {
            if (!visite[a->destination]) {
                visite[a->destination] = 1;
                enfiler_simple(file, a->destination);
            }
            a = a->suivant;
        }
    }
    detruire_file_simple(file);
    free(visite);
}

/* ============================================================
 *  DÉTECTION DE CYCLE (DFS avec coloration 3 états)
 *  Complexité : O(V + E)
 * ============================================================ */

static int cycle_dfs(const Graphe* g, int u, int* couleur) {
    couleur[u] = EN_COURS; /* Gris : en exploration */
    Arete* a = g->noeuds[u].aretes;
    while (a) {
        int v = a->destination;
        if (couleur[v] == EN_COURS) return 1; /* Arête de retour = cycle */
        if (couleur[v] == NON_VISITE && cycle_dfs(g, v, couleur)) return 1;
        a = a->suivant;
    }
    couleur[u] = VISITE; /* Noir : exploration terminée */
    return 0;
}

int detecter_cycle(const Graphe* g) {
    int* couleur = (int*)calloc(g->capacite_max, sizeof(int));
    int a_cycle = 0;
    for (int i = 0; i < g->capacite_max && !a_cycle; i++) {
        if (g->noeuds[i].actif && couleur[i] == NON_VISITE)
            a_cycle = cycle_dfs(g, i, couleur);
    }
    free(couleur);
    return a_cycle;
}

/* ============================================================
 *  POINTS D'ARTICULATION (algorithme de Tarjan-Hopcroft)
 *  Variables : disc[u] = temps découverte, low[u] = plus petit
 *  disc accessible depuis le sous-arbre de u
 *  Complexité : O(V + E)
 * ============================================================ */

typedef struct ContextArticulation {
    const Graphe* g;
    int*  disc;      /**< Temps de découverte */
    int*  low;       /**< Valeur low */
    int*  parent;    /**< Parent dans l'arbre DFS */
    int*  est_ap;    /**< est_ap[u] = 1 si u est point d'articulation */
    int   timer;
    /* Pour les ponts */
    int** ponts;
    int   nb_ponts;
} ContextArticulation;

static void ap_dfs(ContextArticulation* ctx, int u) {
    ctx->disc[u] = ctx->low[u] = ctx->timer++;
    int nb_enfants = 0;

    Arete* a = ctx->g->noeuds[u].aretes;
    while (a) {
        int v = a->destination;
        if (ctx->disc[v] == -1) {
            /* Arête d'arbre */
            nb_enfants++;
            ctx->parent[v] = u;
            ap_dfs(ctx, v);

            /* Mise à jour low[u] */
            if (ctx->low[v] < ctx->low[u]) ctx->low[u] = ctx->low[v];

            /* Point d'articulation : racine avec plusieurs enfants */
            if (ctx->parent[u] == -1 && nb_enfants > 1) ctx->est_ap[u] = 1;

            /* Point d'articulation : non-racine */
            if (ctx->parent[u] != -1 && ctx->low[v] >= ctx->disc[u])
                ctx->est_ap[u] = 1;

            /* Pont : low[v] > disc[u] */
            if (ctx->low[v] > ctx->disc[u]) {
                ctx->ponts = (int**)realloc(ctx->ponts,
                                (ctx->nb_ponts + 1) * sizeof(int*));
                ctx->ponts[ctx->nb_ponts] = (int*)malloc(2 * sizeof(int));
                ctx->ponts[ctx->nb_ponts][0] = u;
                ctx->ponts[ctx->nb_ponts][1] = v;
                ctx->nb_ponts++;
            }
        } else if (v != ctx->parent[u]) {
            /* Arête de retour : mise à jour low */
            if (ctx->disc[v] < ctx->low[u]) ctx->low[u] = ctx->disc[v];
        }
        a = a->suivant;
    }
}

void trouver_points_articulation(const Graphe* g, ResultatSecurite* res) {
    int V = g->capacite_max;
    ContextArticulation ctx;
    ctx.g       = g;
    ctx.disc    = (int*)malloc(V * sizeof(int));
    ctx.low     = (int*)malloc(V * sizeof(int));
    ctx.parent  = (int*)malloc(V * sizeof(int));
    ctx.est_ap  = (int*)calloc(V, sizeof(int));
    ctx.timer   = 0;
    ctx.ponts   = NULL;
    ctx.nb_ponts = 0;

    for (int i = 0; i < V; i++) { ctx.disc[i] = ctx.low[i] = -1; ctx.parent[i] = -1; }

    for (int i = 0; i < V; i++)
        if (g->noeuds[i].actif && ctx.disc[i] == -1) ap_dfs(&ctx, i);

    /* Collecter les points d'articulation */
    res->nb_points_articulation = 0;
    for (int i = 0; i < V; i++) if (ctx.est_ap[i]) res->nb_points_articulation++;

    res->points_articulation = (int*)malloc(res->nb_points_articulation * sizeof(int));
    int idx = 0;
    for (int i = 0; i < V; i++) if (ctx.est_ap[i]) res->points_articulation[idx++] = i;

    /* Récupérer les ponts */
    res->ponts    = ctx.ponts;
    res->nb_ponts = ctx.nb_ponts;

    free(ctx.disc); free(ctx.low); free(ctx.parent); free(ctx.est_ap);
}

/* ============================================================
 *  ALGORITHME DE TARJAN : COMPOSANTES FORTEMENT CONNEXES
 *  Complexité : O(V + E)
 * ============================================================ */

typedef struct ContextTarjan {
    const Graphe* g;
    int*  disc;
    int*  low;
    int*  sur_pile;
    int*  composante; /**< composante[u] = numéro du SCC de u */
    Pile* pile;
    int   timer;
    int   nb_scc;
} ContextTarjan;

static void tarjan_dfs(ContextTarjan* ctx, int u) {
    ctx->disc[u] = ctx->low[u] = ctx->timer++;
    empiler(ctx->pile, u);
    ctx->sur_pile[u] = 1;

    Arete* a = ctx->g->noeuds[u].aretes;
    while (a) {
        int v = a->destination;
        if (ctx->disc[v] == -1) {
            tarjan_dfs(ctx, v);
            if (ctx->low[v] < ctx->low[u]) ctx->low[u] = ctx->low[v];
        } else if (ctx->sur_pile[v]) {
            if (ctx->disc[v] < ctx->low[u]) ctx->low[u] = ctx->disc[v];
        }
        a = a->suivant;
    }

    /* Racine d'un SCC */
    if (ctx->low[u] == ctx->disc[u]) {
        int v;
        do {
            v = depiler(ctx->pile);
            ctx->sur_pile[v] = 0;
            ctx->composante[v] = ctx->nb_scc;
        } while (v != u);
        ctx->nb_scc++;
    }
}

void tarjan_scc(const Graphe* g, ResultatSecurite* res) {
    int V = g->capacite_max;
    ContextTarjan ctx;
    ctx.g          = g;
    ctx.disc       = (int*)malloc(V * sizeof(int));
    ctx.low        = (int*)malloc(V * sizeof(int));
    ctx.sur_pile   = (int*)calloc(V, sizeof(int));
    ctx.composante = (int*)malloc(V * sizeof(int));
    ctx.pile       = creer_pile();
    ctx.timer      = 0;
    ctx.nb_scc     = 0;

    for (int i = 0; i < V; i++) { ctx.disc[i] = -1; ctx.composante[i] = -1; }

    for (int i = 0; i < V; i++)
        if (g->noeuds[i].actif && ctx.disc[i] == -1) tarjan_dfs(&ctx, i);

    res->composantes    = ctx.composante;
    res->nb_composantes = ctx.nb_scc;

    free(ctx.disc); free(ctx.low); free(ctx.sur_pile);
    detruire_pile(ctx.pile);
}

/* ============================================================
 *  ANALYSE GLOBALE DE SÉCURITÉ
 * ============================================================ */

ResultatSecurite* analyser_securite(const Graphe* g) {
    ResultatSecurite* res = (ResultatSecurite*)calloc(1, sizeof(ResultatSecurite));

    printf("\n[ANALYSE DE SÉCURITÉ EN COURS...]\n");
    res->a_cycle = detecter_cycle(g);
    trouver_points_articulation(g, res);
    tarjan_scc(g, res);
    return res;
}

void afficher_resultats_securite(const ResultatSecurite* res, const Graphe* g) {
    printf("\n=== RAPPORT DE SÉCURITÉ ===\n");
    printf("Cycles détectés      : %s\n", res->a_cycle ? "OUI (risque de boucle)" : "NON");

    printf("\nPoints d'articulation (%d) :\n", res->nb_points_articulation);
    for (int i = 0; i < res->nb_points_articulation; i++)
        printf("  [CRITIQUE] Nœud %d - %s\n",
               res->points_articulation[i],
               g->noeuds[res->points_articulation[i]].nom);

    printf("\nPonts (arêtes critiques) (%d) :\n", res->nb_ponts);
    for (int i = 0; i < res->nb_ponts; i++)
        printf("  [PONT] %s -> %s\n",
               g->noeuds[res->ponts[i][0]].nom,
               g->noeuds[res->ponts[i][1]].nom);

    printf("\nComposantes Fortement Connexes : %d sous-réseau(x)\n",
           res->nb_composantes);
}

void detruire_resultat_securite(ResultatSecurite* res) {
    if (!res) return;
    free(res->points_articulation);
    for (int i = 0; i < res->nb_ponts; i++) free(res->ponts[i]);
    free(res->ponts);
    free(res->composantes);
    free(res);
}
