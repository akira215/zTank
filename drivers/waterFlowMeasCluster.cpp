/*
  zTank 
  Repository: https://github.com/akira215/
  License: GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
  Author: Akira Shimahara
*/

#include "waterFlowMeasCluster.h"

#include <esp_log.h> 

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
    ESP_LOGV(ZCLUSTER_TAG, "WaterFlow impulsion,  _pulseCount %d ", 
                        instance->_pulseCount);


}


// Device sent its tick without regarding kFactor. It only sent the number of tick
// and reset its counter to 0 each time the attribute is reported
// we use kFactor here only to save in to device NVS so
// that z2m will know this value whatever it happpen (restart, device quit network, ...)
WaterFlowMeasCluster::WaterFlowMeasCluster():
                                ZbFlowMeasCluster(false, 0, 0, ESP_ZB_ZCL_VALUE_U16_NONE - 1),
                                _Kfactor("Kfactor", 1.0f),
                                _pulseCount(0)
{
    // setup the embedded Kfactor cluster (analog value)
    //float_t currentFactor = _Kfactor;
    _kfactorCluster = new ZbAnalogValueCluster(false, false, _Kfactor);
    //_kfactorCluster->addAttribute(ESP_ZB_ZCL_ATTR_ANALOG_VALUE_DESCRIPTION_ID, 
    //                                        (void*)K_FACTOR_ATTR);
                      
    
    _kfactorCluster->registerEventHandler(&WaterFlowMeasCluster::setKfactor, this);

    registerEventHandler(&WaterFlowMeasCluster::onAttrReported, this);


    _irqMeter.enableInterrupt(GPIO_INTR_NEGEDGE);

    // Create a pointer of Pointer to be able to access to this in the Event Handler
    WaterFlowMeasCluster** ptr = new WaterFlowMeasCluster*(this);

    // System Event Loop
    esp_event_loop_create_default();    // Create System Event Loop
    _irqMeter.setEventHandler(&impulsionHandler, ptr);
    
}

ZbCluster* WaterFlowMeasCluster::getKfactorCluster()
{
    return _kfactorCluster;
}

void WaterFlowMeasCluster::onAttrReported(clusterEvent_t event, std::vector<attribute_t> attrs)
{
    (void)attrs; // unused parameter

    if (event != ATTR_REPORTED)
        return;

    ESP_LOGV(ZCLUSTER_TAG, "WaterFlow attribute reported %d, resetting to 0", 
                        _pulseCount);

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
            ESP_LOGV(ZCLUSTER_TAG, "WaterFlowMeas setKfactor event : %d", 
                event);

            float_t currentFactor = *(static_cast<float_t*>(value));
            ESP_LOGV(ZCLUSTER_TAG, "WaterFlowMeas setKfactor currentFactor : : %f", 
                currentFactor);
            
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