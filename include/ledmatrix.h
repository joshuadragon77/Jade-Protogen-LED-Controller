#include <GraphicsMagick/magick/api.h>
#include <string.h>

#include "led-matrix-c.h"

#define NO_FLIP 0
#define VERTICAL_FLIP 1
#define HORIZONTAL_FLIP 2


void requestRender();
void renderToBuffer(Image * offscreenImage);
int initLedMatrix(int argc, char ** argv);
void clearMatrix();

extern char renderBuffer[41];
extern uint8_t textMode;