#ifndef JADE_PRERENDER
#define JADE_PRERENDER

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

typedef struct _Face{
    ImageSequence * eyeAnimation;
    ImageSequence * eyeBlinkAnimation;

    ImageSequence * noseAnimation;

    ImageSequence * mouthAnimation;
    ImageSequence * mouthOpenAnimation;
} Face;


Image * generateStaticImage(Polygon * polygon);
Image * generateAnimationLinearInImage(Polygon * fromPolygon, Polygon * toPolygon, unsigned int maximumFrame);
ImageSequence generateImageSequence(Image * images, unsigned int numberOfImages);
ImageSequence copyReverseImageSequence(ImageSequence parent);

int initPreRender();

#endif