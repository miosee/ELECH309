/* 
 * File:   adc.h
 * Author: mosee
 *
 * Created on 10 février 2020, 11:40
 */

#ifndef ADC_H
#define	ADC_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include "ctype.h"


    // Configure l'ADC1 pour une conversion en 12bits par polling de AN0
    void ADC_Init(void);
    // Lance une conversion
    void adcConversionStart(void);
    /* Indique si la conversion est finie :
     *  0 si une conversion est en cours
     *  1 sinon */
    uint16_t ADC_conversionFinished(void);
    /* Retourne le résultat de la dernière conversion.
     * Si une conversion est en cours, retourne -32768 */
    int16_t ADC_read(void);


#ifdef	__cplusplus
}
#endif

#endif	/* ADC_H */

