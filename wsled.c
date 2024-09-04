#include "macros.h"

#include <unistd.h>
#include <stdio.h>

#include "color.h"
#include "wsled.h"

ws2811_t ledStrip = {
    .freq = WS2811_TARGET_FREQ ,
    .dmanum = 10,
    .channel = {
        [0] = {
            .gpionum = 10,
            .invert = 0,
            .count = 60,
            .strip_type = WS2811_STRIP_GRB,
            .brightness = 5
        }
    }
};

void renderLEDStrips(){
    for (int i = 0;i<60;i++){
        float colorFactor = (float)i / 60;

        const JColor * color = determineColorAtIndex(&colorFactor);

        unsigned int colorValue = color->red << 16 | color->green << 8 | color->blue;

        ledStrip.channel[0].leds[i] = colorValue;
    }

    ws2811_render(&ledStrip);
}

void clearLEDStrips(){
    for (int i = 0;i<60;i++){
        ledStrip.channel[0].leds[i] = 0;
    }

    ws2811_render(&ledStrip);
}

int initWS2812BDriver(){

    ws2811_init(&ledStrip);

    clearLEDStrips();

    return 0;
}
