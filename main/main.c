//#define NUM_LEDS 150

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <stdio.h>
#include "ws2812_control.h"
#include "ledstrip_effect.h"

rgbVal *pixels;

void random_effects(void *pvParameters){
   int count = 0;
  uint8_t red = 0xff;
  uint8_t green = 0xff;
  uint8_t blue = 0xff;
  while (1) {
    if (count == 0) {
      red = 0xff;
      green = 0x00;
      blue = 0x00;
      count = 1;
    } else if (count == 1)
    {
      red = 0x00;
      green = 0xff;
      blue = 0x00;
      count = 2;
    } else if (count == 2) {
      red = 0x00;
      green = 0x00;
      blue = 0xff;
      count = 3;
    } else {
      red = range_random(0xff);
      blue = range_random(0xff);
      count = range_random(0xff);
    }
    cylonBounce(pixels, NUM_LEDS, red, green, blue, 4, 10, 50);
    colorWipe(pixels, NUM_LEDS, red, green, blue, 50);
    colorWipe(pixels, NUM_LEDS, 0x00, 0x00, 0x00, 50);
    meteorRain(pixels, NUM_LEDS, red, green, blue, 5, 64, true, 30);
    meteorRain(pixels, NUM_LEDS, red, green, blue, 2, 64, true, 10);
    meteorRain(pixels, NUM_LEDS, red, green, blue, 15, 64, true, 10);
    meteorRain(pixels, NUM_LEDS, red, green, blue, 2, 64, true, 20);
    meteorRain(pixels, NUM_LEDS, red, green, blue, 2, 64, true, 5);
    rainbowCycle(pixels, NUM_LEDS, 20);
    RGBLoop(pixels, NUM_LEDS, 5);
  }
}

void app_main()
{
  nvs_flash_init();
  ws2812_control_init();
  pixels = malloc(sizeof(rgbVal) * NUM_LEDS);
  xTaskCreate(random_effects, "ws2812 random_effects", 4096, NULL, 10, NULL);
  return;
}
