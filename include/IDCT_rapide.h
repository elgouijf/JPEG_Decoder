#ifndef IDCT_rapide
#define IDCT_rapide
#include <stdint.h>
#include <stdio.h>


void niveau4_int(double vect[8], int source[8]) ;
// Fonction faisant les calculs inverses du "stage4" pour des matrices dont les élements sont de type int16_t
void niveau4_double(double vect[8], double source[8]) ;
// Fonction faisant les calculs inverses du "stage4" pour des matrice dont les élements sont de type double
void niveau3(double vect[8], double source[8]) ;
// Fonction faisant les calculs inverses du "stage3"
void niveau2(double vect[8], double source[8]) ;
// Fonction faisant les calculs inverses du "stage2"
void niveau1(double vect[8], double source[8]) ;
// Fonction faisant les calculs inverses du "stage1"


void amplitude_to_color_8_R(int Matrice_zigzag[8][8], uint8_t output[8][8]);
/* Fonction calculant sur une entrée output la transformée en cosinus discréte inverse 
   d'une matrice zigzaguée n = 8. Il s'agit  d'une version optimise de la procédure IDCT */
#endif