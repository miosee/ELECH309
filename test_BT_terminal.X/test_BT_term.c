/*****************************************************************************
 * 
 * Demo of the AMB2300 library, created the 02/04/2020
 * 
 * The AMB2300 is the bluetooth-serial interface on the main board of the
 * ELECH309 robot.
 * This demo was written for this board. It implements an "echo" through
 * the bluetooth connexion to the host (i.e. a PC).
 * 
 * The dsPIC will wait for the host to send 1 byte through the bluetooth connexion.
 * It answers by sending back this byte.
 * A simple serial terminal (such as Termite) can be used on the PC to send bytes
 * and read the answer.
 * 
 * V1.0 : creation
 * 
 *****************************************************************************/ 

#include "xc.h"
#include "../libs/init.h"
#include "../libs/bluetooth.h"

int main(void) {
    int8_t data;
    
    // Initialise the main oscillator to run at Fosc=80MHz => Fcy=40MHz
    oscillatorInit();
    // Initialise the bluetooth communication
    btInit();

    while(1) {
        if (btGetRxBufferSpace() > 0) {     // if a byte has been received
            btGetData(&data, 1);            // it is read and stored in 'data'
            btSendData(&data, 1);           // it is sent back to the host
        }
    }
    
    return 0;
}
