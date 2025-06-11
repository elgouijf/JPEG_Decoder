#include "../include_tests/test_zigzag.h" 

void test_zigzag() {
    // Vecteur de test
    printf("Le vecteur de test est :\n");
    int ZQ[64];
    for (uint32_t i = 0; i <= 63; i++) {
        ZQ[i] = i + 1;
        printf("%2u ", ZQ[i]);
        if ( (i + 1) % 8 == 0){printf("\n");}
    }
    printf( "\n");

    
    // Constuction d'une matrice 8×8 en appellant la fonction à tester 
    int (*U)[8] = inverse_zig_zag(ZQ);

    // Affichage 
    printf("La matrice construite en appliquant le zigzag inverse est :\n");

    for (uint32_t i = 0; i < 8; i++) {
        for (uint32_t j = 0; j < 8; j++) {
            /* Affichage des coefficient de la matrice
               la format %2u permet de les afficher avec u champ de largeur minimale de 2 même pour
               des chiifres ce qui permet une meilleure lisibilité : solution d'affichage proposée par l'IA */
            printf("%2u ", U[i][j]);
        }
        printf("\n");
    }

}
