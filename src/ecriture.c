#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../include/ecriture.h"

void decimal_to_binary(uint8_t valeur){
    int i = 8;
    char binaire[9];
    while( i >= 0 ){
        binaire[i] = (char) (valeur % 2) + '0';
        valeur = valeur /2;
        i--;
    }
    binaire[8] = '\0';
    printf("%s",binaire);
}


void ecriture(uint32_t longeur,uint32_t largeur,struct pixel** matrice,char* nom, uint8_t nb_comp){
    uint32_t len = strlen(nom);
    uint32_t debut = 0;
    uint32_t fin = 0;
    for(uint32_t j = 0 ; j < len ; j++){
        if (nom[j] == '/'){
            debut = j + 1;
        }
        if (nom[j] == '.'){
            fin = j;
        }
    }
    char nv_nom[100];
    uint32_t k =7;
    strcpy(nv_nom, "images/");
    for (uint32_t j = debut ;j < fin; j++){
        nv_nom[k++] = nom[j];
    }
    nv_nom[k] = '\0';  
    if (nb_comp == 1) {
        strcat(nv_nom, ".pgm");
    }  
    else if (nb_comp == 3) {
        strcat(nv_nom, ".ppm");
    }
    FILE* fichier = fopen(nv_nom,"wb");
    if (nb_comp == 1) {
        fprintf(fichier, "P5\n%u %u\n255\n", largeur, longeur);
        for(uint32_t i = 0;i < longeur ; i++){
            for(uint32_t j = 0;j < largeur ;j ++){
                uint8_t pix = matrice[i][j].R;
                fwrite(&pix, 1, 1, fichier);
            }
        }
    }
    else if (nb_comp == 3) {
        fprintf(fichier, "P6\n%u %u\n255\n", largeur, longeur);
        for(uint32_t i = 0  ; i < longeur ; i++){
            for(uint32_t j = 0 ; j < largeur ;j++){
                struct pixel pix = matrice[i][j];
                fwrite(&pix, 1, 3, fichier);
            }
        }
    }

    fclose(fichier);
}

/* int main_bis() {
    struct pixel image[8][8];
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            image[i][j].R = 255;
            image[i][j].G = 0;
            image[i][j].B = 0;
        }
    }
    
    ecriture(8, 8,image);
    return 0;
}  */