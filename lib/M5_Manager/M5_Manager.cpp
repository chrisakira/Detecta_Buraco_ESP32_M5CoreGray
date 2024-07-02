#include "M5_Manager.h"


bool M5_Manager::check_LCD()
{
    if (this->lcd_initialized == false)
    {
        if (this->logger_manager_ptr != NULL)
            this->logger_manager_ptr->info("[M5_Manager] LCD Not initialized");
        return false;
    }
    return true;
}

bool M5_Manager::initialize_LCD()
{
    if (this->check_LCD() == false)
        return false;
    M5.Lcd.begin();
    M5.Lcd.setRotation(3);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(0, 0);
    return true;
}

bool M5_Manager::reset_LCD()
{
    if (this->check_LCD() == false)
        return false;
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(0, 0);
    return true;
}

void M5_Manager::update_mpu_battery_level(void *z)
{
    for (;;)
    {
        M5.Lcd.setCursor(0, 0);
        M5.Lcd.println("Battery: " + String(M5.Power.getBatteryLevel()) + "%");
        vTaskDelay(1000);
    }
}

void M5_Manager::update_mpu_data(void *z)
{
    this->logger_manager_ptr->info("[M5_Manager] Update MPU Data Task started");
    M5.IMU.Init();
    TickType_t lastWakeTime = xTaskGetTickCount();
    const TickType_t interval = pdMS_TO_TICKS(1);
    float gyro_X_;
    float gyro_Y_;
    float gyro_Z_;  
    float accel_X_;
    float accel_Y_;
    float accel_Z_;
    float pitch_;
    float roll_;
    float yaw_;
    
    this->logger_manager_ptr->info("[M5_Manager] Starting MPU Data Update Task");
    for (;;)
    {
        M5.IMU.getGyroData(&gyro_X_, &gyro_Y_, &gyro_Z_);
        M5.IMU.getAccelData(&accel_X_, &accel_Y_,  &accel_Z_); 
        M5.IMU.getAhrsData(&pitch_ , &roll_ ,&yaw_ );
        this->gyro_X = gyro_X_;
        this->gyro_Y = gyro_Y_;
        this->gyro_Z = gyro_Z_;
        this->accel_X = accel_X_;
        this->accel_Y = accel_Y_;
        this->accel_Z = accel_Z_;
        this->pitch = pitch_;
        this->roll = roll_;
        this->yaw = yaw_;
        this->logger_manager_ptr->info("[M5_Manager] Data: pitch: " + String(this->pitch) + " roll: " + String(this->roll) + " yaw: " + String(this->yaw));
        vTaskDelayUntil(&lastWakeTime, interval);
    }
}

void M5_Manager::M5_begin(bool LCDEnable, bool SDEnable, bool SerialEnable, bool I2CEnable)
{
    M5.begin(LCDEnable, SDEnable, SerialEnable, I2CEnable);
    M5.Power.begin();
    M5.Lcd.println("Battery: " + String(M5.Power.getBatteryLevel()) + "%");
    if (this->logger_manager_ptr != NULL)
        this->logger_manager_ptr->info("[M5_Manager] M5Core2 initialized");

    if (LCDEnable == true)
    {
        this->initialize_LCD();
        if (this->logger_manager_ptr != NULL)
            this->logger_manager_ptr->info("[M5_Manager] LCD initialized");
    }
}

bool M5_Manager::create_tasks()
{
    try
    {
        if (this->is_task_created == false)
        {
            this->is_task_created = true;
            xTaskCreatePinnedToCore([](void *z)
                                    { static_cast<M5_Manager *>(z)->update_mpu_data(z); },
                                    "Update the MPU data", 2048, this, 0, &this->update_mpu_data_task_handle, 1);
            xTaskCreatePinnedToCore([](void *z)
                                    { static_cast<M5_Manager *>(z)->update_mpu_battery_level(z); },
                                    "Update the Battery level", 2048, this, 0, &this->update_battery_level_task_handle, 1);
            if (this->logger_manager_ptr != NULL)
                this->logger_manager_ptr->info("[M5_Manager] Update MPU Task created");
            return true;
        }
        return false;
    }
    catch (const std::exception &e)
    {
        if (this->logger_manager_ptr != NULL)
            this->logger_manager_ptr->error("[M5_Manager] MPU Updater Task not created \n[M5_Manager] " + String(e.what()));
        return false;
    }
    return false;
}

const char *M5_Manager::get_current_time()
{
    time_t now = time(0);
    struct tm *timeinfo = localtime(&now);
    static char formatted_time[20];
    sprintf(formatted_time, "%02d-%02d-%04d-%02d-%02d-%02d", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    return formatted_time;
}

bool M5_Manager::connect_wifi()
{
    WiFi.begin(this->ssid.c_str(), this->password.c_str());
    M5.Lcd.print("Connecting to WiFi");
    if (this->logger_manager_ptr != NULL)
        this->logger_manager_ptr->info("[M5_Manager] Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        vTaskDelay(2000);
        M5.Lcd.print(".");
        if (this->logger_manager_ptr != NULL)
            this->logger_manager_ptr->info("[M5_Manager] Connecting to WiFi...");
    }
    M5.Lcd.println("Connected to WiFi");
    if (this->logger_manager_ptr != NULL)
        this->logger_manager_ptr->info("[M5_Manager] Connected to WiFi");

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    return true;
}

void M5_Manager::update_unix_time()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {   
        M5.Lcd.println("Failed to obtain time");
        M5.Lcd.println("Rebooting in 5s");
        if (this->logger_manager_ptr != NULL)
            this->logger_manager_ptr->error("[M5_Manager] Failed to obtain time");
        vTaskDelay(5000);
        exit(-1);
    }
    this->now = mktime(&timeinfo);
    if (this->logger_manager_ptr != NULL){
        this->logger_manager_ptr->info("[M5_Manager] Unix Time: " + String(now));
         
    }
    M5.Lcd.println("Unix Time: " + String(now));
    micros_now = esp_timer_get_time();
}
