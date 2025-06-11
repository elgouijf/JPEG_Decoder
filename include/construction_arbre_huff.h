#ifndef construction_arbre_huff
#define construction_arbre_huff
# include <stdint.h>
# include <stdlib.h>
# include <stdio.h>
# include <ctype.h>
# include <stdbool.h>
# include "entete_jpeg.h"

struct abr{
    int valeur;
    struct abr* fg;
    struct abr* fd;
};

/* La fonction créee un noeud 
*/
struct abr* creer_noeud(void);
/* On ajoute la feuille correspondante au symbole
*/
void ajouter_arborescence(struct abr* abr,uint32_t code,uint8_t symbole,uint8_t long_code);
/* On vide la mémoire
*/
/* Fonction Décodage Huffmann
*/
struct abr* decodage_huff(Table_de_Huffman tab_huff);
/*Fonction libérant la mémoire*/
void free_abr(struct abr* a);

#endif