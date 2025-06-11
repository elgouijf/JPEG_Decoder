# include <stdint.h>
# include <stdlib.h>
# include <stdio.h>
# include <ctype.h>
# include <stdbool.h>
# include <string.h>
# include "../include/construction_arbre_huff.h"
# include "../include/entete_jpeg.h"

struct abr* creer_noeud(void){
    struct abr* noeud = malloc(sizeof(struct abr));
    noeud->fd = NULL;
    noeud->fg = NULL;
    noeud->valeur = 0;
    return noeud;
}

void ajouter_arborescence(struct abr* abr,uint32_t code,uint8_t symbole, uint8_t long_code){
    uint32_t tab[long_code];
    int i = long_code -1;
    while(i >= 0){
            tab[i] = code % 2; /* On écrit la valuer en binaire*/
            code = code / 2;
            i--;
    }
   struct abr* cour = abr;
   for (uint32_t  j = 0; j < long_code; j++){
      if (tab[j] == 0){
        if(cour->fg == NULL){
            
            cour->fg = creer_noeud();
        }
        cour = cour->fg;
      }else{

        if(cour->fd == NULL){
            cour->fd = creer_noeud();
        }
        cour = cour->fd;
      }
   }
   cour->valeur = symbole;
   
}

struct abr* decodage_huff(Table_de_Huffman tab_huff){

    uint8_t *longueurs = tab_huff.nb_symboles_par_longueur; // on récupère de la structure la table faisant correspondance entre 
                                                            // longueures et nombre de codes d'une taille
    uint8_t *symboles = tab_huff.tab_symboles;              // on récupère de la structure la table des symbole
    
    
    struct abr* abr = creer_noeud();
    int i;
    int k = 0;
    uint32_t code = 0;
    for (i = 0 ; i < 16; i++){ // on parcour toutlongueure possible
        for(uint32_t j = 0 ; j < longueurs[i] ; j++){ // on itère sur le nombre de codes rencontré
            ajouter_arborescence(abr, code ,symboles[k] , i+1); // on ajoute l'arborescence correspondante à (code,symbol)
            code = code + 1;  // incrémenter code de 1 permet d'obtenir un  nouveau code pour la même longueure
            k = k+1; // on consomme les symboles ( len(symboles) = sum(longueurs[i]))
        }
        code = code << 1; /* Décalge à gauche de 1 afin de respecter la condition du préfixe*/
    }
    return abr;
}


void free_abr(struct abr* a) { // on libère de la mémoire
    if (!a) return;
    free_abr(a->fg);
    free_abr(a->fd);
    free(a);
}

