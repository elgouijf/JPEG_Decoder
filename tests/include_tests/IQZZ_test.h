#ifndef IQZZ
#define IQZZ
#include <stdint.h>

void inverse_quantification( int Z[64], uint8_t * Q );

/* Fonction prenant en entrée un vecteur 1*64 et une matrice de quantification 8*8 et donne en sortie
    un vecteur 1*64 qui est le produit terme à terme des deux entrées */

int (*inverse_zig_zag(int *ZQ))[8];

/* Fonction prenant en entrée un vecteur 1*64 et donne en sortie une matrice 8*8 construite en appliquant la procédure
    de zig-zag*/

#endif