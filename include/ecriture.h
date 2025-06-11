#ifndef _ECRITURE_H_
#define _ECRITURE_H_
#include <stdint.h>

struct pixel{
    uint8_t R;
    uint8_t G;
    uint8_t B;
};

void decimal_to_binary(uint8_t valeur) ;

void ecriture(uint32_t hauteur,uint32_t largeur,struct pixel** matrice, char* nom_fichier_jpeg, uint8_t nb_comp) ;


#endif
