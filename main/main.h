#pragma once

#include "zbNode.h"
#include "cppgpio.h" // to del
#include "buttonTask.h" // to del
#include "blinkTask.h" // to del

#include "flowMeter.h" // to del

#include "zbHaCluster.h"


#define SENSOR_ENDPOINT          10      /* esp temperature sensor device endpoint, used for temperature measurement */
/* Attribute values in ZCL string format
 * The string should be started with the length of its own.
 */
#define MANUFACTURER_NAME               "\x05""AKIRA" //TODO del
#define MODEL_IDENTIFIER                "\x08""ASH-Tank"  //TODO del


// Main class used for testing only
class Main final
{
    GpioInput _button {GPIO_NUM_9,true};
    ButtonTask* _buttonTask = nullptr;
    inline static Main* _this = nullptr;

public:
    Main();
    void run(void);
    void setup(void);

    static void initWhenJoined();
    static void shortPressHandler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data);
    static void longPressHandler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data);

    static void identifyHandler(uint16_t attrId, void* value);
    static void lightOnOffHandler(uint16_t attrId, void* value);
private:
    ZbNode* _zbDevice = nullptr;
    ZbTemperatureMeasCluster* _tempMeasurement = nullptr;
    ZbTimeCluster* _timeCluster = nullptr;

    TaskHandle_t _xHandle;

    FlowMeter _fMeter;


}; // Main Class