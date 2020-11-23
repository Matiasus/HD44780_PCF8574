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
  // define LCD struct
  struct HD44780_PCF8547_Structure LCD;
  // init LCD struct
  HD44780_PCF8547_Struct_Init(LCD, 0x27);

  // init LCD with address
  LCD.Init(LCD.addr);

  // EXIT
  // ------------------------------------------------- 
  // return & exit
  return 0;
}
