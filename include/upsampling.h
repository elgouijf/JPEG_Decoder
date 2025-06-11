#ifndef _UPSAMPLING_H_
#define _UPSAMPLING_H_
#include <stdint.h>

#define NB_MAX_BLOCS_PAR_MCU 16
#define comp_Y 0
#define comp_Cb 1
#define comp_Cr 2

typedef struct {
    uint8_t Y[NB_MAX_BLOCS_PAR_MCU][8][8] ; 
    uint8_t Cb[NB_MAX_BLOCS_PAR_MCU][8][8] ;
    uint8_t Cr[NB_MAX_BLOCS_PAR_MCU][8][8] ;
} MCU ;


/* Fonction qui prend une matrice 8x8 et un facteur f 
et renvoie une matrice de taille 8x(8xf) en répliquant
chaque coeffs f fois horizontalement */

void elargir_matrice_h(uint8_t matrice_input[8][8], uint8_t facteur, uint8_t matrice_output[8][8 * facteur]) ;


/* Fonction utilitaire qui recopie une matrice 8x8 dans une autre matrice 8x8 */

void copier_bloc(uint8_t bloc[8][8], uint8_t copie_bloc[8][8]) ;


/* Fonction qui prend une matrice de taille 8x(8xf) et 
la scinde en f matrices de taille 8x8 (horizontalement mais c'est implicite) 
en les stockant dans un tableau */

void scinder_matrice_h(uint8_t facteur, uint8_t matrice_input[8][8 * facteur], uint8_t tab_matrices[facteur][8][8]) ;


/* Fonction qui prend une matrice 8x8 et un facteur f 
et renvoie une matrice de taille (8xf)x8 en répliquant
chaque coeffs f fois verticalement */

void elargir_matrice_v(uint8_t matrice_input[8][8], uint8_t facteur, uint8_t matrice_output[8 * facteur][8]) ;


/* Fonction qui prend une matrice de taille 8x(8xf) et 
la scinde en f matrices de taille 8x8 (verticalement) 
en les stockant dans un tableau */

void scinder_matrice_v(uint8_t facteur, uint8_t matrice_input[8 * facteur][8], uint8_t tab_matrices[facteur][8][8]) ;


/* Fonction qui prend un MCU et suréchantillonne ses composantes Cb et Cr 
horizontalement */

MCU upsampling_h(MCU mcu, uint8_t h_Y, uint8_t v_Y, uint8_t h_Cb, uint8_t h_Cr,
                    uint8_t v_Cb, uint8_t v_Cr) ;


/* Fonction qui prend un MCU et suréchantillonne ses composantes Cb et Cr 
verticalement */

MCU upsampling_v(MCU mcu, uint8_t h_Y, uint8_t v_Y, uint8_t v_Cb, uint8_t v_Cr, uint8_t h_Cb, uint8_t h_Cr) ;


/* Fonction utilitaire qui renvoie le nombre de MCUs en ligne */

uint16_t nb_MCUs_H(uint16_t largeur, uint8_t h_Y) ;


/* Fonction qui renvoie le nombre de MCUs en colonne */

uint16_t nb_MCUs_V(uint16_t hauteur, uint8_t v_Y) ; 


/* Fonction qui fait le upsampling horizontal puis vertical */

MCU upsampling(MCU mcu,uint8_t h_Y, uint8_t v_Y, uint8_t h_Cb, uint8_t v_Cb,
                uint8_t h_Cr, uint8_t v_Cr) ;


#endif