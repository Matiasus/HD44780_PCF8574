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

/* @var error status */  
char _twi_error_stat = TWI_ERROR_NONE;

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
 * @return  void
 */
void TWI_MT_Start(void)
{ 
  // init status
  char status = TWI_STATUS_INIT;
  // START
  // ----------------------------------------------
  // request for bus
  TWI_START();
  // wait till flag set
  TWI_WAIT_TILL_TWINT_IS_SET();
  // status read
  status = TWI_STATUS;
  // test if start or repeated start acknowledged
  if ((status != TWI_START_ACK) && (status != TWI_REP_START_ACK)) {
    // error status
    TWI_Error(status, TWI_START_ACK);
  }
}

/**
 * @desc    TWI Send address + write
 *
 * @param   char
 *
 * @return  void
 */
void TWI_Transmit_SLAW(char address)
{
  // init status
  char status = TWI_STATUS_INIT;
  // SLA+W
  // ----------------------------------------------
  TWI_TWDR = (address << 1);
  // enable
  TWI_MSTR_ENABLE_ACK();
  // wait till flag set
  TWI_WAIT_TILL_TWINT_IS_SET();
  // status read
  status = TWI_STATUS;
  // find
  if (status != TWI_MT_SLAW_ACK) {
    // error status
    TWI_Error(status, TWI_MT_SLAW_ACK);
  }
}

/**
 * @desc    TWI Send address + read
 *
 * @param   char
 *
 * @return  void
 */
void TWI_Transmit_SLAR(char address)
{
  // init status
  char status = TWI_STATUS_INIT;
  // SLA+R
  // ----------------------------------------------
  TWI_TWDR = (address << 1) | TWI_READ;
  // enable
  TWI_MSTR_ENABLE_ACK();
  // wait till flag set
  TWI_WAIT_TILL_TWINT_IS_SET();
  // status read
  status = TWI_STATUS;
  // find
  if (status != TWI_MR_SLAR_ACK) {
    // error status
    TWI_Error(status, TWI_MR_SLAR_ACK);
  }
}

/**
 * @desc    TWI Transmit data
 *
 * @param   char
 *
 * @return  void
 */
void TWI_Transmit_Byte(char data)
{
  // init status
  char status = TWI_STATUS_INIT;
  // DATA SEND
  // ----------------------------------------------
  TWI_TWDR = data;
  // enable
  TWI_MSTR_ENABLE_ACK();
  // wait till flag set
  TWI_WAIT_TILL_TWINT_IS_SET();
  // status read
  status = TWI_STATUS;
  // send with success
  if (status != TWI_MT_DATA_ACK) {
    // error status
    TWI_Error(status, TWI_MT_DATA_ACK);
  }
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
  // init status
  char status = TWI_STATUS_INIT;
  // DATA RECEIVE
  // ----------------------------------------------
  // enable with NACK
  TWI_MSTR_ENABLE_NACK();
  // wait till flag set
  TWI_WAIT_TILL_TWINT_IS_SET();
  // status read
  status = TWI_STATUS;
  // send with success
  if (status != TWI_MR_DATA_NACK) {
    // error status
    TWI_Error(status, TWI_MR_DATA_NACK);
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

/**
 * @desc    TWI Error
 *
 * @param   char
 * @param   char
 *
 * @return  void
 */
void TWI_Error(char status, char expected)
{ 
  // some procedure
  
  // error status  
//  _twi_error_stat = TWI_STATUS_INIT;
}
