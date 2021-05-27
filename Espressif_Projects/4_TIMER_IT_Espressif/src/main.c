//Without FreeRTOS of ESP32

#include <stdio.h>
#include "esp_intr_alloc.h"
#include "esp_attr.h"
#include "driver/timer.h"
#include "driver/gpio.h"

#define LED1 2
#define TIMER_DIVIDER 80 //Prescaler (E.g. 80MHz / 80 = 1MHz)
#define TIMER_SCALE (TIMER_BASE_CLK/ TIMER_DIVIDE)

volatile int t = 1000; //Setting the time for ON and OFF 1000 = 1ms ON time and 1ms OFF time (500 Hz)
volatile uint8_t led_status = 0;

static intr_handle_t timer_handle;

/*Iterrupt Service Routine for TIMER0*/
static void timer_proc_isr(void* arg){
    TIMERG0.int_clr_timers.t0 =1;
    TIMERG0.hw_timer[0].config.alarm_en = 1;
    led_status = 1 - led_status;
    gpio_set_level(LED1, led_status);
    gpio_set_level(5, led_status);
}

/*Initializing TIMER0*/
void init_timer(int TIMER_RELOAD_US){

timer_config_t config = {
    .divider = TIMER_DIVIDER,
    .counter_dir = TIMER_COUNT_UP,
    .counter_en = false,
    .alarm_en = true,
    .auto_reload = true,
    .intr_type= TIMER_INTR_LEVEL,
};
    timer_init(TIMER_GROUP_0,TIMER_0, &config);
    timer_set_counter_value(TIMER_GROUP_0,TIMER_0,0);
    timer_set_alarm_value(TIMER_GROUP_0,TIMER_0, TIMER_RELOAD_US);
    timer_enable_intr(TIMER_GROUP_0,TIMER_0);
    timer_isr_register(TIMER_GROUP_0,TIMER_0, &timer_proc_isr, NULL, 0, &timer_handle);
    timer_start(TIMER_GROUP_0,TIMER_0);
}

void app_main() {    
    gpio_pad_select_gpio(LED1); 
    gpio_set_direction(LED1, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(5);
    gpio_set_direction(5, GPIO_MODE_OUTPUT);
    init_timer(t); //E.g.: t = 1000000 = 1 sec
    while (true)
    {
    }       
}