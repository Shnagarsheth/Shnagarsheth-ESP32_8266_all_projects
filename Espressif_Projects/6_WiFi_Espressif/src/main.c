#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
//#include <NTPClient_Generic.h>
//#include <Lctr_Lcd.h>
#include <Arduino.h>
#include <ESPAsync_WiFiManager.h>


//Use only Single-core
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

//Pins
#define BLINK_GPIO 2

//Task 1: LED Blink
void toggleLED1 (void *parameter){

    while(1){
        
        /* Blink off (output low) */
        printf("Task1: Turning off the LED\n");
        gpio_set_level(BLINK_GPIO, 0);
        //Non-blocking delay function - Tells scheduler to keep other task running wihtout blocking them
        vTaskDelay(500 / portTICK_PERIOD_MS); 
        
        /* Blink on (output high) */
        printf("Task1: Turning on the LED\n");
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

//Task 2: Different timing for LED_blinking
void toggleLED2 (void *parameter){

    while(1){
        
        /* Blink off (output low) */
        printf("Task2: Turning off the LED\n");
        gpio_set_level(BLINK_GPIO, 0);
        //Non-blocking delay function - Tells scheduler to keep other task running wihtout blocking them
        vTaskDelay(323 / portTICK_PERIOD_MS); 
        
        /* Blink on (output high) */
        printf("Task2: Turning on the LED\n");
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(323 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    gpio_reset_pin(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    //Create a task
    //xTaskCreatePinnedToCore(toggleLED,"Toggle LED", 1024, NULL, 1, NULL, app_cpu);
    xTaskCreatePinnedToCore( //Use xTaskCreate() for Vanilla FreeRTOS
        toggleLED1,     //Function to called as a task
        "Toggle LED1",  //Name of the task
        1024,          // Stack size (bytes in ESP32, words in FreeRTOS)
        NULL,          // Parameter to pass to function
        1,             // Task priority (0 to configMAX_PRIORITIES - 1)
        NULL,          // Task handle
        app_cpu);      // Run on one core for demo purposes (ESP32 only)

    xTaskCreatePinnedToCore(toggleLED2, "Toggle LED2", 1024, NULL, 0, NULL, app_cpu);
    //if it was vanila freeRTOS, we'd to call the below function after defining all the tasks.
    //vTaskStartScheduler(); 
}