# include <stdint.h>
# include <stdlib.h>
# include <stdio.h>

void inverse_quantification( int Z[64], uint8_t * Q ){
    /* Déclaration des indices i et j pour le parcours des lignes et colones des entrées*/
    uint32_t i;
    for (i = 0; i < 64; i++){
        Z[i] = Z[i]*Q[i] ;
    
    }
}

int (*inverse_zig_zag(int *ZQ))[8] {
    /* Déclaration des indices de ligne et de colones de U*/
    uint32_t i = 0;
    uint32_t j = 0;
    /* Déclaration d'un indice de parcour sur le vecteur ZQ*/
    uint32_t k = 0;
    /* Déclaraion de la matrice à retourner*/
    static int U[8][8];


    while ( i + j < 15 ){
        U[i][j] = ZQ[k];
        /*Si i + j est pair le parcours zigzag se fait en montée diagonale*/
        if ((i + j)%2 == 0){  
            
            if ((0 < i ) ){ // 0 <= i - 1 
                /* Element intérieur à la matrice*/
                if ((j  < 7 )){ // j + 1 <= 7
                    /*Remontée*/
                    i = i - 1;
                    j = j + 1;}
                /* Element aux bores verticaux de la matrices*/
                else {
                    /* Descendre d'un élement*/
                    i = i + 1;
                    
                }
            }
            /* Element aux bores horizontaux de la matrices*/
            else {
                /* Se déplacer à droite */
                j = j + 1;}
            }
        
        else { 
            /*Si i + j est pair le parcours zigzag se fait en descente diagonale*/
            if ((0 < j) ){ // 0 <= j - 1
                if ((i  < 7 )){ //i + 1 <= 7
                    /* Element intérieur à la matrice */
                    /* Descente */
                    i = i + 1;
                    j = j - 1;
                    }
                else {
                    /* Element aux bores horisontaux de la matrices*/
                    j = j + 1;  
                }
            }
            else {
                if (i  < 7) { // i + 1 <= 7
                    /* Descendre d'un élement*/
                    i = i + 1;}
                else {
                    /* Se déplacer à gauche*/
                    j = j + 1;
                }
            }
        }
        /* Element suivant*/
        k += 1;}
        /* Mettre le dérnier élement*/
        U[i][j] = ZQ[k];
        /* on retourne la matrice U*/
        return U;
    }