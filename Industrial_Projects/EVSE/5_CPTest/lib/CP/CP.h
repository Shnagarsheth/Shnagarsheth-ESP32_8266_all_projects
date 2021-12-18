/**
 * @file CP.h
 * @author Shaival Nagarsheth
 * @brief  Control Pilot for EVSE with Pilot Signal States. 
 * @version 0.1
 * @date 2021-12-13
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef CP_H
#define CP_H

//Define PWM pin for CP_OUT 1 and 2 and CP IN 1 and 2

#define CPOUT1 32     // Control Pilot Out IO32
#define CPOUT2 14     // Caution: Synced with ESP_TMS IO14 
#define CPIN1  34     //ADC2 pin IO34
#define CPIN2  36     //ADC2 pin IO36 (Sensor_VP)

//Define Dutycycle based on Current requirement for Charging (e.g. 15A, 16A, 30A, 32A) 
#define C05A   5440
#define C16A   17477  //Pilot Wire Example Duty Cycles for 16A
#define C32A   34931  //32A
#define C15A   16384  //15A
#define C30A   32768  //30A
#define DC100  65536  //DC - 100% Duty Cycle

//Define Parameters for PWM Channel 0 and 1
#define CPfreq 1000   //1khz
#define RESOL  16     //Choosing 16-bit resolution 0 to 65536 to use 0 to 100% Duty Cycle with more precision
#define ChanNo1 0     //PWM Channel0 
#define ChanNo2 1     //PWM Channel1 

enum EVChargingStates{A=1,AB,B,BC,C,CD,D,DE,E,F=0};

/**
 * @brief States Explained
 * State A (enumValue = 1) - EV Not Connected
 * State AB (2) - between A and B
 * State B  (3) - 8V to 10V EV Connected; Ready to Charge.
 * State BC (4) - 7V to 8V
 * State C  (5) - 5V to 7V EV Charging 
 * State CD (6) - 4V to 5V
 * State D  (7) - 2V to 4V EV Chargin; Ventillation Required.
 * State DE (8) - Below 2V
 * State E  (9) - Error (Below 1990 Counts)
 * State F  (0) - Unknown Error
 */

class CP
{
private:
    /* data */
    void setCP1(void);
    void setCP2(void);
    uint16_t getADCValue(uint8_t pin);
    
public:
    CP();
    void CPInIt(int duty, uint8_t CPNumber);
    char getCPEVState(uint8_t CPNumber);
    ~CP();   
};



#endif