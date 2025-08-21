/*
  zTank 
  Repository: https://github.com/akira215/
  License: GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
  Author: Akira Shimahara
*/
#pragma once

#include "persistedValue.h"
#include "ZbPressureMeasCluster.h"
#include "zbAnalogValueCluster.h"
#include "periodicSoftTask.h"

#define K_FACTOR_ATTR                   "\x07""Kfactor" //TODO del
#define GPIO_VOLUME_METER               CONFIG_VOLUME_METER_PIN  //!< GPIO number connect to the Meter


/// @brief Driver to read pressure from ADS1115 analog converter
class WaterPressureMeasCluster : public ZbPressureMeasCluster
{
    //GpioInput _irqMeter     { static_cast<gpio_num_t>(GPIO_VOLUME_METER) };


    PersistedValue<float_t> _Kfactor;
    ZbAnalogValueCluster*   _kfactorCluster = nullptr;
    //PeriodicSoftTask*       _reportTask = nullptr;
    uint16_t                _pulseCount; // pulse counter re
    uint64_t                _currentVolume = 0; // In fact it is not the current volume but number of tick
public:
    WaterPressureMeasCluster();

    /// @brief set the k factor of the volumetric meter (write to NVS)
    /// @param kFactor the factor (unit shall be the same as current volume)
    void setKfactor(clusterEvent_t event, std::vector<attribute_t> attrs);

    /// @brief return a pointer to the embedded kfactor cluster
    ZbCluster* getKfactorCluster();

   
    //TODEL
    void printAttrPointers();

private:

    // Set the attribute value for the cluster
    void setFlowMeasuredValue(uint16_t newValue);

    /// @brief callback when attr are reported
    void onAttrReported(clusterEvent_t event, std::vector<attribute_t> attrs);


};