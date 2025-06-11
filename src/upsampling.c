#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "../include/upsampling.h"



void copier_bloc(uint8_t bloc[8][8], uint8_t copie_bloc[8][8]) {
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            copie_bloc[i][j] = bloc[i][j] ;
}


uint16_t nb_MCUs_H(uint16_t largeur, uint8_t h_Y) {
    if (largeur % (8 * h_Y) == 0) {
        return largeur / (8 * h_Y) ;
    }
    else { 
        return  (largeur / (8 * h_Y)) + 1 ;
    }
}

uint16_t nb_MCUs_V(uint16_t hauteur, uint8_t v_Y) {
    if (hauteur % (8 * v_Y) == 0) {
        return hauteur / (8 * v_Y) ;
    }
    else { 
        return  (hauteur / (8 * v_Y)) + 1 ;
    }
}


void elargir_matrice_h(uint8_t matrice_input[8][8], uint8_t facteur, uint8_t matrice_output[8][8 * facteur]) {
    uint8_t f ;
    for (uint8_t i = 0 ; i < 8 ; i++) {
        uint8_t id_col = 0 ;
        for (uint8_t j = 0 ; j < 8 ; j++) {
            uint8_t coeff = matrice_input[i][j] ;
            for (f = 0 ; f < facteur ; f++) {
                matrice_output[i][id_col + f] = coeff ;
            }
            id_col += f ;
        }
    }
}


void scinder_matrice_h(uint8_t facteur, uint8_t matrice_input[8][8 * facteur], uint8_t tab_matrices[facteur][8][8]) {
    uint8_t id_der_col ;
    for (uint8_t f = 0 ; f < facteur ; f++) {
        id_der_col = 8 * f ;  
        for (uint8_t i = 0 ; i < 8 ; i++) {
            for (uint8_t j = 0 ; j < 8 ; j++) {
                tab_matrices[f][i][j] = matrice_input[i][id_der_col + j] ;
            }
        }
    }
}


void elargir_matrice_v(uint8_t matrice_input[8][8], uint8_t facteur, uint8_t matrice_output[8 * facteur][8]) {
    uint8_t f ;
    for (uint8_t j = 0 ; j < 8 ; j++) {
        uint8_t id_l = 0 ;
        for (uint8_t i = 0 ; i < 8 ; i++) {
            uint8_t coeff = matrice_input[i][j] ;
            for (f = 0 ; f < facteur ; f++) {
                matrice_output[f + id_l][j] = coeff ;
            }
            id_l += f ;
        }
    }
}


void scinder_matrice_v(uint8_t facteur, uint8_t matrice_input[8 * facteur][8], uint8_t tab_matrices[facteur][8][8]) {
    uint8_t id_der_ligne ;
    for (uint8_t f = 0 ; f < facteur ; f++) {
        id_der_ligne = 8 * f ;
        for (uint8_t j = 0 ; j < 8 ; j++) {
            for (uint8_t i = 0 ; i < 8 ; i++) {
                tab_matrices[f][i][j] = matrice_input[id_der_ligne + i][j] ;
            }
        }
    }
}


void elargir_puis_scinder(uint8_t bloc_input[8][8], uint8_t fact_h, uint8_t fact_v,
                        uint8_t blocs_output[fact_h * fact_v][8][8])  

{
    uint8_t bloc_elargi_h[8][8 * fact_h] ;
    elargir_matrice_h(bloc_input, fact_h, bloc_elargi_h) ;

    uint8_t bloc_scinde_h[fact_h][8][8] ;
    scinder_matrice_h(fact_h, bloc_elargi_h, blocs_output) ;
    uint8_t id_der_bloc = 0;
    for (uint8_t i = 0 ; i < fact_h ; i++) {
        uint8_t bloc_elargi_v[8 * fact_v][8];
        elargir_matrice_v(bloc_scinde_h[i], fact_h, bloc_elargi_v);

        uint8_t bloc_scinde_v[fact_v][8][8];
        scinder_matrice_v(fact_v, bloc_elargi_v, bloc_scinde_v);

        for (uint8_t j = 0 ; j < fact_v ; j++) {
            copier_bloc(bloc_scinde_v[j], blocs_output[id_der_bloc++]);
        }
    } 
}



MCU upsampling_h(MCU mcu, uint8_t h_Y, uint8_t v_Y, uint8_t h_Cb, uint8_t h_Cr, uint8_t v_Cb, uint8_t v_Cr) {
    MCU mcu_upsampled = {0} ; 

    /* On recopie la composante Y dans le nouveau MCU */
    for (uint8_t n = 0 ; n < h_Y * v_Y ; n++) {
        copier_bloc(mcu.Y[n], mcu_upsampled.Y[n]) ;
    }

    /* Facteur d'élargissement des blocs pour Cb et Cr */
    uint8_t fact_dup_Cb = h_Y / h_Cb ; 
    uint8_t fact_dup_Cr = h_Y / h_Cr ; 

    /* On élargit chaque bloc Cb en le scindant en fact_dup_Cb blocs horizontalement */
    uint8_t id_der_bloc_Cb = 0 ; 
    for (uint8_t bloc = 0 ; bloc < h_Cb * v_Cb ; bloc ++) {

        /* On élargit le bloc selon le facteur */
        uint8_t bloc_elargi_Cb[8][8 * fact_dup_Cb] ;
        elargir_matrice_h(mcu.Cb[bloc], fact_dup_Cb, bloc_elargi_Cb) ;

        /* Puis on scinde le bloc resultat en fact_dup_Cb blocs 8x8 */
        uint8_t bloc_scinde_Cb[fact_dup_Cb][8][8] ;
        scinder_matrice_h(fact_dup_Cb, bloc_elargi_Cb, bloc_scinde_Cb) ;

        /* On met les blocs 8x8 issus dans le MCU */
        for (uint8_t k = 0 ; k < fact_dup_Cb ; k++) {
            copier_bloc(bloc_scinde_Cb[k], mcu_upsampled.Cb[id_der_bloc_Cb ++]) ;
        } 
    }

    /* On élargit chaque bloc Cr en le scindant en fact_dup_Cr blocs horizontalement */
    uint8_t id_der_bloc_Cr = 0 ; 
    for (uint8_t bloc = 0 ; bloc < h_Cr * v_Cr ; bloc ++) {

        /* On élargit le bloc selon le facteur */
        uint8_t bloc_elargi_Cr[8][8 * fact_dup_Cr] ;
        elargir_matrice_h(mcu.Cr[bloc], fact_dup_Cr, bloc_elargi_Cr) ;

        /* Puis on scinde le bloc resultat en fact_dup_Cr blocs 8x8 */
        uint8_t bloc_scinde_Cr[fact_dup_Cr][8][8] ;
        scinder_matrice_h(fact_dup_Cr, bloc_elargi_Cr, bloc_scinde_Cr) ;
        
        /* Enfin, on met les blocs ainsi construits dans le MCU */
        for (uint8_t k = 0 ; k < fact_dup_Cr ; k++) {
            copier_bloc(bloc_scinde_Cr[k], mcu_upsampled.Cr[id_der_bloc_Cr ++]) ;
        }  
    }
    return mcu_upsampled ;
}


MCU upsampling_v(MCU mcu, uint8_t h_Y, uint8_t v_Y, uint8_t v_Cb, uint8_t v_Cr, uint8_t h_Cb, uint8_t h_Cr) {
    MCU mcu_upsampled ; 

    /* On recopie la composante Y dans le nouveau MCU */
    for (uint8_t n = 0 ; n < h_Y * v_Y ; n++) {
        copier_bloc(mcu.Y[n], mcu_upsampled.Y[n]) ;
    }

    /* Facteur d'élargissement des blocs pour Cb et Cr */
    uint8_t fact_dup_Cb = v_Y / v_Cb ; 
    uint8_t fact_dup_Cr = v_Y / v_Cr ; 

    /* On élargit chaque bloc Cb en le scindant en fact_dup_Cb blocs verticalement */
    uint8_t id_der_bloc_Cb = 0 ; 
    for (uint8_t bloc = 0 ; bloc < h_Cb * v_Cb ; bloc ++) {

        /* On élargit le bloc selon le facteur */
        uint8_t bloc_elargi_Cb[8 * fact_dup_Cb][8] ;
        elargir_matrice_v(mcu.Cb[bloc], fact_dup_Cb, bloc_elargi_Cb) ;

        /* Puis on scinde le bloc resultat en fact_dup_Cb blocs 8x8 */
        uint8_t bloc_scinde_Cb[fact_dup_Cb][8][8] ;
        scinder_matrice_v(fact_dup_Cb, bloc_elargi_Cb, bloc_scinde_Cb) ;

        /* On met les blocs 8x8 issus dans le MCU */
        for (uint8_t k = 0 ; k < fact_dup_Cb ; k++) {
            copier_bloc(bloc_scinde_Cb[k], mcu_upsampled.Cb[id_der_bloc_Cb ++]) ;
        }
    }

    /* On élargit chaque bloc Cr en le scindant en nb_fact_dup_Cr blocs verticalement */
    uint8_t id_der_bloc_Cr = 0 ; 
    for (uint8_t bloc = 0 ; bloc < h_Cr * v_Cr ; bloc ++) {

        /* On élargit le bloc selon le facteur */
        uint8_t bloc_elargi_Cr[8 * fact_dup_Cr][8] ;
        elargir_matrice_v(mcu.Cr[bloc], fact_dup_Cr, bloc_elargi_Cr) ;

        /* Puis on scinde le bloc resultat en fact_dup_Cr blocs 8x8 */
        uint8_t bloc_scinde_Cr[fact_dup_Cr][8][8] ;
        scinder_matrice_v(fact_dup_Cr, bloc_elargi_Cr, bloc_scinde_Cr) ;
        
        /* Enfin, on met les blocs ainsi construits dans le MCU */
        for (uint8_t k = 0 ; k < fact_dup_Cr ; k++) {
            copier_bloc(bloc_scinde_Cr[k], mcu_upsampled.Cr[id_der_bloc_Cr ++]) ;
        }  
    }
    return mcu_upsampled ;

}


MCU upsampling(MCU mcu,uint8_t h_Y, uint8_t v_Y, uint8_t h_Cb, uint8_t v_Cb,
                uint8_t h_Cr, uint8_t v_Cr) {
    MCU mcu_upsampled = {0} ;

    /* Recopie de Y dans le nv MCU */
    for (uint8_t n = 0 ; n < h_Y * v_Y ; n++) {
        copier_bloc(mcu.Y[n], mcu_upsampled.Y[n]) ;
    }

    uint8_t fact_dup_h_Cb = h_Y / h_Cb ;
    uint8_t fact_dup_v_Cb = v_Y / v_Cb ;

    uint8_t fact_dup_h_Cr = h_Y / h_Cr ;
    uint8_t fact_dup_v_Cr = v_Y / v_Cr ;

    /* Upsampling de Cb */
    uint8_t id_bloc_Cb = 0 ;
    for (uint8_t bloc = 0 ; bloc < h_Cb * v_Cb ; bloc++) {
        uint8_t bloc_elargi_h[8][8 * fact_dup_h_Cb];
        elargir_matrice_h(mcu.Cb[bloc], fact_dup_h_Cb, bloc_elargi_h);

        uint8_t blocs_h[fact_dup_h_Cb][8][8];
        scinder_matrice_h(fact_dup_h_Cb, bloc_elargi_h, blocs_h);

        for (uint8_t my = 0 ; my < fact_dup_v_Cb ; my++) {
            for (uint8_t mx = 0 ; mx < fact_dup_h_Cb ; mx++) {
                uint8_t bloc_v[8 * fact_dup_v_Cb][8];
                elargir_matrice_v(blocs_h[mx], fact_dup_v_Cb, bloc_v);

                uint8_t blocs_final[fact_dup_v_Cb][8][8];
                scinder_matrice_v(fact_dup_v_Cb, bloc_v, blocs_final);

                copier_bloc(blocs_final[my], mcu_upsampled.Cb[id_bloc_Cb++]);
            }
        }
    }

    /* Upsampling de Cr */
    uint8_t id_bloc_Cr = 0 ;
    for (uint8_t bloc = 0 ; bloc < h_Cr * v_Cr ; bloc++) {
        uint8_t bloc_elargi_h[8][8 * fact_dup_h_Cr];
        elargir_matrice_h(mcu.Cr[bloc], fact_dup_h_Cr, bloc_elargi_h);

        uint8_t blocs_h[fact_dup_h_Cr][8][8];
        scinder_matrice_h(fact_dup_h_Cr, bloc_elargi_h, blocs_h);

        for (uint8_t my = 0 ; my < fact_dup_v_Cr ; my++) {
            for (uint8_t mx = 0 ; mx < fact_dup_h_Cr ; mx++) {
                uint8_t bloc_v[8 * fact_dup_v_Cr][8];
                elargir_matrice_v(blocs_h[mx], fact_dup_v_Cr, bloc_v);

                uint8_t blocs_final[fact_dup_v_Cr][8][8];
                scinder_matrice_v(fact_dup_v_Cr, bloc_v, blocs_final);

                copier_bloc(blocs_final[my], mcu_upsampled.Cr[id_bloc_Cr++]);
            }
        }
    }

    return mcu_upsampled;
}
