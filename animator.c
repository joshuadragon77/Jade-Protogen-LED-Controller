#include "macros.h"

#include "animator.h"

struct timeval timeStart;
struct timeval timeCache;
unsigned long stopwatchStart;

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
    gettimeofday(&timeCache, 0);

    return &timeCache;
}

void obtainMicroseconds(unsigned long * time){
    struct timeval * currentTime = obtainCurrentTime();
    struct timeval * timeOffset = &timeStart;

    *time = currentTime->tv_sec * 1000000 + currentTime->tv_usec;
}

JadeTimeTrack * initializeTimeTrack(JadeTimeTrack * timeTrack){
    timeTrack->animationDurationUS = 1000000;
    timeTrack->cooldownMode = RestOnStart;
    timeTrack->loopMode = NoLoop;
    timeTrack->playMode = Stopped;
    
    return timeTrack;
}

void playTimeTrack(JadeTimeTrack * timeTrack){
    obtainMicroseconds(&timeTrack->timeState.timeOffset);
    timeTrack->playMode = Played;
}

void pauseTimeTrack(JadeTimeTrack * timeTrack){
    unsigned long currentTime;
    obtainMicroseconds(&currentTime);
    timeTrack->playMode = Paused;
    timeTrack->timeState.pauseGap = currentTime - timeTrack->timeState.timeOffset;
}

void resumeTimeTrack(JadeTimeTrack * timeTrack){
    unsigned long currentTime;
    obtainMicroseconds(&currentTime);
    timeTrack->playMode = Played;
    timeTrack->timeState.timeOffset = currentTime - timeTrack->timeState.pauseGap;
}

void stopTimeTrack(JadeTimeTrack * timeTrack){
    timeTrack->playMode = Stopped;
}

void startStopwatch(){
    obtainMicroseconds(&stopwatchStart);
}

unsigned long printStopwatchUS(){
    unsigned long currentTime;
    obtainMicroseconds(&currentTime);
    
    return currentTime - stopwatchStart;
}

void obtainTimeTrackFactor(JadeTimeTrack * timeTrack, double * timeFactor){
    unsigned long currentTime;
    obtainMicroseconds(&currentTime);

    unsigned long deltaTime = currentTime - timeTrack->timeState.timeOffset;
    switch(timeTrack->playMode){
        case Paused:{};
        case Played:{


            if (deltaTime >= timeTrack->animationDurationUS){
                switch(timeTrack->loopMode){
                    case Loop:{
                        deltaTime -= timeTrack->animationDurationUS;
                        timeTrack->timeState.timeOffset = currentTime - deltaTime;

                        *timeFactor = fclamp((double)deltaTime / timeTrack->animationDurationUS, 0, 1);
                        return;
                    }
                    case LoopDelay:{
                        deltaTime -= timeTrack->animationDurationUS;
                        timeTrack->timeState.timeOffset = currentTime - deltaTime;

                        *timeFactor = fclamp((double)deltaTime / timeTrack->animationDurationUS, 0, 1);
                        timeTrack->playMode = Cooldown;
                        break;
                    }
                    case NoLoop:{
                        deltaTime = 0;
                        timeTrack->timeState.timeOffset = currentTime;
                        timeTrack->playMode = Stopped;
                        break;
                    }
                }
            }else{
                *timeFactor = fclamp((double)deltaTime / timeTrack->animationDurationUS, 0, 1);
                break;
            }
        };
        case Stopped:{
            switch(timeTrack->cooldownMode){
                case RestOnEnd:{
                    *timeFactor = 1;
                    return;
                }
                case RestOnStart:{
                    *timeFactor = 0;
                    return;
                }
            }
            break;
        }
        case Cooldown:{

            if (timeTrack->loopMode == NoLoop){
                deltaTime = 0;
                timeTrack->timeState.timeOffset = currentTime;
                timeTrack->playMode = Stopped;
            }

            if (deltaTime >= timeTrack->cooldownDurationUS){
                deltaTime -= timeTrack->cooldownDurationUS;

                timeTrack->timeState.timeOffset = currentTime - deltaTime;
                timeTrack->playMode = Played;
            }
            switch(timeTrack->cooldownMode){
                case RestOnEnd:{
                    *timeFactor = 1;
                    return;
                }
                case RestOnStart:{
                    *timeFactor = 0;
                    return;
                }
            }
            break;
        }
    }
}

int initAnimator(){

    gettimeofday(&timeStart, 0);

    return 0;
}