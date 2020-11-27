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
  char *str = "ERR";
  char addr = PCF8574_ADDRESS;
  unsigned long int voltage;
  unsigned short int adc_value;

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

  // ADC
  // ------------------------------------------------- 
  // read value
  adc_value = AdcReadADCH(1);
  // calculate voltage
  voltage = (long) adc_value * 5000/255;

  // infinitive loop
  while (1) {
    // read value
    adc_value = AdcReadADCH(1);
    // calculate voltage
    voltage = (long) adc_value * 5000/255;
    // set position
    HD44780_PCF8574_PositionXY(addr, 7, 0);
    // draw string
    HD44780_PCF8574_DrawString(addr, AdcValToDecStr(voltage, str));
    // delay
    _delay_ms(500);
  }
}
