/* 
 * File:   dac.h
 * Author: mosee
 *
 * Created on 12 février 2020, 9:00
 */

#ifndef DAC_H
#define	DAC_H

#ifdef	__cplusplus
extern "C" {
#endif
    

    /** Initialise the DAC right channel
     *  The sampling frequency is given by:
     *      fs = (Fosc/256) / freqDiv
     *  freqDiv must be between 1 and 128
     *  Fosc is 80MHz if a 10MHz crystal is used with oscillatorInit() */
    void DAC_init(uint16_t freqDiv);
    
    /** Send a new sample to the DAC */
    void DAC_write(uint16_t data);


#ifdef	__cplusplus
}
#endif

#endif	/* DAC_H */

