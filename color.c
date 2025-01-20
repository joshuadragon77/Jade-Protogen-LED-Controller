#include "macros.h"

#include <stdio.h>

#include "animator.h"
#include "color.h"
#include "transition.h"

JColor colorArray[255];

JadeTimeTrack timeTrack;
JadeTimeTrack colorTransitionTimeTrack;
JadeTimeTrack introTimeTrack;

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
        // float specificAnimationFactor = fmodf((float)i / 255 + animationFactor, remainder);
        float specificAnimationFactor = (sinf(2 * M_PI * (i / 255.0 + animationFactor)) + 1 ) / 2;

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
    obtainTimeTrackFactor(&colorTransitionTimeTrack, &animationFactor);

    double animationFactor3;
    obtainTimeTrackFactor(&introTimeTrack, &animationFactor3);

    animationFactor3 = 1 - pow(animationFactor3 - 1, 2);

    animationFactor3 *= 1.25;

    float animationFactor2 = (float)animationFactor;

    JColor black = {0, 0, 0};

    for (int i = 0;i<255;i++){

        float animationFactor5 = fclamp((animationFactor3 * 255.0 - (255 - i) ) / 50.0, 0, 1);

        lerpColor(&fromColorArray[i], &toColorArray[i], &colorArray[i], &animationFactor2);
        lerpColor(&black, &colorArray[i], &colorArray[i], &animationFactor5);
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
    initializeTimeTrack(&introTimeTrack);

    timeTrack.animationDurationUS = SECONDS_TO_US(3);
    timeTrack.loopMode = Loop;

    colorTransitionTimeTrack.animationDurationUS = SECONDS_TO_US(1);
    colorTransitionTimeTrack.cooldownMode = RestOnEnd;

    introTimeTrack.animationDurationUS = SECONDS_TO_US(4);
    introTimeTrack.cooldownMode = RestOnEnd;
    introTimeTrack.loopMode = NoLoop;

    playTimeTrack(&timeTrack);
    playTimeTrack(&introTimeTrack);

    for (int i = 0;i<255;i++){
        colorArray[i].red = 0;
        colorArray[i].green = 0;
        colorArray[i].blue = 0;
    }

    return 0;
}



