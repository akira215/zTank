/*
  zTank 
  Repository: https://github.com/akira215/
  License: GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
  Author: Akira Shimahara
*/
#pragma once

#include "cppgpio.h"
#include "persistedValue.h" 


#define GPIO_VOLUME_METER  CONFIG_VOLUME_METER_PIN  //!< GPIO number connect to the LED

/// @brief Driver to count volume from impulsion Meter - active low
class VolumetricMeter
{
    GpioInput _irqMeter { static_cast<gpio_num_t>(GPIO_VOLUME_METER) };
    PersistedValue<uint16_t> _Kfactor;
    uint32_t    _currentVolume = 0;

public:
    VolumetricMeter();
    int getLevel();
    void setKfactor(uint16_t kFactor);

private:
    // Event Handler for cppButton
    static void impulsionHandler(void *handler_args, esp_event_base_t base,
                                 int32_t id, void *event_data);

};