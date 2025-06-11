#include "../../include/construction_arbre_huff.h"
#include <stdio.h>
#include "../../include/entete_jpeg.h"

void print_abr_helper(struct abr* a, int depth);
void print_abr(struct abr* a);
void abr_Huff(void);