/** 
 * ---------------------------------------------------+ 
 * @desc        Voltmeter Library h
 * ---------------------------------------------------+ 
 * @copyright   Copyright (C) 2020 Marian Hrinko.
 * @email       mato.hrinko@gmail.com
 * @author      Marian Hrinko
 * @datum       31.07.2020
 * @file        voltmeter.h
 * @version     1.0
 * @tested      AVR Atmega16
 * ---------------------------------------------------+
 */

/** @definition */
#ifndef __VOLTMETER_H__
#define __VOLTMETER_H__

  #define BLACK 0x0000
  #define WHITE 0xffff
  #define RED   0xf000

  /**
   * @desc   Voltmeter
   *
   * @param  void
   *
   * @return void
   */
  void Voltmeter (void);

#endif
