/*
  zTank 
  Repository: https://github.com/akira215/
  License: GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
  Author: Akira Shimahara
*/

#include "waterPressureMeasCluster.h"
#include "adsDriver.h"

#include <string>


#include <iostream> //TODEL


// Static handler when conversion is received
void WaterPressureMeasCluster::adc_event_handler(double value)
{
    int16_t attr = value*1000*_Kfactor;
    std::cout << "WaterPressure ADC Callback - value: " << value << " - attr " << attr << std::endl;
    setPressureMeasuredValue(attr);
}



WaterPressureMeasCluster::WaterPressureMeasCluster(uint8_t channel):
                                ZbPressureMeasCluster(false, 0, 0, ESP_ZB_ZCL_VALUE_U16_NONE - 1),
                                _Kfactor(std::string("pFactor").append(std::to_string(channel)), 1.0f)
{
    // setup the embedded pFactor cluster (analog value)
    _kfactorCluster = new ZbAnalogValueCluster(false, false, _Kfactor);
                      
    _kfactorCluster->registerEventHandler(&WaterPressureMeasCluster::setKfactor, this);

    // Register the callback handler for ADC converter
    // WARNING, do not start the ADC driver prior to start the zigbee stack
    // Otherwise it will try to set an attribute and will fail
    AdsDriver::getInstance().registerAdsHandler(&WaterPressureMeasCluster::adc_event_handler, this, channel);
}

ZbCluster* WaterPressureMeasCluster::getKfactorCluster()
{
    return _kfactorCluster;
}

void WaterPressureMeasCluster::setKfactor(clusterEvent_t event, std::vector<attribute_t> attrs)
{
    if (event != ATTR_UPDATED_REMOTELY)
        return;
    
    for (auto & el : attrs){
        uint16_t attrId = el.attrId;
        void* value = el.value;
        if (attrId == ESP_ZB_ZCL_ATTR_ANALOG_VALUE_PRESENT_VALUE_ID){
            std::cout << "setKfactor event : " << event << std::endl;

            float_t currentFactor = *(static_cast<float_t*>(value));
            std::cout << "setKfactor currentFactor : " << currentFactor << std::endl;
            
            //_kfactorCluster.setAttribute(ESP_ZB_ZCL_ATTR_ANALOG_VALUE_PRESENT_VALUE_ID,
            //                                (void*)&currentFactor);
            _Kfactor = currentFactor;
            _Kfactor.save(); // Save will write in the NVS
        }
    }
    
}

void WaterPressureMeasCluster::setPressureMeasuredValue(int16_t newValue)
{
    setAttribute(ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_VALUE_ID,
                &newValue);
}