#include <stdio.h>
#include "esp_intr_alloc.h"
#include "esp_attr.h"
#include "driver/timer.h"
#include "driver/gpio.h"

#define LED1 2
#define TIMER_DIVIDER 80
#define TIMER_SCALE (TIMER_BASE_CLK/ TIMER_DIVIDE)

volatile uint8_t led_status = 0;

static intr_handle_t timer_handle;

static void timer_proc_isr(void* arg){
    TIMERG0.int_clr_timers.t0 =1;
    TIMERG0.hw_timer[0].config.alarm_en = 1;
    led_status = 1 - led_status;
    gpio_set_level(LED1, led_status);
    //printf("hello\n");
}

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
    init_timer(1000000);   
}