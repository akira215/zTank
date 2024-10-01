/*
  zTank 
  Repository: https://github.com/akira215/
  License: GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
  Author: Akira Shimahara
*/

#include "volumetricMeter.h"

#include <iostream> //TODEL

//Static
void VolumetricMeter::impulsionHandler(void *handler_args, esp_event_base_t base, 
                                    int32_t id, void *event_data)
{
    VolumetricMeter* instance = *(static_cast<VolumetricMeter**>(event_data));

    // This interrupt is triggered on rising and falling edge (whatever)
    if(instance->_irqMeter.read())
       instance->_currentVolume += instance->_Kfactor;

    std::cout << "Impulsion: K factor " << (instance)->_Kfactor << std::endl;
    //this->currentVolume += 1;
}



VolumetricMeter::VolumetricMeter():_Kfactor("Kfactor")
{
    _irqMeter.enableInterrupt(GPIO_INTR_NEGEDGE);

    // Create a pointer of Pointer to be able to access to this in event loop
    VolumetricMeter** ptr = new VolumetricMeter*(this);

    // System Event Loop
    esp_event_loop_create_default();    // Create System Event Loop
    _irqMeter.setEventHandler(&impulsionHandler, ptr);

}

//TODEL for debugging purpose
int VolumetricMeter::getPinLevel()
{
    return _irqMeter.read();
}

void VolumetricMeter::setKfactor(uint16_t kFactor)
{
    _Kfactor = kFactor;
    _Kfactor.save(); // Save will write in the NVS
}

void VolumetricMeter::resetCounter()
{
    _currentVolume = 0;
}

uint32_t VolumetricMeter::getCurrentVolume() const
{
    return _currentVolume;
}