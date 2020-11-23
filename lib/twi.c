/** 
 * ---------------------------------------------------+ 
 * @desc        Two Wire Interface / I2C Communication
 * ---------------------------------------------------+ 
 *              Copyright (C) 2020 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @datum       06.09.2020
 * @file        twi.c
 * @tested      AVR Atmega16
 * ---------------------------------------------------
 */
 
// include libraries
#include "twi.h"

/* @var error flag */  
char _twi_error_flag = TWI_ERROR_FLAG_INIT;

/* @var error status */  
char _twi_error_stat = TWI_ERROR_STAT_INIT;

/**
 * @desc    TWI init - initialize frequency
 *
 * @param   void
 *
 * @return  void
 */
void TWI_Init(void)
{
  // +++++++++++++++++++++++++++++++++++++++++++++
  // Calculation fclk:
  //
  // fclk = (fcpu)/(16+2*TWBR*4^Prescaler)
  // --------------------------------------------- 
  // Calculation TWBR:
  // 
  // TWBR = {(fcpu/fclk) - 16 } / (2*4^Prescaler)
  // +++++++++++++++++++++++++++++++++++++++++++++
  // @16MHz
  // @param1 value of TWBR, 
  //    fclk = 400 kHz; TWBR = 3
  //    fclk = 200 kHz; TWBR = 8
  //    fclk = 100 kHz; TWBR = 18
  // @8MHz
  // @param1 value of TWBR, 
  //    fclk = 200 kHz; TWBR = 3
  //    fclk = 100 kHz; TWBR = 8
  // @param2 value of Prescaler = 1
  TWI_FREQ(8, 1);
}

/**
 * @desc    TWI MT Start
 *
 * @param   void
 *
 * @return  char
 */
char TWI_MT_Start(void)
{
  // null status flag
  TWI_TWSR &= ~0xA8;
  // START
  // ----------------------------------------------
  // request for bus
  TWI_START();
  // wait till flag set
  TWI_WAIT_TILL_TWINT_IS_SET();
  // test if start or repeated start acknowledged
  if ((TWI_STATUS != TWI_START_ACK) && (TWI_STATUS != TWI_REP_START_ACK)) {
    // return status
    return TWI_STATUS;
  }
  // success
  return TWI_SUCCESS;
}

/**
 * @desc    TWI Send address + write
 *
 * @param   char
 *
 * @return  char
 */
char TWI_Transmit_SLAW(char address)
{
  // SLA+W
  // ----------------------------------------------
  TWI_TWDR = (address << 1);
  // enable
  TWI_MSTR_ENABLE_ACK();
  // wait till flag set
  TWI_WAIT_TILL_TWINT_IS_SET();

  // find
  if (TWI_STATUS != TWI_MT_SLAW_ACK) {
    // return status
    return TWI_STATUS;
  }
  // return found device address
  return TWI_SUCCESS;
}

/**
 * @desc    TWI Send address + read
 *
 * @param   char
 *
 * @return  char
 */
char TWI_Transmit_SLAR(char address)
{
  // SLA+R
  // ----------------------------------------------
  TWI_TWDR = (address << 1) | 0x01;
  // enable
  TWI_MSTR_ENABLE_ACK();
  // wait till flag set
  TWI_WAIT_TILL_TWINT_IS_SET();

  // find
  if (TWI_STATUS != TWI_MR_SLAR_ACK) {
    // return status
    return TWI_STATUS;
  }
  // return found device address
  return TWI_SUCCESS;
}

/**
 * @desc    TWI Transmit data
 *
 * @param   char
 *
 * @return  char
 */
char TWI_Transmit_Byte(char data)
{
  // DATA
  // ----------------------------------------------
  TWI_TWDR = data;
  // enable
  TWI_MSTR_ENABLE_ACK();
  // wait till flag set
  TWI_WAIT_TILL_TWINT_IS_SET();

  // send with success
  if (TWI_STATUS != TWI_MT_DATA_ACK) {
    // return status
    return TWI_STATUS;
  }
  // return success
  return TWI_SUCCESS;
}

/**
 * @desc    TWI Receive 1 byte
 *
 * @param   void
 *
 * @return  char
 */
char TWI_Receive_Byte(void)
{
  // enable with NACK
  TWI_MSTR_ENABLE_NACK();
  // wait till flag set
  TWI_WAIT_TILL_TWINT_IS_SET();

  // send with success
  if (TWI_STATUS != TWI_MR_DATA_NACK) {
    // set error flag 
    _twi_error_flag = 1;
    // error status status
    _twi_error_stat = TWI_STATUS;
    // return error
    return TWI_ERROR;
  }
  // received data
  return TWI_TWDR;
}

/**
 * @desc    TWI stop
 *
 * @param   void
 *
 * @return  void
 */
void TWI_Stop(void)
{
  // End TWI
  // -------------------------------------------------
  // send stop sequence
  TWI_STOP();
  // wait for TWINT flag is set
//  TWI_WAIT_TILL_TWINT_IS_SET();
}
