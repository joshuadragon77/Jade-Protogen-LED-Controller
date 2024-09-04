#include "macros.h"

#include "color.h"

#include <stdio.h>

#include "animator.h"

JColor colorArray[255];
JadeTimeTrack timeTrack;

const JColor * determineColorAtIndex(const float * colorFactor){

    // printf("%u\n", index);

    unsigned char selectedIndex = 255 * *colorFactor;

    return &colorArray[selectedIndex];
}

void lerpColor(const JColor * colorFrom, const JColor * colorTo, JColor * lerpedColor, const float * lerpFactor){
    lerpedColor->red = (unsigned short)(colorFrom->red + (colorTo->red - colorFrom->red) * *lerpFactor);
    lerpedColor->green = (unsigned short)(colorFrom->green + (colorTo->green - colorFrom->green) * *lerpFactor);
    lerpedColor->blue = (unsigned short)(colorFrom->blue + (colorTo->blue - colorFrom->blue) * *lerpFactor);
}

JColor rainbowElement[] = {
    {255, 0, 0},
    {248, 161, 18},
    {233, 240, 18},
    {9, 240, 18},
    {9, 213, 245},
    {218, 20, 245},
};

void obtainRainbow(const float * colorFactor, JColor * color){
    unsigned int selectedIndex = 6 * *colorFactor;
    unsigned int nextIndex = selectedIndex + 1;
    if (nextIndex >= 6){
        nextIndex = 0;
    }

    JColor * from = &rainbowElement[selectedIndex];
    JColor * to = &rainbowElement[nextIndex];

    float lerpFactor = (*colorFactor - (float)selectedIndex / 6) / (1.0f / 6);

    lerpColor(from, to, color, &lerpFactor);
}

void updateColorIndex(){
    float animationFactor;
    JColor from = {0, 170, 255};
    JColor to = {10, 10, 29};
    obtainTimeTrackFactor(&timeTrack, &animationFactor);
    for (int i = 0;i<255;i++){
        float remainder = 1.0f;
        float specificAnimationFactor = sinf(M_PI * fmodf((float)i / 255 + animationFactor, remainder));

        obtainRainbow(&specificAnimationFactor, &colorArray[i]);
        // lerpColor(&from, &to, &colorArray[i], &specificAnimationFactor);
    }
}

int initColor(){
    initializeJadeTimeTrack(&timeTrack);
    timeTrack.animationDurationMS = 4000;

    for (int i = 0;i<255;i++){
        colorArray[i].red = 0;
        colorArray[i].green = 0;
        colorArray[i].blue = 0;
    }

    return 0;
}


