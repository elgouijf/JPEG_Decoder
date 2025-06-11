# include <stdint.h>
# include <stdlib.h>
# include <stdio.h>
# include <math.h>


uint8_t get_red(uint8_t Y, uint8_t Cb, uint8_t Cr){
    float R;
    R = Y - (0.0009267*(Cb - 128)) + (1.4016868*(Cr - 128)) ;
    /*Cas de dépassement des bornes*/
    if (R <= 0) { return 0; }
    else if (R > 255) { return 255; }
    
    else {
        /*Arrondissement*/
        return (uint8_t)round(R);
        }
    }



uint8_t get_blue(uint8_t Y, uint8_t Cb, uint8_t Cr){
    float B;
    B = Y + (1.7721604*(Cb - 128)) + (0.0009902*(Cr - 128)) ;
    /*Cas de dépassement des bornes*/
    if (B <= 0) { return 0; }
    else if (B > 255) { return 255; }

    else {
        /*Arrondissement*/
        return (uint8_t)round(B);
        }
    }



uint8_t get_green(uint8_t Y, uint8_t Cb, uint8_t Cr){
    float G;
    G = Y - (0.3436954*(Cb - 128)) - (0.7141690*(Cr - 128) );
    /*Cas de dépassement des bornes*/
    if (G <= 0) { return 0; }
    else if (G > 255) { return 255; }

    else {
        /*Arrondissement*/
        return (uint8_t)round(G);
        }
        }
        
    


