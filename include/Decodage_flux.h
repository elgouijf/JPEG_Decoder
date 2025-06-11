# ifndef Decodage_flux
# define Decodage_flux
# include <stdint.h>
# include <stdlib.h>
# include <stdio.h>
# include "../include/construction_arbre_huff.h"


struct Bits{
    uint8_t* flux; // flux d'entier
    uint8_t position_bit; //uint8_t car il se situe dans un octet ie 8 bits => codé sur 3 bits au maximum
    size_t position_octet; //size_t car le flux peut etre trops grand 
    size_t taille; // la taille en octets du fichiers (permet de lire le fichier sans dépasser sa taille)
    uint8_t depassement; // vérifie si il y' a dépassement de la taille du fichier
};

void flux_to_Bits_initialize(struct Bits* lire, uint8_t* flux, size_t taille);
/* Fonction permettant l'initialisation de la lecture bit à bit */
uint8_t lire_un_bit(struct Bits* lire);
/* Fonction permettant de lire un bit*/
uint16_t lire_des_bits(struct Bits* lire, int nbre_bits);
/* Fonction permettant de lire nbre_bits bit */
int Reconnaitre_symbol_Huffman(struct abr* arbre, struct Bits* lire);
/* Fonction permettant de reconnaitre un symbol en lisant des bits bit à bit */
int Huffman_DC(struct abr* arbre, struct Bits* lire, int dc_pred);
/* Fonction permettant de retourner les coefficients DC */
int* Huffman_AC(struct abr* arbre, struct Bits* lire);
/* Fonction permettant de retourner les coefficients AC */

# endif

