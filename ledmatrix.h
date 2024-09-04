#include "led-matrix-c.h"
#include <GraphicsMagick/magick/api.h>
#include <string.h>

#define NO_FLIP 0
#define VERTICAL_FLIP 1
#define HORIZONTAL_FLIP 2


void requestRender();
void renderToBuffer(Image * offscreenImage);
int initLedMatrix(int argc, char ** argv);
void clearMatrix();