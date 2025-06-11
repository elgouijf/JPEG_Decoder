#include "../include/reconstruction.h"


void construire_arbres_huff_DC(struct abr* arbres_DC[NB_MAX_TABS_HUFFMAN], Donnees_section_DHT data_dht) {

    for (uint8_t i = 0 ; i < data_dht.nb_tabs_DC ; i++) {
        Table_de_Huffman tab_huff_dc = data_dht.tab_Huffman[DC][i] ;
        arbres_DC[i] = decodage_huff(tab_huff_dc) ;
    }
}


void construire_arbres_huff_AC(struct abr* arbres_AC[NB_MAX_TABS_HUFFMAN], Donnees_section_DHT data_dht) {

    for (uint8_t i = 0 ; i < data_dht.nb_tabs_AC ; i++) {
        Table_de_Huffman tab_huff_ac = data_dht.tab_Huffman[AC][i] ;
        arbres_AC[i] = decodage_huff(tab_huff_ac) ;
    }
}

void detruire_arbres_DC(struct abr* arbres_DC[NB_MAX_TABS_HUFFMAN], uint8_t nb_tabs_DC) {
    for (uint8_t i = 0; i < nb_tabs_DC; i++) {
        free_abr(arbres_DC[i]);
    }
}

void detruire_arbres_AC(struct abr* arbres_AC[NB_MAX_TABS_HUFFMAN], uint8_t nb_tabs_AC) {
    for (uint8_t i = 0; i < nb_tabs_AC; i++) {
        free_abr(arbres_AC[i]);
    }
}



void reconstruire_bloc(struct Bits* lecteur_de_bits,
                     int dc_prec[3], struct abr* arbre_huff_dc, struct abr* arbre_huff_ac, uint8_t* tab_quant,
                      uint8_t bloc[8][8], uint8_t c) {

    /* On décode DC et les coeffs AC */
    int dc = Huffman_DC(arbre_huff_dc, lecteur_de_bits, dc_prec[c]) ;
    dc_prec[c] = dc ;
    int* ac = Huffman_AC(arbre_huff_ac, lecteur_de_bits) ;

    /* On stocke les coeffs DC et AC dans un vecteur */
    int vect_dc_ac[64] ;
    vect_dc_ac[0] = dc ;
    for (uint8_t i = 1 ; i <= 63 ; i++) {
        vect_dc_ac[i] = ac[i - 1] ;
    }
    free(ac) ;

    /* Quantification inverse */
    inverse_quantification(vect_dc_ac, tab_quant) ;

    /* Réorganisation zig-zag */
    int (*matrice_zig_zag)[8] = inverse_zig_zag(vect_dc_ac) ;


    /* Transformation en consinus discrète inverse */
/*     for (uint8_t i = 0 ; i < 8 ; i++) {
        for (uint8_t j = 0 ; j < 8 ; j++) {
            bloc[i][j] = amplitude_to_color_8(i, j, matrice_zig_zag) ;
        }
    } */

        amplitude_to_color_8_R(matrice_zig_zag, bloc) ;
}



void reconstruire_pixels(MCU mcu, uint8_t h_Y, uint8_t v_Y, 
                        struct pixel matrice_de_pixels[8 * v_Y][8 * h_Y],
                        uint8_t nb_composantes) {
    /* Nombre de blocs par composante : imposé par le facteur
    d'échantillonnage de Y */
    uint8_t nb_blocs = h_Y * v_Y ;
    if (nb_composantes == 3) {
        for (uint8_t b = 0 ; b < nb_blocs ; b++) {
            uint8_t bloc_i = (b / h_Y) * 8 ;  
            uint8_t bloc_j = (b % h_Y) * 8 ;
            for (uint8_t i = 0 ; i < 8 ; i++) {
                for (uint8_t j = 0 ; j < 8 ; j++) {
                    uint8_t Y = mcu.Y[b][i][j] ;
                    uint8_t Cb = mcu.Cb[b][i][j] ;
                    uint8_t Cr = mcu.Cr[b][i][j] ;
                    matrice_de_pixels[i + bloc_i][j + bloc_j].R = get_red(Y, Cb, Cr) ;
                    matrice_de_pixels[i + bloc_i][j + bloc_j].G = get_green(Y, Cb, Cr) ;
                    matrice_de_pixels[i + bloc_i][j + bloc_j].B = get_blue(Y, Cb, Cr) ;
                    }}}}
    else if (nb_composantes == 1) {
        for (uint8_t b = 0 ; b < nb_blocs ; b++) {
        uint8_t bloc_i = (b / h_Y) * 8 ;  
        uint8_t bloc_j = (b % h_Y) * 8 ;
        for (uint8_t i = 0 ; i < 8 ; i++) {
            for (uint8_t j = 0 ; j < 8 ; j++) {
                uint8_t Y = mcu.Y[b][i][j] ;
                matrice_de_pixels[i + bloc_i][j + bloc_j].R = get_red(Y, 128, 128) ;
                matrice_de_pixels[i + bloc_i][j + bloc_j].G = get_green(Y, 128, 128) ;
                matrice_de_pixels[i + bloc_i][j + bloc_j].B = get_blue(Y, 128, 128) ;}}}}
}


struct pixel ** initialiser_image_finale(uint16_t hauteur, uint16_t largeur) {
    struct pixel** image_finale = malloc(hauteur * sizeof(struct pixel*));
    if (image_finale == NULL) {
        fprintf(stderr, "Allocation mémoire pour la hauteur de l'image échouée.\n") ;
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < hauteur; i++) {
        image_finale[i] = malloc(largeur * sizeof(struct pixel));
        if (image_finale[i] == NULL) {
            fprintf(stderr, "Allocation mémoire pour la largeur de l'image échouée.\n") ;
            exit(EXIT_FAILURE);
        }
    }
    return image_finale ;
}

void free_image(struct pixel ** image_finale, uint16_t hauteur) {
    for (uint16_t i = 0; i < hauteur; i++) {
        free(image_finale[i]) ;
    }
    free(image_finale) ;
}

void construire_composante_mcu(MCU* mcu_courant,struct Bits* lecteur_de_bits, 
                                int dc_prec[3], struct abr* arbre_huff_dc, struct abr* arbre_huff_ac, uint8_t* tab_quant, uint8_t h_comp, 
                                uint8_t v_comp, uint8_t composante) {
    /* Nombre de blocs de la composante courante :
    donné par le facteur d'échantillonnage = h x v */
    uint8_t nb_blocs = h_comp * v_comp ;

    /* Parcours des blocs */
    uint8_t bloc[8][8] ;
    for (uint8_t b = 0 ; b < nb_blocs ; b++ ) {

        /* On reconstruit le bloc : extraction, décompression,
        quantification inverse, réorganisation zig-zag, iDCT */
        reconstruire_bloc(lecteur_de_bits, dc_prec, arbre_huff_ac, arbre_huff_dc,
         tab_quant, bloc, composante) ;
        switch (composante) {
            case 0 :
                copier_bloc(bloc, mcu_courant->Y[b]) ;
                break ;
            case 1 :
                copier_bloc(bloc, mcu_courant->Cb[b]) ;
                break ; 
            case 2 : 
                copier_bloc(bloc, mcu_courant->Cr[b]) ;
                break ;
            default:
                fprintf(stderr, "Composante invalide \n") ;
                break ;
        }
    }
    /* Fin de parcours de blocs */
}


void remplir_image_finale(struct pixel** image_finale, uint32_t mcu_i, uint32_t mcu_j,
                        uint8_t h_Y, uint8_t v_Y, uint16_t hauteur, uint16_t largeur,
                        struct pixel matrice_de_pixels[8 * v_Y][8 * h_Y], 
                        uint32_t* count_pixels) {

    for (uint8_t i = 0 ; i < 8 * v_Y ; i++) {
        for (uint8_t j = 0 ; j < 8 * h_Y ; j++) {
            uint32_t x = (mcu_i * 8 * v_Y)+ i ;
            uint32_t y = (mcu_j * 8 * h_Y) + j ;

            if (x < hauteur && y < largeur) {
                image_finale[x][y] = matrice_de_pixels[i][j] ;
                (*count_pixels) ++ ;
            }
        }
    }
}