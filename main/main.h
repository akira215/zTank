#pragma once

#include "zbNode.h"
#include "cppgpio.h" // to del
#include "buttonTask.h" // to del
#include "blinkTask.h" // to del

#include "waterMeterCluster.h" // to del

#include "zbHaCluster.h"
#include "zbTimeClusterClient.h" 


#define SENSOR_ENDPOINT          10      /* esp temperature sensor device endpoint, used for temperature measurement */
/* Attribute values in ZCL string format
 * The string should be started with the length of its own.
 */
#define MANUFACTURER_NAME               "\x09""AkiraCorp" //TODO del
#define MODEL_IDENTIFIER                "\x10""WaterTankMonitor"  //TODO del


// Main class used for testing only
class Main final
{
    GpioInput               _button {GPIO_NUM_9,true};
    ButtonTask*             _buttonTask = nullptr;

    static BlinkTask*       _ledBlinking;
    static GpioOutput       _led;
    
    //inline static Main* _this = nullptr;

public:
    Main();
    void run(void);
    void setup(void);

    void zbDeviceEventHandler(ZbNode::nodeEvent_t event);
    static void shortPressHandler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data);
    static void longPressHandler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data);

    static void identifyHandler(uint16_t attrId, void* value);
    static void lightOnOffHandler(uint16_t attrId, void* value);

private:
    /// @brief Helper to flash led
    /// @param speed flash cycle in ms. if 0, led will be set to off, 
    /// if -1 led will be switch on
    static void ledFlash(uint64_t speed);
    
    ZbNode*                     _zbDevice           = nullptr;
    ZbTemperatureMeasCluster*   _tempMeasurement    = nullptr;
    ZbTimeClusterClient*         _timeClient        = nullptr;

    TaskHandle_t                _xHandle            = nullptr;
    TaskHandle_t                _eventLoopHandle    = nullptr;

    WaterMeterCluster*          _fMeter             = nullptr;


}; // Main Class