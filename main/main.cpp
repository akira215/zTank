//#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

#include "main.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

#include <iostream>

#include "zbEndpoint.h" // to del

/*
#include "zbBasicCluster.h" //TODEL
#include "zbPowerCfgCluster.h"
#include "zbIdentifyCluster.h"
#include "zbTemperatureMeasCluster.h"
*/

#include "esp_check.h" // todel

// to del
#define ESP_TEMP_SENSOR_UPDATE_INTERVAL (1)     /* Local sensor update interval (second) */
#define ESP_TEMP_SENSOR_MIN_VALUE       (-10)   /* Local sensor min measured value (degree Celsius) */
#define ESP_TEMP_SENSOR_MAX_VALUE       (80)    /* Local sensor max measured value (degree Celsius) */


static const char *TAG = "Main_app";


Main App;

Main::Main()
{
    _this = this;
    //_zbDevice->getInstance();
    //esp_log_level_set("Main_app", ESP_LOG_DEBUG); 
    esp_log_level_set("ZB_CPP", ESP_LOG_DEBUG); 
}

//Static
void Main::shortPressHandler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data)
{
    //ESP_LOGI(TAG,"Short Press detected %ld -",id);
    esp_err_t ret = ZbNode::getInstance()->joinNetwork();

    if (ret == ESP_ERR_NOT_ALLOWED)
    {

        ESP_EARLY_LOGI(TAG, "Send 'on_off toggle' command");

        ZbNode::getInstance()->sendCommand(1,ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
                                    true,ESP_ZB_ZCL_CMD_ON_OFF_TOGGLE_ID);
    }
}

//Static
void Main::longPressHandler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data)
{

    //ESP_LOGI(TAG,"Long Press detected %ld -",id);
    ZbNode::getInstance()->leaveNetwork();

}

// Static
void Main::identifyHandler(uint16_t attrId, void* value)
{
    if(attrId != ESP_ZB_ZCL_ATTR_IDENTIFY_IDENTIFY_TIME_ID)
        return;
    
    ESP_LOGI(TAG,"Identifying time %d",*((uint16_t*)value));
    if (*((uint16_t*)value) !=0)
        ZbNode::getInstance()->ledFlash(50);
    else
        ZbNode::getInstance()->ledFlash(0);
}

// Static
void Main::lightOnOffHandler(uint16_t attrId, void* value)
{
    if(attrId != ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID)
        return;

    ESP_LOGI(TAG,"On Off set attr to %d",*((uint16_t*)value));

    esp_zb_lock_acquire(portMAX_DELAY); 
    esp_zb_zcl_attr_t *attr =  esp_zb_zcl_get_attribute(2, 
                    ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
                    ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 
                    ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID);
    esp_zb_lock_release();
    
    ESP_LOGI(TAG,"On Off attr is %d",*((bool*)(attr->data_p)));
}


void Main::setup(void)
{
    ESP_LOGI(TAG,"---------- Setup ----------");

    _fMeter.setKfactor(1); // 1 liter per impulsion

    _button.enablePullup();
    _buttonTask = new ButtonTask (_button);
    _buttonTask->setShortPressHandler(&shortPressHandler);
    _buttonTask->setLongPressHandler(&longPressHandler,(void*)this);

    ESP_LOGD(TAG,"Creating Zigbee device");
    _zbDevice->getInstance();
    //esp_log_level_set("ZB_CPP", ESP_LOG_DEBUG);

    ZbEndPoint* switchEp = new ZbEndPoint(1, 
                            ESP_ZB_HA_ON_OFF_SWITCH_DEVICE_ID);
    
    ZbEndPoint* lightEp = new ZbEndPoint(2, 
                            ESP_ZB_HA_ON_OFF_LIGHT_DEVICE_ID);

    ZbEndPoint* tempEp = new ZbEndPoint(SENSOR_ENDPOINT, 
                            ESP_ZB_HA_TEMPERATURE_SENSOR_DEVICE_ID);
    
    ZbBasicCluster* basicCl = new ZbBasicCluster(false,
                        ESP_ZB_ZCL_BASIC_ZCL_VERSION_DEFAULT_VALUE,
                        (uint8_t)0x01);
    basicCl->addAttribute(ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID,
                        (void*)MANUFACTURER_NAME);
    basicCl->addAttribute(ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID,
                        (void*)MODEL_IDENTIFIER);
    
    ZbIdentifyCluster* identifyServer = new ZbIdentifyCluster();
    identifyServer->setCallback(&identifyHandler);
    
    ZbIdentifyCluster* identifyClient = new ZbIdentifyCluster(true);
    _tempMeasurement  = new ZbTemperatureMeasCluster(false,
                                ESP_TEMP_SENSOR_MIN_VALUE,
                                ESP_TEMP_SENSOR_MAX_VALUE,
                                ESP_ZB_ZCL_TEMP_MEASUREMENT_MEASURED_VALUE_DEFAULT);
    
    _timeCluster = new ZbTimeCluster(true);

    ZbIdentifyCluster* identifyServer2 = new ZbIdentifyCluster(*identifyServer);

    ZbIdentifyCluster* identifyServer3 = new ZbIdentifyCluster(*identifyServer);


    //ZbTemperatureMeasCluster* onOffCl  = new ZbTemperatureMeasCluster(*tempMeasurement);
    ZbOnOffCluster* onOffCl = new ZbOnOffCluster(true);
    ZbOnOffCluster* onOfflightCl = new ZbOnOffCluster(false);
    onOfflightCl->setCallback(&lightOnOffHandler);

    tempEp->addCluster(basicCl);
    tempEp->addCluster(identifyServer);
    tempEp->addCluster(identifyClient);
    tempEp->addCluster(_tempMeasurement);
    tempEp->addCluster(_timeCluster);

    switchEp->addCluster(identifyServer2);
    switchEp->addCluster(onOffCl);

    lightEp->addCluster(identifyServer3);
    lightEp->addCluster(onOfflightCl);

    _zbDevice->addEndPoint(*switchEp);
    _zbDevice->addEndPoint(*tempEp);
    _zbDevice->addEndPoint(*lightEp);

    
    //driver_init();

    ESP_LOGI(TAG,"---------------- Starting ZbDevice ------------------------");
    _zbDevice->setReadyCallback(initWhenJoined);

    //ZbApsData* inst = ZbApsData::getInstance();
    _zbDevice->start();

    vTaskDelay(pdMS_TO_TICKS(7000));
    // Obtain the handle of a task from its name.
    _xHandle = xTaskGetHandle( "button_task" );
        

}
//Static
void Main::initWhenJoined()
{
    static bool isInitialized = false;

    _this->_timeCluster->readAttribute(ESP_ZB_ZCL_ATTR_TIME_TIME_ID);

    isInitialized = true;

  
}

/*
// static
void Main::temp_sensor_handler(float temperature)
{
    int16_t measured_value = (int16_t)(temperature * 100);
    ESP_LOGD(TAG,"trying to update value %d", measured_value);
    // Update temperature sensor measured value 
    esp_zb_lock_acquire(portMAX_DELAY);

    esp_zb_zcl_set_attribute_val(SENSOR_ENDPOINT,
        ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE,
        ESP_ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_ID, &measured_value, false);
    esp_zb_lock_release();
}

esp_err_t Main::driver_init()
{

    _tempDriver = new TemperatureDriver(ESP_TEMP_SENSOR_MIN_VALUE, 
                        ESP_TEMP_SENSOR_MAX_VALUE,
                        TEMPERATURE_SENSOR_CLK_SRC_DEFAULT,
                        ESP_TEMP_SENSOR_UPDATE_INTERVAL, 
                        Main::temp_sensor_handler);
    return ESP_OK;
}
*/

void Main::run(void)
{
    ESP_LOGI(TAG,"Main task high water mark %d", 
                            uxTaskGetStackHighWaterMark(NULL));

    ESP_LOGI(TAG,"Counter read %d", _fMeter.getPinLevel());

    _tempMeasurement->setReporting(ESP_ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_ID);

/*
    ESP_LOGI(TAG,"Task Handle %lx", (uint32_t)_xHandle);
    ESP_LOGI(TAG,"Button task high water mark %d", uxTaskGetStackHighWaterMark(_xHandle));
    ESP_LOGI(TAG,"Zigbee task high water mark %d", 
                            uxTaskGetStackHighWaterMark(_zbDevice->getZbTask()));
    ESP_LOGI(TAG,"Main task high water mark %d", 
                            uxTaskGetStackHighWaterMark(NULL));
*/

    vTaskDelay(pdMS_TO_TICKS(2000));

    ESP_LOGD(TAG,"App is running");
    
}


extern "C" void app_main(void)
{
    App.setup();

    while (true)
    {
        App.run();
    }    

    //should not reach here
}


