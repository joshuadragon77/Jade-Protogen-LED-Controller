#include "macros.h"

#include "assets.h"
#include "transition.h"
#include "color.h"
#include "animator.h"

Image * defaultMawImage;
Image * defaultNoseImage;
Image * defaultEyeImage;

JColor rainbowElement[] = {
    {255, 0, 0},
    {248, 161, 18},
    {233, 240, 18},
    {9, 240, 18},
    {9, 213, 245},
    {218, 20, 245},
};

double jadenariumAnimationFactor(double timeFactor){
    
    timeFactor = ( - cos(M_PI * timeFactor) + 1) / 2;
    // return ((timeFactor - 0.4) * (timeFactor - 1.4) * (timeFactor - 2.4) + 1.344) / 1.68;
    // return (sin(timeFactor + 3.8) + 0.611857890943) / -0.384306717893;

    return (sin(timeFactor + 7.05) - 0.69384494493) / 0.287004890691;
    // return (sin(timeFactor - 1.8) + 0.973847630878) / 0.256491539979;
}

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

void obtainJadenarium(const float * colorFactor, JColor * color){
    JColor from = {0, 170, 255};
    JColor to = {0, 255, 255};
    // JColor from = {255, 0, 0};
    // JColor to = {0, 255, 0};

    lerpColor(&from, &to, color, colorFactor);
}

void obtainAlder(const float * colorFactor, JColor * color){
    JColor from = {255, 128, 200};
    JColor to = {255, 128, 200};

    lerpColor(&from, &to, color, colorFactor);
}

void obtainAngry(const float * colorFactor, JColor * color){
    JColor from = {255, 0, 0};
    JColor to = {255, 0, 0};

    lerpColor(&from, &to, color, colorFactor);
}

void obtainHappy(const float * colorFactor, JColor * color){
    JColor from = {0, 255, 0};
    JColor to = {0, 255, 0};

    lerpColor(&from, &to, color, colorFactor);
}

void obtainNeutral(const float * colorFactor, JColor * color){
    JColor from = {255, 100, 0};
    JColor to = {255, 100, 0};

    lerpColor(&from, &to, color, colorFactor);
}

FaceState normalFace = {
    &JadesFace_eyePolygon,
    &JadesFace_mouthPolygon,
    &JadesFace_nosePolygon,
    0,
    {0, 0},
    {0, 0},
    {0, 0},
    obtainJadenarium    
};

FaceState blinkFace = {
    &JadesFaceBlink_eyePolygon,
    &JadesFace_mouthPolygon,
    &JadesFace_nosePolygon,
    0,
    {0, 50},
    {0, -15},
    {-10, 0},
    obtainJadenarium    
};

FaceState approvalFace = {
    &JadesApprovalFace_eyePolygon,
    &JadesFace_mouthPolygon,
    &JadesFace_nosePolygon,
    0,
    {-10, 0},
    {0, -20},
    {0, -5},
    obtainHappy    
};

FaceState angryFace = {
    &JadesAngryFace_eyePolygon,
    &JadesAngryFace_mouthPolygon,
    &JadesFace_nosePolygon,
    0,
    {20, 0},
    {0, 30},
    {-10, 0},
    obtainAngry    
};

FaceState prideFace = {
    &JadesPrideFace_eyePolygon,
    &JadesFace_mouthPolygon,
    &JadesFace_nosePolygon,
    0,
    {30, 0},
    {5, 5},
    {5, 5},
    obtainRainbow    
};

FaceState boopFace = {
    &JadesHappyBoop_eyePolygon,
    &JadesFace_mouthPolygon,
    &JadesFace_nosePolygon,
    0,
    {60, 0},
    {-10, -10},
    {-20, 0},
    obtainRainbow    
};


FaceImageCollection * faceStateStaticImages;
FaceAnimationCollection ** faceStateTransitionImages;

JadeTimeTrack transitionTimeTrack;

FaceState * previousFaceState = &normalFace;
FaceState * currentFaceState = &normalFace;
FaceState * nextFaceState = 0;
u_int16_t nextTransitionTimeMS = 1000;

FaceImageCollection obtainFaceImages(){

    if (nextFaceState && transitionTimeTrack.playMode == Stopped && nextFaceState != currentFaceState){
        previousFaceState = currentFaceState;
        currentFaceState = nextFaceState;
        nextFaceState = 0;
        
        transitionToColorTransformFunction(currentFaceState->colorTransformFunction, nextTransitionTimeMS);
        transitionTimeTrack.animationDurationUS = MILLISECONDS_TO_US(nextTransitionTimeMS);
        playTimeTrack(&transitionTimeTrack);       
    }

    if (transitionTimeTrack.playMode == Played){
        double timeFactor = 0;
        obtainTimeTrackFactor(&transitionTimeTrack, &timeFactor);

        double animationFactor = jadenariumAnimationFactor(timeFactor);

        FaceAnimationCollection animationCollection = faceStateTransitionImages[previousFaceState->faceStateID][currentFaceState->faceStateID];

        FaceImageCollection faceImageReturns;

        faceImageReturns.eyeImage = animationCollection.eyeImage.images[
            clamp((u_int8_t)(animationCollection.eyeImage.numberOfImages * animationFactor), 0, animationCollection.eyeImage.numberOfImages - 1)];
        faceImageReturns.mouthImage = animationCollection.mouthImage.images[
            clamp((u_int8_t)(animationCollection.mouthImage.numberOfImages * animationFactor), 0, animationCollection.mouthImage.numberOfImages - 1)];
        faceImageReturns.noseImage = animationCollection.noseImage.images[
            clamp((u_int8_t)(animationCollection.noseImage.numberOfImages * animationFactor), 0, animationCollection.noseImage.numberOfImages - 1)];

        faceImageReturns.eyeOffset.xOffset = previousFaceState->eyeOffset.xOffset + (currentFaceState->eyeOffset.xOffset - previousFaceState->eyeOffset.xOffset) * animationFactor;
        faceImageReturns.eyeOffset.yOffset = previousFaceState->eyeOffset.yOffset + (currentFaceState->eyeOffset.yOffset - previousFaceState->eyeOffset.yOffset) * animationFactor;

        faceImageReturns.mouthOffset.xOffset = previousFaceState->mouthOffset.xOffset + (currentFaceState->mouthOffset.xOffset - previousFaceState->mouthOffset.xOffset) * animationFactor;
        faceImageReturns.mouthOffset.yOffset = previousFaceState->mouthOffset.yOffset + (currentFaceState->mouthOffset.yOffset - previousFaceState->mouthOffset.yOffset) * animationFactor;

        faceImageReturns.noseOffset.xOffset = previousFaceState->noseOffset.xOffset + (currentFaceState->noseOffset.xOffset - previousFaceState->noseOffset.xOffset) * animationFactor;
        faceImageReturns.noseOffset.yOffset = previousFaceState->noseOffset.yOffset + (currentFaceState->noseOffset.yOffset - previousFaceState->noseOffset.yOffset) * animationFactor;

        return faceImageReturns;
    }else{
        FaceImageCollection faceImageReturns = faceStateStaticImages[currentFaceState->faceStateID];

        faceImageReturns.eyeOffset = currentFaceState->eyeOffset;
        faceImageReturns.mouthOffset = currentFaceState->mouthOffset;
        faceImageReturns.noseOffset = currentFaceState->noseOffset;

        return faceImageReturns;
    }
    
}

void attemptTransition(FaceState * newFaceState, unsigned int transitionTimeMS){
    nextFaceState = newFaceState;
    nextTransitionTimeMS = transitionTimeMS;
}

int initTransition(){

    initializeTimeTrack(&transitionTimeTrack);

    transitionTimeTrack.loopMode = NoLoop;
    transitionTimeTrack.cooldownMode = RestOnEnd;

    FaceState * faceStateRepository[] = {
        &normalFace,
        &blinkFace,
        &angryFace,
        &approvalFace,
        &prideFace,
        &boopFace
    };

    unsigned short numberOfFaceStates = sizeof(faceStateRepository)/sizeof(FaceState * );
    faceStateStaticImages = (FaceImageCollection *)malloc(numberOfFaceStates * sizeof(FaceImageCollection));

    for (int i = 0;i<numberOfFaceStates;i++){
        FaceState * faceState = faceStateRepository[i];
        faceStateStaticImages[i].eyeImage = generateStaticImage(faceState->eyePolygon);
        faceStateStaticImages[i].mouthImage = generateStaticImage(faceState->mouthPolygon);
        faceStateStaticImages[i].noseImage = generateStaticImage(faceState->nosePolygon);
        
        faceState->faceStateID = i;
    }

    faceStateTransitionImages = (FaceAnimationCollection **)malloc(numberOfFaceStates * sizeof(FaceAnimationCollection *));

    for (int i = 0;i<numberOfFaceStates;i++){

        FaceAnimationCollection * imageTransitionTable = (FaceAnimationCollection *)malloc(numberOfFaceStates * sizeof(FaceAnimationCollection));

        for (int r = 0;r<numberOfFaceStates;r++){
            imageTransitionTable[r].eyeImage.numberOfImages = 0;
            imageTransitionTable[r].mouthImage.numberOfImages = 0;
            imageTransitionTable[r].noseImage.numberOfImages = 0;
        }

        faceStateTransitionImages[i] = imageTransitionTable;
    }

    for (int i = 0;i<numberOfFaceStates;i++){
        FaceState * fromState = faceStateRepository[i];

        FaceAnimationCollection * imageTransitionTable = faceStateTransitionImages[i] ;
        for (int r = 0;r<numberOfFaceStates;r++){
            FaceState * toState = faceStateRepository[r];

            if (i == r)
                continue;

            if (imageTransitionTable[r].eyeImage.numberOfImages != 0){
                continue;
            }

            ImageSequence eyeImageSequence;
            ImageSequence mouthImageSequence;
            ImageSequence noseImageSequence;

            if (fromState->eyePolygon == toState->eyePolygon){
                eyeImageSequence = generateImageSequence(generateStaticImage(fromState->eyePolygon), 1);
            }else
                eyeImageSequence = generateImageSequence(generateAnimationLinearInImage(fromState->eyePolygon, toState->eyePolygon, 50), 50);

            if (fromState->mouthPolygon == toState->mouthPolygon){
                mouthImageSequence = generateImageSequence(generateStaticImage(fromState->mouthPolygon), 1);
            }else
                mouthImageSequence = generateImageSequence(generateAnimationLinearInImage(fromState->mouthPolygon, toState->mouthPolygon, 50), 50);

            if (fromState->nosePolygon == toState->nosePolygon){
                noseImageSequence = generateImageSequence(generateStaticImage(fromState->nosePolygon), 1);
            }else
                noseImageSequence = generateImageSequence(generateAnimationLinearInImage(fromState->nosePolygon, toState->nosePolygon, 50), 50);

            imageTransitionTable[r].eyeImage = eyeImageSequence;
            imageTransitionTable[r].mouthImage = mouthImageSequence;
            imageTransitionTable[r].noseImage = noseImageSequence;

            faceStateTransitionImages[r][i].eyeImage  = copyReverseImageSequence(eyeImageSequence);
            faceStateTransitionImages[r][i].mouthImage  = copyReverseImageSequence(mouthImageSequence);
            faceStateTransitionImages[r][i].noseImage  = copyReverseImageSequence(noseImageSequence);
        }
    }

    return 0;
}