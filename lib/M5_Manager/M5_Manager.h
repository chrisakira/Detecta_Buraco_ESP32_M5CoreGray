#pragma once

#include <Arduino.h>
#include <M5Stack.h>
#include <TinyGPS++.h> 
#include <WiFi.h>
#include <ctime>
#include "time.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "Logger_Manager.h"
class M5_Manager {
public:
    M5_Manager(Logger_Manager *logger_manager_ptr, String ssid = "Perdi", String password = "Perdi"): 
    logger_manager_ptr(logger_manager_ptr), ssid(ssid), password(password) {}

    ~M5_Manager(){}

    void M5_begin(bool LCDEnable = true, bool SDEnable = true, bool SerialEnable = true, bool I2CEnable = true);
    
    bool check_LCD();

    bool initialize_LCD();

    bool reset_LCD();
    
    bool create_tasks();

    void update_mpu_data(void *z);
    
    void update_mpu_battery_level(void *z);
    
    void write_mpu_data(void *z);
    
    const char *get_current_time();
    
    bool connect_wifi();

    void update_unix_time();

    volatile double Latitude     = 0.0F;
    volatile double Longitude    = 0.0F;
    volatile double Altitude     = 0.0F;
    volatile double Speed        = 0.0F;

    volatile float accel_X = 0.0F;
    volatile float accel_Y = 0.0F;
    volatile float accel_Z = 0.0F;

    volatile float gyro_X = 0.0F;
    volatile float gyro_Y = 0.0F;
    volatile float gyro_Z = 0.0F;

    volatile float pitch = 0.0F;
    volatile float roll  = 0.0F;
    volatile float yaw   = 0.0F;

    float temperature = 0.0F;
    time_t now = 0;
    uint_fast64_t micros_now = 0;
private:
    bool gps_status = false;
    uint_fast64_t GPS_last_seen = 0;
    
    static const int RXPin = SDA, TXPin = SCL;
    static const uint32_t GPSBaud = 9600;

    TinyGPSPlus gps; 
    String ssid;
    String password;
    const char* ntpServer        = "pool.ntp.org"; 
    const long gmtOffset_sec     = -10800; // GMT offset for Sao Paulo (UTC-3)
    const int daylightOffset_sec = 3600;
    

    Logger_Manager *logger_manager_ptr = NULL;
    bool lcd_initialized = false;
    bool is_task_created = false;
    TaskHandle_t update_mpu_data_task_handle;
    TaskHandle_t update_battery_level_task_handle;
};