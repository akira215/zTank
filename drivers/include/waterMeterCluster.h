/*
  zTank 
  Repository: https://github.com/akira215/
  License: GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
  Author: Akira Shimahara
*/
#pragma once

#include "cppgpio.h"
#include "persistedValue.h"
#include "zbMeteringCluster.h"
#include "zbAnalogValueCluster.h"
#include "periodicSoftTask.h"

#define K_FACTOR_ATTR                   "\x07""Kfactor" //TODO del
#define GPIO_VOLUME_METER               CONFIG_VOLUME_METER_PIN  //!< GPIO number connect to the Meter
#define ATTR_METERING_CURRENT_VOLUME_ID 0x0025

/// @brief Driver to count volume from impulsion Meter - active low
class WaterMeterCluster : public ZbMeteringCluster
{
    GpioInput _irqMeter     { static_cast<gpio_num_t>(GPIO_VOLUME_METER) };
    PersistedValue<float_t> _Kfactor;
    ZbAnalogValueCluster*   _kfactorCluster = nullptr;
    //PeriodicSoftTask*       _reportTask = nullptr;
    uint64_t                _currentVolume = 0; // In fact it is not the current volume but number of tick
public:
    WaterMeterCluster();
    int getPinLevel();

    /// @brief set the k factor of the volumetric meter (write to NVS)
    /// @param kFactor the factor (unit shall be the same as current volume)
    void setKfactor(clusterEvent_t event, std::vector<attribute_t> attrs);

    /// @brief return a pointer to the embedded kfactor cluster
    ZbCluster* getKfactorCluster();

    /// @brief set the update period for reporting data (write to NVS)
    //void setUpdatePeriod(clusterEvent_t event, std::vector<attribute_t> attrs);

    /// @brief reset the Counter, after a reading for example
    void resetCounter();

    /// @brief get the volume counted from last reset
    /// @return the volume. Unit is the same as K factor
    uint32_t getCurrentVolume() const;

    void setCurrentSummationDelivered(uint64_t newSum);
    void incrementCurrentVolume();

    //void reportCurrentSummation();

    //void startReporting();

private:
    // Event Handler for cppButton
    static void impulsionHandler(void *handler_args, esp_event_base_t base,
                                 int32_t id, void *event_data);

};