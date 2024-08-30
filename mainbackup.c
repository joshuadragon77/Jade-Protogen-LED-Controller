// This version pushes all the heavy rendering onto GraphicsMagick to see whether if it can handle the rendering. Turns out yes but on the PI no. It uses 60% of the CPU and brings the system temperature to 80C. Not efficient.

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>

#include <GraphicsMagick/magick/api.h>

#include "facegenerator.h"

#include "led-matrix-c.h"

struct RGBLedMatrix * ledMatricies;
struct LedCanvas * ledCanvas;
struct LedCanvas * offscreenCanvas;

Image * offscreenImage;
PixelPacket * pixelsOffscreenImage;
ImageInfo imageInfo;

struct LedFont * primaryFont;

struct timeval time;
unsigned long timeOffset = 0;

# define M_PI		3.14159265358979323846

float fclamp(float number, float min, float max){
    return fmaxf(fminf(number, max), min);
}

struct timeval * obtainCurrentTime(){
    gettimeofday(&time, 0);
    return &time;
}

#define NO_FLIP 0
#define VERTICAL_FLIP 1
#define HORIZONTAL_FLIP 2

void obtainImageBuffer(PixelPacket * pixels, unsigned int imageWidth, unsigned int imageHeight, uint8_t * buffer, const uint8_t columns, const uint8_t rows, uint8_t flipMode){


    for (int x = 0;x < columns;x++){
        for (int y = 0;y < rows;y++){

            int readXPixel = imageWidth * x / columns;
            int readYPixel = imageHeight * y / rows;

            if (flipMode & VERTICAL_FLIP){
                readYPixel = imageHeight - readYPixel;
            }
            if (flipMode & HORIZONTAL_FLIP){
                readXPixel = imageWidth - readXPixel;
            }

            PixelPacket * pixel = pixels + readXPixel + readYPixel * imageWidth;

            int index = 3 * (x + y * columns);

            uint8_t red = pixel->red;
            uint8_t green = pixel->green;
            uint8_t blue = pixel->blue;
            uint8_t alpha = 255 - pixel->opacity;

            buffer[index + 0] = red;
            buffer[index + 1] = green;
            buffer[index + 2] = blue;

        }
    }
}


void obtainFrameIndex(const uint16_t durationMS, unsigned long * frame, unsigned long * maximumFrame){
    uint8_t frameRate = 30;

    struct timeval * currentTime = obtainCurrentTime();

    unsigned long observedTime = (currentTime->tv_sec - timeOffset) * 1000000 + currentTime->tv_usec;

    *frame = observedTime 
        % (durationMS * 1000) 
        * frameRate / 1000;

    *maximumFrame = durationMS * 1000 * frameRate / 1000;
}

void drawFace(){


    PixelPacket backgroundColor = {0, 0, 0};

    unsigned long frame;
    unsigned long maximumFrame;
    obtainFrameIndex(4000, &frame, &maximumFrame);

    float animationFactor = (float)frame / maximumFrame;
    float xOffset = cosf(animationFactor * 2 * M_PI);
    float yOffset = sinf(animationFactor * 2 * M_PI);

    unsigned long eyeFrame;
    unsigned long eyeMaximumFrame;
    obtainFrameIndex(3000, &eyeFrame, &eyeMaximumFrame);


    float eyeAnimationFactor = (float)eyeFrame / eyeMaximumFrame;
    float eyeAnimationFactor2 = sinf(eyeAnimationFactor * M_PI / 0.15f);

    if (eyeAnimationFactor >= 0.15f){
        eyeAnimationFactor2 = 0.0f;
    }

    unsigned int currentSequenceIndex = eyeSequence.numberOfImages * eyeAnimationFactor2;

    CompositeImage(offscreenImage, OverCompositeOp, eyeSequence.images[currentSequenceIndex], 463 + 15 * xOffset, 15 * yOffset);

    // DrawContext context = DrawAllocateContext(0, offscreenImage);

    // DrawRender(context);

    // DrawDestroyContext(context);

    // DisplayImages(&imageInfo, offscreenImage);
}

void render(){
    uint8_t matrixWidth = 64;
    uint8_t matrixHeight = 32;
    uint8_t buffer[matrixWidth * matrixHeight * 3];

    obtainImageBuffer(pixelsOffscreenImage, offscreenImage->columns, offscreenImage->rows, buffer, matrixWidth, matrixHeight, HORIZONTAL_FLIP);

    set_image(ledCanvas, 0, 0, buffer, sizeof(buffer), matrixWidth, matrixHeight, 0);

    obtainImageBuffer(pixelsOffscreenImage, offscreenImage->columns, offscreenImage->rows, buffer, matrixWidth, matrixHeight, NO_FLIP );

    set_image(ledCanvas, 64, 0, buffer, sizeof(buffer), matrixWidth, matrixHeight, 0);

    // offscreenCanvas = led_matrix_swap_on_vsync(ledMatricies, offscreenCanvas);

    PixelPacket resetColor = {0, 0, 0};
    SetImageColor(offscreenImage, &resetColor);
}


int initializeMatrix(int argc, char ** argv){
    printf("Initializing LED Matricies...\n");

    struct RGBLedMatrixOptions options;
    memset(&options, 0, sizeof(options));

    options.brightness = 100;
    options.rows = 32;       
    options.chain_length = 2;
    options.cols = 64;
    options.pwm_bits = 2;
    options.scan_mode = 1;
    options.led_rgb_sequence = "RBG";
    options.hardware_mapping = "adafruit-hat";
    options.pwm_lsb_nanoseconds = 500;
    options.disable_hardware_pulsing = true;

    ledMatricies = led_matrix_create_from_options(&options, &argc, &argv);

    if (ledMatricies == 0) {
        printf("Failed to initialize LED Matricies...\n");
        led_matrix_print_flags(stderr);
        return 1;
    }

    ledCanvas = led_matrix_get_canvas(ledMatricies);
    offscreenCanvas = led_matrix_create_offscreen_canvas(ledMatricies);

    primaryFont = load_font("./fonts/ShareTechMono.bdf");

    printf("Loaded LED Matricies\n");

    return 0;
}

int initializeRender(){
    printf("Initializing Render Engine...\n");
    struct timeval * currentTime = obtainCurrentTime();

    timeOffset = currentTime->tv_sec;
    printf("Set Current Time Offset @ %lu\n", currentTime->tv_sec);

    printf("Initializing GraphicsMagick...\n");
    InitializeMagick(NULL);

    ImageInfo imageInfo;
    GetImageInfo(&imageInfo);
    
    PixelPacket backgroundColor = {0, 170, 255, 1};
    char size[] = "720x360";

    // imageInfo.background_color = backgroundColor;
    imageInfo.size = size;

    offscreenImage = AllocateImage(&imageInfo);
    SyncImage(offscreenImage);
    pixelsOffscreenImage = GetImagePixels(offscreenImage, 0, 0, offscreenImage->columns, offscreenImage->rows);

    initFaceGenerator();
    
    printf("Loaded Render Engine\n");

    return 0;
}

int main(int argc, char **argv) {
    if (initializeMatrix(argc, argv) || initializeRender()){
        return 1;
    }


    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);


    while (true){
        
        char command[256];

        read(0, command, sizeof(command));

        if (strcmp("exit\n", command) == 0){
            printf("Protogen Out! o7\n");
            return 0;
        }

        drawFace();
        render();
        usleep(40000);

    }
}