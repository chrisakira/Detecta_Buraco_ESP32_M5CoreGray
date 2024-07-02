#include <Arduino.h>

#include "M5_Manager.h"
#include "SD_Manager.h"
#include "Collector_Manager.h"
#include "Logger_Manager.h"
#include "Uploader_Manager.h"
#include <TinyGPSPlus.h>

//#define WITH_SD true

hw_timer_t *timer = NULL;
// The TinyGPSPlus object

TinyGPSPlus gps;
SemaphoreHandle_t xMutex = NULL;
SemaphoreHandle_t xSemaphore = NULL;
SemaphoreHandle_t xSemaphore_Uploader = NULL;

Logger_Manager logger_manager(DEBUG);
M5_Manager m5_manager(&logger_manager, "VIVOFIBRA-F46E", "EAEA8FF46E");
Collector_Manager collector_manager(&logger_manager, &xMutex, &xSemaphore);
Uploader_Manager uploader_manager("http://192.168.15.21:5000", &logger_manager, &collector_manager, &m5_manager, &xMutex, &xSemaphore);
SD_Manager sd_manager(&logger_manager, &m5_manager, &collector_manager, &uploader_manager, &xMutex, &xSemaphore, &xSemaphore_Uploader);

TickType_t lastWakeTime = xTaskGetTickCount();

void setup()
{
    disableCore0WDT();

    xMutex = xSemaphoreCreateMutex();
    xSemaphore = xSemaphoreCreateBinary();
    xSemaphore_Uploader = xSemaphoreCreateBinary();

    sd_manager.set_logger_manager(&logger_manager);
    m5_manager.M5_begin(true, false, true, true);
    m5_manager.reset_LCD();
    M5.Lcd.println("Initializing...");
    m5_manager.connect_wifi();
    m5_manager.update_unix_time();
    M5.Lcd.println("Pinging the API...");
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    m5_manager.reset_LCD();
    while(!uploader_manager.get_alive())
    {
        M5.Lcd.println("API not reachable.");
        M5.Lcd.println("Trying again in 10s.");
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
    M5.Lcd.println("API connection confirmed.");    
    collector_manager.alloc_buffers();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    m5_manager.reset_LCD();

    if (m5_manager.create_tasks())
        M5.Lcd.println("M5 manager Tasks created.");
    else
    {
        M5.Lcd.println("M5 Manager Tasks not created.");
        M5.Lcd.println("Rebooting in 10s.");
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        esp_restart();
    }
    Serial.println("M5 Manager Tasks created.");
    #ifdef WITH_SD
        if (sd_manager.create_tasks())
            M5.Lcd.println("M5 manager Tasks created.");
        else
        {
            M5.Lcd.println("SD Manager Tasks not created.");
            M5.Lcd.println("Rebooting in 10s.");
            vTaskDelay(10000 / portTICK_PERIOD_MS);
            esp_restart();
        }
    #endif

    if(uploader_manager.create_task())
        M5.Lcd.println("Uploader manager Tasks created.");
    else{
        M5.Lcd.println("Uploader Manager Tasks not created.");
        M5.Lcd.println("Rebooting in 10s.");
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        esp_restart();
    }
    
    Serial.println("Uploader Manager Tasks created.");
}

void loop()
{

    TickType_t lastWakeTime = xTaskGetTickCount();
    const TickType_t interval = pdMS_TO_TICKS(15);
    for (;;)
    {
        lastWakeTime = xTaskGetTickCount();
        uint_fast64_t timestamp = ((uint_fast64_t)m5_manager.now) * 1000000 + (esp_timer_get_time() - m5_manager.micros_now);
        
        float pitch  = m5_manager.pitch;
        float roll   = m5_manager.roll;
        float yaw    = m5_manager.yaw;
        float acc_x  = m5_manager.accel_X;
        float acc_y  = m5_manager.accel_Y;
        float acc_z  = m5_manager.accel_Z;
        float gyro_x = m5_manager.gyro_X;
        float gyro_y = m5_manager.gyro_Y;
        float gyro_z = m5_manager.gyro_Z;

        collector_manager.add_sample(roll,   1, timestamp);
        collector_manager.add_sample(yaw,    2, timestamp);
        collector_manager.add_sample(pitch,  3, timestamp);

        collector_manager.add_sample(acc_x, 4, timestamp);
        collector_manager.add_sample(acc_y, 5, timestamp);
        collector_manager.add_sample(acc_z, 6, timestamp);

        collector_manager.add_sample(gyro_x, 7, timestamp);
        collector_manager.add_sample(gyro_y, 8, timestamp);
        collector_manager.add_sample(gyro_z, 9, timestamp); 
        vTaskDelayUntil(&lastWakeTime, interval);
    }
    
}
