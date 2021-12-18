/*
  MCP2301x.h
  2016-06-29
  Copyright (c) 2016 Peter J. Sanders. All right reserved. 
  Inspired by I2C_expander library by Koen Warffemius

 +This class provides a easy to use interface to a MCP23017 or MCP23018
 +digital expansion IC access via the two-wire I2C bus.
 +The MCP2301x ICs provide 16 digital IO ports each
 +The interface of this class looks as much the same as the default IO functions of the arduino
 +This class uses the Wire library for the i2c communication.

  versions:
  2016-06-29 – V:0.01 start of the project named MCP2301x
   
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#if defined(_DEBUG_)
#include <Arduino.h>
#ifndef _MCP2301x_DEBUG_
#define _MCP2301x_DEBUG_
#endif // _MCP2301x_DEBUG_
#endif // _DEBUG_

#if defined(FULL_INTERRUPT_SUPPORT)
#endif // FULL_INTERRUPT_SUPPORT

#include <MCP2301x.h>
#include <Wire.h>
// #include "WProgram.h"


#ifndef LOW
#define LOW  0
#endif
#ifndef HIGH
#define HIGH  1
#endif

#ifdef FULL_INTERRUPT_SUPPORT

extern "C" void attachInterrupt(uint8_t hostPin, voidFuncPtr handler, int mode); 
extern "C" voidFuncPtr callBackList[];

typedef struct {
    uint8_t port, hostPinNum, instanceAddress;
    class MCP2301x* data;    
} HOST_INT_DATA;

static bool firstInstanceFlag = false;
static HOST_INT_DATA hostIntData[NUM_MCP2301x_ICs*2];
#endif // FULL_INTERRUPT_SUPPORT

extern "C" bool writeData(uint8_t address, uint8_t regAddress, uint8_t* bytePtr, uint8_t count);
extern "C" bool readData(uint8_t address, uint8_t regAddress, uint8_t* bytePtr, uint8_t count);

// Constructors ////////////////////////////////////////////////////////////////
#ifdef _MCP2301X_DEBUG_

voidFuncPtrStr  errFunc;;  // the calling program provides an error-output function

MCP2301x::MCP2301x(voidFuncPtrStr errorCallBack)
{
    errFunc = errorCallBack;
    
#else
    
MCP2301x::MCP2301x()
{
#endif // _MCP2301X_DEBUG_

#ifdef FULL_INTERRUPT_SUPPORT
    this->hostIntPins[0] = this->hostIntPins[1] = 0xff;
#endif // FULL_INTERRUPT_SUPPORT
}

// NB: address is the sum of the address applied to the three hardware addressing
// pins A0, A1 & A2 and MCP2301X_CTRL_ID
#ifdef _MCP2301X_DEBUG_
MCP2301x::MCP2301x(voidFuncPtrStr errorCallBack, uint8_t address, uint8_t mode, uint32_t I2Cfreq = I2C_FREQ100K)
{
    errFunc = errorCallBack;
    
#else
    
MCP2301x::MCP2301x(uint8_t address, uint8_t mode, uint32_t I2Cfreq = I2C_FREQ100K)
{
    
#endif // _MCP2301X_DEBUG_
    
#ifdef FULL_INTERRUPT_SUPPORT
    this->hostIntPins[0] = this->hostIntPins[1] = 0xff;
#endif // FULL_INTERRUPT_SUPPORT
    init(address, mode, I2Cfreq);
}

// Public Methods //////////////////////////////////////////////////////////////
// This function initialises AND/OR RE-INITIALISES the chip to safe defaults
bool MCP2301x::init(uint8_t address, uint8_t mode, uint32_t I2Cfreq = I2C_FREQ100K)
{
#ifdef FULL_INTERRUPT_SUPPORT
    int i, j;
#endif // FULL_INTERRUPT_SUPPORT
    
    if(address < MCP2301X_CTRL_ID || address > (MCP2301X_CTRL_ID+7)) {
#ifdef _MCP2301X_DEBUG_
        (*errFunc)(String("Bad ID/address in init()"));
#endif // _MCP2301X_DEBUG_
        return false;
    } 
    this->addr = address;
    
#ifdef FULL_INTERRUPT_SUPPORT 
    // This initialisation occurs at instantiation of each instance of this class.
    this->intInitialisedFlags[1] = this->intInitialisedFlags[0] = ALL_DISABLED;   
    for(i=0; i < 2; i++) {
        for(j=0; j < 7; j++) {
            this->clientCallbacks[i][j] = (voidFuncPtr)NULL;
        }
    }
    // This initialisation only occurs on initialisation of the first instance of this class
    // and will NEVER be re-initialised by this init()    
    // First instance is detectable by the 'firstInstanceFlag' being false
    if( ! firstInstanceFlag) {
        firstInstanceFlag = true;
        for(i=0; i < (NUM_MCP2301x_ICs*2); i++) {
            hostIntData[i].instanceAddress = 0xff;
            hostIntData[i].hostPinNum = 0xff;
            hostIntData[i].port = 0xff;
            hostIntData[i].data = (class MCP2301x*)NULL;
        }
   }
#endif // FULL_INTERRUPT_SUPPORT

    Wire.begin();              // TODO: How do multiple instances effect this?
    Wire.setClock(I2Cfreq);    // TODO: What about other libraries calling the Wire library?

//  IOCON.SEQOP and IOCON.BANK bits must both be 0 to use this library. Set the IOCON for this IC
//  corresponding to this instance of the MCP2301x class.
    this->iocon = mode & (IOCON_INTCC | IOCON_INTPOL | IOCON_ODR | IOCON_MIRROR); // allowed modes

#ifdef _MCP2301X_DEBUG_
//  (*errFunc)(String("Setting operating modes"));
#endif // _MCP2301X_DEBUG_

// By default all pins are inputs. Send the IOCON mode selections mask.
    if( ! writeData(this->addr, (uint8_t)(REGISTER_IOCON), &(this->iocon), 1) || ! pinModePort(0, ALL_INPUT) || ! pinModePort(1, ALL_INPUT)) {
        return false;
    }

    pinPolarityPort(0, ALL_NORMAL);     // By default outputs are not inverted.
    pinPolarityPort(0, ALL_NORMAL);  
      
    digitalWritePort(0, ALL_LOW);
    digitalWritePort(1, ALL_LOW);

#ifdef INTERRUPT_SUPPORT    
    pinIntEnablePort(0, ALL_DISABLED);
    pinIntEnablePort(1, ALL_DISABLED);
// At this point, interrupts may be triggered, clear them.
    if(this->iocon & IOCON_INTCC) {
//      (*errFunc)(String("Clearing Interrupts"));
        if(getAllInterruptFlags() == 0xFFFF) return false;      //Clear interrupts 
    }   
#endif // INTERRUPT_SUPPORT
  
    return readData(this->addr, (uint8_t)(REGISTER_GPIO0), ports, 2); // If IOCON.INTCC is not set, this will clear interrupts
}

// ##################################################################
// If mode = 0, that pin is required to be an input but the
// corresponding bit in the IODIR register must be a 1
// BUT pinMode() calls pinModePort() which inverts all bits, so
// in the mask passed to pinModePort(), bits which are to be
// inputs must be 0
bool MCP2301x::pinMode(uint8_t port,uint8_t pin, bool mode)
{
    uint8_t temp = 0, mask;
    
    if(port > 1 ) {  // Only ports 0 and 1
#ifdef _MCP2301X_DEBUG_
        (*errFunc)(String("Error in pinMode(), invalid port number"));
#endif // _MCP2301X_DEBUG_
        return false;
    }
    if(pin > 7) {  // Only pins 0 to 7
#ifdef _MCP2301X_DEBUG_
        (*errFunc)(String("Error in pinMode(), invalid pin number"));
#endif // _MCP2301X_DEBUG_
        return false;
    }

#ifdef _MCP2301X_DEBUG_
//    (*errFunc)(String("Setting one bit's direction"));
#endif // _MCP2301X_DEBUG_

    mask = ~(this->dirRegisters[port]);
    
	temp = 1 << pin;
    if(mode){   // mode = 1 pin is to be an output, mask bit must be 1
		mask |= temp;
    }else{   // mode = 0 pin is to be an input, mask bit must be 0
	    temp = ~temp;
		mask &= temp;
    }
    return pinModePort(port, mask);
}

// ###################################################################
bool MCP2301x::pinModePort(uint8_t port, uint8_t mask)
{
#ifdef _MCP2301X_DEBUG_
//    (*errFunc)(String("Setting port bits direction"));
#endif // _MCP2301X_DEBUG_
   
    if(port > 1) { // we only have two ports
#ifdef _MCP2301X_DEBUG_
    (*errFunc)(String("Error in pinModePort(), invalid port number"));
#endif // _MCP2301X_DEBUG_
        return false;
    }
    
	// if an input bit=0, that pin is to be an input, so MCP2301x IODIR bit must be a 1
    mask = ~mask;   // MCP2301x chip convention is the inverse of Arduino convention.
	this->dirRegisters[port] = mask;
    
    return writeData(this->addr, (uint8_t)(REGISTER_IODIR0+port), &mask, 1);
}

// ##################################################################
// Set pin polarity effects only those set to be inputs
bool MCP2301x::pinPolarity(uint8_t port,uint8_t pin, bool mode)
{
    uint8_t temp = 0, mask;
    
    if(port > 1 ) {  
#ifdef _MCP2301X_DEBUG_
        (*errFunc)(String("Error in pinPolarity(), invalid port number"));
#endif // _MCP2301X_DEBUG_
        return false;
    }
    if(pin > 7) {
#ifdef _MCP2301X_DEBUG_
        (*errFunc)(String("Error in pinModePort(), invalid pin number"));
#endif // _MCP2301X_DEBUG_
        return false;
    }

    
// TODO: Error message if the pin is not an input.    
    
    
#ifdef _MCP2301X_DEBUG_
//    (*errFunc)(String("Setting one bit's polarity");
#endif // _MCP2301X_DEBUG_

    mask = this->polRegisters[port];
    
	temp = 1 << pin;
    if(mode){  //  LOW = normal, HIGH = INVERTED
        mask |= temp;
    } else {
        temp = ~temp;
        mask &= temp;
    }
    return pinPolarityPort(port, mask);   
}

// #############################################
// Set pin polarity effects only those set to be inputs
bool MCP2301x::pinPolarityPort(uint8_t port, uint8_t mask)
{
    if(port > 1) {  // we only have two ports
#ifdef _MCP2301X_DEBUG_
        (*errFunc)(String("Error in pinPolarityPort(), invalid port number"));
#endif // _MCP2301X_DEBUG_
        return false;
    }
  
#ifdef _MCP2301X_DEBUG_
//    (*errFunc)(String("Setting all port bits polarity");
#endif // _MCP2301X_DEBUG_
   
	this->polRegisters[port] = mask;
    
    //send new config to IPOL REGISTER
    return writeData(this->addr, (uint8_t)(REGISTER_IPOL0+port), &mask, 1);
}

// ################################################		
bool MCP2301x::digitalWrite(uint8_t port, uint8_t pin, bool value)
{
	uint8_t temp = 0;

    if(port > 1 ) { 
#ifdef _MCP2301X_DEBUG_
        (*errFunc)(String("Error in digitalWrite(), invalid port number"));
#endif // _MCP2301X_DEBUG_
        return false;
    }
    if(pin > 7) { 
#ifdef _MCP2301X_DEBUG_
        (*errFunc)(String("Error in digitalWrite(), invalid pin number"));
#endif // _MCP2301X_DEBUG_
        return false;
    }

// TODO: Error message if one or more pins is not an input.    


    
	//check if we are dealing with an output pin
	temp = 1 << pin;
	if(this->dirRegisters[port] & temp) {
#ifdef _MCP2301X_DEBUG_
        (*errFunc)(String("Error in digitalWrite(), writing to an input pin"));
#endif // _MCP2301X_DEBUG_
        return false;  //if the bit in dirRegisters[port] is 1 its an input and we return false;
    }
	
	if(value){
		//set HIGH = 1;
		this->ports[port] |= temp;
	}else{
        // st LOW = 0
		temp = ~temp;
		this->ports[port] &= temp;
	}	
    return writeData(this->addr, (uint8_t)(REGISTER_GPIO0+port), &(this->ports[port]), 1);
}

// ##################################################
bool MCP2301x::digitalWritePort(uint8_t port, uint8_t value)
{
    if(port > 1) { 
#ifdef _MCP2301X_DEBUG_
        (*errFunc)(String("Error in digitalWritePort(), invalid port number"));
#endif // _MCP2301X_DEBUG_
        return false;
    } 
	
    ports[port] = value;  // only effects those pins which are outputs
	
    return writeData(this->addr, (uint8_t)(REGISTER_GPIO0+port), &(this->ports[port]), 1);
}

//#######################################################		
int MCP2301x::digitalRead(uint8_t port, uint8_t pin)
{
	uint8_t temp = 0;
    
	if(port > 1 ) {
#ifdef _MCP2301X_DEBUG_
        (*errFunc)(String("Error in digitalRead(), invalid port number"));
#endif // _MCP2301X_DEBUG_
        return -1;
    }
    if(pin > 7) {
#ifdef _MCP2301X_DEBUG_
        (*errFunc)(String("Error in digitalRead(), invalid pin number"));
#endif // _MCP2301X_DEBUG_
        return -1;
    }
    
    readData(this->addr, (uint8_t)(REGISTER_GPIO0+port), &(this->ports[port]), 1);
	
	temp = 1 << pin;
	if(this->ports[port] & temp) return HIGH; //if the pin in the ports[port] register is high we return HIGH;
	return LOW;
} 

//############################
int MCP2301x::digitalReadPort(uint8_t port)
{
	if(port > 1) {
#ifdef _MCP2301X_DEBUG_
        (*errFunc)(String("Error in digitalReadPort(), invalid port number"));
#endif // _MCP2301X_DEBUG_
        return -1;
    } 
    
    readData(this->addr, (uint8_t)(REGISTER_GPIO0+port), &(this->ports[port]), 1);
	
    return this->ports[port];
} 


#ifdef FULL_INTERRUPT_SUPPORT // ********************************************************************************
// ############################################################################################################
// This routine provides a callback function (interruptService()) which is called when the chip activates the
// specified pin on the host Arduino.  That service routine then calls the 'clientCallback' function specified
// if the chip flags that it was the specified pin on the MCP2301x IC that initiated the interrupt
bool MCP2301x::createInterrupt(uint8_t hostPin, uint8_t port, uint8_t pin, voidFuncPtr clientCallBack, int mode)
{
    int i;
    uint8_t temp;
    
    temp = 1 << pin;
    if( ! (temp & this->dirRegisters[port])) {
 #ifdef _MCP2301X_DEBUG_
            (*errFunc)(String("In createInterrupt(): Cannot create an interrupt. Pin is an output"));
#endif // _MCP2301X_DEBUG_  
        return false;       
    }
    if(temp & this->intInitialisedFlags[port]) {
#ifdef _MCP2301X_DEBUG_
            (*errFunc)(String("In createInterrupt(): This MCP2301x pin already assigned an interrupt"));
#endif // _MCP2301X_DEBUG_  
        return false;      
    }
    
    if(this->hostIntPins[port] == 0xff) {   // No interrupts yet assigned to this port of this instance.
/*
        for(i=0; i < (NUM_MCP2301x_ICs*2); i++) {
            if(hostIntData[i].hostPinNum ==  hostPin){
                
// TODO: Deal with possibility of multiple ports interrupts attached to a single arduino pin.
// such as when IOCON.MIRROR is set and both ports of one IC can assert interrupt on one host pin.
// Also, if IOCON.ODR is set, multiple IC's can assert interrupt on one host pin.                
             
#ifdef _MCP2301X_DEBUG_
                (*errFunc)(String("In createInterrupt(): This Arduino pin already assigned to an interrupt"));
#endif // _MCP2301X_DEBUG_
                return false;  
            }
        }
 */
        for(i=0; i < (NUM_MCP2301x_ICs*2); i++) {
            if(hostIntData[i].hostPinNum == 0xff) {
                goto foundVacancy;
            }
        }
#ifdef _MCP2301X_DEBUG_
        (*errFunc)(String("Out of allocated space to store interrupt access data."));
#endif // _MCP2301X_DEBUG_
        return false;
        
foundVacancy:
        hostIntData[i].instanceAddress = this->addr;
        hostIntData[i].port = port;
        hostIntData[i].hostPinNum = hostPin;
        hostIntData[i].data = this;   // Save a pointer to the current instance.
        
        attachInterrupt(hostPin, callBackList[i], mode); 

        this->clientCallbacks[port][pin] = clientCallBack; 
        this->intArrayIndex[port] = i;
        
    } else {    // Interrupt assigned to this port but not this pin
        this->intInitialisedFlags[port] |= temp;        
    }   
    this->clientCallbacks[port][pin] = clientCallBack;  // A new MCP2301x interrupt is created.
        
    return true;
}

// ###############################################################
bool MCP2301x::removeInterrupt(uint8_t port, uint8_t pin)
{
    uint8_t temp;
    
    temp = 1 << pin;
    
    if( ! (temp & this->intInitialisedFlags[port])) {
#ifdef _MCP2301X_DEBUG_
        (*errFunc)(String("In removeInterrupt(): No interrupt has been initialised at this port and pin."));
#endif // DEBUG_
        return false;
    }
    temp = ~temp;
    this->intInitialisedFlags[port] &= temp;
    this->clientCallbacks[port][pin] = (voidFuncPtr)NULL;
    
    if( ! this->intInitialisedFlags[port]) { // If there are now no remaining interrupts set up on this port.
        hostIntData[this->intArrayIndex[port]].instanceAddress = 0xff;    
        hostIntData[this->intArrayIndex[port]].port = 0xff;
        hostIntData[this->intArrayIndex[port]].hostPinNum = 0xff;
        hostIntData[this->intArrayIndex[port]].data = (class MCP2301x*)NULL;
        this->intArrayIndex[port] = 0xff;
    }
    return true;
}

#endif // FULL_INTERRUPT_SUPPORT
#ifdef INTERRUPT_SUPPORT

// ################################################################
// Re-enable all previously initialised chip interrupts, both ports
bool MCP2301x::allowInterrupts(void)
{
    return writeData(this->addr, (uint8_t)(REGISTER_GPINTEN0), this->intEnableRegs, 2);
}

// ###############################################################
// same as above but effects one port only.
bool MCP2301x::allowInterrupts(uint8_t port)
{
    return writeData(this->addr, (uint8_t)(REGISTER_GPINTEN0+port), &(this->intEnableRegs[port]), 1);
}

// ################################################################
// Disable all chip interrupts only at the chip, both ports.
// All data set up by attachInterrupt() calls remain current
// and will be re-activated by an interrupts() call.
bool MCP2301x::stopInterrupts(void)
{
    uint8_t allZero[2] = { 0, 0 };
    
    return writeData(this->addr, (uint8_t)(REGISTER_GPINTEN0), allZero, 2);
}

// #################################################################
// Same as above but effects one port only.
bool MCP2301x::stopInterrupts(uint8_t port)
{
    uint8_t allZero = { 0 };
    
    return writeData(this->addr, (uint8_t)(REGISTER_GPINTEN0+port), &allZero, 1);
}
    
//##############################
// This operation also clears all interrupts if the IOCON.INTCC bit is set
// The calling routine will need to call getErrCode() to find out if an error occurred.
uint16_t MCP2301x::getAllInterruptFlags(void)
{
    if( ! readData(this->addr, (uint8_t)REGISTER_INTCAP0, &(this->intFlagRegs[0]), 2)) return 0xFFFF;
    
    return (((uint16_t)this->intFlagRegs[1] << 8) | (uint16_t)this->intFlagRegs[0]);
}

// #################################
uint8_t MCP2301x::getInterruptFlagsPort(uint8_t port)               // Return interrupt flags from one port.
{
    if( ! readData(this->addr, (uint8_t)REGISTER_INTCAP0, &(this->intFlagRegs[0]), 2)) return 0xFF;
    return this->intFlagRegs[port];
}

// ##################################################
bool MCP2301x::pinIntEnable(uint8_t port, uint8_t pin, bool value)
{
	uint8_t temp1 = 0, temp2;

    if(port > 1 ) { 
#ifdef _MCP2301X_DEBUG_
        (*errFunc)(String("Error in pinIntEnable(), invalid port number"));
#endif // _MCP2301X_DEBUG_
        return false;
    }
    if(pin > 7) { 
#ifdef _MCP2301X_DEBUG_
        (*errFunc)(String("Error in pinIntEnable(), invalid pin number"));
#endif // _MCP2301X_DEBUG_
        return false; 
    }
    
    temp2 = this->intEnableRegs[port];
        
	temp1 = 1 << pin;
	if(value){
		temp2 |= temp1;
	} else {
		temp1 = ~temp1;
	    temp2 &= temp1;
	}
    return pinIntEnablePort(port, temp2);
}

// ###########################################################
bool MCP2301x::pinIntEnablePort(uint8_t port, uint8_t mask)
{
    if(port > 1) {
#ifdef _MCP2301X_DEBUG_
        (*errFunc)(String("Error in pinIntEnablePort(), invalid port number"));
#endif // _MCP2301X_DEBUG_
        return false;
    } // Only two ports
  
    this->intEnableRegs[port] = mask;  // only effective on those pins which are inputs
    return writeData(this->addr, (uint8_t)(REGISTER_GPINTEN0+port), &mask, 1);
}

#endif // INTERRUPT_SUPPORT

// ####################################

extern "C" {

bool writeData(uint8_t address, uint8_t regAddress, uint8_t* bytePtr, uint8_t count)
{
    if(count < 1 || count > 2) {
#ifdef _MCP2301X_DEBUG_
        (*errFunc)(String("Error in witeData(), byte count < 1 or > 2"));
#endif // _MCP2301X_DEBUG_
        return false;
    }
    Wire.beginTransmission(address);  
    Wire.write(regAddress);  	     //select Output latches
    Wire.write(bytePtr[0]);  		 //write to register
    if(--count) {
        Wire.write(bytePtr[1]);      // write to second register
    }
    if(Wire.endTransmission()) {
#ifdef _MCP2301X_DEBUG_
        (*errFunc)(String("Error in writeData(), transmission error"));
#endif // _MCP2301X_DEBUG_
	    return false;
    } else { // endTransmission returns 0 on no Tx error
	    return true;
    }
}

//##########################################
bool readData(uint8_t address, uint8_t regAddress, uint8_t* bytePtr, uint8_t count)
{
    if(count < 1 || count > 2) {
#ifdef _MCP2301X_DEBUG_
        (*errFunc)(String("Error in readData(), byte count < 1 or > 2"));
#endif // _MCP2301X_DEBUG_
        return false;
    }
    Wire.beginTransmission(address);  
    Wire.write(regAddress);      //select register
    if(Wire.endTransmission()) { // returns 0 on no Tx error
#ifdef _MCP2301X_DEBUG_
        (*errFunc)(String("Error in readData(), no I2C communication available"));
#endif // _MCP2301X_DEBUG_
        return false;
    }
  
    Wire.requestFrom(address, count);   //request count bytes.
    if(Wire.available() == count){
        bytePtr[0] = Wire.read();   //receive one byte
        if(--count) {
            bytePtr[1] = Wire.read(); // receive second byte
        }
		return true;
	} else {
#ifdef _MCP2301X_DEBUG_
    (*errFunc)(String("Error in readData(), receive error"));
#endif // _MCP2301X_DEBUG_
		return false;
    }    
}
       
#ifdef FULL_INTERRUPT_SUPPORT

// Service an interrupt using data stored by createInterrupt()
// This function must handle interrupts for every active instance.
void interruptService(int arrayIndex)
{
    class MCP2301x* dataPtr;
    uint8_t temp, intFlags;
    int i;

// Recall the pointer to the data belonging to the appropriate instance of class MCP2301x
// IE. get a copy of the 'this' pointer to the instance which set up this interrupt.    
    dataPtr = hostIntData[arrayIndex].data;
    
// if IOCON.INTCC is set this will clear the interrupt on the IC's port.
    readData(hostIntData[arrayIndex].instanceAddress, (uint8_t)(REGISTER_INTCAP0+hostIntData[arrayIndex].port), &(dataPtr->intFlagRegs[0]), 1);
    intFlags = dataPtr->intFlagRegs[hostIntData[arrayIndex].port];
    
// Shift through all the bits, calling functions if INTCAP bits are active    
// NB: bit 0 is highest priority, bit 7 is lowest.
    temp = 1;
    for(i=0; i <= 7; i++, temp <<= 1) {
        if((temp & intFlags)) {
            if(dataPtr->clientCallbacks[hostIntData[arrayIndex].port][i] == (voidFuncPtr)NULL) {
#ifdef _MCP2301X_DEBUG_
                (*errFunc)(String("An interrupt occurred on a bit with no corresponding service defined"));
#endif // DEBUG_
            } else {
                (*(dataPtr->clientCallbacks[hostIntData[arrayIndex].port][i]))();  // Call the users function
            }                
        }    
    }
}

void hostCallBack00(void)
{
    interruptService(0);    
}
void hostCallBack01(void)
{
    interruptService(1);    
}

#if (NUM_MCP2301x_ICs > 1)
void hostCallBack02(void)
{
    interruptService(2);    
}
void hostCallBack03(void)
{
    interruptService(3);    
}
#endif // NUM_MCP2301x_ICs > 1

#if (NUM_MCP2301x_ICs > 2)
void hostCallBack04(void)
{
    interruptService(4);    
}
void hostCallBack05(void)
{
    interruptService(5);    
}
#endif // NUM_MCP2301x_ICs > 2

#if (NUM_MCP2301x_ICs > 3)
void hostCallBack06(void)
{
    interruptService(6);    
}
void hostCallBack07(void)
{
    interruptService(7);    
}
#endif // NUM_MCP2301x_ICs > 3

#if (NUM_MCP2301x_ICs > 4)
void hostCallBack08(void)
{
    interruptService(8);    
}
void hostCallBack09(void)
{
    interruptService(9);    
}
#endif // NUM_MCP2301x_ICs > 4

#if (NUM_MCP2301x_ICs > 5)
void hostCallBack10(void)
{
    interruptService(10);    
}
void hostCallBack11(void)
{
    interruptService(11);    
}
#endif // NUM_MCP2301x_ICs > 5

#if (NUM_MCP2301x_ICs > 6)
void hostCallBack12(void)
{
    interruptService(12);    
}
void hostCallBack13(void)
{
    interruptService(13);    
}
#endif // NUM_MCP2301x_ICs > 6

#if (NUM_MCP2301x_ICs > 7)
void hostCallBack14(void)
{
    interruptService(14);    
}
void hostCallBack15(void)
{
    interruptService(15);    
}
#endif // NUM_MCP2301x_ICs > 7

#if (NUM_MCP2301x_ICs < 2)
voidFuncPtr callBackList[2] = { hostCallBack00, hostCallBack01 };
#else
#if (NUM_MCP2301x_ICs == 2)
voidFuncPtr callBackList[4] = { hostCallBack00, hostCallBack01, hostCallBack02, hostCallBack03 }; 
#else
#if (NUM_MCP2301x_ICs == 3)
voidFuncPtr callBackList[6] = { hostCallBack00, hostCallBack01, hostCallBack02, hostCallBack03, 
                                hostCallBack04, hostCallBack05 };
#else
#if (NUM_MCP2301x_ICs == 4)
voidFuncPtr callBackList[8] = { hostCallBack00, hostCallBack01, hostCallBack02, hostCallBack03, 
                                hostCallBack04, hostCallBack05, hostCallBack06, hostCallBack07 };
#else
#if (NUM_MCP2301x_ICs == 5)
voidFuncPtr callBackList[10] = { hostCallBack00, hostCallBack01, hostCallBack02, hostCallBack03, 
                                 hostCallBack04, hostCallBack05, hostCallBack06, hostCallBack07,
                                 hostCallBack08, hostCallBack09 };
#else
#if (NUM_MCP2301x_ICs == 6)
voidFuncPtr callBackList[12] = { hostCallBack00, hostCallBack01, hostCallBack02, hostCallBack03, 
                                 hostCallBack04, hostCallBack05, hostCallBack06, hostCallBack07,
                                 hostCallBack08, hostCallBack09, hostCallBack10, hostCallBack11 };
#else
#if (NUM_MCP2301x_ICs == 7)
voidFuncPtr callBackList[14] = { hostCallBack00, hostCallBack01, hostCallBack02, hostCallBack03, 
                                 hostCallBack04, hostCallBack05, hostCallBack06, hostCallBack07,
                                 hostCallBack08, hostCallBack09, hostCallBack10, hostCallBack11,
                                 hostCallBack12, hostCallBack13 };
#else
#if ((NUM_MCP2301x_ICs > 7)
voidFuncPtr callBackList[16] = { hostCallBack00, hostCallBack01, hostCallBack02, hostCallBack03, 
                                 hostCallBack04, hostCallBack05, hostCallBack06, hostCallBack07,
                                 hostCallBack08, hostCallBack09, hostCallBack10, hostCallBack11,
                                 hostCallBack12, hostCallBack13, hostCallBack14, hostCallBack15 };
#endif // (NUM_MCP2301x_ICs > 7)
#endif // (NUM_MCP2301x_ICs == 7)
#endif // (NUM_MCP2301x_ICs == 6)
#endif // (NUM_MCP2301x_ICs == 5)
#endif // (NUM_MCP2301x_ICs == 4)
#endif // (NUM_MCP2301x_ICs == 3)
#endif // (NUM_MCP2301x_ICs == 2)
#endif // (NUM_MCP2301x_ICs < 2)

#endif // FULL_INTERRUPT_SUPPORT
} // extern "C"
