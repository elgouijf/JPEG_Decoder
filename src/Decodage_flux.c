# include <stdint.h>
# include <stdlib.h>
# include <stdio.h>
# include "../include/construction_arbre_huff.h"
# include "../include/Decodage_flux.h"

// La lecture dans le flux de bits nécessite une lecture bit à bit, ce qui est en général
// impossible en C, mais là on va recourir à une structure qui va nous permettre de faire exactemetn ça


void flux_to_Bits_initialize(struct Bits* lire, uint8_t* flux, size_t taille){
    lire->flux = flux;
    lire->taille = taille;
    lire->position_bit = 0;
    lire->position_octet = 0;
    lire->depassement = 0; 
}

uint8_t lire_un_bit(struct Bits* lire){
    uint8_t octet;
    uint8_t* flux;
    size_t position_octet;
    uint8_t position_bit;
    uint8_t bit;
    flux = lire->flux;
    position_octet = lire->position_octet;
    position_bit = lire->position_bit;
    if (position_octet >= lire->taille){
        lire->depassement = 1; // la présence de ce booléen  dans notre strucutre va permettre la gestion des dépassement en bas
                               // peu importe ce qu'on retourne à ce stade , il sera négligé.
        return 0;
     }
    octet = flux[position_octet];
    bit = (octet >> (7 - position_bit)) & 1; // on utilise un masque pour récupérer le bit en question
    lire->position_bit += 1; // passer au bit suivant

    //incrémenter la position de bit peut impliquer celle de la position de l'octet
    if (lire->position_bit == 8){ 
        lire->position_bit = 0;
        lire->position_octet += 1;
        if (flux[lire->position_octet - 1] == 0xff) {
            lire->position_octet += 1 ;
        }
    }
    return bit;
}

uint16_t lire_des_bits(struct Bits* lire, int nbre_bits){ 
                                                        
    uint16_t mes_bits = 0;
    uint8_t i;
    for (i = 0; i < nbre_bits; i++){
        uint8_t bit = lire_un_bit(lire);
        if (lire->depassement == 1){ 
            return 0x0000 ; }              // on est obligé de retourner un uint16_t donc je retourne 0x0000, mais lire->depassement == 1 
                                           // va nous permettre de le négliger
        mes_bits = (mes_bits << 1) | bit;   
    }
    return mes_bits;
}


int Reconnaitre_symbol_Huffman(struct abr* arbre, struct Bits* lire){
    struct abr* parcour = arbre;
    while (parcour->fg != NULL || parcour->fd != NULL){
        uint8_t bit = lire_un_bit(lire);
        if (lire->depassement == 1) {return -1; } // ici par contre on peut retourner -1 car 
                                                  // la fonction on question retourne un int( normalement magnitude est un uint8_t 
                                                  // mais ça ne permet pas de gérer l'errueur, car si non on va lire des bits qui n'ont aucun sens
                                                  // et l'arbre peut retourner des errurs)
        if (bit == 0) {
            parcour = parcour->fg;
        }
        else {
            parcour = parcour->fd;
        }
    }
    int val = parcour->valeur;
    return val ;
}


int Huffman_DC(struct abr* arbre, struct Bits* lire, int dc_pred){ //les coefficients dc et ac peuvent etre négatifs => int

    // Récupération du symbole courant
    int magnitude = Reconnaitre_symbol_Huffman(arbre, lire);
    if (lire->depassement == 1 || magnitude == -1) { 
        fprintf(stderr, "Message d'erreur : %s\n", "Taille de flus dépassée");
        return -1;}

    if (magnitude == 0){ return dc_pred;}

    // On lit magnitude bits (on lit 6 bits par exemple)
    uint16_t index_magn = lire_des_bits(lire, magnitude);
    // Au lieu d'utuliser une structure (dictionnaire par exemple) pour stocker les tableaux
    // Correspondants à chauqe magnitude j' utilise les formules suivantes
    int marge;
    if ((index_magn >> (magnitude - 1)) & 1){
        marge = index_magn; // car seul les valeurs positifs commencent avec des 1, et le plus petit parmis eux
                       // est 2^(magnitude - 1) = index_min(positif) >> (magnitude - 1) et les autres s'obtiennent par incrémentation succéssive
    }
    else { marge = index_magn - (1 << magnitude) + 1; } 
                        // car seul les valeurs négatifs commencent avec des 0, et le plus petit parmi eux 
                        // est -2^(magnitude) + 1 = index_min - (1 << magnitude) + 1 = 0 - (1 << magnitude) + 1 
                        // et les autres s'obtiennent par incrémentation succéssive

    return dc_pred + marge;
}

// Contrairement à dc il y'a 63 coefficient ac , on ne retourne pas dpnc un int mais plutot un int*
int* Huffman_AC(struct abr* arbre, struct Bits* lire){
    int* ac_63 = malloc(63* sizeof(int));
    int i;
    for (i = 0; i < 63;){
    
        int code_rle = Reconnaitre_symbol_Huffman(arbre, lire);
        if (lire->depassement == 1 || code_rle  == -1) { 
            fprintf(stderr, "Message d'erreur : %s\n", "Taille de flus dépassée");
            free(ac_63);
            return NULL;} // comme là il s'agit dun pointeur on retourne plutot NULL au lieu de -1

        // On divise la valeure obtenue en 2 partie ( Codage RLE )
        // offset = nombre de 0 avant un coefficient non nulle
        int offset = (code_rle >> 4);
        // la magnitude est stockée dans les 4 bits du poids faible du premier coefficient non nul rencontré
        int magnitude = code_rle % 16;
        if ( offset == 0 && magnitude == 0 ){ // End of Block 0x00
            int j;
            // tous les coefficients qui restent sont nuls
            for( j = i; j < 63; j++){
                ac_63[j] = 0;
            }
            break;
        } 

        if ( offset == 15 && magnitude == 0){ // 16 coefficients nuls 0xF0
            int j;
            // les 16 coefficients suivants sont nuls
            for (j = 0; j < 16; j++){
                ac_63[i] = 0 ;
                i += 1;
            }
            continue;
        }

        if ( magnitude != 0 ){  // cas qlq 0xalpha.gamma
            int j;
            // on parcourt alpha coefficients nuls
            for (j = 0; j < offset; j++) {
                ac_63[i] = 0;
                i += 1;
            }

            uint16_t index_magn = lire_des_bits( lire, magnitude);
            int ac;
            if ((index_magn >> (magnitude - 1)) & 1){
                ac = index_magn; // car seul les valeurs positifs commencent avec des 1, et le plus petit parmis eux
                            // est 2^(magnitude - 1) = index_min(positif) >> (magnitude - 1) et les autres s'obtiennent par incrémentation succéssive
            }
            else { ac = index_magn - (1 << magnitude) + 1; } 
                                // car seul les valeurs négatifs commencent avec des 0, et le plus petit parmis eux
                                // est -2^(magnitude) - 1 = index_min - (1 << m) + 1 = 0 - (1 << m) + 1 
                                // et les autres s'obtiennent par incrémentation succéssive
                                    
            ac_63[i] = ac;
            i += 1 ;

        }

        else { // cas 0x?0
            fprintf(stderr, "Symbole invalide"); //Erreur

        }

    }
    return ac_63;
}