/*
  zTank 
  Repository: https://github.com/akira215/
  License: GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
  Author: Akira Shimahara
*/

//#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

#include "adsDriver.h"

#include <esp_log.h> // TODEL development purpose

#include <iostream> // TODEL development purpose

static const char *TAG = "Ads_driver";



// Static event handler
void AdsDriver::ads1115_event_handler(uint16_t input, double value)
{
    ESP_LOGI(TAG, "Callback Main Ads1115 input: %d - value: %f", input-4, value);

    // MUX_X_GND is between 4 and 7. To get the correct range, minus 4
    AdsDriver::getInstance().setVoltage(input-4, value);
}

AdsDriver& AdsDriver::getInstance() 
{
    static AdsDriver instance; // Guaranteed to be destroyed.
                                // Instantiated on first use.
    return instance;
}

// Private constructor
AdsDriver::AdsDriver(): 
        _i2c_master(I2C_MASTER_NUM, I2C_MASTER_SDA_IO, I2C_MASTER_SCL_IO, true),
        _ads(&_i2c_master,Ads1115::Addr_Gnd, ADS_I2C_FREQ_HZ)
{
    
}

void AdsDriver::start(void)
{
    vTaskDelay(pdMS_TO_TICKS(2000));
    ESP_LOGI(TAG, "Conversion : %f", _ads.getVoltage(Ads1115::MUX_2_GND));
}

void AdsDriver::stop(void)
{
    //TODO implement
}

void AdsDriver::setup(void)
{
    Ads1115::Cfg_reg cfg = _ads.getConfig();
    ESP_LOGD(TAG, "Config: %d", cfg.reg.reg);
    ESP_LOGD(TAG, "COMP QUE:    %x",cfg.bit.COMP_QUE);
    ESP_LOGD(TAG, "COMP LAT:    %x",cfg.bit.COMP_LAT);
    ESP_LOGD(TAG, "COMP POL:    %x",cfg.bit.COMP_POL);
    ESP_LOGD(TAG, "COMP MODE:   %x",cfg.bit.COMP_MODE);
    ESP_LOGD(TAG, "DataRate:    %x",cfg.bit.DR);
    ESP_LOGD(TAG, "MODE:        %x",cfg.bit.MODE);
    ESP_LOGD(TAG, "PGA:         %x",cfg.bit.PGA);
    ESP_LOGD(TAG, "MUX:         %x",cfg.bit.MUX);
    ESP_LOGD(TAG, "OS:          %x",cfg.bit.OS);

    Ads1115::reg2Bytes_t regData;
    regData = _ads.readRegister(Ads1115::reg_lo_thresh);
    ESP_LOGD(TAG, "Reg Lo Thresh : %x", regData.reg);
    ESP_LOGD(TAG, "Reg Lo MSB : %x", regData.MSB);
    ESP_LOGD(TAG, "Reg Lo LSB : %x", regData.LSB);

    regData = _ads.readRegister(Ads1115::reg_hi_thresh);
    ESP_LOGD(TAG, "Reg Hi Thresh : %x", regData.reg);
    ESP_LOGD(TAG, "Reg Hi MSB : %x", regData.MSB);
    ESP_LOGD(TAG, "Reg Hi LSB : %x", regData.LSB);
    
    ESP_LOGD(TAG, "Changing config --------------");
    regData.MSB = 0x01; 
    //ads.writeRegister(Ads1115::reg_hi_thresh,regData);

    regData = _ads.readRegister(Ads1115::reg_hi_thresh);
    ESP_LOGD(TAG, "Reg Hi Thresh : %x", regData.reg);

    ESP_LOGD(TAG, "Changing config --------------");
    regData.MSB = 0x01; 
    //ads.writeRegister(Ads1115::reg_lo_thresh,regData);

    regData = _ads.readRegister(Ads1115::reg_lo_thresh);
    ESP_LOGD(TAG, "Reg Lo Thresh : %x", regData.reg);


    regData = _ads.readRegister(Ads1115::reg_configuration);
    ESP_LOGD(TAG, "Configuration : %x", regData.reg);
    ESP_LOGD(TAG, "Cfg MSB : %x", regData.MSB);
    ESP_LOGD(TAG, "Cfg LSB : %x", regData.LSB);

    ESP_LOGD(TAG, "Starting --------------");
 

    _ads.setPga(Ads1115::FSR_4_096); // Setting range for PGA optimized to 3.3V Power supply
    _ads.setSps(Ads1115::SPS_8); // Setting range for PGA optimized to 3.3V Power supply

    // event handler shall have signature void(uint16_t input, int16_t value)
    _ads.setReadyPin(GPIO_NUM_3, &ads1115_event_handler);

    regData = _ads.readRegister(Ads1115::reg_configuration);
    ESP_LOGD(TAG, "Configuration : %x", regData.reg);

    regData = _ads.readRegister(Ads1115::reg_lo_thresh);
    ESP_LOGD(TAG, "Reg Lo Thresh : %x", regData.reg);

    regData = _ads.readRegister(Ads1115::reg_hi_thresh);
    ESP_LOGD(TAG, "Reg Hi Thresh : %x", regData.reg);
}

void AdsDriver::setVoltage(uint8_t input, double value)
{
    if (input>3)
    {
        ESP_LOGE(TAG, "Unable to write input > 3 (was %d)", input);
        return;
    }

    _voltage[input] = value;
}

double AdsDriver::getVoltage(uint8_t input)
{
    if (input>3)
    {
        ESP_LOGE(TAG, "Unable to read input > 3 (was %d)", input);
        return 0.0f;
    }
    return _voltage[input];
}
/*
extern "C" void app_main(void)
{
    App.setup();

    while (true)
    {
        App.run();
    }    

    //should not reach here
    //ESP_ERROR_CHECK(i2c_driver_delete(I2C_MASTER_NUM));
    //ESP_ERROR_CHECK(i2c_del_master_bus(i2c_handle));
    //ESP_LOGD(TAG, "I2C de-initialized successfully");
}
    */