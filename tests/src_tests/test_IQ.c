# include "../include_tests/test_IQ.h"

void test_quantification() {

    // Création et affichage de la matrice de quantification
    printf("Matrice de quantification :\n");
    uint8_t Q[64];
    for (int i = 0; i < 64; i++) {
        Q[i] = 2; 
        printf("%2u ", Q[i]);
        if ((i+1) % 8 == 0){ printf("\n");}}
    printf("\n");

    // Création et affuchage du vecteur à tester
    printf("Vecteur à tester :\n");
    int Z[64];
    for (int i = 0; i < 64; i++) {
        Z[i] = 1;
        printf("%2u ", Z[i]);
        if ( (i+1) % 8 == 0){ printf("\n");}}
    printf("\n");

    // Quantificaton inverse
    inverse_quantification( Z , Q );
    

    // Affichage du résultat
    printf("Résultat de la quantification inverse :\n");
    for (int i = 0; i < 64; i++) {
        printf("%2u ", Z[i]);
        if ((i+1) % 8 == 0){ printf("\n");}
    }

    
}