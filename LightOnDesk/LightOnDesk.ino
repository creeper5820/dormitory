#include "FastLED.h"

#define NUM_LEDS 30 // LED灯珠数量
#define LED_DT D3 // Arduino输出控制信号引脚
#define LED_TYPE WS2812 // LED灯带型号
#define COLOR_ORDER GRB // RGB灯珠中红色、绿色、蓝色LED的排列顺序

CRGB leds[NUM_LEDS]; // 建立光带leds

void setup()
{
    LEDS.addLeds<LED_TYPE, LED_DT, COLOR_ORDER>(leds, NUM_LEDS);

    pinMode(BUILTIN_LED, OUTPUT);
    digitalWrite(BUILTIN_LED, HIGH);
}

void loop()
{
    FastLED.setBrightness(255);
    fill_solid(leds, 30, CRGB(255, 198, 107));
    FastLED.show();
}
