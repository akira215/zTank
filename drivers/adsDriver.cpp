#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE


#include "adsDriver.h"

#include <esp_log.h> // TODEL development purpose

#include <iostream>

static const char *TAG = "Ads_driver";



AdsDriver::AdsDriver(): 
        i2c_master(I2C_MASTER_NUM, I2C_MASTER_SDA_IO, I2C_MASTER_SCL_IO, true),
        ads(&i2c_master,Ads1115::Addr_Gnd, ADS_I2C_FREQ_HZ)
{
    
}

void AdsDriver::run(void)
{
    vTaskDelay(pdMS_TO_TICKS(2000));
    ESP_LOGI(TAG, "Conversion : %f", ads.getVoltage(Ads1115::MUX_2_GND));
}

void AdsDriver::setup(void)
{
    Ads1115::Cfg_reg cfg = ads.getConfig();
    ESP_LOGI(TAG, "Config: %d", cfg.reg.reg);
    ESP_LOGI(TAG, "COMP QUE:    %x",cfg.bit.COMP_QUE);
    ESP_LOGI(TAG, "COMP LAT:    %x",cfg.bit.COMP_LAT);
    ESP_LOGI(TAG, "COMP POL:    %x",cfg.bit.COMP_POL);
    ESP_LOGI(TAG, "COMP MODE:   %x",cfg.bit.COMP_MODE);
    ESP_LOGI(TAG, "DataRate:    %x",cfg.bit.DR);
    ESP_LOGI(TAG, "MODE:        %x",cfg.bit.MODE);
    ESP_LOGI(TAG, "PGA:         %x",cfg.bit.PGA);
    ESP_LOGI(TAG, "MUX:         %x",cfg.bit.MUX);
    ESP_LOGI(TAG, "OS:          %x",cfg.bit.OS);

    Ads1115::reg2Bytes_t regData;
    regData = ads.readRegister(Ads1115::reg_lo_thresh);
    ESP_LOGI(TAG, "Reg Lo Thresh : %x", regData.reg);
    ESP_LOGI(TAG, "Reg Lo MSB : %x", regData.MSB);
    ESP_LOGI(TAG, "Reg Lo LSB : %x", regData.LSB);

    regData = ads.readRegister(Ads1115::reg_hi_thresh);
    ESP_LOGI(TAG, "Reg Hi Thresh : %x", regData.reg);
    ESP_LOGI(TAG, "Reg Hi MSB : %x", regData.MSB);
    ESP_LOGI(TAG, "Reg Hi LSB : %x", regData.LSB);
    
    ESP_LOGI(TAG, "Changing config --------------");
    regData.MSB = 0x01; 
    //ads.writeRegister(Ads1115::reg_hi_thresh,regData);

    regData = ads.readRegister(Ads1115::reg_hi_thresh);
    ESP_LOGI(TAG, "Reg Hi Thresh : %x", regData.reg);

    ESP_LOGI(TAG, "Changing config --------------");
    regData.MSB = 0x01; 
    //ads.writeRegister(Ads1115::reg_lo_thresh,regData);

    regData = ads.readRegister(Ads1115::reg_lo_thresh);
    ESP_LOGI(TAG, "Reg Lo Thresh : %x", regData.reg);


    regData = ads.readRegister(Ads1115::reg_configuration);
    ESP_LOGI(TAG, "Configuration : %x", regData.reg);
    ESP_LOGI(TAG, "Cfg MSB : %x", regData.MSB);
    ESP_LOGI(TAG, "Cfg LSB : %x", regData.LSB);

    ESP_LOGI(TAG, "Starting --------------");
 

    ads.setPga(Ads1115::FSR_4_096); // Setting range for PGA optimized to 3.3V Power supply
    ads.setSps(Ads1115::SPS_8); // Setting range for PGA optimized to 3.3V Power supply

    // event handler shall have signature void(uint16_t input, int16_t value)
    ads.setReadyPin(GPIO_NUM_3, &ads1115_event_handler);

    regData = ads.readRegister(Ads1115::reg_configuration);
    ESP_LOGI(TAG, "Configuration : %x", regData.reg);

    regData = ads.readRegister(Ads1115::reg_lo_thresh);
    ESP_LOGI(TAG, "Reg Lo Thresh : %x", regData.reg);

    regData = ads.readRegister(Ads1115::reg_hi_thresh);
    ESP_LOGI(TAG, "Reg Hi Thresh : %x", regData.reg);
}

void AdsDriver::ads1115_event_handler(uint16_t input, int16_t value)
{
    ESP_LOGI(TAG, "Callback Main Ads1115 input : %d", input);
    ESP_LOGI(TAG, "Callback Main Ads1115 value : %d", value);
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