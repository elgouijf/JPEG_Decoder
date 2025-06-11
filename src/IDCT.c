#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "IDCT.h"
#include <math.h>



float Coefficient_iDCT( uint8_t freq ){
    if (freq == 0){ return 1/sqrtf(2.0f);}
    else {
        return 1.0f ;
    }
}

/* Doit renvoyer un uint8_t +++ */
int  amplitude_to_color_8( uint8_t x, uint8_t y, int Phi[8][8]){
    /* Indices de la sommation double*/
    uint8_t lambda;
    uint8_t mu;
    
    float C_lambda;
    float C_mu;

    /* Initialisation du bloc spatial */
    float S_x_y = 0.0f ;
    for (lambda = 0; lambda < 8; lambda++){
        /* Calcul du coefficient C en lambda*/
        C_lambda = Coefficient_iDCT(lambda);
        /* Initialisation de la sommation en mu*/
        float somme_mu = 0.0f ;

        for (mu = 0; mu < 8; mu++){
            /* Calcul du coefficient C en j (ie en mu)*/
            C_mu = Coefficient_iDCT(mu);

            somme_mu += C_mu * cosf((( 2*y + 1 ) * mu * PI)/16.0f) * Phi[lambda][mu];
        }

        S_x_y +=  somme_mu * C_lambda * cosf((( 2*x + 1 ) * lambda * PI)/16.0f);
        }
    
    S_x_y = (S_x_y / 4.0f) + 128.0f;
    
    /* Arondissement de S*/
    if (S_x_y < 0.0) {return 0; }
    else if (S_x_y > 255.0f) {return 255;}
    else {
        uint32_t S = (int)S_x_y ;
        if ( S_x_y - S  > 0.5 ){ return S + 1; }
        else { return S; }
    }
}