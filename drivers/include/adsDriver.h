/*
  zTank 
  Repository: https://github.com/akira215/
  License: GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
  Author: Akira Shimahara
*/

#pragma once

#include <iostream>

#include "cppgpio.h"
#include "cppi2c.h"
#include "ads1115.h"

#define I2C_MASTER_SCL_IO           CONFIG_I2C_MASTER_SCL      //!< GPIO number used for I2C master clock 
#define I2C_MASTER_SDA_IO           CONFIG_I2C_MASTER_SDA      //!< GPIO number used for I2C master data  
#define I2C_MASTER_NUM              I2C_NUM_0                  //!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip 
#define ADS_I2C_FREQ_HZ             400000                     //!< I2C master clock frequency 
#define ADS_I2C_TIMEOUT_MS          1000
#define GPIO_INPUT_IO_READY         CONFIG_ADS1115_READY_INT   //!< GPIO number connect to the ready pin of the converter


class AdsDriver final
{
public:
    AdsDriver();
    void run(void);
    void setup(void);

    // Event handler when conversion is received
    static void ads1115_event_handler(uint16_t input, int16_t value);
    
private:
    I2c i2c_master;
    Ads1115 ads;

}; // Main Class