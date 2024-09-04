#include <sys/time.h>
#include <math.h>

typedef enum _CooldownMode{
    RestOnStart, RestOnEnd
} CooldownMode;

typedef void (*JadeAnimationTrack)(const float * input, float * output);

typedef struct _JadeTimeTrack{
    unsigned long timeOffset;
    unsigned int animationDurationMS;
    unsigned int cooldownDurationMS;

    unsigned short frameRate;

    CooldownMode cooldownMode;

} JadeTimeTrack;


float fclampf(float number, float min, float max);
double fclamp(double number, double min, double max);
int clamp(int number, int min, int max);
struct timeval * obtainCurrentTime();
void obtainTimeFactor(const unsigned long * frame, const unsigned long * maximumFrame, float * timeFactor);
void obtainFrameIndex(const unsigned short * frameRate, const unsigned int * durationMS, const unsigned long * timeOffset, unsigned long * frame, unsigned long * maximumFrame);
JadeTimeTrack * initializeJadeTimeTrack(JadeTimeTrack * jadeTimeTrack);
void obtainTimeTrackFactor(const JadeTimeTrack * jadeTimeTrack, float * timeFactor);
void obtainAnimationTrackFactor(const JadeAnimationTrack jadeAnimationTrack, const float * timeFactor, float * animationFactor);
int initAnimator();