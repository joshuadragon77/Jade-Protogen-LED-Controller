#include <GraphicsMagick/magick/api.h>

typedef struct _Point{
    unsigned int x;
    unsigned int y;
} Point;

typedef struct _Polygon{
    Point * points;
    unsigned int numberOfPoints;
    unsigned int width;
    unsigned int height;
    const char * name;
} Polygon;

typedef struct _ImageSequence{
    Image ** images;
    unsigned int numberOfImages;
} ImageSequence;

extern Image * defaultEyeImage;
extern ImageSequence eyeSequence;
extern Image * defaultMawImage;
extern Image * defaultNoseImage;
extern Image * defaultEyeBoopImage;
extern ImageSequence boopEyeSequence;

int initPreRender();