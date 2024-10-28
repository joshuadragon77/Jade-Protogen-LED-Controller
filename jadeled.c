#include "macros.h"

#include <fcntl.h>
#include <unistd.h>
#include <pigpio.h>

#include "animator.h"
#include "color.h"
#include "ledmatrix.h"
#include "wsled.h"
#include "prerender.h"
#include "transition.h"

JadeTimeTrack faceMovementTimeTrack;
JadeTimeTrack eyeBlinkTimeTrack;
JadeTimeTrack boopTimeTrack;

void renderToBuffer(Image * offscreenImage){

    PixelPacket backgroundColor = {0, 0, 0};

    double timeFactor = 0;
    obtainTimeTrackFactor(&faceMovementTimeTrack, &timeFactor);

    float xOffset = cosf(timeFactor * 2 * M_PI);
    float yOffset = sinf(timeFactor * 2 * M_PI);
    
    FaceImageCollection faceImageReturns = obtainFaceImages();

    CompositeImage(offscreenImage, OverCompositeOp, faceImageReturns.eyeImage, 425 + 5 * xOffset + faceImageReturns.eyeOffset.xOffset, 5 + 5 * yOffset + faceImageReturns.eyeOffset.yOffset);
    CompositeImage(offscreenImage, OverCompositeOp, faceImageReturns.mouthImage, 25 + 15 * xOffset + faceImageReturns.mouthOffset.xOffset, 196 + 15 * yOffset + faceImageReturns.mouthOffset.yOffset);
    CompositeImage(offscreenImage, OverCompositeOp, faceImageReturns.noseImage, 28 + 5 * xOffset + faceImageReturns.noseOffset.xOffset, 25 + 5 * yOffset + faceImageReturns.noseOffset.yOffset);
}



#define LOAD_MODULE(FUNCTION,...)\
printf(SYSTEM_MESSAGE(Init Module: FUNCTION));\
startStopwatch();\
FUNCTION(__VA_ARGS__);\
printf(SYSTEM_MESSAGE_WITH_PARAM(Fin Module in %lu µs: FUNCTION, printStopwatchUS()));

int main(int argc, char **argv) {

    // LOAD_MODULE(gpioInitialise);


    // gpioSetMode(25, PI_INPUT);

    LOAD_MODULE(initWS2812BDriver);
    LOAD_MODULE(InitializeMagick, NULL);
    LOAD_MODULE(initTransition);
    LOAD_MODULE(initLedMatrix, argc, argv);
    LOAD_MODULE(initPreRender);
    LOAD_MODULE(initAnimator);
    LOAD_MODULE(initColor);

    initializeTimeTrack(&faceMovementTimeTrack);
    initializeTimeTrack(&eyeBlinkTimeTrack);
    initializeTimeTrack(&boopTimeTrack);
    faceMovementTimeTrack.animationDurationUS = SECONDS_TO_US(5);
    faceMovementTimeTrack.loopMode = Loop;

    eyeBlinkTimeTrack.loopMode = LoopDelay;
    eyeBlinkTimeTrack.animationDurationUS = MILLISECONDS_TO_US(750);
    eyeBlinkTimeTrack.cooldownDurationUS = SECONDS_TO_US(2);

    boopTimeTrack.animationDurationUS = SECONDS_TO_US(4);

    playTimeTrack(&faceMovementTimeTrack);
    playTimeTrack(&eyeBlinkTimeTrack);


    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);

    while (true){
        
        char command[1];
        char emptyString[1];

        read(0, command, sizeof(command));

        if (command[0] == 'e'){
            printf("Protogen Out! o7\n");
            clearLEDStrips();
            clearMatrix();
            return 0;
        }
        if (command[0] == 'b'){
            attemptTransition(&blinkFace, 500);
        }
        if (command[0] == 'n'){
            attemptTransition(&normalFace, 500);
        }
        if (command[0] == 'h'){
            attemptTransition(&approvalFace, 500);
        }
        if (command[0] == 'a'){
            attemptTransition(&angryFace, 500);
        }
        if (command[0] == 'p'){
            attemptTransition(&prideFace, 500);
        }
        updateColor();
        requestRender();
        usleep(5000);
        renderLEDStrips();
        // clearLEDStrips();
        usleep(5000);

    }

    gpioTerminate();
    
}
