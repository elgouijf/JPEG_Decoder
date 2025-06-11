#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


void elargir_matrice_h(uint8_t matrice_input[8][8], uint8_t facteur, uint8_t matrice_output[8][8 * facteur]);
void scinder_matrice_h(uint8_t facteur, uint8_t matrice_input[8][8 * facteur], uint8_t tab_matrices[facteur][8][8]);
void elargir_matrice_v(uint8_t matrice_input[8][8], uint8_t facteur, uint8_t matrice_output[8 * facteur][8]);
void copier_bloc(uint8_t bloc[8][8], uint8_t copie_bloc[8][8]);
void scinder_matrice_v(uint8_t facteur, uint8_t matrice_input[8 * facteur][8], uint8_t tab_matrices[facteur][8][8]);
void afficher_matrice(uint8_t matrice[8][8]);
void upsampling();

