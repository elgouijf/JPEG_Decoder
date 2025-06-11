#include "../include/entete_jpeg.h"

bool fact_ech_valides(uint8_t h, uint8_t v) {
    if (h < 1 || h > 4) {
        return false ; 
    }
    if (v < 1 || v > 4) {
        return false ;
    }
    return true ;
}

bool contraintes_fact_ech_validees(uint8_t hi[MAX_COMPOSANTES], uint8_t vi[MAX_COMPOSANTES], uint8_t N) {
    uint8_t sum_hi_vi = 0 ;
    for (uint8_t i = 0 ; i < N ; i++) {
       sum_hi_vi += hi[i] * vi[i] ;
    }  
    /* YCbCr => il faut que que h(Cb) et h(Cr) divisent h(Y) et idem pour v */
    if (N == 3) {
        if ((hi[comp_Y] % hi[comp_Cb] != 0) || (hi[comp_Y] % hi[comp_Cr] != 0)) {
            return false ;
        }
         if ((vi[comp_Y] % vi[comp_Cb] != 0) || (vi[comp_Y] % vi[comp_Cr] != 0)) {
            return false ;
        }
    }
    if (sum_hi_vi > 10) {
        return false ; 
    }
    return true ;
}

uint16_t marqueur_correspondant(uint8_t* double_octet)
{
    uint16_t marqueur_potentiel = (double_octet[0] << 8) | double_octet[1] ;
    switch (marqueur_potentiel) {
        case SOI :
            printf("Marqueur lu : Start Of Image. \n") ;
            return SOI ;
        case EOI :
            printf("Marqueur lu : End Of Image. \n") ;
            return EOI ;
        case APP0 :
            printf("Marqueur lu : Application Data. \n") ;
            return APP0 ;
        case COM : 
            printf("Marqueur lu : Commentaire. \n") ;
            return COM ;
        case DQT :
            printf("Marqueur lu : Define Quantization Table. \n") ;
            return DQT ;
        case SOF0 :
            printf("Marqueur lu : Start Of Frame. \n") ;
            return SOF0 ;
        case DHT :
            printf("Marqueur lu : Define Huffman Table. \n") ;
            return DHT ;
        case SOS :
            printf("Marqueur lu : Start Of Scan. \n") ;
            return SOS ;
        default :
            //printf("Ce n'est pas un marqueur. \n") ;
            return PAS_MARQUEUR ;
    }
}


Donnees_entete_JPEG extraire_sections(FILE* f_jpeg)
{
    Donnees_entete_JPEG data_jpeg = {0} ; 

    /* Déclaration des 2 octets qui seront lus */
    uint8_t double_octet[2]  ;

    /* Parcours de fichier_jpeg 2 octets par 2 octets
    à la recherche des marqueurs de sections */

    bool debut_donnees_brutes = false ;
    /* On s'arrête juste après la fin de la section SOS là
    où commencent les données brutes */
    while (!debut_donnees_brutes) {
        size_t nb_octets_lus = fread(double_octet, 1, 2, f_jpeg) ;
        if (nb_octets_lus != 2) {
            fprintf(stderr, "Nombre d'octets lus incorrect\n") ;
            exit(EXIT_FAILURE) ;
        }
        /* Pour débugger  */
        // printf("Octet 1 : %x\n", double_octet[0]) ;
        // printf("Octet 2 : %x\n", double_octet[1]) ;

        /* Identification de la section courante */
        uint16_t marqueur = marqueur_correspondant(double_octet) ;
        switch (marqueur)
        {
            case SOF0 :
                data_jpeg.data_sof0 = extraire_donnees_sof0(f_jpeg) ;
                afficher_donnees_sof0(data_jpeg.data_sof0) ;
                break ;

            case DQT : 
                extraire_donnees_dqt(f_jpeg, &data_jpeg.data_dqt) ; 
                afficher_donnees_dqt(data_jpeg.data_dqt) ;
                break ; 
            
            case DHT : 
                extraire_donnees_dht(f_jpeg, &data_jpeg.data_dht) ;
                afficher_donnees_dht(data_jpeg.data_dht) ;
                break ;
            
            case SOS :
                data_jpeg.data_sos = extraire_donnees_sos(f_jpeg) ;
                afficher_donnees_sos(data_jpeg.data_sos) ;
                debut_donnees_brutes = true ;
                break ;
            
            case APP0 :
                verifier_format_APP0(f_jpeg) ;
                break ;

            default :
                break ;
        }

        /* Reculer d'un octet en arrière pour traiter toutes
        les paires d'octets et ne pas rater de marqueurs si on n'est pas
        au début des données brutes */
        if (!debut_donnees_brutes) {
            fseek(f_jpeg, -1, SEEK_CUR) ;
        }
    }

    /* Normalement ici, le curseur du fichier pointe sur le début
    des données brutes : le dire à Faical pour discuter de la lecture bit par bit */
    /* Pour débogage */
    // fread(double_octet, 1, 2, f_jpeg) ;
    // printf("Premiers 2 octets des données brutes : %x %x\n", double_octet[0], double_octet[1]) ;

    return data_jpeg ;
}


Donnees_section_SOF0 extraire_donnees_sof0(FILE* f_jpeg) 
{ 
    Donnees_section_SOF0 data_sof0 ;

    /* Déclaration des 2 octets qui seront lus */
    uint8_t double_octet[2] ;

    /* Les 2 premiers octets correspondent à la longueur
    de la section  */
    size_t nb_octets_lus = fread(double_octet, 1, 2, f_jpeg) ;
    if (nb_octets_lus != 2) {
        fprintf(stderr, "Nombre d'octets lus incorrects\n") ;
        exit(EXIT_FAILURE) ;
    }
  
    /* L'octet qui suit correspond à la précision en bits par composante
    (toujours 8 pour le baseline) */
    nb_octets_lus = fread(double_octet, 1, 1, f_jpeg) ; 
    if (nb_octets_lus != 1) {
        fprintf(stderr, "Nombre d'octets lus incorrect\n") ;
        exit(EXIT_FAILURE) ;
    }
    data_sof0.precision = double_octet[0] ;

    /* Les 2 octets suivants correspondent à la hauteur de l'image */
    nb_octets_lus = fread(double_octet, 1, 2, f_jpeg) ;
    if (nb_octets_lus != 2) {
        fprintf(stderr, "Nombre d'octets lus incorrect\n") ;
        exit(EXIT_FAILURE) ;
    }
    data_sof0.hauteur = (double_octet[0] << 8) | double_octet[1] ; 

    /* Les 2 octets suivants correspondent à la largeur de l'image */
    nb_octets_lus = fread(double_octet, 1, 2, f_jpeg) ;
    if (nb_octets_lus != 2) {
        fprintf(stderr, "Nombre d'octets lus incorrect\n") ;
        exit(EXIT_FAILURE) ;
    }
    data_sof0.largeur = (double_octet[0] << 8) | double_octet[1] ; 

    /* L'octet suivant correspond au nombre de composantes N (3 pour YCbCr, 1 pour les niveaux de gris)*/
    nb_octets_lus = fread(double_octet, 1, 1, f_jpeg) ;
    if (nb_octets_lus != 1) {
        fprintf(stderr, "Nombre d'octets lus incorrect\n") ;
        exit(EXIT_FAILURE) ;
    }

    if (double_octet[0] == 0 || double_octet[0] > MAX_COMPOSANTES) {
        fprintf(stderr, "Nombre de composantes N : %u invalide\n", double_octet[0]) ; 
        exit(EXIT_FAILURE);
    }
    data_sof0.N = double_octet[0] ;

    /* Les octets restants correspondent à l'identifiant de composante i_C, 
    les facteurs d'échantillonnage horizontal et vertical, et
    l'indice de table de quantification i_Q dans cet ordre, pour chaque composante */

    for (uint8_t k = 0 ; k < data_sof0.N ; k++) {
        /* Le premier octet est i_c */
        nb_octets_lus = fread(double_octet, 1, 1, f_jpeg) ;
        if (nb_octets_lus != 1) {
            fprintf(stderr, "Nombre d'octets lus incorrect\n") ;
            exit(EXIT_FAILURE) ;
        }
        data_sof0.i_C[k] = double_octet[0] ;

        /* L'octet suivant contient en MSB 4 bits le facteur
        d'échantillonnage horizontal et en LSB 4 bits celui vertical */
        nb_octets_lus = fread(double_octet, 1, 1, f_jpeg) ;
        if (nb_octets_lus != 1) {
            fprintf(stderr, "Nombre d'octets lus incorrects\n") ;
            exit(EXIT_FAILURE) ;
        }

        uint8_t fact_ech_h = (double_octet[0] >> 4) & 0x0f ;
        uint8_t fact_ech_v = double_octet[0] & 0x0f ;
        /* Vérification de la validité des facteurs d'échantillonnage :
            1 <= h <= 4 et 1 <= v <= 4 */
        if (!fact_ech_valides(fact_ech_h , fact_ech_v)) {
            fprintf(stderr, "Facteurs d'échantillonnage invalides \n") ;
            exit(EXIT_FAILURE) ;
        }
        data_sof0.fact_ech_h[k] = fact_ech_h ; 
        data_sof0.fact_ech_v[k] = fact_ech_v ;

        /* Le dernier octet est i_Q */
        nb_octets_lus = fread(double_octet, 1, 1, f_jpeg) ;
        if (nb_octets_lus != 1) {
            fprintf(stderr, "Nombre d'octets lus incorrect\n") ;
            exit(EXIT_FAILURE) ;
        }
        data_sof0.i_Q[k] = double_octet[0] ;
    }

    /* Vérification des contraintes sur les facteurs d'échantillonnage
    somme des h_i * v_i <= 10 et la valeur de ceux des chrominances divise celle
    de la luminance */
    if (! contraintes_fact_ech_validees(data_sof0.fact_ech_h, data_sof0.fact_ech_v, data_sof0.N)) {
        fprintf(stderr, "Contraintes des facteurs d'échantillonnage non respectées \n") ;
        exit(EXIT_FAILURE) ;
    }

    return data_sof0 ;
}


void extraire_donnees_dqt(FILE* f_jpeg, Donnees_section_DQT* data_dqt) 
{
    uint8_t double_octet[2] ;

    /* Lecture de la longueur de la section */
    size_t nb_octets_lus = fread(double_octet, 1, 2, f_jpeg) ;
    if (nb_octets_lus != 2) {
        fprintf(stderr, "Nombre d'octets lus incorrect\n") ;
        exit(EXIT_FAILURE) ;
    }
    uint16_t longueur_section = (double_octet[0] << 8) | double_octet[1] ;

    /* On extrait toutes les tables de quantification pouvant
    être présentes dans cette même section DQT
        => La table de quantification fait 64 octets 
        => i_Q et précision font 1 octet 
        ==> Il faut donc vérifier si à partir de l'offset 
        courant, il reste encore 65 octets correspondant aux 
        données d'une nouvelle table de quantification */

    uint16_t offset = 0 ; 
    while (offset + 1 + NB_MAX_COEFFS_QUANT <= longueur_section - 2) {

        /* Le premier octet contient la précision dans les 4 MSB
        et l'indice i_Q de la table de quantification dans les 4 LSB */
        nb_octets_lus = fread(double_octet, 1, 1, f_jpeg) ;
        if (nb_octets_lus != 1) {
            fprintf(stderr, "Nombre d'octets lus incorrect\n") ;
            exit(EXIT_FAILURE) ;
        }
        uint8_t precision = (double_octet[0] >> 4) & 0x0f ; 
        uint8_t i_Q = double_octet[0] & 0x0f ; 

        if (i_Q >= NB_MAX_COEFFS_QUANT) {
            fprintf(stderr, "i_Q invalide : %u\n", i_Q) ;
            exit(EXIT_FAILURE) ;
        }

        data_dqt->precision[i_Q] = precision ;           /* 0 si 8 bits et 1 si 16 bits */

        /* Les 64 octets suivants correspondent aux valeurs de la table de 
        quantification stockées au format zig-zag :
            => Chaque coeff est d'un octet +++ */

        for (uint8_t k = 0 ; k < NB_MAX_COEFFS_QUANT ; k++) {
            nb_octets_lus = fread(double_octet, 1, 1, f_jpeg) ;
            if (nb_octets_lus != 1) {
                fprintf(stderr, "Nombre d'octets lus incorrect\n") ;
                exit(EXIT_FAILURE) ;
            }
            uint8_t coeff = double_octet[0] ;
            data_dqt->val_tab_quant[i_Q][k] = coeff ; 
        }
        
        /* La table d'indice i_Q a été extraite */
        data_dqt->tables_presentes[i_Q] = true ;  

        /* Réactualisation de offset : 65 octets ont été lus */
        offset += 1 + NB_MAX_COEFFS_QUANT ;
    }
}


void extraire_donnees_dht(FILE* f_jpeg, Donnees_section_DHT* data_dht) 
{
    uint8_t double_octet[2] ;
    // Donnees_section_DHT data_dht = {0} ;

    /* Lecture de la longueur de la section  */
    size_t nb_octets_lus = fread(double_octet, 1, 2, f_jpeg) ;
    if (nb_octets_lus != 2) {
        fprintf(stderr, "Nombre d'octets lus incorrect\n") ;
        exit(EXIT_FAILURE) ;
    }
    uint16_t longueur_section = (double_octet[0] << 8) | double_octet[1] ;
    longueur_section -= 2 ;         /* On enlève la taille de la longueur de section */

    /* Début des données de la section */
    uint16_t offset = 0 ;
    while (offset < longueur_section) {

        /* Le premier octet est scindé en : 
            - 3 bits (MSB) :  non utilisés, sont à 0 sinon erreur 
            - 1 bit : type (0 = DC | 1 = AC) 
            - 4 bits (LSB) : indice (0,...,3 ou erreur) */
        nb_octets_lus = fread(double_octet, 1, 1, f_jpeg) ;
        if (nb_octets_lus != 1) {
            fprintf(stderr, "Nombre d'octets lus incorrect\n") ;
            exit(EXIT_FAILURE) ;
        }
        uint8_t premier_octet = double_octet[0] ;

        uint8_t bits_non_utilises = (premier_octet >> 5) & 0x07 ;     
        if (bits_non_utilises != 0) {
            fprintf(stderr, "Les bits inutilisés dans la section DHT ne sont pas à 0 ! \n") ;
            exit(EXIT_FAILURE) ;
        }
                
        uint8_t type_table_huffman = (premier_octet >> 4) & 0x01 ;      /* 0 si DC et 1 si AC */

        if (type_table_huffman == DC) {
            data_dht->nb_tabs_DC += 1 ;
        }
        else if (type_table_huffman == AC) {
            data_dht->nb_tabs_AC += 1 ; 
        }

        uint8_t indice = premier_octet & 0x07 ;
        if (indice >= NB_MAX_TABS_HUFFMAN) {
            fprintf(stderr, "Indice de table Huffman invalide : %u\n", indice);
            exit(EXIT_FAILURE);
        }   

        /* Les 16 octets suivants correspondent aux nombres
        de symboles avec des codes de longueur 1 à 16.
        La somme de ces valeurs reprèsente le nombre total 
        de codes et doit être inférieure à 256
        => ce que j'ai compris : le 1er octet correspond au nombre de symboles
                                de longueur 1, le 2ème octet correspond au nombre
                                de symboles de longueur 2, et ainsi de suite... */

        /* J'ai mis un pointeur ici pour que les modifications faites sur tab_huff 
        sont aussi faites dans le champ tab_Huffman de la data de la section  */
        Table_de_Huffman *tab_huff = &data_dht->tab_Huffman[type_table_huffman][indice] ;
        data_dht->tab_presentes[type_table_huffman][indice] = true ;
                
        uint16_t nb_symboles = 0 ;
        for (uint8_t k = 0 ; k < MAX_LONGUEUR ; k++) {
            nb_octets_lus = fread(double_octet, 1, 1, f_jpeg) ;
            if (nb_octets_lus != 1) {
                fprintf(stderr, "Nombre d'octets lus incorrect\n") ;
                exit(EXIT_FAILURE) ;
            }
            tab_huff->nb_symboles_par_longueur[k] = double_octet[0] ;
            nb_symboles += tab_huff->nb_symboles_par_longueur[k] ;
        }          
        tab_huff->nb_symboles = nb_symboles ;

        /* Les octets restants (= nb_symboles) de la section correspondent
        à la table contenant les symboles triés par longueur */

        /* Pour vérifier que la section est bien complète 
        et contient tous les symboles */
        if (offset + 1 + MAX_LONGUEUR + nb_symboles > longueur_section) {
            fprintf(stderr, "Taille de section DHT insuffisante pour lire les symboles\n");
            exit(EXIT_FAILURE);
        }

        /* Lecture des symboles à proprement parler */
        for(uint8_t k = 0 ; k < nb_symboles ; k++) {
            nb_octets_lus = fread(double_octet, 1, 1, f_jpeg) ;
            if (nb_octets_lus != 1) {
                fprintf(stderr, "Nombre d'octets lus incorrect\n") ;
                exit(EXIT_FAILURE) ;
            }
            tab_huff->tab_symboles[k] = double_octet[0] ;
        } 
        offset += 1 + MAX_LONGUEUR + nb_symboles ;  
    }
}


Donnees_section_SOS extraire_donnees_sos(FILE* f_jpeg)
{
    Donnees_section_SOS data_sos = {0} ;
    uint8_t double_octet[2] ;

    /* Lecture de la longueur de la section */
    size_t nb_octets_lus = fread(double_octet, 1, 2, f_jpeg) ;
    if (nb_octets_lus != 2) {
        fprintf(stderr, "Nombre d'octets lus incorrect\n") ;
        exit(EXIT_FAILURE) ;
    }

    /* Le 1er octet correspond au nombre de composantes N */
    nb_octets_lus = fread(double_octet, 1, 1, f_jpeg) ;
    if (nb_octets_lus != 1) {
        fprintf(stderr, "Nombre d'octets lus incorrect\n") ;
        exit(EXIT_FAILURE) ;
    }
    uint8_t N = double_octet[0] ;
    if (N == 0 || N > MAX_COMPOSANTES) {
        fprintf(stderr, "Nombre de composantes invalide dans la section SOS : %u\n", N);
        exit(EXIT_FAILURE); 
    }
    data_sos.N = N ;

    /* Les 2N octets suivants sont tels que :
        - 1er octet correspond à l'identifiant i_C de la composante 
        - 4 bits pour l'nidice de la table de Huffman i_H pour les coefficients DC 
        - 4 bits pour l'nidice de la table de Huffman i_H pour les coefficients AC */
    for (uint8_t k = 0 ; k < N ; k++) {
        nb_octets_lus = fread(double_octet, 1, 2, f_jpeg) ;
        if (nb_octets_lus != 2) {
            fprintf(stderr, "Nombre d'octets lus incorrect\n") ;
            exit(EXIT_FAILURE) ;
        }
        data_sos.i_C[k] = double_octet[0] ;

        uint8_t i_H = double_octet[1] ;

        uint8_t i_H_DC = (i_H >> 4) & 0x0f ;
        data_sos.i_H_DC[k] = i_H_DC ;

        uint8_t i_H_AC = i_H & 0x0f ; 
        data_sos.i_H_AC[k] = i_H_AC ;
    }

    /* L'octet suivant correspond à Ss : 1er indice de la sélection 
    spectrale : doit valoir 0 en mode baseline */
    nb_octets_lus = fread(double_octet, 1, 1, f_jpeg) ;
    if (nb_octets_lus != 1) {
        fprintf(stderr, "Nombre d'octets lus incorrect\n") ;
        exit(EXIT_FAILURE) ;
    }
    uint8_t Ss = double_octet[0] ; 
    // printf("1er indice de sélection spectrale : %u\n", Ss) ;

    /* L'octet d'après correspond au dernier indice de la sélection spectrale : 
    doit valoir 63 en mode baseline */
    nb_octets_lus = fread(double_octet, 1, 1, f_jpeg) ;
    if (nb_octets_lus != 1) {
        fprintf(stderr, "Nombre d'octets lus incorrect\n") ;
        exit(EXIT_FAILURE) ;
    }
    uint8_t Se = double_octet[0] ;
    // printf("Dernier indice de sélection spectrale : %u\n", Se) ;

    /* Le dernier octet correspond à l'approximation successive :
        - 4 bits de poids fort : Ah (doit valoir 0 en mode baseline) 
        - 4 bits de poids faible : Al (doit valoir 0 en mode baseline) */
    nb_octets_lus = fread(double_octet, 1 ,1, f_jpeg) ;
    if (nb_octets_lus != 1) {
        fprintf(stderr, "Nombre d'octets lus incorrect\n") ;
        exit(EXIT_FAILURE) ;
    }
    uint8_t approx_succ = double_octet[0] ;

    uint8_t Ah = (approx_succ >> 4) & 0x0f ; 
    // printf("Ah = %u\n", Ah) ;

    uint8_t Al = approx_succ & 0x0f ; 
    // printf("Al = %u\n", Al) ;

    if (Ss != 0 || Se != 63 || Ah != 0 || Al != 0) {
        fprintf(stderr, "Paramètres non valides pour le mode JPEG baseline.\n");
        exit(EXIT_FAILURE);
    }

    return data_sos ;
}


void afficher_donnees_sof0(Donnees_section_SOF0 data_sof0)
{
    printf("----- Données extraites depuis la section SOF0 -----\n") ;
    printf("Précision en bits de composante : %u\n", data_sof0.precision) ;
    printf("Hauteur de l'image : %u\n", data_sof0.hauteur) ;
    printf("Largeur de l'image : %u\n", data_sof0.largeur) ;
    printf("Nombre de composantes : %u\n", data_sof0.N) ;
    for (uint8_t i = 0 ; i < data_sof0.N ; i++) {
        printf("Identifiant de la composante %u : %u\n", i + 1, data_sof0.i_C[i]) ;
        printf("Indice de la table de quantification associée à la composante %u : %u\n", i + 1, data_sof0.i_Q[i]) ;
        printf("Facteur d'échantillonnage horizontal de la composante %u : %u\n", i + 1, data_sof0.fact_ech_h[i]) ;
        printf("Facteur d'échantillonnage vertical de la composante %u : %u\n", i + 1, data_sof0.fact_ech_v[i]) ;
        printf("\n") ;
    }
}


void afficher_donnees_dqt(Donnees_section_DQT data_dqt) 
{
    printf("----- Données extraites depuis la section DQT -----\n") ;
    for (uint8_t i = 0 ; i < NB_MAX_TAB_QUANT ; i++) {
        if (data_dqt.tables_presentes[i]) {
            printf("Table d'indice i_Q = %u :\n", i) ;
            if (data_dqt.precision[i] == 0) {
                printf("Précision sur 8 bits car 4 bits lus =  %u\n", data_dqt.precision[i]) ;
            }
            else if (data_dqt.tables_presentes[i] == 1) {
                printf("Précision sur 16 bits car 4 bits lus =  %u\n", data_dqt.precision[i]) ;
            }
            printf("Table de quantification d'indice %u : \n", i) ;
            for (uint8_t k = 0 ; k < NB_MAX_COEFFS_QUANT ; k++) {
                printf("%3u ", data_dqt.val_tab_quant[i][k]) ;
                if ((k + 1) % 8 == 0) {
                    printf("\n") ;
                }
            }
            printf("\n") ;
        }
    }
}


void afficher_donnees_dht(Donnees_section_DHT data_dht) 
{
    printf("----- Données extraites depuis la section DHT -----\n") ;
    for (uint8_t type = 0 ; type < NB_TYPES_HUFF ; type++) {
            for (uint8_t indice = 0 ; indice < NB_MAX_TABS_HUFFMAN ; indice++) {
                
                if (data_dht.tab_presentes[type][indice]) {
                    printf("Table Huffman %s d'indice %u :\n", (type == DC ? "DC" : "AC"), indice);

                    Table_de_Huffman tab_huff = data_dht.tab_Huffman[type][indice];

                    printf("  Nombre total de symboles : %u\n", tab_huff.nb_symboles);
                    printf("  Nombre de symboles par longueur :\n    ");
                    for (uint8_t k = 0 ; k < MAX_LONGUEUR ; k++) {
                        printf("%u ", tab_huff.nb_symboles_par_longueur[k]);
                    }

                    printf("\n  Symboles (dans l'ordre de codage) :\n    ");
                    for (uint16_t k = 0; k < tab_huff.nb_symboles; k++) {
                        printf("%3u ", tab_huff.tab_symboles[k]);
                        if ((k + 1) % 16 == 0) printf("\n    ");
                    }
                    printf("\n\n");
                }
            }
        }
}


void afficher_donnees_sos(Donnees_section_SOS data_sos)
{
    printf("----- Données extraites depuis la section SOS -----\n") ;
    printf("Nombre de composantes N : %u\n", data_sos.N) ;
    for (uint8_t i = 0 ; i < data_sos.N ; i++) {
        printf("i_C de la composante %u : %u\n", i + 1, data_sos.i_C[i]) ;
        printf("i_H_DC de la composante %u : %u\n", i + 1, data_sos.i_H_DC[i]) ;
        printf("i_H_AC de la composante %u : %u\n", i + 1, data_sos.i_H_AC[i]) ;
        printf("\n") ; 
    }
}


void verifier_format_APP0(FILE* f_jpeg) 
{
    uint8_t double_octet[2] ;
    /* Consommer d'abord les 2 octets de la longueur de section */
    size_t nb_octets_lus = fread(double_octet, 1, 2, f_jpeg) ;
    if (nb_octets_lus != 2) {
        fprintf(stderr, "Nombre d'octets lus incorrect\n") ;
        exit(EXIT_FAILURE) ;
    }

    /* Les 5 premiers octets correspondent à la chaine */
    nb_octets_lus = fread(double_octet, 1 ,2, f_jpeg) ;
    if (nb_octets_lus != 2) {
        fprintf(stderr, "Nombre d'octets lus incorrect\n") ;
        exit(EXIT_FAILURE) ;
    }
    uint8_t j = double_octet[0] ;
    uint8_t f1 = double_octet[1] ;
    nb_octets_lus = fread(double_octet, 1 ,2, f_jpeg) ;
    if (nb_octets_lus != 2) {
        fprintf(stderr, "Nombre d'octets lus incorrect\n") ;
        exit(EXIT_FAILURE) ;
    }
    uint8_t i = double_octet[0] ;
    uint8_t f2 = double_octet[1] ;
    nb_octets_lus = fread(double_octet, 1 ,1, f_jpeg) ;
    if (nb_octets_lus != 1) {
        fprintf(stderr, "Nombre d'octets lus incorrect\n") ;
        exit(EXIT_FAILURE) ;
    }
    uint8_t zero = double_octet[0] ;

    if (j != 'J' || f1 != 'F' || i != 'I' || f2 != 'F' || zero != '\0') {
        fprintf(stderr, "Chaîne JFIF non rencontrée \n") ;
        exit(EXIT_FAILURE) ;
    }

    /* Les 2 octets suivants correspondent à la version */
    nb_octets_lus = fread(double_octet, 1 ,2, f_jpeg) ;
    if (nb_octets_lus != 2) {
        fprintf(stderr, "Nombre d'octets lus incorrect\n") ;
        exit(EXIT_FAILURE) ;
    }
    uint8_t v1 = double_octet[0] ;
    uint8_t v2 = double_octet[1] ;

    if (v1 != 1 || v2 != 1) {
        fprintf(stderr, "Version JFIF non conforme \n") ;
        exit(EXIT_FAILURE) ;
    }

    printf("La section APP0 est conforme \n\n") ;
}


uint32_t recuperer_taille_donnees_brutes(FILE* f_jpeg) 
{
    long offset_debut_donnees_brutes = ftell(f_jpeg) ;

    /* On cherche le marqueur de fin EOI */
    int octet_precedent = 0 ;
    int octet_courant ;

    while (true) {
        octet_courant = fgetc(f_jpeg) ;
        if (octet_courant == EOF) {
            break ; 
        }
        if (octet_precedent == 0xff && octet_courant == 0xd9) {
            break ; 
        }
        octet_precedent = octet_courant ;
    }

    long offset_fin_donnees_brutes = ftell(f_jpeg) - 2 ;    /* - 2 correspond à la taille du marqueur EOI */

    /* On revient au début des données brutes */
    fseek(f_jpeg, offset_debut_donnees_brutes, SEEK_SET) ;

    return (uint32_t) offset_fin_donnees_brutes - offset_debut_donnees_brutes ;
}


uint8_t trouver_i_C_correspondant(Donnees_section_SOS data_sos, uint8_t i_C_SOF0, uint8_t N) {
    for (uint8_t i = 0 ; i < N ; i++) {
        if (data_sos.i_C[i] == i_C_SOF0) {
            return i ;
        }
    }
    fprintf(stderr, "Erreur : i_C non trouvé dans la section SOS\n");
    exit(EXIT_FAILURE);
}