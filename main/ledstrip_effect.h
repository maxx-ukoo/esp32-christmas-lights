#ifndef LEDSTRIP_EFFECTS_H
#define LEDSTRIP_EFFECTS_H

#include <stdint.h>
#include <stdbool.h>
#include "ws2812_control.h"

void setAll(rgbVal *pixels, int pixel, uint8_t red, uint8_t green, uint8_t blue);

void colorWipe(rgbVal *pixels, int pixels_count, uint8_t red, uint8_t green, uint8_t blue, int speed_delay);
void meteorRain(rgbVal *pixels, int pixels_count, uint8_t red, uint8_t green, uint8_t blue, uint8_t meteorSize, uint8_t meteorTrailDecay, bool meteorRandomDecay, uint8_t SpeedDelay);
void cylonBounce(rgbVal *pixels, int pixels_count, uint8_t red, uint8_t green, uint8_t blue, int EyeSize, int SpeedDelay, int ReturnDelay);
void rainbowCycle(rgbVal *pixels, int pixels_count, int SpeedDelay);
void RGBLoop(rgbVal *pixels, int pixels_count, int count);

void setPixel(rgbVal *pixels, int pixel, uint8_t red, uint8_t green, uint8_t blue);
rgbVal get_pixel_color(rgbVal *pixels, int pixel);
uint32_t range_random(uint32_t max);


#endif /* LEDSTRIP_EFFECTS_H */