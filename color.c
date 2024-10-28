#include "macros.h"

#include <stdio.h>

#include "animator.h"
#include "color.h"
#include "transition.h"

JColor colorArray[255];

JadeTimeTrack timeTrack;
JadeTimeTrack colorTransitionTimeTrack;

const JColor * determineColorAtIndex(const float * colorFactor){

    unsigned char selectedIndex = 255 * *colorFactor;

    return &colorArray[selectedIndex];
}

void lerpColor(const JColor * colorFrom, const JColor * colorTo, JColor * lerpedColor, const float * lerpFactor){
    lerpedColor->red = (unsigned short)(colorFrom->red + (colorTo->red - colorFrom->red) * *lerpFactor);
    lerpedColor->green = (unsigned short)(colorFrom->green + (colorTo->green - colorFrom->green) * *lerpFactor);
    lerpedColor->blue = (unsigned short)(colorFrom->blue + (colorTo->blue - colorFrom->blue) * *lerpFactor);
}

void updateColorIndex(JColor * colorArray, void colorTransform(const float * colorFactor, JColor * color)){
    double animationFactor;
    obtainTimeTrackFactor(&timeTrack, &animationFactor);
    for (int i = 0;i<255;i++){
        float remainder = 1.0f;
        float specificAnimationFactor = sinf(M_PI * fmodf((float)i / 255 + animationFactor, remainder));

        colorTransform(&specificAnimationFactor, &colorArray[i]);
    }
}

ColorTransformFunction currentColorTransformFunction = obtainJadenarium;
ColorTransformFunction previousColorTransformFunction = obtainJadenarium;

void updateColor(){
    JColor fromColorArray[255];
    JColor toColorArray[255];

    updateColorIndex(fromColorArray, previousColorTransformFunction);
    updateColorIndex(toColorArray, currentColorTransformFunction);

    double animationFactor;
    obtainTimeTrackFactor(&colorTransitionTimeTrack, (double *)&animationFactor);

    float animationFactor2 = (float)animationFactor;

    for (int i = 0;i<255;i++){

        lerpColor(&fromColorArray[i], &toColorArray[i], &colorArray[i], &animationFactor2);
    }
}

void transitionToColorTransformFunction(ColorTransformFunction newTransformFunction, unsigned int transitionTimeMS){
    previousColorTransformFunction = currentColorTransformFunction;
    currentColorTransformFunction = newTransformFunction;

    colorTransitionTimeTrack.animationDurationUS = MILLISECONDS_TO_US(transitionTimeMS);
    playTimeTrack(&colorTransitionTimeTrack);
}

int initColor(){
    initializeTimeTrack(&timeTrack);
    initializeTimeTrack(&colorTransitionTimeTrack);

    timeTrack.animationDurationUS = SECONDS_TO_US(2);
    timeTrack.loopMode = Loop;

    colorTransitionTimeTrack.animationDurationUS = SECONDS_TO_US(1);
    colorTransitionTimeTrack.cooldownMode = RestOnEnd;

    playTimeTrack(&timeTrack);

    for (int i = 0;i<255;i++){
        colorArray[i].red = 0;
        colorArray[i].green = 0;
        colorArray[i].blue = 0;
    }

    return 0;
}



