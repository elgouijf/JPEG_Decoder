#ifndef _ENTETE_JPEG_H_
#define _ENTETE_JPEG_H_
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* Définition des marqueurs de sections */

#define SOI 0xffd8              /* Start Of Image */
#define EOI 0xffd9              /* End Of Image */
#define APP0 0xffe0             /* Application data */
#define COM 0xfffe              /* Commentaire */
#define DQT 0xffdb              /* Define Quantization Table */
#define SOF0 0xffc0             /* Start Of Frame */
#define DHT 0xffc4              /* Define Huffman Table */
#define SOS 0xffda              /* Start Of Scan */
#define PAS_MARQUEUR 0xffff


#define comp_Y 0
#define comp_Cb 1
#define comp_Cr 2

/* Définition du nombre maximal de composantes */
#define MAX_COMPOSANTES 3

/* Définition de la longueur maximale d'un mot de code */
#define MAX_LONGUEUR 16

/* Définition de la taille maximale d'une table de quantification */
#define NB_MAX_COEFFS_QUANT 64

/* Définition du nombre maximal de tables de quantification */
#define NB_MAX_TAB_QUANT 4

/* Définition du nombre maximal de symboles */
#define NB_MAX_SYMB 256

/* Définition du nombre maximal de table de Huffman par type */
#define NB_MAX_TABS_HUFFMAN 4

/* Définition des types AC = 1 et DC = 0 des tables de Huffman */
#define DC 0
#define AC 1
#define NB_TYPES_HUFF 2


/* Définition d'une structure Donnees_section_DQT dont les (qqs) champs 
 correspondent aux données (pertinentes) extraites de la section DQT : 
    - Un champ precision : pour la précision en nombre de bits (selon mode baseline / progressif +++)
    - Un champ val_tab_quant : correspondant aux valeurs de la table de quantification
                               stockées au format zig-zag 
    - Un champ tables_presentes : pour voir si la table d'indice i_Q est effectivement présente ou pas */

typedef struct {
    uint8_t precision[NB_MAX_TAB_QUANT] ; 
    uint8_t val_tab_quant[NB_MAX_TAB_QUANT][NB_MAX_COEFFS_QUANT] ;
    bool tables_presentes[NB_MAX_TAB_QUANT] ;
} Donnees_section_DQT ;


/* Définition d'une structure Donnees_section_SOF0 dont les champs 
 correspondent aux données (pertinentes) extraites de la section SOF0 : 
    - Un champ precision : pour la précision en nombre de bits 
    - Un champ hauteur : pour la hauteur de l'image 
    - Un champ largeur : pour la largeur de l'image 
    - Un champ N : pour le nombre de composantes 
    - Un champ i_C : pour identifier les composantes 
    - Un champ fact_ech_h : pour le facteur d'échnatiollonnage horizontal 
    - Un champ fact_ech_v : pour le facteur d'échantillonnage vertical 
    - Un champ i_Q : pour l'indice de la table de quantification */

typedef struct {
    uint8_t precision ; 
    uint16_t hauteur ;
    uint16_t largeur ; 
    uint8_t N ;
    uint8_t i_C[MAX_COMPOSANTES] ;
    uint8_t fact_ech_h[MAX_COMPOSANTES] ;
    uint8_t fact_ech_v[MAX_COMPOSANTES] ;
    uint8_t i_Q[MAX_COMPOSANTES] ;
} Donnees_section_SOF0 ;


/* Définition d'une structure pour modéliser une table de Huffman avec : 
    - Un champ nb_symboles_par_longueur : pour stoker le tableau du nombre de symbole par longueur 
    - Un champ nb_symboles : pour avoir le nombre de symboles effectivement présents 
    - Un champ tab_symbole : pour stocker les symboles par ordre croissant de longueur */

typedef struct {
    uint8_t nb_symboles_par_longueur[MAX_LONGUEUR] ;
    uint16_t nb_symboles ;
    uint8_t tab_symboles[NB_MAX_SYMB] ; 
} Table_de_Huffman ;


/* Définition d'une structure Donnees_section_DHT avec :
    - Un champ tab_Huffman : contenant les tables de Huffman extraites
    - Un champ tab_presentes : pour identifier les tables effectivement extraites */

typedef struct {
    Table_de_Huffman tab_Huffman[NB_TYPES_HUFF][NB_MAX_TABS_HUFFMAN];
    bool tab_presentes[NB_TYPES_HUFF][NB_MAX_TABS_HUFFMAN] ;
    uint8_t nb_tabs_DC ;
    uint8_t nb_tabs_AC ;  
} Donnees_section_DHT ;



/* Définition d'une structure Donnees_section_SOS avec :
    - Un champ N : nombre de composante
    - Un champ i_C : pour l'identifiant de chaque composante
    - Un champ i_H_DC : pour l'indice de la table de Huffman pour les coeffs DC
                        de chaque composante
    - Un champ  i_H_AC : pour l'indice de la table de Huffman pour les coeffs AC 
                         de chaque composante */

typedef struct {
    uint8_t N ; 
    uint8_t i_C[MAX_COMPOSANTES] ;
    uint8_t i_H_DC[MAX_COMPOSANTES] ;
    uint8_t i_H_AC[MAX_COMPOSANTES] ;
} Donnees_section_SOS ;


/* Définition d'une structure Donnees_entete_JPEG avec un champ pour les données
de chaque section */

typedef struct {
    Donnees_section_SOF0 data_sof0 ; 
    Donnees_section_DQT data_dqt ;
    Donnees_section_DHT data_dht ;
    Donnees_section_SOS data_sos ;
} Donnees_entete_JPEG ;


/* Fonction utilitaire pour comparer 2 octets lus depuis un fichier JPEG 
aux marqueurs de section 
    - Entrée : 2 octets 
    - Sortie : marqueur de section correspondant s'il existe, sinon NULL */

uint16_t marqueur_correspondant(unsigned char* double_octet) ;


/* Fonction pour extraire toutes les sections JPEG depuis un fichier JPEG
    - Entrée : un fichier JPEG ouvert en mode lecture binaire
    - Sortie : les données de l'en-tête JPEG
            => S'arrête à la fin de la section SOS */

Donnees_entete_JPEG extraire_sections(FILE* fichier_jpeg) ; 


/* Fonction pour extraire les données de la section SOF0
    - Entrée : un fichier JPEG ouvert en mode lecture binaire et pointant vers le début d'une section SOF0
    - Sortie : les données extraites stockées dans la structure correspondante */

Donnees_section_SOF0 extraire_donnees_sof0(FILE* fichier_jpeg) ;


/* Fonction pour extraire les données de la section DQT 
    - Entrée : un fichier JPEG ouvert en mode lecture binaire et pointant vers le début d'une section DQT
    - Sortie : les données extraites stockées dans la structure correspondante */

void extraire_donnees_dqt(FILE* fichier_jpeg, Donnees_section_DQT* data_dqt) ;


/* Fonction pour extraire les données de la section DHT 
    - Entrée : un fichier JPEG ouvert en mode lecture binaire et pointant vers le début d'une section DHT
    - Sortie : les données extraites stockées dans la structure correspondante */

void extraire_donnees_dht(FILE* fichier_jpeg, Donnees_section_DHT* data_dht) ;


/* Fonction pour extraire les données de la section SOS
    - Entrée : un fichier JPEG ouvert en mode lecture binaire et pointant vers le début d'une section SOS
    - Sortie : les données extraites stockées dans la structure correspondante */

Donnees_section_SOS extraire_donnees_sos(FILE* fichier_jpeg) ;


/* Fonctions utilitaires pour afficher sur la sortie standard les données 
de chaque section */

void afficher_donnees_sof0(Donnees_section_SOF0 data_sof0) ;
void afficher_donnees_dqt(Donnees_section_DQT data_dqt) ;
void afficher_donnees_dht(Donnees_section_DHT data_dht) ;
void afficher_donnees_sos(Donnees_section_SOS data_sos) ;


/* Fonction utilitaire pour vérifier que la section
APP0 est conforme à sa description 
    - Il faut trouver la phrase JFIF
    - Il faut que la version soit à 1.1 */

void verifier_format_APP0(FILE* fichier_jpeg) ;


/* Fonciton utilitaire qui prend un fichier pointant sur le début des données
brutes d'une image JPEG et qui renvoie leur taille */

uint32_t recuperer_taille_donnees_brutes(FILE* f_jpeg) ;


/* Fonction utilitaire qui fait la correspondante entre les identifiants i_C
des deux sections SOF0 et SOS */

uint8_t trouver_i_C_correspondant(Donnees_section_SOS data_sos, uint8_t i_C_SOF0, uint8_t N) ;

/* Fonction utilitaire pour vérifier que les facteurs d'échantillonnage sont bien compris
entre 1 et 4 */

bool fact_ech_valides(uint8_t h, uint8_t v) ;

/* Fonction utilitaire pour vérifier que les facteurs d'échantillonnage  
respectent les contraintes : 
    - Somme des h_i * v_i <= 10 
    - Les facteurs d'échantillonnage des chrominances divisent 
    parfaitement ceux de la luminance. */
    
bool contraintes_fact_ech_validees(uint8_t hi[MAX_COMPOSANTES], uint8_t vi[MAX_COMPOSANTES], uint8_t N) ;

#endif