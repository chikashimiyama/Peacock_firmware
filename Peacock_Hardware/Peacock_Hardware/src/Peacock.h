
#ifndef _PEACOCK_H_
#define _PEACOCK_H_


#define SERIAL_TEST

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>
#include "Descriptors.h"
#include <LUFA/Drivers/Board/LEDs.h>
#include <LUFA/Drivers/Board/Joystick.h>
#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Platform/Platform.h>
#include "lcd.h"



// Peacock sensors
#define COLUMN 7
#define ROW 5
#define NUM_SENSORS COLUMN * ROW
// LED
#define LED_PORT PORTC 
#define LED_DDR	 DDRC
#define LED_PIN  PORTC4

// BUTTON
#define BUTTON_BT0_PORT PORTD
#define BUTTON_BT0_DDR  DDRD
#define BUTTON_BT0_PIN  PORTD0

#define BUTTON_BT1_PORT PORTD
#define BUTTON_BT1_DDR  DDRD
#define BUTTON_BT1_PIN  PORTD1

#define BUTTON_BT2_PORT PORTD
#define BUTTON_BT2_DDR  DDRD
#define BUTTON_BT2_PIN  PORTD2

#define BUTTON_BT3_PORT PORTD
#define BUTTON_BT3_DDR  DDRD
#define BUTTON_BT3_PIN  PORTD3

// SPI
#define SPI_PORT PORTB 
#define SPI_DDR  DDRB
#define SPI_SCK  PORTB1
#define SPI_MOSI PORTB2
#define SPI_MISO PORTB3

#define SPI_CS0_PORT PORTB
#define SPI_CS0_DDR  DDRB
#define SPI_CS0_PIN  PORTB0
 
#define SPI_CS1_PORT PORTC
#define SPI_CS1_DDR  DDRC
#define SPI_CS1_PIN  PORTD2

#define SPI_CS2_PORT PORTD
#define SPI_CS2_DDR  DDRD
#define SPI_CS2_PIN  PORTD4

#define SPI_CS3_PORT PORTD
#define SPI_CS3_DDR  DDRD
#define SPI_CS3_PIN  PORTD5

#define SPI_CS4_PORT PORTD
#define SPI_CS4_DDR  DDRD
#define SPI_CS4_PIN  PORTD6

#define SPI_SINGLE_MODE_MASK 0x18 // bit 3 4 HI

// protocol

// delimeter
#define START_DELIMETER 0xFF
#define END_DELIMETER 0xFE

// type of message
#define DATA_PACKET 0xFD
#define COMMAND_PACKET 0xFC
#define ERROR_PACKET 0xFB

// data max
#define DATA_MAX 0xFA // 250

void setupHardware(void);
uint8_t spiReadWrites(uint8_t byte);
void sendMessage(void);	
void toggleLED(bool sw);
void EVENT_USB_Device_Connect(void);
void EVENT_USB_Device_Disconnect(void);
void EVENT_USB_Device_ConfigurationChanged(void);
void EVENT_USB_Device_ControlRequest(void);

#endif

