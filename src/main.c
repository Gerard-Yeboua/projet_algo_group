/**
 * @file main.c
 * @brief Point d'entrée principal - NetFlow Optimizer & Security Analyzer
 * @details Interface interactive en ligne de commande
 *
 * @authors Groupe NetFlow - UVCI ALC2101 2025-2026
 *
 * Compilation : make
 * Usage       : ./netflow [fichier_reseau.txt]
 */

#ifdef _WIN32
#include <windows.h>
#endif
#include "graphe.h"
#include "dijkstra.h"
#include "securite.h"
#include "liste_chainee.h"

/* ============================================================
 *  MENUS INTERACTIFS
 * ============================================================ */

static void afficher_menu_principal(void)
{
    printf("\n+--------------------------------------------------+\n");
    printf("|       NetFlow Optimizer & Security Analyzer      |\n");
    printf("|              UVCI - ALC2101 - 2025-2026          |\n");
    printf("+--------------------------------------------------+\n");
    printf("|  1. Charger un réseau depuis un fichier          |\n");
    printf("|  2. Créer un réseau manuellement                 |\n");
    printf("|  3. Afficher le réseau courant                   |\n");
    printf("|  4. Ajouter un nœud / une arête                  |\n");
    printf("|  5. Supprimer un nœud / une arête                |\n");
    printf("+--------------------------------------------------+\n");
    printf("|  6. Dijkstra (plus court chemin - latence)       |\n");
    printf("|  7. Bellman-Ford                                  |\n");
    printf("|  8. Chemin avec contraintes (backtracking)       |\n");
    printf("|  9. K plus courts chemins                        |\n");
    printf("+--------------------------------------------------+\n");
    printf("| 10. Analyse de sécurité complète                 |\n");
    printf("| 11. Détecter les cycles                          |\n");
    printf("| 12. Points d'articulation et ponts               |\n");
    printf("| 13. Composantes Fortement Connexes (Tarjan)      |\n");
    printf("+--------------------------------------------------+\n");
    printf("| 14. Simulation de flux de paquets                |\n");
    printf("| 15. Sauvegarder le réseau                        |\n");
    printf("+--------------------------------------------------+\n");
    printf("|  0. Quitter                                      |\n");
    printf("+--------------------------------------------------+\n");
    printf("Votre choix : ");
}

static int saisir_entier(const char *message)
{
    int val;
    printf("%s", message);
    if (scanf("%d", &val) != 1)
    {
        scanf("%*[^\n]");
        return -1;
    }
    return val;
}

static float saisir_float(const char *message)
{
    float val;
    printf("%s", message);
    if (scanf("%f", &val) != 1)
    {
        scanf("%*[^\n]");
        return -1.0f;
    }
    return val;
}

/* ============================================================
 *  ACTIONS DU MENU
 * ============================================================ */

static void action_dijkstra(const Graphe *g)
{
    int src = saisir_entier("  Nœud source      : ");
    int dest = saisir_entier("  Nœud destination : ");
    if (!noeud_existe(g, src) || !noeud_existe(g, dest))
    {
        printf("  [ERREUR] Nœuds invalides.\n");
        return;
    }
    Chemin *c = dijkstra(g, src, dest);
    if (c)
    {
        afficher_chemin(c, g);
        detruire_chemin(c);
    }
    else
        printf("  Aucun chemin trouvé.\n");
}

static void action_bellman_ford(const Graphe *g)
{
    int src = saisir_entier("  Nœud source      : ");
    int dest = saisir_entier("  Nœud destination : ");
    int cycle = 0;
    Chemin *c = bellman_ford(g, src, dest, &cycle);
    if (cycle)
    {
        printf("  [ALERTE] Cycle négatif détecté !\n");
        return;
    }
    if (c)
    {
        afficher_chemin(c, g);
        detruire_chemin(c);
    }
    else
        printf("  Aucun chemin trouvé.\n");
}

static void action_backtracking(const Graphe *g)
{
    int src = saisir_entier("  Nœud source      : ");
    int dest = saisir_entier("  Nœud destination : ");

    Contraintes c;
    c.bw_min_requise = saisir_float("  BW minimale (Mbps) : ");
    c.cout_max_autorise = saisir_float("  Budget maximum     : ");
    c.securite_min = saisir_entier("  Sécurité min (0-10): ");
    c.noeuds_oblgatoires = NULL;
    c.nb_obligatoires = 0;
    c.noeuds_exclus = NULL;
    c.nb_exclus = 0;

    printf("  [Recherche en cours avec backtracking...]\n");
    Chemin *chemin = chemin_contraint_backtracking(g, src, dest, &c);
    if (chemin)
    {
        afficher_chemin(chemin, g);
        detruire_chemin(chemin);
    }
    else
        printf("  Aucun chemin satisfaisant les contraintes.\n");
}

static void action_simulation(void)
{
    int capacite = saisir_entier("  Capacite de la file : ");
    int nb_paquets = saisir_entier("  Nombre de paquets   : ");
    int src = saisir_entier("  Source              : ");
    int dest = saisir_entier("  Destination         : ");

    FileAttente *f = creer_file_attente(capacite);
    simuler_flux(f, nb_paquets, src, dest);
    detruire_file_attente(f);
}

/* ============================================================
 *  MAIN
 * ============================================================ */

int main(int argc, char *argv[])
{
    Graphe *g = NULL;
    int choix;

#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif
    printf("\nBienvenue dans NetFlow Optimizer & Security Analyzer\n");

    /* Chargement automatique si fichier passé en argument */
    if (argc == 2)
    {
        g = charger_graphe(argv[1]);
        if (!g)
            printf("[WARN] Chargement echoue, demarrage sans reseau.\n");
    }

    do
    {
        afficher_menu_principal();
        if (scanf("%d", &choix) != 1)
        {
            scanf("%*[^\n]");
            continue;
        }

        switch (choix)
        {
        case 1:
        {
            char fichier[256];
            printf("  Chemin du fichier : ");
            scanf("%255s", fichier);
            Graphe *nouveau = charger_graphe(fichier);
            if (nouveau)
            {
                detruire_graphe(g);
                g = nouveau;
            }
            break;
        }
        case 2:
        {
            int n = saisir_entier("  Nombre de noeuds max : ");
            detruire_graphe(g);
            g = creer_graphe(n, 1);
            printf("  Graphe vide cree (%d noeuds max).\n", n);
            break;
        }
        case 3:
            if (!g)
            {
                printf("  Aucun reseau charge.\n");
                break;
            }
            afficher_graphe(g);
            break;
        case 4:
        {
            if (!g)
            {
                printf("  Chargez d\'abord un reseau.\n");
                break;
            }
            int type = saisir_entier("  Ajouter (1=noeud, 2=arete) : ");
            if (type == 1)
            {
                int id = saisir_entier("  ID : ");
                char nom[TAILLE_MAX_NOM];
                printf("  Nom : ");
                scanf("%49s", nom);
                if (ajouter_noeud(g, id, nom) == 0)
                    printf("  Noeud ajoute.\n");
            }
            else
            {
                int s = saisir_entier("  Source : ");
                int d = saisir_entier("  Dest   : ");
                float lat = saisir_float("  Latence  : ");
                float bw = saisir_float("  BW       : ");
                float cout = saisir_float("  Cout     : ");
                int sec = saisir_entier("  Securite : ");
                if (ajouter_arete(g, s, d, lat, bw, cout, sec) == 0)
                    printf("  Arete ajoutee.\n");
            }
            break;
        }
        case 5:
        {
            if (!g)
                break;
            int type = saisir_entier("  Supprimer (1=noeud, 2=arete) : ");
            if (type == 1)
            {
                int id = saisir_entier("  ID du noeud : ");
                supprimer_noeud(g, id);
            }
            else
            {
                int s = saisir_entier("  Source : ");
                int d = saisir_entier("  Dest   : ");
                supprimer_arete(g, s, d);
            }
            break;
        }
        case 6:
            if (!g)
            {
                printf("  Aucun reseau charge.\n");
                break;
            }
            action_dijkstra(g);
            break;
        case 7:
            if (!g)
                break;
            action_bellman_ford(g);
            break;
        case 8:
            if (!g)
                break;
            action_backtracking(g);
            break;
        case 9:
        {
            if (!g)
                break;
            int src = saisir_entier("  Source      : ");
            int dest = saisir_entier("  Destination : ");
            int k = saisir_entier("  K chemins   : ");
            Chemin *liste = k_plus_courts_chemins(g, src, dest, k);
            Chemin *c = liste;
            int i = 1;
            while (c)
            {
                printf("\n--- Chemin %d ---\n", i++);
                afficher_chemin(c, g);
                c = c->suivant;
            }
            detruire_liste_chemins(liste);
            break;
        }
        case 10:
        {
            if (!g)
                break;
            ResultatSecurite *res = analyser_securite(g);
            afficher_resultats_securite(res, g);
            detruire_resultat_securite(res);
            break;
        }
        case 11:
            if (!g)
                break;
            printf("  Cycle détecté : %s\n", detecter_cycle(g) ? "OUI" : "NON");
            break;
        case 12:
        {
            if (!g)
                break;
            ResultatSecurite res = {0};
            trouver_points_articulation(g, &res);
            afficher_resultats_securite(&res, g);
            break;
        }
        case 13:
        {
            if (!g)
                break;
            ResultatSecurite res = {0};
            tarjan_scc(g, &res);
            printf("  %d composante(s) fortement connexe(s)\n", res.nb_composantes);
            free(res.composantes);
            break;
        }
        case 14:
            action_simulation();
            break;
        case 15:
        {
            if (!g)
                break;
            char fichier[256];
            printf("  Nom du fichier : ");
            scanf("%255s", fichier);
            if (sauvegarder_graphe(g, fichier) == 0)
                printf("  Sauvegarde réussie.\n");
            break;
        }
        case 0:
            printf("\nAu revoir !\n");
            break;
        default:
            printf("  Choix invalide.\n");
        }
    } while (choix != 0);

    detruire_graphe(g);
    return 0;
}