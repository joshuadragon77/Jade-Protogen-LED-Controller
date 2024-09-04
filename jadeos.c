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

void renderToBuffer(Image * offscreenImage){


    PixelPacket backgroundColor = {0, 0, 0};

    float timeFactor = 0;
    obtainTimeTrackFactor(&faceMovementTimeTrack, &timeFactor);

    float xOffset = cosf(timeFactor * 2 * M_PI);
    float yOffset = sinf(timeFactor * 2 * M_PI);

    obtainTimeTrackFactor(&eyeBlinkTimeTrack, &timeFactor);
    float eyeAnimationFactor2 = sinf(timeFactor * M_PI / 0.2f);

    if (timeFactor >= 0.2f){
        eyeAnimationFactor2 = 0.0f;
    }

    unsigned int currentSequenceIndex = clamp(eyeSequence.numberOfImages * eyeAnimationFactor2, 0, eyeSequence.numberOfImages - 1);

    CompositeImage(offscreenImage, OverCompositeOp, eyeSequence.images[currentSequenceIndex], 425 + 5 * xOffset, 5 + 5 * yOffset + eyeAnimationFactor2 * 90);
    CompositeImage(offscreenImage, OverCompositeOp, defaultMawImage, 25 + 15 * xOffset, 196 + 15 * yOffset);
    CompositeImage(offscreenImage, OverCompositeOp, defaultNoseImage, 28 + 5 * xOffset, 25 + 5 * yOffset);
}


int main(int argc, char **argv) {

    initWS2812BDriver();

    systemNormaLog("Initializing GraphicsMagick...");
    InitializeMagick(NULL);
    systemNormaLog("Initialized GraphicsMagick");
    if (initLedMatrix(argc, argv) || initPreRender() || initAnimator() || initColor()){
        return 1;
    }


    initializeJadeTimeTrack(&faceMovementTimeTrack);
    initializeJadeTimeTrack(&eyeBlinkTimeTrack);
    faceMovementTimeTrack.animationDurationMS = 4000;
    eyeBlinkTimeTrack.animationDurationMS = 3000;


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

        updateColorIndex();
        requestRender();
        usleep(5000);
        renderLEDStrips();
        // clearLEDStrips();
        usleep(5000);

    }

    
}