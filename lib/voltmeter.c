/** 
 * ---------------------------------------------------------------+ 
 * @desc        Voltmeter 
 * ---------------------------------------------------------------+ 
 *              Copyright (C) 2020 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @datum       27.11.2020
 * @file        voltmeter.c
 * @tested      AVR Atmega328p
 *
 * @depend      hd44780pcf8574.h, adc.h, voltmeter.h
 * ---------------------------------------------------------------+
 */
#include <util/delay.h>
#include "adc.h"
#include "voltmeter.h"
#include "hd44780pcf8574.h"

/**
 * @desc   Voltmeter
 *
 * @param  void
 *
 * @return void
 */
void Voltmeter (void)
{
  char *str = " ERR0R ";
  char addr = PCF8574_ADDRESS;
  unsigned int adc_value;
  unsigned long int voltage;

  // factor calculation
  // Umax = 32.2V
  // Udiv =  2.0V at Umax
  float k = 32200 / (1024 * (2.00/5.00));

  // INIT PERIPHERAL
  // -------------------------------------------------   
  // init ADC
  AdcInit();
  // init LCD with address
  HD44780_PCF8574_Init(addr);

  // DISPLAY - SCREEN TEXT
  // -------------------------------------------------
  // display clear
  HD44780_PCF8574_DisplayClear(addr);
  // display on
  HD44780_PCF8574_DisplayOn(addr);
  // draw char
  HD44780_PCF8574_DrawString(addr, "U [V]:");
  // position
  HD44780_PCF8574_PositionXY(addr, 0, 1);
  // draw char
  HD44780_PCF8574_DrawString(addr, "I [A]:");

  // infinitive loop
  while (1) {
    // read value
    adc_value = AdcReadADC(2);
    // calculate voltage
    voltage = (long) (k * adc_value);
    // set position
    HD44780_PCF8574_PositionXY(addr, 7, 0);
    // draw string
    HD44780_PCF8574_DrawString(addr, AdcValToDecStr(voltage, str));
    // delay
    // in future -> replace with timer
    _delay_ms(500);
  }
}
