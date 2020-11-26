/** 
 * ---------------------------------------------------------------+ 
 * @desc        HD44780 with PCF8574 8-bit I/O expander for TWI 
 * ---------------------------------------------------------------+ 
 *              Copyright (C) 2020 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @datum       18.11.2020
 * @file        hd44780pcf8547.c
 * @tested      AVR Atmega328p
 *
 * @depend      twi, pcf8547
 * ---------------------------------------------------------------+
 */

// include libraries
#include <stdio.h>
#include <util/delay.h>
#include <avr/io.h>
#include "twi.h"
#include "hd44780pcf8574.h"

// +---------------------------+
// |         Power on          |
// | Wait for more than 15 ms  |   // 15 ms wait
// | after VCC rises to 4.5 V  |
// +---------------------------+
//              |
// +---------------------------+ 
// |  RS R/W DB7 DB6 DB5 DB4   |   
// |   0   0   0   0   1   1   |   // Initial sequence 0x30
// | Wait for more than 4.1 ms |   // 4.1 ms us writing DATA into DDRAM or CGRAM
// +---------------------------+
//              |
// +---------------------------+
// |  RS R/W DB7 DB6 DB5 DB4   |   
// |   0   0   0   0   1   1   |   // Initial sequence 0x30
// | Wait for more than 0.1 ms |   // 100 us writing DATA into DDRAM or CGRAM
// +---------------------------+
//              |
// +---------------------------+
// |  RS R/W DB7 DB6 DB5 DB4   |   // Initial sequence 0x30
// |   0   0   0   0   1   1   |   // 37 us writing DATA into DDRAM or CGRAM 4 us tadd - time after busy flag disapeared
// | Wait for more than 45 us  |   // 37 us + 4 us = 41 us * (270/250) = 45us
// +---------------------------+  
//              |
// +---------------------------+   // 4bit mode 0x20 !!! MUST BE SET TIME, BF CHECK DOESN'T WORK CORRECTLY !!!
// |  RS R/W DB7 DB6 DB5 DB4   |   // 
// |   0   0   0   0   1   0   |   // 37 us writing DATA into DDRAM or CGRAM 4 us tadd - time after busy flag disapeared
// | Wait for more than 45 us  |   // !!! MUST BE SET DELAY TIME, BUSY FLAG CHECK DOESN'T WORK CORRECTLY !!!
// +---------------------------+
//              |
// +---------------------------+
// |  RS R/W DB7 DB6 DB5 DB4   |   // Display off 0x08
// |   0   0   0   0   0   0   |   // 
// |   0   0   1   0   0   0   |   // 
// |    Wait for BF Cleared    |   // Wait for BF Cleared
// +---------------------------+
//              |
// +---------------------------+
// |  RS R/W DB7 DB6 DB5 DB4   |   // Display clear 0x01
// |   0   0   0   0   0   0   |   //
// |   0   0   0   0   0   1   |   //
// |    Wait for BF Cleared    |   // Wait for BF Cleared
// +---------------------------+
//              |
// +---------------------------+
// |  RS R/W DB7 DB6 DB5 DB4   |   // Entry mode set 0x06
// |   0   0   0   0   0   0   |   // 
// |   0   0   0   1   1   0   |   // shift cursor to the left, without text shifting
// |    Wait for BF Cleared    |   // Wait for BF Cleared
// +---------------------------+

/**
 * @desc    LCD init - initialisation routine structure
 *
 * @param   HD44780_PCF8547_Structure
 * @param   char
 *
 * @return  void
 */
void HD44780_PCF8547_Struct_Init (struct HD44780_PCF8547_Structure LCD, char addr)
{
  // lcd address
  LCD.addr = addr;
  // Init LCD
  LCD.Init = HD44780_PCF8547_Init;
}

/**
 * @desc    LCD init - initialisation routine
 *
 * @param   c
 *
 * @return  char
 */
char HD44780_PCF8547_Init (char addr)
{
  // delay > 15ms
  _delay_ms(16);

  // Init TWI
  TWI_Init();

  // TWI: start
  // -------------------------
  TWI_MT_Start();
  // TWI: send SLAW
  // -------------------------
  TWI_Transmit_SLAW(addr);

  // DB4=1, DB5=1 / BF cannot be checked in these instructions
  // ---------------------------------------------------------------------
  TWI_Transmit_Byte(PCF8574_PIN_DB4 | PCF8574_PIN_DB5);
  // delay > 4.1ms
  _delay_ms(5);

  // DB4=1, DB5=1 / BF cannot be checked in these instructions
  // ---------------------------------------------------------------------
  TWI_Transmit_Byte(PCF8574_PIN_DB4 | PCF8574_PIN_DB5);
  // delay > 100us
  _delay_us(110);

  // DB4=1, DB5=1 / BF cannot be checked in these instructions
  // ---------------------------------------------------------------------
  TWI_Transmit_Byte(PCF8574_PIN_DB4 | PCF8574_PIN_DB5);
  // delay > 45us (=37+4 * 270/250)
  _delay_us(50);

  // DB5=1 / 4 bit mode 0x20 / BF cannot be checked in these instructions
  // ----------------------------------------------------------------------
  TWI_Transmit_Byte(PCF8574_PIN_DB5);
  // delay > 45us (=37+4 * 270/250)
  _delay_us(50);

  // 
  // ----------------------------------------------------------------------
  TWI_Transmit_Byte(HD44780_4BIT_MODE | HD44780_2_ROWS | HD44780_FONT_5x8);
  // check BF

/*  
  // 4-bit & 2-lines & 5x8-dots 0x28 - send 8 bits in 4 bit mode
  HD44780_SendInstruction(HD44780_4BIT_MODE | HD44780_2_ROWS | HD44780_FONT_5x8);

  // display off 0x08 - send 8 bits in 4 bit mode
  HD44780_SendInstruction(HD44780_DISP_OFF);

  // display clear 0x01 - send 8 bits in 4 bit mode
  HD44780_SendInstruction(HD44780_DISP_CLEAR);

  // entry mode set 0x06 - send 8 bits in 4 bit mode
  HD44780_SendInstruction(HD44780_ENTRY_MODE);
*/

  // TWI Stop
  TWI_Stop();

  // success
  return TWI_SUCCESS;
}

/**
 * @desc    LCD check BF
 *
 * @param   void
 *
 * @return  char
 */
char HD44780_PCF8547_CheckBF (void)
{
  // init status
  char status = HD44780_STATUS;
  // busy flag
  char data = 0x00;

  // TWI: start
  // -------------------------
  status = TWI_MT_Start();
  // check error
  if (TWI_ERROR_NONE != _twi_error_stat) {
    // return error
    return TWI_ERROR;
  // set new error status
  } else if (TWI_SUCCESS != status) {
    // error status
    TWI_Error(status, TWI_SUCCESS);
    // return error
    return TWI_ERROR;
  }
  // TWI: send SLAR
  // -------------------------
  status = TWI_Transmit_SLAR(PCF8574_ADDRESS);
  // check status
  if (TWI_ERROR == _twi_error_stat) {
    // return error
    return TWI_ERROR;
  // set new error status
  } else if (TWI_SUCCESS != status) {
    // error status
    TWI_Error(status, TWI_SUCCESS);
    // return error
    return TWI_ERROR;
  }

  // get data
  data = TWI_Receive_Byte();
  // check status
  if (TWI_ERROR == _twi_error_stat) {
    // error status status
    _twi_error_stat = status;
    // return error
    return TWI_ERROR;
  }
  
  // return 0
  return (PCF8574_PIN_DB7 & data);
}

/**
 * @desc    LCD send instruction
 *
 * @param   char
 *
 * @return  char
 */
char HD44780_PCF8547_SendInstruction (char data)
{
/*
  // init status
  char status = HD44780_STATUS;

  // TWI: start
  // -------------------------
  status = TWI_MT_Start();
  // request - start TWI
  if (TWI_SUCCESS != status) {
    // error
    return status;
  }

  // TWI: send SLAW
  // -------------------------
  status = TWI_Transmit_SLAW(PCF8574_ADDRESS);
  // request - send SLAW
  if (TWI_SUCCESS != status) {
    // error
    return status;
  }

  // Clear RS
  // HD44780_PORT_RS &= ~(1 << HD44780_RS);
  // ----------------------------------------------------------------------
  status = TWI_Transmit_Byte((~PCF8574_PIN_RS) & data);
  // request - start TWI
  if (TWI_SUCCESS != status) {
    // error
    return status;
  }

  // TWI Stop
  TWI_Stop();
*/
  // success
  return TWI_SUCCESS;
}
