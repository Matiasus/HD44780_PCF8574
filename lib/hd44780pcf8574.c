/** 
 * ---------------------------------------------------------------+ 
 * @desc        HD44780 with PCF8574 8-bit I/O expander for TWI 
 * ---------------------------------------------------------------+ 
 *              Copyright (C) 2020 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @datum       18.11.2020
 * @file        hd44780pcf8574.c
 * @tested      AVR Atmega328p
 *
 * @depend      twi, pcf8574
 * ---------------------------------------------------------------+
 */

// include libraries
#include <stdio.h>
#include <util/delay.h>
#include <avr/io.h>
#include "twi.h"
#include "st7735.h"
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
// |   0   0   0   0   1   0   |   // 
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
 * @param   HD44780_PCF8574_Structure
 * @param   char
 *
 * @return  void
 */
void HD44780_PCF8574_Struct_Init (struct HD44780_PCF8574_Structure LCD)
{
  // Init LCD
  LCD.Init = HD44780_PCF8574_Init;
  // clear screen
  LCD.Clear = HD44780_PCF8574_DisplayClear;
  // cursor on
  LCD.CursorOn = HD44780_PCF8574_CursorOn;
}

/**
 * @desc    LCD init - initialisation routine
 *
 * @param   char
 *
 * @return  char
 */
char HD44780_PCF8574_Init (char addr)
{
  // delay > 15ms
  _delay_ms(16);

  // Init TWI
  TWI_Init();

  // TWI: start
  // -------------------------
  TWI_MT_Start();
  // check if success
  if (_twi_error_stat != TWI_ERROR_NONE) {
    // return error
    return PCF8574_ERROR;
  }

  // TWI: send SLAW
  // -------------------------
  TWI_Transmit_SLAW(addr);
  // check if success
  if (_twi_error_stat != TWI_ERROR_NONE) {
    // return error
    return PCF8574_ERROR;
  }

  // DB7 BD6 DB5 DB4 P3 E RW RS 
  // DB4=1, DB5=1 / BF cannot be checked in these instructions
  // ---------------------------------------------------------------------
  HD44780_PCF8574_Send_4bits_M4b_I(PCF8574_PIN_DB4 | PCF8574_PIN_DB5);
  // delay > 4.1ms
  _delay_ms(5);

  // DB4=1, DB5=1 / BF cannot be checked in these instructions
  // ---------------------------------------------------------------------
  HD44780_PCF8574_Send_4bits_M4b_I(PCF8574_PIN_DB4 | PCF8574_PIN_DB5);
  // delay > 100us
  _delay_us(110);

  // DB4=1, DB5=1 / BF cannot be checked in these instructions
  // ---------------------------------------------------------------------
  HD44780_PCF8574_Send_4bits_M4b_I(PCF8574_PIN_DB4 | PCF8574_PIN_DB5);
  // delay > 45us (=37+4 * 270/250)
  _delay_us(50);

  // DB5=1 / 4 bit mode 0x20 / BF cannot be checked in these instructions
  // ----------------------------------------------------------------------
  HD44780_PCF8574_Send_4bits_M4b_I(PCF8574_PIN_DB5);
  // delay > 45us (=37+4 * 270/250)
  _delay_us(50);

  // TWI Stop
  TWI_Stop();

  // +---------------------------+
  // |  RS R/W DB7 DB6 DB5 DB4   |   // Display off 0x08
  // |   0   0   0   0   1   0   |   // 
  // |   0   0   1   0   0   0   |   // 
  // |    Wait for BF Cleared    |   // Wait for BF Cleared
  // +---------------------------+
  // d7 d6 d5 d4 p3 cs rw rs
  //  0  0  1  0  0  0  0  0
  HD44780_PCF8574_SendInstruction(addr, HD44780_4BIT_MODE | HD44780_2_ROWS | HD44780_FONT_5x8);

  // display off 0x08 - send 8 bits in 4 bit mode
  HD44780_PCF8574_SendInstruction(addr, HD44780_DISP_OFF);

  // display clear 0x01 - send 8 bits in 4 bit mode
  HD44780_PCF8574_SendInstruction(addr, HD44780_DISP_CLEAR);

  // entry mode set 0x06 - send 8 bits in 4 bit mode
  HD44780_PCF8574_SendInstruction(addr, HD44780_ENTRY_MODE);

  // return success
  return PCF8574_SUCCESS;
}

/**
 * @desc    LCD send 4bits instruction in 4 bit mode
 *
 * @param   char
 *
 * @return  void
 */
void HD44780_PCF8574_Send_4bits_M4b_I (char data)
{
  char str[10];

  // Send upper nibble, E up
  // ----------------------------------
  TWI_Transmit_Byte(data | PCF8574_PIN_E);
  // PWeh delay time > 450ns
  _delay_us(0.5);

  sprintf(str, "%.2x-", (char) (data | PCF8574_PIN_E));
  DrawString(str, BLACK, X1);

  // E down
  TWI_Transmit_Byte(data & ~PCF8574_PIN_E);
  // PWeh delay time > 450ns
  _delay_us(0.5);

  sprintf(str, "%.2x ", (char) (data & ~PCF8574_PIN_E));
  DrawString(str, BLACK, X1);
}

/**
 * @desc    LCD send 8bits instruction in 4 bit mode
 *
 * @param   char
 * @param   char
 *
 * @return  void
 */
void HD44780_PCF8574_Send_8bits_M4b_I (char addr, char data)
{
  char str[10];
  // upper nible
  char up_nibble = data & 0xF0;
  // lower nibble
  char low_nibble = data << 4;

  // TWI: start
  // -------------------------
  TWI_MT_Start();

  // TWI: send SLAW
  // -------------------------
  TWI_Transmit_SLAW(addr);

  // Send upper nibble, E up
  // ----------------------------------
  TWI_Transmit_Byte(up_nibble | PCF8574_PIN_E);
  // PWeh delay time > 450ns
  _delay_us(0.5);

  sprintf(str, "%.2x-", (char) (up_nibble | PCF8574_PIN_E));
  DrawString(str, BLACK, X1);

  // E down
  TWI_Transmit_Byte(up_nibble & ~PCF8574_PIN_E);
  // PWeh delay time > 450ns
  _delay_us(0.5);

  sprintf(str, "%.2x:", (char) (up_nibble & ~PCF8574_PIN_E));
  DrawString(str, BLACK, X1);

  // Send lower nibble, E up
  // ----------------------------------
  TWI_Transmit_Byte(low_nibble | PCF8574_PIN_E);
  // PWeh delay time > 450ns
  _delay_us(0.5);

  sprintf(str, "%.2x-", (char) (low_nibble | PCF8574_PIN_E));
  DrawString(str, BLACK, X1);

  // E down
  TWI_Transmit_Byte(low_nibble & ~PCF8574_PIN_E);
  // PWeh delay time > 450ns
  _delay_us(0.5);

  sprintf(str, "%.2x ", (char) (low_nibble & ~PCF8574_PIN_E));
  DrawString(str, BLACK, X1);

  // TWI Stop
  TWI_Stop();
}

/**
 * @desc    LCD check BF
 *
 * @param   char
 *
 * @return  void
 */
void HD44780_PCF8574_CheckBF (char addr)
{
  // busy flag
  char data = 0x00;

  // TWI: start
  // -------------------------
  TWI_MT_Start();
  // TWI: send SLAW
  // -------------------------
  TWI_Transmit_SLAW(addr);
  // RW=1 / Read operation
  // ----------------------------------------------------------------------
  TWI_Transmit_Byte(PCF8574_PIN_RW);

  // TWI: start
  // -------------------------
  TWI_MT_Start();
  // TWI: send SLAR
  // -------------------------
  TWI_Transmit_SLAR(addr);

  // check if BF->DB7 cleared
  do {
    // TWI: read data
    // -------------------------
    // get upper Nibble of data
    data = TWI_Receive_Byte();
    // get lower Nibble of data
    TWI_Receive_Byte();

  } while (PCF8574_PIN_DB7 & data);

  // TWI Stop
  TWI_Stop();  
}

/**
 * @desc    LCD Send instruction 8 bits in 4 bits mode
 *
 * @param   char
 * @param   char
 *
 * @return  void
 */
void HD44780_PCF8574_SendInstruction (char addr, char instruction)
{
  // send instruction
  HD44780_PCF8574_Send_8bits_M4b_I(addr, instruction);
  // check BF
  //HD44780_PCF8574_CheckBF(addr);
  _delay_ms(50);
}

/**
 * @desc    LCD Go to position x, y
 *
 * @param   char
 * @param   char
 *
 * @return  char
 */
char HD44780_PCF8574_PositionXY (char addr, char x, char y)
{
  if (x > HD44780_COLS || y > HD44780_ROWS) {
    // error
    return PCF8574_ERROR;
  }
  // check which row
  if (y == 0) {
    // send instruction 1st row
    HD44780_PCF8574_SendInstruction(addr, (HD44780_POSITION | (HD44780_ROW1_START + x)));
  } else if (y == 1) {
    // send instruction 2nd row
    HD44780_PCF8574_SendInstruction(addr, (HD44780_POSITION | (HD44780_ROW2_START + x)));
  }
  // success
  return PCF8574_SUCCESS;
}

/**
 * @desc    LCD display clear
 *
 * @param   char
 *
 * @return  void
 */
void HD44780_PCF8574_DisplayClear (char addr)
{
  // Diplay clear
  HD44780_PCF8574_SendInstruction(addr, HD44780_DISP_CLEAR);
}

/**
 * @desc    LCD display on
 *
 * @param   char
 *
 * @return  void
 */
void HD44780_PCF8574_DisplayOn (char addr)
{
  // send instruction - display on
  HD44780_PCF8574_SendInstruction(addr, HD44780_DISP_ON);
}

/**
 * @desc    LCD cursor on, display on
 *
 * @param   char
 *
 * @return  void
 */
void HD44780_PCF8574_CursorOn (char addr)
{
  // send instruction - cursor on
  HD44780_PCF8574_SendInstruction(addr, HD44780_CURSOR_ON);
}

/**
 * @desc    LCD cursor blink, cursor on, display on
 *
 * @param   char
 *
 * @return  void
 */
void HD44780_PCF8574_CursorBlink (char addr)
{
  // send instruction - Cursor blink
  HD44780_PCF8574_SendInstruction(addr, HD44780_CURSOR_BLINK);
}

/**
 * @desc    LCD draw char
 *
 * @param   char
 * @param   char
 *
 * @return  void
 */
void HD44780_PCF8574_DrawChar (char addr, char character)
{
/*
  // Diplay clear
  HD44780_SendData(character);
*/
}

/**
 * @desc    LCD draw string
 *
 * @param   char
 * @param   char *
 *
 * @return  void
 */
void HD44780_PCF8574_DrawString (char addr, char *str)
{
/*
  unsigned char i = 0;
  // loop through 5 bytes
  while (str[i] != '\0') {
    //read characters and increment index
    HD44780_SendData(str[i++]);
  }
*/
}

/**
 * @desc    Error
 *
 * @param   char
 *
 * @return  void
 */
void HD44780_PCF8574_Error (char error)
{

}

