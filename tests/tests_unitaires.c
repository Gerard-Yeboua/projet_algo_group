/**
 * @file tests_unitaires.c
 * @brief Tests unitaires pour tous les modules du projet
 */

#include <assert.h>
#include <stdio.h>
#include "../src/graphe.h"
#include "../src/dijkstra.h"
#include "../src/securite.h"
#include "../src/liste_chainee.h"
#include "../src/backtracking.h"
#include "../src/utils.h"
#include "../src/liste_chainee.h"

/* Compteur de tests */
static int tests_passes = 0;
static int tests_total  = 0;

#define TEST(nom, condition) do { \
    tests_total++; \
    if (condition) { printf("  [OK] %s\n", nom); tests_passes++; } \
    else { printf("  [FAIL] %s (ligne %d)\n", nom, __LINE__); } \
} while(0)

/* ============================================================
 *  TESTS MODULE 1 : GRAPHE
 * ============================================================ */

void test_graphe(void) {
    printf("\n--- Module 1 : Graphe ---\n");

    /* Creation */
    Graphe* g = creer_graphe(10, 1);
    TEST("creer_graphe non NULL", g != NULL);
    TEST("nb_noeuds initial = 0", g->nb_noeuds == 0);

    /* Ajout de nœuds */
    TEST("ajouter_noeud(0)", ajouter_noeud(g, 0, "Paris") == 0);
    TEST("ajouter_noeud(1)", ajouter_noeud(g, 1, "Lyon") == 0);
    TEST("ajouter_noeud(2)", ajouter_noeud(g, 2, "Marseille") == 0);
    TEST("nb_noeuds = 3", g->nb_noeuds == 3);
    TEST("noeud_existe(0)", noeud_existe(g, 0) == 1);
    TEST("noeud_existe(9) = 0", noeud_existe(g, 9) == 0);

    /* Ajout d'aretes */
    TEST("ajouter_arete(0->1)", ajouter_arete(g, 0, 1, 10.0f, 100.0f, 5.0f, 8) == 0);
    TEST("ajouter_arete(1->2)", ajouter_arete(g, 1, 2, 15.0f, 50.0f, 3.0f, 7) == 0);
    TEST("ajouter_arete(0->2)", ajouter_arete(g, 0, 2, 30.0f, 80.0f, 8.0f, 9) == 0);
    TEST("nb_aretes = 3", g->nb_aretes == 3);

    /* Trouver une arete */
    Arete* a = trouver_arete(g, 0, 1);
    TEST("trouver_arete(0->1) non NULL", a != NULL);
    TEST("latence arete = 10.0", a && a->latence == 10.0f);

    /* Suppression */
    TEST("supprimer_arete(0->1)", supprimer_arete(g, 0, 1) == 0);
    TEST("arete supprimee", trouver_arete(g, 0, 1) == NULL);

    detruire_graphe(g);
    TEST("detruire_graphe sans crash", 1);
}

/* ============================================================
 *  TESTS MODULE 2 : DIJKSTRA
 * ============================================================ */

void test_dijkstra(void) {
    printf("\n--- Module 2 : Dijkstra ---\n");

    /* Graphe de test :
       0 --(10)--> 1 --(15)--> 2
       0 --(30)--> 2
       Chemin optimal 0->2 : 0->1->2 (25ms) et non 0->2 (30ms)
    */
    Graphe* g = creer_graphe(5, 1);
    ajouter_noeud(g, 0, "A"); ajouter_noeud(g, 1, "B"); ajouter_noeud(g, 2, "C");
    ajouter_arete(g, 0, 1, 10.0f, 100.0f, 5.0f, 8);
    ajouter_arete(g, 1, 2, 15.0f, 100.0f, 3.0f, 7);
    ajouter_arete(g, 0, 2, 30.0f, 100.0f, 8.0f, 9);

    Chemin* c = dijkstra(g, 0, 2);
    TEST("dijkstra retourne un chemin", c != NULL);
    TEST("latence optimale = 25ms", c && c->latence_totale == 25.0f);
    TEST("longueur chemin = 3", c && c->longueur == 3);
    detruire_chemin(c);

    /* Pas de chemin */
    ajouter_noeud(g, 3, "D");
    Chemin* c2 = dijkstra(g, 0, 3);
    TEST("pas de chemin = NULL", c2 == NULL);

    detruire_graphe(g);
}

/* ============================================================
 *  TESTS MODULE 3 : SÉCURITÉ
 * ============================================================ */

void test_securite(void) {
    printf("\n--- Module 3 : Securite ---\n");

    /* Graphe sans cycle */
    Graphe* g = creer_graphe(5, 1);
    ajouter_noeud(g, 0, "A"); ajouter_noeud(g, 1, "B"); ajouter_noeud(g, 2, "C");
    ajouter_arete(g, 0, 1, 10.0f, 100.0f, 5.0f, 8);
    ajouter_arete(g, 1, 2, 15.0f, 100.0f, 3.0f, 7);

    TEST("pas de cycle (graphe acyclique)", detecter_cycle(g) == 0);

    /* Ajout d'un cycle */
    ajouter_arete(g, 2, 0, 5.0f, 100.0f, 2.0f, 6);
    TEST("cycle detecte apres ajout", detecter_cycle(g) == 1);

    detruire_graphe(g);
}

/* ============================================================
 *  TESTS MODULE 4 : FILE DE PRIORITÉ
 * ============================================================ */

void test_file_attente(void) {
    printf("\n--- Module 4 : File d'attente ---\n");

    FileAttente* f = creer_file_attente(5);
    TEST("creer_file_attente non NULL", f != NULL);
    TEST("file vide initialement", file_vide(f) == 1);

    /* Insertion */
    TEST("enqueue paquet prio 3", enqueue(f, 1, 3, 1.0f, 0, 1, 0.0f) == 0);
    TEST("enqueue paquet prio 7", enqueue(f, 2, 7, 2.0f, 0, 1, 0.0f) == 0);
    TEST("enqueue paquet prio 1", enqueue(f, 3, 1, 0.5f, 0, 1, 0.0f) == 0);
    TEST("taille = 3", f->taille_actuelle == 3);

    /* peek doit retourner le plus prioritaire */
    Paquet* p = peek(f);
    TEST("peek retourne prio max (7)", p && p->priorite == 7);

    /* dequeue doit extraire le plus prioritaire */
    Paquet* ext = dequeue(f);
    TEST("dequeue retourne prio 7", ext && ext->priorite == 7);
    free(ext);
    TEST("taille apres dequeue = 2", f->taille_actuelle == 2);

    /* Capacite maximale */
    enqueue(f, 4, 5, 1.0f, 0, 1, 0.0f);
    enqueue(f, 5, 2, 1.0f, 0, 1, 0.0f);
    enqueue(f, 6, 4, 1.0f, 0, 1, 0.0f);
    TEST("file pleine", file_pleine(f) == 1);
    TEST("enqueue sur file pleine = -1", enqueue(f, 99, 9, 1.0f, 0, 1, 0.0f) == -1);

    detruire_file_attente(f);
    TEST("detruire_file sans crash", 1);
}

/* ============================================================
 *  TESTS STRUCTURES UTILITAIRES
 * ============================================================ */

void test_pile(void) {
    printf("\n--- Structures : Pile ---\n");

    Pile* p = creer_pile();
    TEST("pile vide", pile_vide(p) == 1);
    empiler(p, 10); empiler(p, 20); empiler(p, 30);
    TEST("taille pile = 3", p->taille == 3);
    TEST("sommet = 30", pile_sommet(p) == 30);
    TEST("depiler = 30", depiler(p) == 30);
    TEST("depiler = 20", depiler(p) == 20);
    detruire_pile(p);
}

/* ============================================================
 *  MAIN DES TESTS
 * ============================================================ */

int main(void) {
    printf("+--------------------------------------+\n");
    printf("|     TESTS UNITAIRES - NetFlow        |\n");
    printf("+--------------------------------------+\n");

    test_graphe();
    test_dijkstra();
    test_securite();
    test_file_attente();
    test_pile();

    printf("\n--------------------------------------\n");
    printf("RESULTAT : %d/%d tests passes\n", tests_passes, tests_total);
    if (tests_passes == tests_total)
        printf("[OK] TOUS LES TESTS PASSES\n");
    else
        printf("✗ %d TEST(S) ÉCHOUÉ(S)\n", tests_total - tests_passes);
    printf("--------------------------------------\n");

    return (tests_passes == tests_total) ? 0 : 1;
}