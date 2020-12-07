/** 
 * ---------------------------------------------------------------+ 
 * @desc        ADC 
 * ---------------------------------------------------------------+ 
 *              Copyright (C) 2020 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @datum       27.11.2020
 * @file        adc.c
 * @tested      AVR Atmega328p
 *
 * @depend      adc.h
 * ---------------------------------------------------------------+
 */
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>
#include "adc.h"

/***
 * @desc   ADC init
 * - reference voltage AVcc with external capacitor at AREF pin
 * - prescaler 64
 *   frequency condition 50-200 kHz -> at 8Mhz & prescaler 64 => 125kHz
 *
 * @param  Void
 *
 * @return Void
 */
void AdcInit(void)
{ 
  // reference voltage AVcc with external capacitor at AREF pin 
  ADMUX = (1 << REFS0);
  //ADMUX |= (1 << REFS1);
  // align to left -> ADCH
  //ADMUX |= (1 << ADLAR);
  // setting adc
  // - ADIE: adc interrupt enable
  // - ADEN: adc enable
  ADCSRA |= (1 << ADIE) | (1 << ADEN);  
  // set prescaler => f = 8Mhz / 64 = 125 kHz
  ADC_SET_PRESCALER(ADC_PRESCALER_64); 
}

/***
 * @desc   Read ADC value, ADLAR = 0
 *
 * @param  char
 *
 * @return unsigned int
 */
unsigned int AdcReadADC(char channel)
{
  // default
  unsigned int value = 0x0000;
  // select ADC channel
  ADC_SET_CHANNEL(channel);
  // start conversion
  ADCSRA |= (1 << ADSC);
  // wait conversion complete
  while (ADCSRA & (1 << ADIF));
  // read ADCL
  value = ADCL;
  // read ADCH
  value |= (ADCH << 8);
  // righ adjusted conversion result
  return value;
}

/***
 * @desc   Read ADCH value, ADLAR = 1, right adjusted result
 *
 * @param  char
 *
 * @return char
 */
char AdcReadADCH(char channel)
{
  // select ADC channel
  ADC_SET_CHANNEL(channel);
  // start conversion
  ADCSRA |= (1 << ADSC);
  // wait conversion complete
  while (ADCSRA & (1 << ADIF));
  // righ adjusted conversion result
  return ADCH;
}

/**
 * @desc    Get string int value
 *
 * @param   unsigned long int
 * @param   char *
 *
 * @return  char *
 */
char * AdcValToDecStr(unsigned long int real_value, char * str)
{
  // number value
  if (real_value < 10) {
    // to 10 mili
    sprintf(str, " 0.00%ld", real_value);  
  } else if (real_value < 100) {
    // to 100 mili
    sprintf(str, " 0.0%ld", real_value); 
  } else if (real_value < 1000) {
    // to 1 000 mili
    sprintf(str, " 0.%ld", real_value); 
  } else if (real_value < 10000) {
    // to 10 000 mili
    sprintf(str, "%ld", real_value);
    sprintf(str, " %c.%c%c%c", str[0], str[1], str[2], str[3]);
  } else if (real_value < 100000) {
    // to  100 000 mili
    sprintf(str, "%ld", real_value);
    sprintf(str, "%c%c.%c%c%c", str[0], str[1], str[2], str[3], str[4]);
  }

  // return value
  return str;
}
