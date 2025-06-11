# include "../include_tests/test_zigzag.h"
# include "../include_tests/test_IQ.h"
# include "../include_tests/test_upsampling.h"
# include "../include_tests/test_construction_arbre_huff.h"
int main(){
    printf( "\n");

    printf( "Test de Zigzag inverse : \n");
    printf( "\n");

    test_zigzag();

    printf( "\n");
    printf( "\n");

    printf( "Test de Quantifictation inverse : \n");
    printf( "\n");

    test_quantification();

    printf( "\n");
    printf( "\n");

    printf( "Test du upsampling : \n");
    printf( "\n");

    upsampling();
    printf( "\n");
    printf( "\n");

    printf( "Test de la construction de l'arbre de Huffman : \n");
    printf( "\n");
    abr_Huff();
    return EXIT_SUCCESS;
}
