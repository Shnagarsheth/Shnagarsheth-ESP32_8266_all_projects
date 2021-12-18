
// #define _DEBUG_

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
  2016-06-29 � V:0.01 start of the project named MCP2301x
   
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

#ifndef MCP2301x_h
#define MCP2301x_h

#ifdef _DEBUG_
#define _MCP2301X_DEBUG_
#include <Wstring.h>
#endif // __DEBUG_
#include <inttypes.h>

#ifdef _MCP2301X_DEBUG_
typedef void (*voidFuncPtrStr)(String errMessg);  // the calling program provides an error-output function
#endif // _MCP2301X_DEBUG_

// Either of these may be #defined here
// #define INTERRUPT_SUPPORT           // Compile partial interrupt support serviced mainly by the calling program
// #define FULL_INTERRUPT_SUPPORT      // Compile comprehensive interrupt support for up to 128 input pins.

// Or they may be #defined in the calling program
#ifdef FULL_INTERRUPT_SUPPORT
typedef void (*voidFuncPtr)(void);
// defines how much space will be allocated for full interrupt support of up to 8 MCP2301x ICs
#ifndef NUM_MCP2301x_ICs            // Minimum 1, maximum 8  may be defined in the calling program
#define NUM_MCP2301x_ICs        1   // A minimum of 1 MUST be defined if FULL_INTERRUPT_SUPPORT is.
#endif // NUM_MCP2301x_ICs
// Make sure that if FULL_SUPPORT is defined, partial support is also
#ifndef INTERRUPT_SUPPORT
#define INTERRUPT_SUPPORT
#endif // INTERRUPT_SUPPORT
#endif // FULL_INTERRUPT_SUPPORT

#define MCP2301X_CTRL_ID     0x50

// Register addresses for IOCON.BANK = 0 which is
// the only addressing mode supported by this library.
// For the sake of library consistency the manufacturer's
// use of the portA, portB convention is abandoned and
// replaced by the use of ports[0], ports[1]
#define REGISTER_IODIR0        0x00
#define REGISTER_IPOL0         0x02
#define REGISTER_GPINTEN0      0x04
#define REGISTER_DEFVAL0       0x06
#define REGISTER_INTCON0       0x08
#define REGISTER_IOCON         0x0A
#define REGISTER_GPPU0         0x0C
#define REGISTER_INTF0         0x0E
#define REGISTER_INTCAP0       0x10
#define REGISTER_GPIO0         0x12
#define REGISTER_OLAT0         0x14

// Bit masks for IOCON register control bits
#define IOCON_INTCC            0x01   // 0=reading GPIO, 1=reading INTCAP clears interrupt
#define IOCON_INTPOL           0x02   // 0=ActiveLow, 1=ActiveHigh interrupt polarity
#define IOCON_ODR              0x04   // 0=ActiveDriver, 1=OpenDrain Interrupt (overrides INTPOL)
// Bits 3 & 4 (0x08 and 0x10) are unimplemented
#define IOCON_SEQOP            0x20   // 0=Enable, 1=Disable sequential & incrementing register addressing
#define IOCON_MIRROR           0x40   // 0=Disable, 1=Enable internal wire-OR-ing of the two interrupts
#define IOCON_BANK             0x80   // 0=Disable, 1=Enable separation of registers into two banks

// Values for bit and port polarity
#define NORMAL                 LOW
#define INVERTED               HIGH
#define ALL_NORMAL             0x00
#define ALL_INVERTED           0xFF

// Values for bit and port interrupt enables
#define PIN_DISABLED           LOW
#define PIN_ENABLED            HIGH
#define ALL_DISABLED           0x00
#define ALL_ENABLED            0xFF

// Values for port directions (already defined for pins)
#define ALL_INPUT              0x00
#define ALL_OUTPUT             0xFF

// Values for port values (already defined for pins)
// Also values for port default values (already defined for pins)
#define ALL_LOW                0x00
#define ALL_HIGH               0xFF

// Values can be specified to Wire.setClock();
#define I2C_FREQ100K         100000     // Default 100kHz initial value
#define I2C_FREQ400K         400000   
#define I2C_FREQ1700K       1700000  

class MCP2301x
{
private:
    uint8_t addr;
    uint8_t iocon;
	uint8_t dirRegisters[2];
	uint8_t polRegisters[2];
	uint8_t ports[2];

public:
#ifdef INTERRUPT_SUPPORT

    uint8_t intEnableRegs[2];
    uint8_t intFlagRegs[2];
    
#ifdef FULL_INTERRUPT_SUPPORT

    uint8_t hostIntPins[2];                // Pins on host Arduino to which this class has attached interrupts
    uint8_t intArrayIndex[2];              // index into the array of stored interrupt data
    uint8_t intInitialisedFlags[2];        // A set bit means that an int has been enabled in this class for that pin.
    voidFuncPtr clientCallbacks[2][8];     // Interrupt Callback Functions attached to up to 16 bits.
    
#endif // FULL_INTERRUPT_SUPPORT
#endif // INTERRUPT_SUPPORT
	
// NB: address is the sum of MCP2301X_CTRL_ID and the value
// applied to the 3 hardware pins A0,A1 & A2 in the range 0 - 7	
#ifdef _MCP2301X_DEBUG_
    MCP2301x(voidFuncPtrStr errCallBack);                                                 
    MCP2301x(voidFuncPtrStr errCallBack, uint8_t address, uint8_t mode, uint32_t i2cfreq); 
#else
    MCP2301x(void);                                             // Each separate IC requires its own instance
    MCP2301x(uint8_t address, uint8_t mode, uint32_t i2cfreq);  //  of this class to be constructed
#endif // _MCP2301X_DEBUG_

    bool init(uint8_t address, uint8_t mode, uint32_t i2Cfreq); // mode is the bit mask to be applied to IOCON
	bool pinMode(uint8_t port, uint8_t pin, bool mode);         // mode is INPUT or OUTPUT
	bool pinModePort(uint8_t port, uint8_t mask);               // mask as a bit mask for INPUTs or OUTPUTs
	bool pinPolarity(uint8_t port, uint8_t pin, bool mode);     // mode is NORMAL or INVERTED
	bool pinPolarityPort(uint8_t port, uint8_t mask);           // mask as a bit mask for NORMAL bits or INVERTED bits
	bool digitalWrite(uint8_t port, uint8_t pin, bool value);   // mode is to be written to one bit
	bool digitalWritePort(uint8_t port, uint8_t value);         // value is a bit mask to be written to the port
	int digitalRead(uint8_t port, uint8_t pin);                 // read one bit
	int digitalReadPort(uint8_t port);                          // read one byte (-1 on error);

#ifdef INTERRUPT_SUPPORT

    uint16_t getAllInterruptFlags(void);                        // Return interrupt flags from both ports combined,
    uint8_t getInterruptFlagsPort(uint8_t port);                // Return interrupt flags from one port.
	bool pinIntEnable(uint8_t port, uint8_t pin, bool value);   // value is to enable an interrupt on one pin
	bool pinIntEnablePort(uint8_t port, uint8_t mask);          // mask is a bit mask to enable interrupts
    bool allowInterrupts(void);                                 // Re-enable using stored data
    bool allowInterrupts(uint8_t port);
    bool stopInterrupts(void);                                  // Non-destructive disable of the IC's interrupts.
    bool stopInterrupts(uint8_t port);

#ifdef FULL_INTERRUPT_SUPPORT

// These two functions (and a lot of extra code within the library) support the interrupt mechanisms
// whereby up to 128 different functions in the calling program may be invoked by interrupts.
    bool createInterrupt(uint8_t hostPin, uint8_t port, uint8_t pin, voidFuncPtr callBack, int mode);
    bool removeInterrupt(uint8_t port, uint8_t pin);
    
#endif // FULL_INTERRUPT_SUPPORT    
#endif // INTERRUPT_SUPPORT
};

#endif

