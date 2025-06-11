#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define cos1 0.98078528
#define sin1 0.19509032
#define cos3 0.83146961
#define sin3 0.55557023
#define cos6 0.38268343
#define sin6 0.92387953
#define racine 0.70710678

// Fonction faisant les calcul de "stage 4" pour des entrées de type int16_t
void niveau4_int(double vect[8], int source[8]) {
    /* cette fonction ne sera appliquer que sur la matrice zigzag,
     et donc on sait au préalable qu'il sera appliquer sur ses lignes.*/
    vect[0] = source[0];
    vect[1] = source[4];
    vect[2] = source[2];
    vect[3] = source[6];
    vect[4] = 0.5*(source[1] - source[7]);
    vect[5] = racine*source[3];
    vect[6] = racine*source[5];
    vect[7] = 0.5*(source[1] + source[7]);
}
// Fonction faisant les calcul de "stage 4" pour des entrées de type double
void niveau4_double(double vect[8], double source[8]) {
    /* cette fonction ne sera appliquer que sur la matrice présentant l'IDCT 1D de la matrice zigzag,
     et donc on sait au préalable qu'il sera appliquer sur ses colones.*/
    vect[0] = source[0];
    vect[1] = source[4];
    vect[2] = source[2];
    vect[3] = source[6];
    vect[4] = 0.5*(source[1] - source[7]);
    vect[5] = racine*source[3];
    vect[6] = racine*source[5];
    vect[7] = 0.5*(source[1] + source[7]);
}

// Fonction faisant les calcul de "stage 3" 
void niveau3(double vect[8], double source[8]) {
    // on calcul au préalable la racine de 2 pour ne le pas refaire à chaque fois
    /* comme cette fonction sera une fois pour traiter des lignes et une autre pour 
    traiter des colones il faut avoir un booléen qui nous indique lequel à viser*/
        vect[0] = 0.5*(source[0] + source[1]);
        vect[1] = 0.5*(source[0] - source[1]);
        vect[2] = racine*(cos6 * source[2] - sin6 * source[3]);
        vect[3] = racine*(cos6 * source[3] + sin6 * source[2]);
        vect[4] = 0.5*(source[4] + source[6]);
        vect[5] = 0.5*(source[7] - source[5]);
        vect[6] = 0.5*(source[4] - source[6]);
        vect[7] = 0.5*(source[7] + source[5]);
  
}

// Fonction faisant les calcul de "stage 2"
void niveau2(double vect[8], double source[8]) {
    /* comme cette fonction sera appeller une fois pour traiter des lignes et une autre pour 
    traiter des colones il faut avoir un booléen qui nous indique lequel à viser*/
        vect[0] = 0.5*(source[0] + source[3]);
        vect[1] = 0.5*(source[1] + source[2]);
        vect[2] = 0.5*(source[1] - source[2]);
        vect[3] = 0.5*(source[0] - source[3]);
        vect[4] = source[4]*cos3 - source[7]*sin3 ;
        vect[5] = source[5]*cos1 - source[6]*sin1;
        vect[6] = source[6]*cos1 + source[5]*sin1;
        vect[7] = source[7]*cos3 + source[4]*sin3;
    
}

// Fonction faisant les calcul de "stage 1"
void niveau1(double vect[8], double source[8]) {
    /* comme cette fonction sera une fois pour traiter des lignes et une autre pour 
    traiter des colones il faut avoir un booléen qui nous indique lequel à viser*/
        vect[0] = 0.5*(source[0] + source[7]);
        vect[1] = 0.5*(source[1] + source[6]);
        vect[2] = 0.5*(source[2] + source[5]);
        vect[3] = 0.5*(source[3] + source[4]);
        vect[4] = 0.5*(source[3] - source[4]);
        vect[5] = 0.5*(source[2] - source[5]);
        vect[6] = 0.5*(source[1] - source[6]);
        vect[7] = 0.5*(source[0] - source[7]);
    
    }



void amplitude_to_color_8_R(int Matrice_zigzag[8][8], uint8_t output_bloc[8][8]) {

    double etage_4[8];
    double etage_3[8];
    double etage_2[8];
    double etage_1[8][8];
    double colonne[8]; 

    //Parcour sur les lignes
    for (int i = 0; i < 8; i++) {
        niveau4_int(etage_4, Matrice_zigzag[i]);
        niveau3(etage_3, etage_4);
        niveau2(etage_2, etage_3);
        niveau1(etage_1[i], etage_2);}

    //Parcour sur les colones
    for (int j = 0; j < 8; j++) {
         //On extracte la jème colonne de etage_1
         for (int i = 0; i < 8; i++) {
            colonne[i] = etage_1[i][j];
        }
        niveau4_double(etage_4, colonne);
        niveau3(etage_3, etage_4);
        niveau2(etage_2, etage_3);
        niveau1(colonne, etage_2);

        for (int i = 0; i < 8; i++) {
            etage_1[i][j] = colonne[i];}}
    
    for (int i = 0; i < 8; i++){
            for (int j = 0; j < 8; j++){
            //On normalise puis on arrondie la valeur à mettre dans le output : 
                double valeur = etage_1[i][j]*8 + 128;
                if (valeur > 255){
                    output_bloc[i][j] = 255;
                }
                else if (valeur < 0){
                    output_bloc[i][j] = 0;
                }
                else{
                    output_bloc[i][j] = (uint8_t)round(valeur); }
}
}}