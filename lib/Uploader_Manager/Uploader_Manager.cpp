#include <Uploader_Manager.h>
bool Uploader_Manager::get_alive()
{
    if(WiFi.status() != WL_CONNECTED)
        return false;

    this->http.begin(this->get_alive_url); 
    int httpCode = http.GET();
    if(this->logger_manager_ptr!=NULL){
        this->logger_manager_ptr->info("[Uploader_Manager] GET alive returned code: " + String(httpCode));
    }
    http.end();
    if(httpCode > 0)
        return true;
    else
        return false;
}

bool Uploader_Manager::post_file(String file_path, SdFat *SDfat)
{
    
    if (WiFi.status() != WL_CONNECTED)
        return false;
    
    this->logger_manager_ptr->info("[Uploader_Manager] Openning file " + file_path);


    String file_header = "attachment; filename= " + file_path;
    this->http.begin(this->post_file_url);
    this->http.addHeader("Content-Type", "application/octet-stream");
    this->http.addHeader("Authorization", "F4ujtjaC7vgFE4oowrgc8Pd6WbT");
    this->http.addHeader("Cookie", "device=" + WiFi.macAddress()); 
    this->http.addHeader("Content-Disposition", file_header.c_str());
    file_path = "/" + file_path;    
    File_ file = SDfat->open(file_path.c_str(), O_READ);
     // Enviar o arquivo via POST
    size_t bytesRead = 0;
    bytesRead = file.readBytes(buffer, Buffer_size+4);
   
    this->logger_manager_ptr->info("[Uploader_Manager] Bytes read: " + String(bytesRead));
    if(bytesRead > 10)
        http.sendRequest("POST", buffer, bytesRead);

    this->http.end();
    return true;
}

bool Uploader_Manager::post_file_buffer(String file_name, uint8_t *buffer)
{
    
    if (WiFi.status() != WL_CONNECTED)
        return false;

    if(this->connect_status == false){
        String file_header = "attachment; filename= " + file_name;
        this->connect_status = true;
        this->http.begin(this->post_file_url);
        this->http.addHeader("Content-Type", "application/octet-stream");
        this->http.addHeader("Authorization", "F4ujtjaC7vgFE4oowrgc8Pd6WbT");
        this->http.addHeader("Cookie", "device="+String(mac_address)); 
        this->http.addHeader("Content-Disposition", file_header.c_str());
    }

    this->http.sendRequest("POST", buffer,  Buffer_size+4);

    // this->http.end();
    return true;
}

bool Uploader_Manager::uploader()
{
    while(xSemaphoreTake(*this->xMutex, portMAX_DELAY) != pdTRUE) {}

    #ifdef WITH_SD
        String file_path = ""; //this->sd_manager_ptr->get_oldest_file("/");
        if (file_path != "" && file_path.indexOf("/System") == -1 && file_path.indexOf("not-ready") == -1)
        {
            this->logger_manager_ptr->info("[Uploader_Manager] File to upload BAAAATATA: " + file_path);
            if (this->post_file(file_path) == true)
            {
                if (this->logger_manager_ptr != NULL)
                    this->logger_manager_ptr->info("[Uploader_Manager] File posted");
                xSemaphoreGive(*this->xMutex);
                return true;
            }
            else
            {
                if (this->logger_manager_ptr != NULL)
                    this->logger_manager_ptr->error("[Uploader_Manager] File not posted"); 
                xSemaphoreGive(*this->xMutex);
                return false;
            }
        }
        else
        {
            if (this->logger_manager_ptr != NULL)
                this->logger_manager_ptr->info("[Uploader_Manager] No file to upload"); 
            xSemaphoreGive(*this->xMutex);
            return false;
        }
        xSemaphoreGive(*this->xMutex); 
    #else
        sprintf(file_name, "%s%s", m5_manager_ptr->get_current_time(), ".aki");
        timer_write = esp_timer_get_time();
        this->post_file_buffer(file_name, this->collector_manager_ptr->buffer_write);
        xSemaphoreGive(*this->xMutex);
        logger_manager_ptr->debug("Sent in :      " + String((esp_timer_get_time() - timer_write)/1000));
    #endif
}

void Uploader_Manager::uploader_task(void *z)
{
    for (;;)
    {
        if (xSemaphoreTake(*this->xSemaphore_Uploader, portMAX_DELAY) == pdTRUE) 
            uploader();
        else
            vTaskDelay(5000);
    }
}

bool Uploader_Manager::create_task()
{
    try
    {
        if (this->is_task_created == false)
        {
            this->is_task_created = true;
            // xTaskCreatePinnedToCore([](void *z)
            //                         { static_cast<Uploader_Manager *>(z)->uploader_task(z); },
            //                         "Uploader", 2048, this, 4, &this->uploader_task_handler, 0);
            // if (this->logger_manager_ptr != NULL)
            //     this->logger_manager_ptr->info("[Uploader_Manager] Uploader Task Created");
            return true;
        }
        return false;
    }
    catch (const std::exception &e)
    {
        if (this->logger_manager_ptr != NULL)
            this->logger_manager_ptr->error("[Uploader_Manager] Uploader Task not created \n[Uploader_Manager] " + String(e.what()));
        return false;
    }
    return false;
}
