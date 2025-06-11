#include "../include/reconstruction.h"
#include <string.h>
#include <stdio.h>


int main(int argc, char** argv) {
   
    if (argc != 2) {
        fprintf(stderr, "Un seul fichier JPEG en argument de : %s\n", argv[0]) ;
        exit(EXIT_FAILURE) ;
    }

    /* Ouverture de fichier_jpeg en mode lecture binaire */
    FILE* f_jpeg = fopen(argv[1], "rb") ;
    if (f_jpeg == NULL) {
        fprintf(stderr, "Ouverture impossible du fichier : %s\n", argv[1]) ;
        return EXIT_FAILURE ;
    }

    /* Extraction de l'en-tête */
    Donnees_entete_JPEG data_jpeg = extraire_sections(f_jpeg) ;

    /* Là  il faut lire bit par bit en décodant au fur et à mesure */

    /* D'abord, obtenir la taille des données brutes */
    uint32_t taille_donnees_brutes = recuperer_taille_donnees_brutes(f_jpeg) ;

    /* Dimensions de l'image */
    uint16_t hauteur = data_jpeg.data_sof0.hauteur ;
    uint16_t largeur = data_jpeg.data_sof0.largeur ;

    /* Nombre de blocs par MCU : conditionné par le facteur 
    d'échantillonnage de Y */
    uint8_t h_Y = data_jpeg.data_sof0.fact_ech_h[0] ;
    uint8_t v_Y = data_jpeg.data_sof0.fact_ech_v[0] ;
 
    /* Récupération du nombre de blocs 8 x 8  */
    uint32_t nb_MCUs_largeur = nb_MCUs_H(largeur, h_Y) ;
    uint32_t nb_MCUs_hauteur = nb_MCUs_V(hauteur, v_Y) ;  
    uint32_t nb_MCUs = nb_MCUs_hauteur * nb_MCUs_largeur ;
    //printf("Nombre des MCUs : %u\n", nb_MCUs) ;
    //printf("Nombre total des blocs : %u\n", nb_MCUs * h_Y * v_Y) ;

    /* On lit le flux */
    uint8_t* flux = malloc(taille_donnees_brutes) ;     /* à libérer après */
    size_t nb_oct_lus = fread(flux, 1, taille_donnees_brutes, f_jpeg) ;

    if (nb_oct_lus != taille_donnees_brutes) {
        fprintf(stderr, "Les données brutes n'ont pas toutes été lues \n") ;
        exit(EXIT_FAILURE) ;
    }

    /* Construction une fois pour toute des arbres de Huffman */
    struct abr* arbres_DC[NB_MAX_TABS_HUFFMAN] ;
    construire_arbres_huff_DC(arbres_DC, data_jpeg.data_dht) ;

    struct abr* arbres_AC[NB_MAX_TABS_HUFFMAN] ;
    construire_arbres_huff_AC(arbres_AC, data_jpeg.data_dht) ;
    

    /* On initialise le lecteur des bits */
    struct Bits lecteur_de_bits ;
    flux_to_Bits_initialize(&lecteur_de_bits, flux, taille_donnees_brutes) ;

    /* Nombre de composantes : */
    uint8_t nb_composantes = data_jpeg.data_sof0.N ;

    /* Initialisation de DC précédent à 0 pour les 3 composantes */
    int dc_prec[3] = {0} ;

    //struct pixel image_finale[hauteur][largeur] ;       /* À allouer manuellement plus tard +++ */

    /* Allocation de la mémoire pour l'image finale */
    struct pixel** image_finale = initialiser_image_finale(hauteur, largeur) ;
    uint32_t count_pixels = 0 ;

    /* Parcours des MCUs */
    for (uint32_t mcu = 0 ; mcu < nb_MCUs ; mcu ++) {
        //printf("MCU %u : \n", mcu) ;

        MCU mcu_courant = {0} ;

        /* Parcours des composantes pour construire le MCU décodé */
        for (uint8_t c = 0 ; c < nb_composantes ; c++) {

            /* Nombre de blocs de la composante courante :
            donné par le facteur d'échantillonnage = h x v */
            uint8_t h = data_jpeg.data_sof0.fact_ech_h[c] ;
            uint8_t v = data_jpeg.data_sof0.fact_ech_v[c] ;

            /* On récupère l'indice de i_C de la composante courante */
            /* +++ Dans SOF, l'ordre des composantes est toujours Y puis Cb puis Cr +++ */
            uint8_t i_C_SOF0 = data_jpeg.data_sof0.i_C[c] ;
            uint8_t index_i_C_SOS = trouver_i_C_correspondant(data_jpeg.data_sos, i_C_SOF0, data_jpeg.data_sof0.N) ;

            /* On récupère l'indice i_Q de la table de quantification associée */
            uint8_t i_Q = data_jpeg.data_sof0.i_Q[c] ;

            /* On récupère la table de quantification */
            uint8_t* tab_quant = data_jpeg.data_dqt.val_tab_quant[i_Q] ;

            /* On récupère les indices des tables de Huffman */
            uint8_t i_H_DC = data_jpeg.data_sos.i_H_DC[index_i_C_SOS] ;
            uint8_t i_H_AC = data_jpeg.data_sos.i_H_AC[index_i_C_SOS] ;


            /* On identifie les arbres de Huffman pour DC et AC */
            struct abr* arbre_huff_dc = arbres_DC[i_H_DC] ;
            struct abr* arbre_huff_ac = arbres_AC[i_H_AC] ;

            construire_composante_mcu(&mcu_courant, &lecteur_de_bits, dc_prec, arbre_huff_ac, arbre_huff_dc, tab_quant, h, v, c) ;

        }
        /* Fin de parcours de composantes */

        /* Sur-échantillonnage de Cb et Cr */
        uint8_t h_Cb = data_jpeg.data_sof0.fact_ech_h[1] ;
        uint8_t v_Cb = data_jpeg.data_sof0.fact_ech_v[1] ;

        uint8_t h_Cr = data_jpeg.data_sof0.fact_ech_h[2] ;
        uint8_t v_Cr = data_jpeg.data_sof0.fact_ech_v[2] ;

        if (nb_composantes == 3) {

            /* Sur-échantillonnage horizontal et vertical si h_Y > h_comp et v_Y > v_comp */
            if (h_Y > h_Cb && v_Y > v_Cb) {
                mcu_courant = upsampling(mcu_courant, h_Y, v_Y, h_Cb, v_Cb, h_Cr, v_Cr)  ;    
            }

            /* Sur-échantillonnage horizontal  : si h_Y > h_comp */
            else if ( h_Y > h_Cb) {
                mcu_courant = upsampling_h(mcu_courant, h_Y, v_Y, h_Cb, h_Cr, v_Cb, v_Cr) ; 
            }

            /* Sur-échantillonnage vertical : si v_Y > v_comp */
            else if (v_Y > v_Cb) {
                mcu_courant = upsampling_v(mcu_courant, h_Y, v_Y, v_Cb, v_Cr, h_Cb, h_Cr) ;
            }
        }

        /* Là le MCU est prêt : conversion YCbCr vers RGB */  
        struct pixel matrice_de_pixels[8 * v_Y][8 * h_Y] ;
        reconstruire_pixels(mcu_courant, h_Y, v_Y, matrice_de_pixels, nb_composantes) ;

        
        uint32_t mcu_i = mcu / nb_MCUs_largeur ;
        uint32_t mcu_j = mcu % nb_MCUs_largeur ;

        /* Écriture du MCU décodé dans l'image finale */
       remplir_image_finale(image_finale, mcu_i, mcu_j, h_Y, v_Y, hauteur, largeur, matrice_de_pixels, &count_pixels) ;
    }

    //printf("Nombre de pixels : %u\n", count_pixels) ;

    /* Écriture de l'image finale dans le fichier PPM / PGM */
    ecriture(hauteur, largeur, image_finale, argv[1], nb_composantes) ;

    /* Libération de la mémoire allouée */
    free_image(image_finale, hauteur) ;
    free(flux) ;
    detruire_arbres_DC(arbres_DC, data_jpeg.data_dht.nb_tabs_DC) ;
    detruire_arbres_AC(arbres_AC, data_jpeg.data_dht.nb_tabs_AC) ;

    /* Fermeture du fichier JPEG */
    fclose(f_jpeg) ;
    return EXIT_SUCCESS ;
}
