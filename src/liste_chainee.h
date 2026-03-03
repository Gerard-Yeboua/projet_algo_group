/**
 * @file liste_chainee.h
 * @brief Module 4 - File de priorite doublement chainee pour paquets reseau
 * @details Pile et File simples sont dans utils.h
 */

#ifndef LISTE_CHAINEE_H
#define LISTE_CHAINEE_H

#include "interfaces.h"

/* File de priorite - Module 4 */
FileAttente* creer_file_attente(int capacite_max);
void         detruire_file_attente(FileAttente* f);
int          enqueue(FileAttente* f, int id, int priorite, float taille,
                     int source, int destination, float timestamp);
Paquet*      dequeue(FileAttente* f);
Paquet*      peek(const FileAttente* f);
int          file_vide(const FileAttente* f);
int          file_pleine(const FileAttente* f);
void         afficher_file(const FileAttente* f);
void         afficher_statistiques(const FileAttente* f);
void         simuler_flux(FileAttente* f, int nb_paquets, int source, int destination);

#endif /* LISTE_CHAINEE_H */
