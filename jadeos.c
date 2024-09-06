// This version pushes all the heavy rendering onto GraphicsMagick to see whether if it can handle the rendering. Turns out yes but on the PI no. It uses 60% of the CPU and brings the system temperature to 80C. Not efficient.

#include "macros.h"

#include <fcntl.h>
#include <unistd.h>

#include "color.h"
#include "wsled.h"
#include "consolescript.h"
#include "prerender.h"
#include "animator.h"
#include "ledmatrix.h"

JadeTimeTrack faceMovementTimeTrack;
JadeTimeTrack eyeBlinkTimeTrack;
JadeTimeTrack boopTimeTrack;

unsigned int boop = 0;

void renderToBuffer(Image * offscreenImage){

    if (boop){
        eyeBlinkTimeTrack.loopMode = NoLoop;

        if (eyeBlinkTimeTrack.playMode == Stopped){
            playTimeTrack(&boopTimeTrack);
            boop = 0;
        }
    }
    if (eyeBlinkTimeTrack.playMode == Stopped && boopTimeTrack.playMode == Stopped){
        eyeBlinkTimeTrack.loopMode = LoopDelay;
        playTimeTrack(&eyeBlinkTimeTrack);
    }


    PixelPacket backgroundColor = {0, 0, 0};

    double timeFactor = 0;
    obtainTimeTrackFactor(&faceMovementTimeTrack, &timeFactor);

    float xOffset = cosf(timeFactor * 2 * M_PI);
    float yOffset = sinf(timeFactor * 2 * M_PI);

    obtainTimeTrackFactor(&eyeBlinkTimeTrack, &timeFactor);
    float eyeAnimationFactor = sinf(timeFactor * M_PI);

    obtainTimeTrackFactor(&boopTimeTrack, &timeFactor);
    float boopAnimationFactor = pow(sinf(timeFactor * M_PI), 0.5);

    if (boopTimeTrack.playMode == Stopped){
        unsigned int currentSequenceIndex = clamp(eyeSequence.numberOfImages * eyeAnimationFactor, 0, eyeSequence.numberOfImages - 1);

        CompositeImage(offscreenImage, OverCompositeOp, eyeSequence.images[currentSequenceIndex], 425 + 5 * xOffset, 5 + 5 * yOffset + eyeAnimationFactor * 90);
    }else{
        unsigned int currentSequenceIndex = clamp(boopEyeSequence.numberOfImages * boopAnimationFactor, 0, boopEyeSequence.numberOfImages - 1);

        CompositeImage(offscreenImage, OverCompositeOp, boopEyeSequence.images[currentSequenceIndex], 425 + 5 * xOffset, 5 + 5 * yOffset + eyeAnimationFactor * 90);
    }

    CompositeImage(offscreenImage, OverCompositeOp, defaultMawImage, 25 + 15 * xOffset, 196 + 15 * yOffset);
    CompositeImage(offscreenImage, OverCompositeOp, defaultNoseImage, 28 + 5 * xOffset, 25 + 5 * yOffset);
}


int main(int argc, char **argv) {

    // initAnimator();

    initWS2812BDriver();

    systemNormaLog("Initializing GraphicsMagick...");
    InitializeMagick(NULL);
    systemNormaLog("Initialized GraphicsMagick");
    if (initLedMatrix(argc, argv) || initPreRender() || initAnimator() || initColor()){
        return 1;
    }


    initializeTimeTrack(&faceMovementTimeTrack);
    initializeTimeTrack(&eyeBlinkTimeTrack);
    initializeTimeTrack(&boopTimeTrack);
    faceMovementTimeTrack.animationDurationUS = SECONDS_TO_US(4);
    faceMovementTimeTrack.loopMode = Loop;

    eyeBlinkTimeTrack.loopMode = LoopDelay;
    eyeBlinkTimeTrack.animationDurationUS = MILLISECONDS_TO_US(500);
    eyeBlinkTimeTrack.cooldownDurationUS = SECONDS_TO_US(1);

    boopTimeTrack.animationDurationUS = SECONDS_TO_US(4);

    playTimeTrack(&faceMovementTimeTrack);
    playTimeTrack(&eyeBlinkTimeTrack);


    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);

    while (true){
        
        char command[256];

        read(0, command, sizeof(command));

        if (strcmp("exit\n", command) == 0){
            printf("Protogen Out! o7\n");
            clearLEDStrips();
            clearMatrix();
            return 0;
        }
        if (strcmp("boop\n", command) == 0){
            printf("Boop.\n");
            boop = 1;
        }
        updateColorIndex();
        requestRender();
        usleep(20000);
        renderLEDStrips();
        // clearLEDStrips();
        usleep(20000);

    }

    
}