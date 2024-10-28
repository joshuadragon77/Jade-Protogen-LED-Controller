#include "prerender.h"

#include "math.h"

Image * generateStaticImage(Polygon * polygon){

    // printf("Generating Images from Polygon %s\n", polygon->name);

    ImageInfo imageInfo;

    GetImageInfo(&imageInfo);

    imageInfo.antialias = 1;
    imageInfo.background_color.red = 0;
    imageInfo.background_color.green = 0;
    imageInfo.background_color.blue = 0;
    imageInfo.background_color.opacity = 0;

    char sizeString[10];
    sprintf(sizeString, "%dx%d", polygon->width, polygon->height);
    imageInfo.size = sizeString;

    Image * image = AllocateImage(&imageInfo);

    DrawInfo * drawInfo = CloneDrawInfo(&imageInfo, 0);
    DrawContext drawContext = DrawAllocateContext(drawInfo, image);

    PixelPacket color = {255, 170, 0};

    DrawSetFillColor(drawContext, &color);
    DrawPathStart(drawContext);
    Point startPoint = polygon->points[0];
    DrawPathMoveToAbsolute(drawContext, startPoint.x, startPoint.y);

    for (int i = 1;i<polygon->numberOfPoints;i++){
        Point * currentPoint = &polygon->points[i];

        DrawPathLineToAbsolute(drawContext, currentPoint->x, currentPoint->y);
    }

    DrawPathClose(drawContext);
    DrawPathFinish(drawContext);

    DrawRender(drawContext);

    DestroyDrawInfo(drawInfo);
    DrawDestroyContext(drawContext);

    // printf("Generated Images from Polygon %s\n", polygon->name);

    return image;
}

Image * generateLinearInImage(Polygon * fromPolygon, Polygon * toPolygon, unsigned int frame, unsigned int maximumFrame){

    unsigned int numberOfPoints = fromPolygon->numberOfPoints > toPolygon->numberOfPoints ? fromPolygon->numberOfPoints : toPolygon->numberOfPoints;
    Point * newPoints = (Point *)malloc(sizeof(Point) * numberOfPoints);

    double lerpFactor = (double)frame / maximumFrame;

    for (int i = 0;i<numberOfPoints;i++){

        Point * fromPointPolygon = &fromPolygon->points[i < fromPolygon->numberOfPoints ? i : fromPolygon->numberOfPoints - 1];
        Point * toPointPolygon = &toPolygon->points[i < toPolygon->numberOfPoints? i : toPolygon->numberOfPoints - 1];
        newPoints[i].x = (int)fromPointPolygon->x + ((int)toPointPolygon->x - (int)fromPointPolygon->x) * lerpFactor;
        newPoints[i].y = (int)fromPointPolygon->y + ((int)toPointPolygon->y - (int)fromPointPolygon->y) * lerpFactor;
    }

    char newName[25];

    sprintf(newName, "T%s(%d/%d)",fromPolygon->name, frame, maximumFrame);

    Polygon newPolygon = {
        newPoints,
        numberOfPoints,
        fromPolygon->width > toPolygon->width ? fromPolygon->width : toPolygon->width,
        fromPolygon->height > toPolygon->height ? fromPolygon->height : toPolygon->height,
        newName
    };

    Image * generatedImage = generateStaticImage(&newPolygon);
    
    free(newPoints);


    return generatedImage;
}

Image * generateAnimationLinearInImage(Polygon * fromPolygon, Polygon * toPolygon, unsigned int maximumFrame){
    Image * startImage = generateLinearInImage(fromPolygon, toPolygon, 0, maximumFrame - 1);
    Image * previousImage = startImage;
    for (int i = 1;i<maximumFrame;i++){
        Image * image = generateLinearInImage(fromPolygon, toPolygon, i, maximumFrame - 1);
        previousImage->next = image;
        image->previous = previousImage;

        previousImage = image;
    }

    previousImage->next = startImage;

    return startImage;
}

ImageSequence generateImageSequence(Image * images, unsigned int numberOfImages){

    Image ** imageSequenceArray = (Image **)malloc(sizeof(images) * numberOfImages);

    Image * currentImage = images;

    for (int i = 0;i<numberOfImages;i++){
        imageSequenceArray[i] = currentImage;

        currentImage = currentImage->next;
    }

    ImageSequence imageSequence = {
        imageSequenceArray,
        numberOfImages
    };

    return imageSequence;
}

ImageSequence copyReverseImageSequence(ImageSequence parentImageSequence){

    Image ** imageSequenceArray = (Image **)malloc(sizeof(Image *) * parentImageSequence.numberOfImages);

    for (int i = 0;i<parentImageSequence.numberOfImages;i++){
        imageSequenceArray[i] = parentImageSequence.images[parentImageSequence.numberOfImages - i - 1];
    }

    ImageSequence imageSequence = {
        imageSequenceArray,
        parentImageSequence.numberOfImages
    };

    return imageSequence;
}

void destroyImageSequence(ImageSequence sequence){
    free(sequence.images);
}

int initPreRender(){
    return 0;
}
