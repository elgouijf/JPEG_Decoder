#ifndef _RECONSTRUCTION_H_
#define _RECONSTRUCTION_H_

#include "../include/Decodage_flux.h"
#include "../include/entete_jpeg.h"
#include "../include/IDCT.h"
#include "../include/IQZZ.h"
#include "../include/construction_arbre_huff.h"
#include "../include/YCbCr_to_RGB.h"
#include "../include/ecriture.h"
#include "../include/upsampling.h"
#include "../include/IDCT_rapide.h"
#include <string.h>
#include <stdio.h>

void construire_arbres_huff_DC(struct abr* arbres_DC[NB_MAX_TABS_HUFFMAN], Donnees_section_DHT data_dht) ;
void detruire_arbres_DC(struct abr* arbres_DC[NB_MAX_TABS_HUFFMAN], uint8_t nb_tabs_DC) ;


void construire_arbres_huff_AC(struct abr* arbres_AC[NB_MAX_TABS_HUFFMAN], Donnees_section_DHT data_dht) ;
void detruire_arbres_AC(struct abr* arbres_AC[NB_MAX_TABS_HUFFMAN], uint8_t nb_tabs_AC) ;


void reconstruire_bloc(struct Bits* lecteur_de_bits,
                     int dc_prec[3], struct abr* arbre_huff_dc, struct abr* arbre_huff_ac, uint8_t* tab_quant, uint8_t bloc[8][8], uint8_t c) ;
/* Fonction calculant et mettant les coefficients ac et dc correspondants au bloc en entrée qui sera ainsi rempli après
 quantification et appliquation du zigzag inverse. */

void reconstruire_pixels(MCU mcu, uint8_t h_Y, uint8_t v_Y, 
                        struct pixel matrice_de_pixels[8 * v_Y][8 * h_Y],
                        uint8_t nb_composantes) ;
/* Fonction qui remplit la matrice de pixels en entrée suivant le nombre de composantes de l'image, pour ce faire elle recourt aux méthodes get_color
du module YCbCr. */

struct pixel ** initialiser_image_finale(uint16_t hauteur, uint16_t largeur) ;
/* Fonction qui alloue de l'espace dans le tas pour l'images finale (argument de la fonction ecriture). */

void free_image(struct pixel ** image_finale, uint16_t hauteur) ;
/* Fonction qui libère l'espace alloué par image finale. */

void construire_composante_mcu(MCU* mcu_courant, struct Bits* lecteur_de_bits, 
                                int dc_prec[3], struct abr* arbre_huff_dc, struct abr* arbre_huff_ac, uint8_t* tab_quant, uint8_t h_comp, 
                                uint8_t v_comp, uint8_t compososante) ;

/* Fonction qui remplit les champs Y, Cb et Cr du mcu courant après reconstruction des blocs. */
void remplir_image_finale(struct pixel** image_finale, uint32_t mcu_i, uint32_t mcu_j,
                        uint8_t h_Y, uint8_t v_Y, uint16_t hauteur, uint16_t largeur,
                        struct pixel matrice_de_pixels[8 * v_Y][8 * h_Y], 
                        uint32_t* count_pixels) ;
/* Fonction qui permet de remplir l'image finale à partir de la matrice de pixels tout en faisant la gestion des trancatures. */

#endif