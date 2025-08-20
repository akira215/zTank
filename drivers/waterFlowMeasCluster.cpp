/*
  zTank 
  Repository: https://github.com/akira215/
  License: GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
  Author: Akira Shimahara
*/

#include "waterFlowMeasCluster.h"

#include <iostream> //TODEL

//Static
void WaterFlowMeasCluster::impulsionHandler(void *handler_args, esp_event_base_t base, 
                                    int32_t id, void *event_data)
{
    WaterFlowMeasCluster* instance = *(static_cast<WaterFlowMeasCluster**>(event_data));

    // This interrupt is triggered on rising and falling edge (whatever)
    //if(instance->_irqMeter.read()){
       instance->_pulseCount += 1; 
       instance->setFlowMeasuredValue(instance->_pulseCount);
    //}

    std::cout << "Impulsion: _pulseCount " << instance->_pulseCount << std::endl;

}
/*
void WaterMeterCluster::reportCurrentSummation()
{
    std::cout << "Periodic Task "  << +_updatePeriod << std::endl;
}
*/

WaterFlowMeasCluster::WaterFlowMeasCluster():
                                ZbFlowMeasCluster(false, 0, 0, ESP_ZB_ZCL_VALUE_U16_NONE - 1),
                                _Kfactor("Kfactor", 1.0f),
                                _pulseCount(0)
{
    // setup the embedded Kfactor cluster (analog value)
    //float_t currentFactor = _Kfactor;
    _kfactorCluster = new ZbAnalogValueCluster(false, false, _Kfactor);
    _kfactorCluster->addAttribute(ESP_ZB_ZCL_ATTR_ANALOG_VALUE_DESCRIPTION_ID, 
                                            (void*)K_FACTOR_ATTR);
                      
    
    _kfactorCluster->registerEventHandler(&WaterFlowMeasCluster::setKfactor, this);

    registerEventHandler(&WaterFlowMeasCluster::onAttrReported, this);


    _irqMeter.enableInterrupt(GPIO_INTR_NEGEDGE);

    // Create a pointer of Pointer to be able to access to this in the Event Handler
    WaterFlowMeasCluster** ptr = new WaterFlowMeasCluster*(this);

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


    
}

void WaterFlowMeasCluster::printAttrPointers()
{
    //TODEL
    std::cout << std::hex << "printAttrPointers - attr_list " << _attr_list << std::endl;
    std::cout << std::hex << "printAttrPointers - attr_list->next " << _attr_list->next << std::endl;
}

ZbCluster* WaterFlowMeasCluster::getKfactorCluster()
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

void WaterFlowMeasCluster::onAttrReported(clusterEvent_t event, std::vector<attribute_t> attrs)
{
    (void)attrs; // unused parameter

    if (event != ATTR_REPORTED)
        return;

    std::cout << "ATTR REPORTED *************** " << std::endl;

    _pulseCount = 0; 
    setFlowMeasuredValue(_pulseCount);



}

void WaterFlowMeasCluster::setKfactor(clusterEvent_t event, std::vector<attribute_t> attrs)
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

void WaterFlowMeasCluster::setFlowMeasuredValue(uint16_t newValue)
{
    setAttribute(ESP_ZB_ZCL_ATTR_FLOW_MEASUREMENT_VALUE_ID,
                &newValue);
}