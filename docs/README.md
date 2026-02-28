# NetFlow Optimizer & Security Analyzer
**UVCI - ALC2101 - Algorithmique et Complexité - 2025-2026**

## Structure du Projet

ProjetNetFlow/
├── src/
│   ├── interfaces.h        ← Structures partagées (LIRE EN PREMIER)
│   ├── graphe.h / .c       ← Module 1 : Modélisation du réseau
│   ├── dijkstra.h / .c     ← Module 2 : Algorithmes de routage
│   ├── securite.h / .c     ← Module 3 : Détection d'anomalies
│   ├── liste_chainee.h/.c  ← Module 4 : Files de paquets
│   └── main.c              ← Interface interactive
├── data/
│   ├── reseau_test1.txt    ← Réseau de test simple (6 nœuds)
│   ├── reseau_test2.txt    ← Réseau moyen (50 nœuds)
│   └── reseau_reel.txt     ← Topologie réelle
├── tests/
│   └── tests_unitaires.c   ← Tests unitaires
├── docs/
│   └── rapport.pdf
└── Makefile
```

## Compilation et Exécution

```bash
make                          # Compilation normale
make debug                    # Avec débogage + AddressSanitizer
make test                     # Tests unitaires
make valgrind                 # Vérification mémoire
./netflow data/reseau_test1.txt   # Lancement avec réseau de test
```

## Format des fichiers réseau

```
NOEUDS <n>
<id> <nom>
...
ARETES <m>
<src> <dest> <latence_ms> <bw_mbps> <cout> <securite_0_10>
...
```

## Algorithmes Implémentés

| Algorithme | Module | Complexité |
|---|---|---|
| Dijkstra | Routage | O((V+E) log V) |
| Bellman-Ford | Routage | O(V×E) |
| Backtracking contraint | Routage | O(b^d) |
| DFS/BFS | Sécurité | O(V+E) |
| Points d'articulation | Sécurité | O(V+E) |
| Tarjan SCC | Sécurité | O(V+E) |
| File de priorité | Paquets | O(n) insert, O(1) extract |

## Membres du groupe et contributions

| Membre | Rôle | Modules |
|---|---|---|
| ... | Chef de projet | Intégration, Git |
| ... | Architecte | interfaces.h |
| ... | Dev Graphes | graphe.c |
| ... | Dev Routage | dijkstra.c |
| ... | Dev Sécurité | securite.c |
| ... | Dev Files | liste_chainee.c |
| ... | Testeur/Doc | tests, rapport |
