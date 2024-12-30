#include "prerender.h"
#include "color.h"

typedef struct _Offset{
    float xOffset;
    float yOffset;
} Offset;

typedef struct _FaceImageCollection{
    Image * eyeImage;
    Image * mouthImage;
    Image * noseImage;

    Offset eyeOffset;
    Offset mouthOffset;
    Offset noseOffset;
} FaceImageCollection;

typedef struct _FaceAnimationCollection{
    ImageSequence eyeImage;
    ImageSequence mouthImage;
    ImageSequence noseImage;
} FaceAnimationCollection;

typedef struct _FaceState{
    Polygon * eyePolygon;
    Polygon * mouthPolygon;
    Polygon * nosePolygon;
    u_int8_t faceStateID;

    Offset eyeOffset;
    Offset mouthOffset;
    Offset noseOffset;

    ColorTransformFunction colorTransformFunction;
} FaceState;

void obtainRainbow(const float * colorFactor, JColor * color);
void obtainJadenarium(const float * colorFactor, JColor * color);
void obtainAlder(const float * colorFactor, JColor * color);
void obtainAngry(const float * colorFactor, JColor * color);
void obtainHappy(const float * colorFactor, JColor * color);
void obtainNeutral(const float * colorFactor, JColor * color);

extern Image * defaultMawImage;
extern Image * defaultNoseImage;
extern Image * defaultEyeImage;

extern FaceState normalFace;
extern FaceState blinkFace;
extern FaceState approvalFace;
extern FaceState angryFace;
extern FaceState prideFace;
extern FaceState boopFace;

FaceImageCollection obtainFaceImages();
void attemptTransition(FaceState * newFaceState, unsigned int transitionTimeMS);

int initTransition();