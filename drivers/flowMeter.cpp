/*
  zTank 
  Repository: https://github.com/akira215/
  License: GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
  Author: Akira Shimahara
*/

#include "flowMeter.h"

#include <iostream> //TODEL

//Static
void FlowMeter::impulsionHandler(void *handler_args, esp_event_base_t base, 
                                    int32_t id, void *event_data)
{
    FlowMeter* instance = *(static_cast<FlowMeter**>(event_data));

    // This interrupt is triggered on rising and falling edge (whatever)
    if(instance->_irqMeter.read())
       instance->_currentVolume += instance->_Kfactor;

    std::cout << "Impulsion: K factor " << (instance)->_Kfactor << std::endl;
    //this->currentVolume += 1;
}


FlowMeter::FlowMeter():_Kfactor("Kfactor")
{
    _irqMeter.enableInterrupt(GPIO_INTR_NEGEDGE);

    // Create a pointer of Pointer to be able to access to this in event loop
    FlowMeter** ptr = new FlowMeter*(this);

    // System Event Loop
    esp_event_loop_create_default();    // Create System Event Loop
    _irqMeter.setEventHandler(&impulsionHandler, ptr);

    //addAttribute()

}

//TODEL for debugging purpose
int FlowMeter::getPinLevel()
{
    return _irqMeter.read();
}

void FlowMeter::setKfactor(uint16_t kFactor)
{
    _Kfactor = kFactor;
    _Kfactor.save(); // Save will write in the NVS
}

void FlowMeter::resetCounter()
{
    _currentVolume = 0;
}

uint32_t FlowMeter::getCurrentVolume() const
{
    return _currentVolume;
}