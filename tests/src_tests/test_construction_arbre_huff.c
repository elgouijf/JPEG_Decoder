
#include <stdio.h>
# include <stdint.h>
# include <stdlib.h>
# include <ctype.h>
# include <stdbool.h>
#define MAX_LONGUEUR 16
#define NB_MAX_SYMB 256
struct abr{
    int valeur;
    struct abr* fg;
    struct abr* fd;
};

typedef struct {
    uint8_t nb_symboles_par_longueur[MAX_LONGUEUR] ;
    uint16_t nb_symboles ;
    uint8_t tab_symboles[NB_MAX_SYMB] ; 
} Table_de_Huffman ;
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
void print_abr_helper(struct abr* a, int depth) {
    if (a == NULL) return;
    print_abr_helper(a->fd, depth + 1);
    for (int i = 0; i < depth; i++) {
        printf("    ");
    }
    if (a->fd == NULL && a->fg == NULL)
        printf("%d\n", a->valeur);
    else
        printf(".\n");
    print_abr_helper(a->fg, depth + 1);
}

//Fonction print_abr générée par CHATGPT
void print_abr(struct abr* a) {
    print_abr_helper(a, 0);
}

void abr_Huff(void) {
    // On teste sur l'exemple donné. 
    
    Table_de_Huffman table;
    table.nb_symboles = 5;
    table.nb_symboles_par_longueur[0] = 0;
    printf("Table de Huffman :\n");
    printf("0");
    table.nb_symboles_par_longueur[1] = 3;
    printf("3");
    table.nb_symboles_par_longueur[2] = 2;
    printf("2");
    for (uint8_t i=3;i<16;i++){
        table.nb_symboles_par_longueur[i] = 0;
        printf("0");
    }
    uint8_t symb = 10;
    printf(" ");
    for (uint8_t i =0;i<5;i++){
        printf("%u ",symb);
        table.tab_symboles[i] = symb;
        symb++;
    }
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    struct abr* abr = decodage_huff(table);
    printf("Arbre de Huffman\n");
    print_abr(abr);

    // libération de la mémoire
    free_abr(abr);

}