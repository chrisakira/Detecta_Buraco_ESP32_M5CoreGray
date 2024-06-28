#pragma once

#include <WiFi.h> 
#include <string.h>
#include <Arduino.h>
#include <HTTPClient.h>

#include "freertos/semphr.h"

#include "SdFat.h"
#include "Logger_Manager.h"
#include "Collector_Manager.h"
#include "M5_Manager.h"
class Uploader_Manager {
public:
    Uploader_Manager(   String base_url = "https://192.168.15.35/akirapi", 
                        Logger_Manager* logger_manager_ptr = NULL,
                        Collector_Manager* collector_manager_ptr = NULL,
                        M5_Manager* m5_manager_ptr = NULL,
                        SemaphoreHandle_t* xMutex = NULL,
                        SemaphoreHandle_t* xSemaphore_Uploader = NULL): 

                    base_url(base_url), 
                    get_alive_url(base_url + "/alive"), 
                    post_json_url(base_url + "/v1/data/json"), 
                    post_file_url(base_url + "/v1/data/akira"), 
                    ping_device_url(base_url + "/v1/ping"),
                    logger_manager_ptr(logger_manager_ptr),
                    collector_manager_ptr(collector_manager_ptr),
                    xMutex(xMutex),
                    xSemaphore_Uploader(xSemaphore_Uploader){  
        this->http.setReuse(true);
        this->http.setTimeout(5000);
        strcpy(mac_address, WiFi.macAddress().c_str());     
    }

    ~Uploader_Manager(){} 

    bool get_alive();
    bool post_file(String file_path = "", SdFat *SDfat = NULL);
    void uploader_task(void *z);
    bool create_task();
    bool uploader();
    void alloc_buffers(){ 
        // buffer = (uint8_t*) heap_caps_malloc(Buffer_size+4, MALLOC_CAP_SPIRAM);
        return;
    }
    bool post_file_buffer(String file_name, uint8_t *buffer);
    uint8_t buffer[2];
private: 
    bool connect_status = false;
    bool is_task_created = false;
    char mac_address[18];
    char file_name[100];
    uint64_t timer_write = 0;
    
    SemaphoreHandle_t* xMutex;
    SemaphoreHandle_t* xSemaphore_Uploader;
    
    Logger_Manager* logger_manager_ptr;
    Collector_Manager* collector_manager_ptr;
    M5_Manager* m5_manager_ptr;

    TaskHandle_t uploader_task_handler;
    WiFiClientSecure client;
    HTTPClient http; 
    String base_url = "";
    String get_alive_url = "";
    String post_json_url = ""; 
    String post_file_url = "";
    String ping_device_url = "";
};