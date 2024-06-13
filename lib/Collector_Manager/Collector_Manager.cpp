#include <Collector_Manager.h>

void Collector_Manager::add_sample(const double value_, const int measurement_, const uint_fast64_t timestamp_)
{   
    double          value       = value_;
    int             measurement = measurement_;
    uint_fast64_t   timestamp   = timestamp_;
    
    memcpy(this->buffer_8_bytes, (uint8_t *)&timestamp, sizeof(timestamp));
        for (uint_fast8_t i = 0; i < 8; i++)
        {
            this->buffer_read[this->j] = this->buffer_8_bytes[i];
            this->j++;
        }
    memcpy(buffer_8_bytes, (uint8_t *)&value, sizeof(value));
        for (uint_fast8_t i = 0; i < 8; i++)
        {
            this->buffer_read[this->j] = this->buffer_8_bytes[i];
            this->j++;
        }
    memcpy(this->buffer_4_bytes, (uint8_t *)&measurement, sizeof(measurement));
        for (uint_fast8_t i = 0; i < 4; i++)
        {
            this->buffer_read[j] = this->buffer_4_bytes[i];
            this->j++;
        }
    if (this->j >= (Buffer_size))
    {
        this->j = 0;    
        while(xSemaphoreTake(*this->xMutex, portMAX_DELAY) != pdTRUE) {
        }
        
        for ( size_t i = 0; i < Buffer_size; i++)
        {
            this->buffer_write[i] = this->buffer_read[i];
        }
        this->buffer_write[Buffer_size+1] = '#';
        this->buffer_write[Buffer_size+2] = 'E';
        this->buffer_write[Buffer_size+3] = 'O';
        this->buffer_write[Buffer_size+4] = 'F';
        xSemaphoreGive(*this->xMutex);
        xSemaphoreGive(*this->xSemaphore);
        uint_fast64_t timer_now = esp_timer_get_time();
        this->logger_manager_ptr->debug("Buffer ready : "  + String((timer_now - this->timer_to_read)/1000));
        this->timer_to_read = timer_now;
    }
    
}