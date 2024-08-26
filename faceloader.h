#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include <GraphicsMagick/magick/api.h>


extern Image * eyeImage;
extern Image * mouthImage;
extern Image * noseImage;

void load();