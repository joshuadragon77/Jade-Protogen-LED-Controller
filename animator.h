#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <math.h>

#define SECONDS_TO_US(X) X * 1000000
#define MILLISECONDS_TO_US(X) X * 1000

typedef struct _JadeTimeState{
    unsigned long pauseGap;
    unsigned long timeOffset;
} JadeTimeState;

typedef enum _CooldownMode{
    RestOnStart, RestOnEnd
} CooldownMode;

typedef enum _LoopMode{
    Loop, NoLoop, LoopDelay
} LoopMode;

typedef enum _PlayMode{
    Played, Paused, Stopped, Cooldown
} PlayMode;


typedef struct _JadeTimeTrack{

    unsigned long animationDurationUS;
    unsigned long cooldownDurationUS;

    CooldownMode cooldownMode;
    LoopMode loopMode;
    PlayMode playMode;

    JadeTimeState timeState;  
} JadeTimeTrack;

float fclampf(float number, float min, float max);
double fclamp(double number, double min, double max);
int clamp(int number, int min, int max);
struct timeval * obtainCurrentTime();
void obtainMicroseconds(unsigned long * time);
JadeTimeTrack * initializeTimeTrack(JadeTimeTrack * timeTrack);
void playTimeTrack(JadeTimeTrack * timeTrack);
void pauseTimeTrack(JadeTimeTrack * timeTrack);
void resumeTimeTrack(JadeTimeTrack * timeTrack);
void stopTimeTrack(JadeTimeTrack * timeTrack);
void obtainTimeTrackFactor(JadeTimeTrack * timeTrack, double * timeFactor);
int initAnimator();