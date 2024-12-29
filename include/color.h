#ifndef JADE_COLOR
#define JADE_COLOR

typedef struct _JColor{
    unsigned short red;
    unsigned short green;
    unsigned short blue;
} JColor;

typedef void (* ColorTransformFunction)(const float * colorFactor, JColor * color);

extern JColor colorArray[255];

int initColor();

void lerpColor(const JColor * colorFrom, const JColor * colorTo, JColor * lerpedColor, const float * lerpFactor);
void updateColorIndex();
void updateColor();
void transitionToColorTransformFunction(ColorTransformFunction newTransformFunction, unsigned int transitionTimeMS);

const JColor * determineColorAtIndex(const float * colorFactor);

#endif