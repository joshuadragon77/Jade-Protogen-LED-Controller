#include "animator.h"
#include "consolescript.h"

struct timeval time;
unsigned long timeOffset = 0;

float fclampf(float number, float min, float max){
    return fmaxf(fminf(number, max), min);
}

double fclamp(double number, double min, double max){
    return fmax(fmin(number, max), min);
}

int clamp(int number, int min, int max){
    return (int)fclamp(number, min, max);
}

struct timeval * obtainCurrentTime(){
    gettimeofday(&time, 0);
    return &time;
}

void obtainTimeFactor(const unsigned long * frame, const unsigned long * maximumFrame, float * timeFactor){
    *timeFactor = ((double)(*frame) / *maximumFrame);
}

void obtainFrameIndex(const unsigned short * frameRate, const unsigned int * durationMS, const unsigned long * timeOffset, unsigned long * frame, unsigned long * maximumFrame){

    struct timeval * currentTime = obtainCurrentTime();

    unsigned long observedTime = (currentTime->tv_sec - *timeOffset) * 1000000 + currentTime->tv_usec;

    *frame = observedTime 
        % (*durationMS * 1000) 
        * *frameRate / 1000;

    *maximumFrame = *durationMS * 1000 * *frameRate / 1000;
}

JadeTimeTrack * initializeJadeTimeTrack(JadeTimeTrack * jadeTimeTrack){
    jadeTimeTrack->animationDurationMS = 1000;
    jadeTimeTrack->cooldownDurationMS = 0;
    jadeTimeTrack->frameRate = 1;
    jadeTimeTrack->cooldownMode = RestOnStart;
    jadeTimeTrack->timeOffset = obtainCurrentTime()->tv_sec;
    return jadeTimeTrack;
}

void obtainTimeTrackFactor(const JadeTimeTrack * jadeTimeTrack, float * timeFactor){
    unsigned long frame = 0;
    unsigned long maximumFrame = 0;

    const unsigned int totalDuration = jadeTimeTrack->animationDurationMS + jadeTimeTrack->cooldownDurationMS;

    obtainFrameIndex(&jadeTimeTrack->frameRate, &totalDuration, &jadeTimeTrack->timeOffset, &frame, &maximumFrame);

    obtainTimeFactor(&frame, &maximumFrame, timeFactor);
    *timeFactor *= (double)totalDuration / jadeTimeTrack->animationDurationMS;

    if (*timeFactor >= 1){
        switch(jadeTimeTrack->cooldownMode){
            case RestOnEnd:{
                *timeFactor = 1;
                break;
            }
            case RestOnStart:{
                *timeFactor = 0;
                break;
            }
        }
    }
}

void obtainAnimationTrackFactor(const JadeAnimationTrack jadeAnimationTrack, const float * timeFactor, float * animationFactor){
    jadeAnimationTrack(timeFactor, animationFactor);
}

int initAnimator(){
    systemNormaLog("Initializing Animator...");
    struct timeval * currentTime = obtainCurrentTime();
    timeOffset = currentTime->tv_sec;

    systemNormaLog("Initialized Animator");
    return 0;
}