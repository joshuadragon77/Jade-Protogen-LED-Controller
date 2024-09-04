#include <ws2811/ws2811.h>

#include <pigpio.h>
#include <unistd.h>

void renderLEDStrips();
void clearLEDStrips();
int initWS2812BDriver();