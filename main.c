/** 
 * ---------------------------------------------------+ 
 * @desc        Main file
 * ---------------------------------------------------+ 
 * @copyright   Copyright (C) 2020 Marian Hrinko.
 * @author      Marian Hrinko
 * @email       mato.hrinko@gmail.com
 * @datum       10.11.2020
 * @file        main.c
 * @version     1.0
 * @tested      AVR Atmega328p
 * ---------------------------------------------------+
 */
#include "lib/hd44780pcf8574.h"

/**
 * @desc   Main function
 *
 * @param  Void
 *
 * @return int
 */
int main(void)
{
  // init LCD with address
  HD44780_PCF8574_Init(PCF8574_ADDRESS);
  // cursor on
  HD44780_PCF8574_CursorBlink(PCF8574_ADDRESS);
  // draw char
  HD44780_PCF8574_DrawString(PCF8574_ADDRESS, "PCF8574 I2C/TWI");
/*
  // clear
  LCD.Clear(0x27);
  // cursor on
  LCD.CursorOn(0x27);
*/
  // EXIT
  // ------------------------------------------------- 
  // return & exit
  return 0;
}
