#pragma once

#include <Arduino.h>

#include "freertos/semphr.h"
#include "freertos/FreeRTOS.h"
#include "Logger_Manager.h" 

#define Buffer_size 1000U

class Collector_Manager
{
public:
  Collector_Manager(
                    Logger_Manager *logger_manager_ptr = NULL,
                    SemaphoreHandle_t *xMutex = NULL, 
                    SemaphoreHandle_t *xSemaphore = NULL): 
                    
                    logger_manager_ptr(logger_manager_ptr),
                    xMutex(xMutex),
                    xSemaphore(xSemaphore){}

  ~Collector_Manager(){}

  void add_sample(double value, int measurement, uint_fast64_t timestamp);
  bool alloc_buffers(){ 
    // buffer_read  = new uint8_t[Buffer_size];//(uint8_t*) malloc(Buffer_size);
    // buffer_write = new uint8_t[Buffer_size+4];//(uint8_t*) malloc(Buffer_size+4);
    return true;
  }
  uint8_t buffer_read[Buffer_size];
  uint8_t buffer_write[Buffer_size+4];
  bool write = false;
private:

  uint_fast64_t timer_to_read = 0;
  
  Logger_Manager *logger_manager_ptr = NULL;

  SemaphoreHandle_t *xMutex;
  SemaphoreHandle_t *xSemaphore;

  uint32_t j = 0;
  uint8_t buffer_8_bytes[8U] = {0};
  uint8_t buffer_4_bytes[4U] = {0};
};