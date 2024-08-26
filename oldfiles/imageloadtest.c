
#include <string.h>

#include <GraphicsMagick/magick/api.h>

int main(){

    size_t blobSize = 0;
    ExceptionInfo exception;

    InitializeMagick(NULL);
    GetExceptionInfo(&exception);

    ImageInfo * imageInfo = CloneImageInfo(0);
    void * blob = FileToBlob("./Kadesey.png", &blobSize, &exception);
    Image * image = BlobToImage(imageInfo, blob, blobSize, &exception);

    printf("%d", image->rows);
}