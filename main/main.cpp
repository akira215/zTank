//#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

#include "main.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <functional>

#include <iostream>

#include <vector> // todel

#include "waterPressureMeasCluster.h"

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
BlinkTask* Main::_ledBlinking = nullptr;
GpioOutput Main::_led { (gpio_num_t)CONFIG_PIN_LED }; //TODO led pin number in config file

Main App;

Main::Main()
{
    //_zbDevice->getInstance();
    //esp_log_level_set("Main_app", ESP_LOG_DEBUG); 
    esp_log_level_set("ZB_CPP", ESP_LOG_DEBUG);
    esp_log_level_set("Ads_driver", ESP_LOG_VERBOSE);
    esp_log_level_set("EventLoop", ESP_LOG_DEBUG);
}

// static
void Main::ledFlash(uint64_t speed)
{
    if(speed == 0) 
    {
        if(_ledBlinking){
        delete _ledBlinking;
        _ledBlinking = nullptr;
        }
        _led.off();
    } else if(speed == -1) {
        if(_ledBlinking){
        delete _ledBlinking;
        _ledBlinking = nullptr;
        }
        _led.on();
    } else {
        if(!_ledBlinking)
            _ledBlinking = new BlinkTask(_led, speed); // very short flash
        else
            _ledBlinking->setBlinkPeriod(speed);
    }

}

//Static
void Main::shortPressHandler(void *handler_args, esp_event_base_t base, int32_t id, void *event_data)
{
    //ESP_LOGI(TAG,"Short Press detected %ld -",id);
    if(ZbNode::getInstance()->isJoined())  
    {
        ESP_EARLY_LOGI(TAG, "Send 'on_off toggle' command");

        ZbNode::getInstance()->sendCommand(1,ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
                                    true,ESP_ZB_ZCL_CMD_ON_OFF_TOGGLE_ID);
    } else {
         ZbNode::getInstance()->joinNetwork();
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
        ledFlash(50);
    else
        ledFlash(0);
}

// Static
void Main::lightOnOffHandler(uint16_t attrId, void* value)
{
    //ZbTimeCluster* timecluster = static_cast<ZbTimeCluster*>(handler_args);
    //ZbCluster::eventArgs* e = static_cast<ZbCluster::eventArgs*>(event_data);
    
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

//void Main::zbDeviceEventHandler(ZbNode::nodeEvent_t event)
void Main::zbDeviceEventHandler(ZbNode::nodeEvent_t event)
{
    static bool isInitialized = false;

    switch(event){
        case ZbNode::JOINED:
            // Device just joined the network
            {
            ledFlash(0);
            // Synchronise RTC clock of the device
            _timeClient->syncRTC();
            // Reload data from coordinator
            //_fMeter->setCurrentSummationDelivered(0);
            //_fMeter->startReporting();
            //esp_zb_lock_acquire(portMAX_DELAY);
            //esp_zb_zcl_attr_t* test = _fMeter->getAttribute(ATTR_METERING_CURRENT_VOLUME_ID);
            // esp_zb_zcl_attr_t *test = esp_zb_zcl_get_attribute(1,ESP_ZB_ZCL_CLUSTER_ID_METERING ,
            //        ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, ATTR_METERING_CURRENT_VOLUME_ID);
            //        ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, ESP_ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID);
            //uint32_t val = *(static_cast<uint32_t*>(test->data_p));

            //ESP_LOGI(TAG,"custom cluster is  id: %d - type: %d - acces: %d - manuf: %d",
            //                            test->id, test->type, test->access, test->manuf_code);
            //ESP_LOGI(TAG,"Value custom cluster is : %ld",
            //                            val);
            //test = esp_zb_zcl_get_attribute(1,ESP_ZB_ZCL_CLUSTER_ID_METERING ,
            //        ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, ESP_ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID);
;
            //uint64_t val = *(static_cast<uint64_t*>(test->data_p));

            //ESP_LOGI(TAG,"Value custom cluster is  id: %d - type: %d - acces: %d - manuf: %d",
            //                            test->id, test->type, test->access, test->manuf_code);
            //esp_zb_lock_release();

            //ESP_LOGI(TAG,"Setting reporting for attributes");

            //int16_t tempChange = 1;
            //_tempMeasurement->setReporting(ESP_ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_ID, &tempChange );

            //uint16_t flowChange = 1;
            //_fMeter->setReporting(ESP_ZB_ZCL_ATTR_FLOW_MEASUREMENT_VALUE_ID, &flowChange);


            }
            break;
        case ZbNode::JOINING:
            ledFlash(FAST_BLINK);
            break;
        case ZbNode::NLME_STATUS:
            if(_zbDevice->isJoined()) 
                ledFlash(0);
            else 
                ledFlash(50); //very short flash
            break;
        case ZbNode::LEAVING:
            ledFlash(50);
            break;
        case ZbNode::LEFT:
            ledFlash(-1);
            break; 
        default:
            ESP_LOGW(TAG,"Device Event Handler, event %d not registered occured", 
                        static_cast<uint>(event));
            break;
    }

    isInitialized = true;
  
}

void Main::setup(void)
{
    ESP_LOGI(TAG,"---------- Setup ----------");

    _button.enablePullup();
    _buttonTask = new ButtonTask (_button);
    _buttonTask->setShortPressHandler(&shortPressHandler);
    _buttonTask->setLongPressHandler(&longPressHandler,(void*)this);

    AdsDriver::getInstance().setup();



    ESP_LOGD(TAG,"Creating Zigbee device");
    _zbDevice = ZbNode::getInstance();
    _zbDevice->registerNodeEventHandler(&Main::zbDeviceEventHandler, this);

    ZbEndPoint* measEp = new ZbEndPoint(1, 
                            ESP_ZB_HA_METER_INTERFACE_DEVICE_ID);
    
    ZbEndPoint* upstreamEp = new ZbEndPoint(2, 
                            ESP_ZB_HA_SIMPLE_SENSOR_DEVICE_ID);
    
    ZbEndPoint* switchEp = new ZbEndPoint(3, 
                            ESP_ZB_HA_ON_OFF_SWITCH_DEVICE_ID);
    
    //ZbEndPoint* lightEp = new ZbEndPoint(2, 
    //                        ESP_ZB_HA_ON_OFF_LIGHT_DEVICE_ID);
    


    ZbEndPoint* tempEp = new ZbEndPoint(SENSOR_ENDPOINT, 
                            ESP_ZB_HA_TEMPERATURE_SENSOR_DEVICE_ID);
    
    ZbBasicCluster* basicCl = new ZbBasicCluster(false,
                        ESP_ZB_ZCL_BASIC_ZCL_VERSION_DEFAULT_VALUE,
                        (uint8_t)0x01);
    basicCl->addAttribute(ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID,
                        (void*)MANUFACTURER_NAME);
    basicCl->addAttribute(ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID,
                        (void*)MODEL_IDENTIFIER);
    
/*
    ZbPowerCfgCluster* powerCl = new ZbPowerCfgCluster(false,
                                    2400, 25);
*/

    ZbIdentifyCluster* identifyServer = new ZbIdentifyCluster();
   
    
    ZbIdentifyCluster* identifyClient = new ZbIdentifyCluster(true);

    // Sensor clusters
    _fMeter = new WaterFlowMeasCluster();

    // Upstream channel 2
    WaterPressureMeasCluster* upstreamPressure = new WaterPressureMeasCluster(CONFIG_UPSTREAM_PRESSURE_CH);


    _tempMeasurement  = new ZbTemperatureMeasCluster(false,
                                ESP_TEMP_SENSOR_MIN_VALUE,
                                ESP_TEMP_SENSOR_MAX_VALUE,
                                ESP_ZB_ZCL_TEMP_MEASUREMENT_MEASURED_VALUE_DEFAULT);
    

    _timeClient = new ZbTimeClusterClient();

    ESP_LOGI(TAG,"---------------- Register ------------------------");

    ZbIdentifyCluster* identifyServer2 = new ZbIdentifyCluster(*identifyServer);
    ZbIdentifyCluster* identifyServer3 = new ZbIdentifyCluster(*identifyServer);
    ZbIdentifyCluster* identifyServer4 = new ZbIdentifyCluster(*identifyServer);
    

    //ZbTemperatureMeasCluster* onOffCl  = new ZbTemperatureMeasCluster(*tempMeasurement);
    ZbOnOffCluster* onOffCl = new ZbOnOffCluster(true);
    
    measEp->addCluster(identifyServer4);
    measEp->addCluster(basicCl);
    measEp->addCluster(_fMeter);
    measEp->addCluster(_fMeter->getKfactorCluster());

    upstreamEp->addCluster(identifyServer3);
    upstreamEp->addCluster(upstreamPressure);
    upstreamEp->addCluster(upstreamPressure->getKfactorCluster());
    
    
    tempEp->addCluster(identifyServer);
    tempEp->addCluster(identifyClient);
    tempEp->addCluster(_tempMeasurement);

    tempEp->addCluster(_timeClient);

    switchEp->addCluster(identifyServer2);
    switchEp->addCluster(onOffCl);
    //switchEp->addCluster(powerCl);

   

    _zbDevice->addEndPoint(*switchEp);
    _zbDevice->addEndPoint(*tempEp);
    _zbDevice->addEndPoint(*upstreamEp);
    _zbDevice->addEndPoint(*measEp);

    //_timeCluster->registerEventHandler(&Main::timeHandler, this);
    _eventLoopHandle = xTaskGetHandle( "ZbEventLoop" );
    
    //driver_init();

    ESP_LOGI(TAG,"---------------- Starting ZbDevice ------------------------");
    //_zbDevice->setReadyCallback(initWhenJoined);
    
    //ZbApsData* inst = ZbApsData::getInstance();
    _zbDevice->start();

    vTaskDelay(pdMS_TO_TICKS(7000));

    // Start AdsDriver after zigbee stack as it will start to change attributes
    // 1000ms for each measures
    AdsDriver::getInstance().start(1000);

    // Obtain the handle of a task from its name.
    _xHandle = xTaskGetHandle( "button_task" ); 

}



void Main::run(void)
{
    /*
    ESP_LOGI(TAG,"Main task high water mark %d", 
                            uxTaskGetStackHighWaterMark(NULL));

    ESP_LOGI(TAG,"Counter pin level read %d", _fMeter->getPinLevel());

    ESP_LOGI(TAG,"Event Loop task high water mark %d", 
                            uxTaskGetStackHighWaterMark(_eventLoopHandle));
*/
/*
    ESP_LOGI(TAG,"Task Handle %lx", (uint32_t)_xHandle);
    ESP_LOGI(TAG,"Button task high water mark %d", uxTaskGetStackHighWaterMark(_xHandle));
    ESP_LOGI(TAG,"Zigbee task high water mark %d", 
                            uxTaskGetStackHighWaterMark(_zbDevice->getZbTask()));
    ESP_LOGI(TAG,"Main task high water mark %d", 
                            uxTaskGetStackHighWaterMark(NULL));
*/

    vTaskDelay(pdMS_TO_TICKS(2000));

    //ESP_LOGD(TAG,"App is running");
    
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


