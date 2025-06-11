#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


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


void copier_bloc(uint8_t bloc[8][8], uint8_t copie_bloc[8][8]) {
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            copie_bloc[i][j] = bloc[i][j] ;
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


void afficher_matrice(uint8_t matrice[8][8]) {
    for (uint8_t i = 0 ; i < 8 ; i++) {
        for (uint8_t j = 0 ; j < 8 ; j++) {
            printf("%4u ", matrice[i][j]) ;
        }
        printf("\n") ;
    }
    printf("\n") ;
}

void upsampling() {
    uint8_t matrice_input[8][8] = {
        {1, 2, 3, 4, 5, 6, 7, 8},
        {9, 10, 11, 12, 13, 14, 15, 16},
        {17, 18, 19, 20, 21, 22, 23, 24},
        {25, 26, 27, 28, 29, 30, 31, 32},
        {33, 34, 34, 36, 37, 38, 39, 40},
        {41, 42, 43, 44, 45, 46, 47, 48},
        {49, 50, 51, 52, 53, 54, 55, 56},
        {57, 58, 59, 60, 61, 62, 63, 64},
    } ;
    uint8_t f = 4 ; 

    uint8_t mat_h[8][f * 8] ;
    elargir_matrice_h(matrice_input, f, mat_h) ; 
    uint8_t mats_h[f][8][8] ;   /* LÃ  mats_h contient f matrices 8x8 dupliquÃ©s horizontalement */
    scinder_matrice_h(f, mat_h, mats_h) ;
/* 
    for (uint8_t m = 0; m < f ; m++) {
        afficher_matrice(mats_h[m]) ; 
    } */

    uint8_t matrice_finale[f][f][8][8] ;
    for (uint8_t mx = 0 ; mx < f ; mx++) {
        uint8_t mat_v[8 * f][8] ;
        elargir_matrice_v(mats_h[mx], f, mat_v) ;
        uint8_t mats_v[f][8][8] ;
        scinder_matrice_v(f, mat_v, mats_v) ;
            
        /* LÃ  mats_v contient f matrices 8x8 dupliquÃ©es verticalement */
        /* On prend chacune de ses matrices et on les mets dans matrice_finale
        au bon indice */
        for (uint8_t my = 0 ; my < f ; my ++) {
            copier_bloc(mats_v[my], matrice_finale[my][mx]) ;
        }
    }

    /* LÃ  matrice_finale contient f*f matrices */  
    for (uint8_t mat_x = 0 ; mat_x < f ; mat_x ++) {
        for (uint8_t mat_y = 0 ; mat_y < f ; mat_y ++) {
            printf("Matrice[%u][%u] : \n", mat_x, mat_y) ;
            afficher_matrice(matrice_finale[mat_x][mat_y]) ;
        }
    }

}

