#include <Logger_Manager.h>

void Logger_Manager::flush()
{
    Serial.flush();
}

void Logger_Manager::info(String message)
{
    if (this->log_level >= INFO)
        Serial.println("[info] " + message);
}

void Logger_Manager::info(int message)
{
    if (this->log_level >= INFO)
        Serial.println("[info] " + String(message));
}

void Logger_Manager::debug(String message)
{
    if (this->log_level >= DEBUG)
        Serial.println("[debug] " + message);
}

void Logger_Manager::debug(uint_fast64_t message)
{
    if (this->log_level >= DEBUG)
        Serial.println(message);
}

void Logger_Manager::warning(String message)
{
    if (this->log_level >= WARNING)
        Serial.println("[warning] " + message);
}

void Logger_Manager::error(String message)
{
    if (this->log_level >= ERROR)
        Serial.println("[error] " + message);
}

void Logger_Manager::critical(String message)
{
    if (this->log_level >= CRITICAL)
        Serial.println("[critical] " + message);
}



