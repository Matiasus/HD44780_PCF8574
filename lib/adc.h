/** 
 * ---------------------------------------------------------------+ 
 * @desc        ADC 
 * ---------------------------------------------------------------+ 
 *              Copyright (C) 2020 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @datum       27.11.2020
 * @file        adc.h
 * @tested      AVR Atmega328p
 *
 * @depend      adc.h
 * ---------------------------------------------------------------+
 */

/** @definition */
#ifndef __ADC_H__
#define __ADC_H__

  // @const ADC prescalers
  #define ADC_PRESCALER_16             4
  #define ADC_PRESCALER_32             5
  #define ADC_PRESCALER_64             6
  #define ADC_PRESCALER_128            7
  // ADC channel selector
  #define ADC_SET_CHANNEL(CHANNEL)     { ADMUX &= 0xF8; ADMUX |= CHANNEL & 0x07; }
  // Set ADC prescaler
  #define ADC_SET_PRESCALER(PRESCALER) { ADCSRA &= ~((1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0)); ADCSRA |= PRESCALER & 0x07; }
 
  /**
   * @desc   Adc init
   *
   * @param  Void
   *
   * @return Void
   */
  void AdcInit(void);

  /**
   * @desc   Read ADC value
   *
   * @param  char
   *
   * @return unsigned int
   */
  unsigned int AdcReadADC(char);

  /**
   * @desc   Read ADCH value
   *
   * @param  char
   *
   * @return char
   */
  char AdcReadADCH(char);

  /**
   * @desc    Get string int value
   *
   * @param   unsigned long int - value which should be converse to decimal number with 3 decimal place, max number 99999
   * @param   char * - string
   *
   * @return  char *
   */
  char * AdcValToDecStr(unsigned long int, char *);

#endif

