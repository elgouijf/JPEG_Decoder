#ifndef IDCT
#define IDCT
#include <stdint.h>
#define PI 3.14159265358979323846

float Coefficient_iDCT( uint8_t freq );
/* Fonction calculant les coefficients C */
int amplitude_to_color_8( uint8_t x, uint8_t y, int Phi[8][8]);
/* Fonction calculant la transformée en cosinus discréte inverse 
   au point de coordonées x,y dans le cas n = 8 */
#endif