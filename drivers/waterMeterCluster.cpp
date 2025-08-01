/*
  zTank 
  Repository: https://github.com/akira215/
  License: GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
  Author: Akira Shimahara
*/

#include "waterMeterCluster.h"

#include <iostream> //TODEL

//Static
void WaterMeterCluster::impulsionHandler(void *handler_args, esp_event_base_t base, 
                                    int32_t id, void *event_data)
{
    WaterMeterCluster* instance = *(static_cast<WaterMeterCluster**>(event_data));

    // This interrupt is triggered on rising and falling edge (whatever)
    if(instance->_irqMeter.read()){
       //instance->_currentVolume += 1;
       instance->incrementCurrentVolume();
       instance->setCurrentSummationDelivered(instance->_currentVolume);  
    }

    std::cout << "Impulsion: _currentVolume " << instance->_currentVolume << std::endl;

}
/*
void WaterMeterCluster::reportCurrentSummation()
{
    std::cout << "Periodic Task "  << +_updatePeriod << std::endl;
}
*/

// sumFormat : 0bXYYYYZZZ ZZZ nb right to decimal 
// YYYY nb left to decimal,  X supress leading zeros
WaterMeterCluster::WaterMeterCluster():
                                ZbMeteringCluster(false, esp_zb_uint48_t({0,0}),
                                0x00, ESP_ZB_ZCL_METERING_UNIT_M3_M3H_BINARY,
                                0b0, ESP_ZB_ZCL_METERING_WATER_METERING),
                                _Kfactor("Kfactor", 1.0f)
{
    // setup the embedded Kfactor cluster (analog value)
    //float_t currentFactor = _Kfactor;
    _kfactorCluster = new ZbAnalogValueCluster(false, false, _Kfactor);
    _kfactorCluster->addAttribute(ESP_ZB_ZCL_ATTR_ANALOG_VALUE_DESCRIPTION_ID, 
                                            (void*)K_FACTOR_ATTR);
                      
    
    _kfactorCluster->registerEventHandler(&WaterMeterCluster::setKfactor, this);


    _irqMeter.enableInterrupt(GPIO_INTR_NEGEDGE);

    // Create a pointer of Pointer to be able to access to this in the Event Handler
    WaterMeterCluster** ptr = new WaterMeterCluster*(this);

    // System Event Loop
    esp_event_loop_create_default();    // Create System Event Loop
    _irqMeter.setEventHandler(&impulsionHandler, ptr);
/*
    // Cluster definition
    uint8_t status = 0x0;
    uint8_t unitOfMeas = ESP_ZB_ZCL_METERING_UNIT_M3_M3H_BINARY ;
   
    uint8_t sumFormat = 0b11111011;
    uint8_t deviceType = ESP_ZB_ZCL_METERING_WATER_METERING;

    setAttribute(ESP_ZB_ZCL_ATTR_METERING_STATUS_ID, &status);
    setAttribute(ESP_ZB_ZCL_ATTR_METERING_UNIT_OF_MEASURE_ID, &unitOfMeas);
    setAttribute(ESP_ZB_ZCL_ATTR_METERING_SUMMATION_FORMATTING_ID, &sumFormat);
    setAttribute(ESP_ZB_ZCL_ATTR_METERING_METERING_DEVICE_TYPE_ID, &deviceType);
*/
    esp_zb_uint24_t multiplier = { 1, 0};
    esp_zb_uint24_t divisor = { 1000, 0};
    
    addAttribute(ESP_ZB_ZCL_ATTR_METERING_MULTIPLIER_ID, &multiplier);
    addAttribute(ESP_ZB_ZCL_ATTR_METERING_DIVISOR_ID, &divisor);
    
    esp_zb_uint48_t test = { 67, 0};
    addCustomAttribute(ATTR_METERING_CURRENT_VOLUME_ID, &test, 
            ESP_ZB_ZCL_ATTR_TYPE_U48,  
            ESP_ZB_ZCL_ATTR_ACCESS_READ_WRITE | ESP_ZB_ZCL_ATTR_ACCESS_REPORTING);
    


    
}

ZbCluster* WaterMeterCluster::getKfactorCluster()
{
    return _kfactorCluster;
}
/*
void WaterMeterCluster::startReporting()
{
    _reportTask = new PeriodicSoftTask(&WaterMeterCluster::reportCurrentSummation, 
                                    this, (uint64_t)(_updatePeriod*1000), "waterMeterReportTask");
}
*/
void WaterMeterCluster::setCurrentSummationDelivered(uint64_t newSum)
{
    esp_zb_uint48_t newVal = static_cast<esp_zb_uint48_t>(newSum);
    setAttribute(ESP_ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID,
                &newVal);
    setAttribute(ATTR_METERING_CURRENT_VOLUME_ID,
                &newVal);
}

//TODEL for debugging purpose
int WaterMeterCluster::getPinLevel()
{
    return _irqMeter.read();
}


void WaterMeterCluster::setKfactor(clusterEvent_t event, std::vector<attribute_t> attrs)
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


void WaterMeterCluster::resetCounter()
{
    _currentVolume = 0;
}

void WaterMeterCluster::incrementCurrentVolume()
{
    _currentVolume += 1;
    //setAttribute(ATTR_METERING_CURRENT_VOLUME_ID,
    //            &_currentVolume);

}


uint32_t WaterMeterCluster::getCurrentVolume() const
{
    return _currentVolume;
}