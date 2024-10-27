typedef struct _JColor{
    unsigned short red;
    unsigned short green;
    unsigned short blue;
} JColor;

extern JColor colorArray[255];

int initColor();

void updateColorIndex();

const JColor * determineColorAtIndex(const float * colorFactor);