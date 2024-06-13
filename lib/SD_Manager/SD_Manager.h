#pragma once

#include <Arduino.h>
#include <vector>
#include <cstdint>

#include "driver/gpio.h"
#include "SdFat.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"


#include "Logger_Manager.h"
#include "Uploader_Manager.h"
#include "M5_Manager.h"
#include "Collector_Manager.h"

#define N_WRITES 2U

class SD_Manager
{
public:
    SD_Manager( Logger_Manager* logger_manager_ptr = NULL,
                M5_Manager* m5_manager_ptr = NULL,
                Collector_Manager* collector_manager_ptr = NULL,
                Uploader_Manager* uploader_manager_ptr = NULL,
                SemaphoreHandle_t* xMutex = NULL,
                SemaphoreHandle_t* xSemaphore = NULL,
                SemaphoreHandle_t* xSemaphore_Uploader = NULL): 
                
                logger_manager_ptr(logger_manager_ptr),
                m5_manager_ptr(m5_manager_ptr),
                collector_manager_ptr(collector_manager_ptr),
                uploader_manager_ptr(uploader_manager_ptr),
                xMutex(xMutex),
                xSemaphore(xSemaphore),
                xSemaphore_Uploader(xSemaphore_Uploader) {}

    ~SD_Manager() {}

    bool create_tasks();
    void sd_writer(void *z);
    String get_oldest_file(const char* dirname);
    bool delete_file(String filename);
    void erase_all_files();
    bool connect_sd();

    void set_logger_manager(Logger_Manager* logger_manager_ptr){
        
    }
    SdFat SDfat;
    bool write = false;
    uint8_t file_writes = 0;
private: 


    char filename[50];
    char filename_ready[50];
    

    uint16_t file_counter = 0;
    bool is_task_created = false;

    SemaphoreHandle_t* xMutex = NULL;
    SemaphoreHandle_t* xSemaphore = NULL;
    SemaphoreHandle_t* xSemaphore_Uploader = NULL;

    Uploader_Manager* uploader_manager_ptr = NULL;
    Collector_Manager* collector_manager_ptr;
    Logger_Manager* logger_manager_ptr;
    M5_Manager* m5_manager_ptr;
    
    
    TaskHandle_t receive_data_buffer_task_handle;
    TaskHandle_t write_data_buffer_task_handle;
    SdFile_ myFile;
};