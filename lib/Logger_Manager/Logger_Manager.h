#pragma once

#include <Arduino.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

enum Log_Level
{
  OFF       = 0,
  CRITICAL  = 1,
  ERROR     = 2,
  WARNING   = 3,
  DEBUG     = 4,
  INFO      = 5
};

class Logger_Manager
{
public:
  Logger_Manager(uint8_t log_level = 0): log_level(log_level) {
    Serial.begin(115200);
  }

  ~Logger_Manager() {
  }
  void flush();
  void info(String message);
  void info(int message);

  void debug(String message);
  void debug(uint_fast64_t message);
  
  void warning(String message);
  void error(String message);
  void critical(String message);
private:
  uint8_t log_level;
};