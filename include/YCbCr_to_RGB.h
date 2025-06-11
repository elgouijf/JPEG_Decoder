#ifndef YCbCr_to_RGB
#define YCbCr_to_RGB
#include <stdint.h>

uint8_t get_red(uint8_t Y, uint8_t Cb, uint8_t Cr);
/*Fonction calculant la composante rouge d'un pixel via sa lumonisté et chrominances*/
uint8_t get_blue(uint8_t Y, uint8_t Cb, uint8_t Cr);
/*Fonction calculant la composante bleue d'un pixel via sa lumonisté et chrominances*/
uint8_t get_green(uint8_t Y, uint8_t Cb, uint8_t Cr);
/*Fonction calculant la composante verte d'un pixel via sa lumonisté et chrominances*/

#endif
