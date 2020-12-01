#include "ledstrip_effect.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define delay_ms(ms) vTaskDelay((ms) / portTICK_RATE_MS)

void setAll(rgbVal *pixels, int pixels_count, uint8_t red, uint8_t green, uint8_t blue) {
  for(int i = 0; i < pixels_count; i++ ) {
    setPixel(pixels, i, red, green, blue);
  }
  ws2812_write_leds(pixels);
}

void fadeToBlack(rgbVal *pixels, int ledNo, uint8_t fadeValue) {
    // NeoPixel
    uint8_t r, g, b;
    rgbVal old_color = get_pixel_color(pixels, ledNo);
    r = old_color.r;
    g = old_color.g;
    b = old_color.b;

    r=(r<=10)? 0 : (int) r-(r*fadeValue/256);
    g=(g<=10)? 0 : (int) g-(g*fadeValue/256);
    b=(b<=10)? 0 : (int) b-(b*fadeValue/256);
   
    setPixel(pixels, ledNo, r,g,b);
}

rgbVal get_pixel_color(rgbVal *pixels, int pixel) {
  return makeRGBVal(pixels[pixel].r, pixels[pixel].g, pixels[pixel].b);
}

void setPixel(rgbVal *pixels, int pixel, uint8_t red, uint8_t green, uint8_t blue) {
    pixels[pixel].r = red;
    pixels[pixel].g = green;
    pixels[pixel].b = blue;
}

void colorWipe(rgbVal *pixels, int pixels_count, uint8_t red, uint8_t green, uint8_t blue, int speed_delay) {
  for(uint16_t i=0; i<pixels_count; i++) {
      setPixel(pixels, i, red, green, blue);
      ws2812_write_leds(pixels);
      delay_ms(speed_delay);
  }
}

uint32_t range_random(uint32_t max) {
   uint32_t random = esp_random();
   float result = (float) max * random / UINT32_MAX;
   return (uint32_t)result;
}

void meteorRain(rgbVal *pixels, int pixels_count, uint8_t red, uint8_t green, uint8_t blue, uint8_t meteorSize, uint8_t meteorTrailDecay, bool meteorRandomDecay, uint8_t SpeedDelay) {
      setAll(pixels, pixels_count, 0,0,0);
 
      for(int i = 0; i < pixels_count + pixels_count; i++) {
      
        // fade brightness all LEDs one step
        for(int j=0; j<pixels_count; j++) {
          if( (!meteorRandomDecay) || (range_random(10)>5) ) {
            fadeToBlack(pixels, j, meteorTrailDecay );        
          }
        }
        ws2812_write_leds(pixels);
      
        // draw meteor
        for(int j = 0; j < meteorSize; j++) {
          if( ( i-j <pixels_count) && (i-j>=0) ) {
            setPixel(pixels, i-j, red, green, blue);
          }
        }
      
        ws2812_write_leds(pixels);
        delay_ms(SpeedDelay);
      }
}

void cylonBounce(rgbVal *pixels, int pixels_count, uint8_t red, uint8_t green, uint8_t blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
    setAll(pixels, pixels_count, 0,0,0);
      for(int i = 0; i < pixels_count-EyeSize-2; i++) {
        setAll(pixels, pixels_count, 0, 0, 0);
        setPixel(pixels, i, red/10, green/10, blue/10);
        for(int j = 1; j <= EyeSize; j++) {
          setPixel(pixels, i+j, red, green, blue);
        }
        setPixel(pixels, i+EyeSize+1, red/10, green/10, blue/10);
        ws2812_write_leds(pixels);
        delay_ms(SpeedDelay);
      }

      delay_ms(ReturnDelay);

      for(int i = pixels_count-EyeSize-2; i > 0; i--) {
        setAll(pixels, pixels_count, 0, 0, 0);
        setPixel(pixels, i, red/10, green/10, blue/10);
        for(int j = 1; j <= EyeSize; j++) {
          setPixel(pixels, i+j, red, green, blue);
        }
        setPixel(pixels, i+EyeSize+1, red/10, green/10, blue/10);
        ws2812_write_leds(pixels);
        delay_ms(SpeedDelay);
      }

      delay_ms(ReturnDelay);
}

uint8_t* Wheel(uint8_t WheelPos) {
  static uint8_t c[3];
 
  if(WheelPos < 85) {
   c[0]=WheelPos * 3;
   c[1]=255 - WheelPos * 3;
   c[2]=0;
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   c[0]=255 - WheelPos * 3;
   c[1]=0;
   c[2]=WheelPos * 3;
  } else {
   WheelPos -= 170;
   c[0]=0;
   c[1]=WheelPos * 3;
   c[2]=255 - WheelPos * 3;
  }

  return c;
}

void rainbowCycle(rgbVal *pixels, int pixels_count, int SpeedDelay) {
  uint8_t *c;
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< pixels_count; i++) {
      c=Wheel(((i * 256 / pixels_count) + j) & 255);
      setPixel(pixels, i, *c, *(c+1), *(c+2));
    }
    ws2812_write_leds(pixels);
    delay_ms(SpeedDelay);
  }
}

void RGBLoop(rgbVal *pixels, int pixels_count, int count) {
  setAll(pixels, pixels_count, 0,0,0);
  for (int n = 0; n<count; n++) {
    for(int j = 0; j < 3; j++ ) {
      // Fade IN
      for(int k = 0; k < 256; k++) {
        switch(j) {
          case 0: setAll(pixels, pixels_count, k, 0, 0); break;
          case 1: setAll(pixels, pixels_count, 0, k, 0); break;
          case 2: setAll(pixels, pixels_count, 0, 0, k); break;
        }
        ws2812_write_leds(pixels);
        delay_ms(3);
      }
      // Fade OUT
      for(int k = 255; k >= 0; k--) {
        switch(j) {
          case 0: setAll(pixels, pixels_count, k, 0, 0); break;
          case 1: setAll(pixels, pixels_count, 0, k, 0); break;
          case 2: setAll(pixels, pixels_count, 0, 0, k); break;
        }
        ws2812_write_leds(pixels);
        delay_ms(3);
      }
    }
  }
}

