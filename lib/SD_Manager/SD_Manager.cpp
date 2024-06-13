#include <SD_Manager.h>
void SD_Manager::sd_writer(void *z)
{
    uint_fast32_t j = 0;
    uint64_t timer_write = 0;
    sprintf(filename, "%s%s", m5_manager_ptr->get_current_time(), "not-ready.aki");
    sprintf(filename_ready, "%s%s", m5_manager_ptr->get_current_time(), ".aki");
    // if (myFile.open(filename, O_WRITE | O_CREAT))
    //     logger_manager_ptr->debug("[SD_Manager.cpp] Started to write");
    // else
    // {
    //     logger_manager_ptr->critical("[SD_Manager.cpp] File not opened");
    //     exit(-1);
    // }
    for (;;) 
    {
        if (xSemaphoreTake(*this->xSemaphore, portMAX_DELAY) == pdTRUE)
        {
            while(xSemaphoreTake(*this->xMutex, portMAX_DELAY) != pdTRUE) {
            
            }
            
            timer_write = esp_timer_get_time();


            // for (size_t i = 0; i < Buffer_size; i++)
            // {
            //     Serial.print(this->collector_manager_ptr->buffer_write[i], HEX);
            //     Serial.print(" ");
            // }
            // Serial.println();
            
            
            
            
            // myFile.write(this->collector_manager_ptr->buffer_write, Buffer_size);

            // if(this->file_writes >= N_WRITES){
            // this->file_writes = 0;
            // myFile.write("#EOF", sizeof("#EOF"));
            // myFile.flush();
            // myFile.rename(filename_ready);
            // myFile.close();
            this->uploader_manager_ptr->post_file_buffer(filename_ready, this->collector_manager_ptr->buffer_write);
            xSemaphoreGive(*this->xMutex);
            
            // this->delete_file(filename_ready);
            // sprintf(filename, "%s%s", m5_manager_ptr->get_current_time(), "not-ready.aki");
            // sprintf(filename_ready, "%s%s", m5_manager_ptr->get_current_time(), ".aki");
            // if (myFile.open(filename, O_WRITE | O_CREAT))
            //     logger_manager_ptr->debug("[SD_Manager.cpp] Started to write");
            // else
            // {
            //     logger_manager_ptr->critical("[SD_Manager.cpp] File not opened");
            //     exit(-1);
            // }
            // }
            logger_manager_ptr->debug("Sent in :      " + String((esp_timer_get_time() - timer_write)/1000));
        }          
        else
        {
            vTaskDelay(1);
        }
    }
}

bool SD_Manager::create_tasks()
{
    try
    {
        if (this->is_task_created == false)
        {
            this->is_task_created = true;
            xTaskCreatePinnedToCore([](void *z)
                                    { static_cast<SD_Manager *>(z)->sd_writer(z); },
                                    "Receive Data Buffer", 8192, this, 1, &receive_data_buffer_task_handle, 0);
            return true;
        }
        return false;
    }
    catch (const std::exception &e)
    {
        if (this->logger_manager_ptr != NULL)
            this->logger_manager_ptr->error("[Collector_Manager] Collector Task not created \n[Collector_Manager] " + String(e.what()));
        return false;
    }
    return false;
}

String SD_Manager::get_oldest_file(const char *dirname)
{
    SdFile_ dir;
    SdFile_ file;

    if (!dir.open(dirname))
    {
        Serial.println("Failed to open directory");
        return "";
    }

    dir.rewind();
    SdFile_ oldestFile;
    uint32_t oldestTime = UINT32_MAX;

    while (file.openNext(&dir, O_READ))
    {
        dir_t dirEntry;

        file.dirEntry(&dirEntry);

        uint32_t fileTime = (uint32_t)dirEntry.creationDate << 16 | dirEntry.creationTime;

        if (fileTime < oldestTime)
        {
            char filename[50];
            file.getName(filename, sizeof(filename));
            String filenameStr(filename);
            if (filenameStr.indexOf("not") == -1 && filenameStr.indexOf("System") == -1)
            {
                oldestTime = fileTime;
                oldestFile = file;
            }
        }

        file.close();
    }

    if (oldestFile.isOpen())
    {
        char filename[50];
        oldestFile.getName(filename, sizeof(filename));
        String filenameStr(filename);
        oldestFile.close();
        if (filenameStr.indexOf("not") == -1 && filenameStr.indexOf("System") == -1)
        {
            return String("/") + String(filename);
        }
        else
        {
            return "";
        }
    }
    else
    {
        return "";
    }
}

void SD_Manager::erase_all_files()
{

    SdFile_ dir;
    if (!dir.open("/"))
    {
        Serial.println("Failed to open directory");
        return;
    }

    dir.rewind();
    SdFile_ file;

    while (file.openNext(&dir, O_READ))
    {
        dir_t dirEntry;
        file.dirEntry(&dirEntry);

        char filename[50];
        file.getName(filename, sizeof(filename));
        String filenameStr(filename);

        if (filenameStr.indexOf("not") == -1 && filenameStr.indexOf("System") == -1)
        {
            if (!SDfat.remove(filename))
            {
                Serial.println("Failed to delete file: " + filenameStr);
            }
        }

        file.close();
    }
}

bool SD_Manager::delete_file(String filename)
{
    if (SDfat.remove(filename.c_str()))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool SD_Manager::connect_sd()
{
    if (this->SDfat.begin(4, SD_SCK_MHZ(26)))
    {
        this->logger_manager_ptr->debug("[SD_Manager.cpp] SD card initialized");
        return true;
    }
    else
    {

        this->logger_manager_ptr->critical("[SD_Manager.cpp] SD card not initialized");
        exit(-1);
    }
    return false;
}
