/**
 * @file CP.cpp
 * @author Shaival Nagarsheth
 * @brief Control Pilot for EVSE with Pilot Signal States. 
 * @version 0.2
 * @date 2021-12-14
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <Arduino.h>
#include <string.h>
#include "CP.h"

/*--Class Constructor---*/
CP::CP(){

}

/*****************************
* Private functions
*****************************/

//------setCP1 (PWM para for Control Pilot)--------
/**
 * @brief setting PWM parameters -- Channel Number, Frequency (1000Hz), Resolution (16Bit) (for dutyCycle) and PinOut
 * @param none They are already pre defined in the CP.h file
 */
void CP::setCP1(void){
    ledcSetup(ChanNo1, CPfreq, RESOL);
    ledcAttachPin(CPOUT1, ChanNo1);
}

/*--Uncomment the below funciton after detaching JTAG Cable (Load with UART only, if we are going to use CP2)--*/
// void CP::setCP2(void){
//     ledcSetup(ChanNo2, CPfreq, RESOL);
//     ledcAttachPin(CPOUT2, ChanNo2);
// }

//------getADCValue--------
/**
 * @brief Read ADC Value 
 * 
 * @param pin the ADC Input PIN; for instance, CPIN1 or CPIN2 
 * @return uint16_t analog counts from 0 to 4095 (0 to 2.45V Vref)
 */

uint16_t CP::getADCValue(uint8_t pin){
    return analogRead(pin);
}

/*****************************
* Public functions
*****************************/

//------CP1InIt--------
/**
 * @brief Initialization of Control Pilot Signal (Sending PWM out from CPOUT1)
 * 
 * @param duty From lookUp table in CP.h select the dutyCycle based on Current requirement by EV
 * @param CPNumber Mention the Control Pilot Number you want to initialize 
 */
void CP::CPInIt(int duty, uint8_t CPNumber){
    if (CPNumber == 1)
    {
        setCP1();    
        ledcWrite(ChanNo1,duty);
    }
    else if (CPNumber == 2)
    {
        //setCP2();    
        //ledcWrite(ChanNo2,duty);
    }
    else{
        Serial.println("Enter a valid control Pilot Number 1 or 2");
    }
}

//------getCP1EVState--------
/**
 * @brief Reads 50 AnalogRead Values from ADC PIN and generates the maxValue out of the samples.
 *        Also, returns the State of EV Charging based on the input voltage to the controller. 
 * @ref TI Designs "Level 1 and Level 2 Electric Vehicle Service Equipment (EVSE) Reference Design".
 * @param CPNumber Mention the Control Pilot Number you want to get the Status off.
 * @return char containing the state
 */
 char CP::getCPEVState(uint8_t CPNumber){
    uint8_t k=0;
    uint16_t adcBuff[50], maxValue=0;
    for(k=0;k<50;k++){
        if(CPNumber == 1){
            adcBuff[k]=getADCValue(CPIN1);
        }
        else if(CPNumber == 2){
            adcBuff[k]=getADCValue(CPIN1);
        }
        else {
            Serial.println("Enter a valid Control Pilot Number 1 or 2"); 
        }
        if(adcBuff[k]>maxValue){
            maxValue=adcBuff[k];
        }
    }
    if (maxValue > 3200){
        return A; //State A: EV Not Connected
    }
    else if ((maxValue < 3200) & (maxValue> 3110))
    {
        return AB;
    }    
    else if ((maxValue < 3110) & (maxValue> 2870)){
        return B; //State B: EV connected ready to charge
    }
    else if ((maxValue < 2870) & (maxValue> 2735))
    {
        return BC;
    }    
    else if ((maxValue < 2735) & (maxValue > 2430)){
        return C; //State C: EV charging
    }
    else if ((maxValue < 2430) & (maxValue> 2380))
    {
        return CD;
    }    
    else if ((maxValue < 2380) & (maxValue > 2005)){
        return D;  //State D: EV charging Ventilation Required
    }
    else if ((maxValue < 2005) & (maxValue > 1990))
    {
        return DE;
    }    
    else if (maxValue < 1990){
        return E; //State E: Error
    } 
    else return F;
}

 CP::~CP()
{
}