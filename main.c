#define __USE_GNU 1
#define __USE_MISC 1

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>

#include <GraphicsMagick/magick/api.h>

#include "led-matrix-c.h"
#include "faceloader.h"

struct RGBLedMatrix * ledMatricies;
struct LedCanvas * ledCanvas;
struct LedCanvas * offscreenCanvas;

Image * offscreenImage;
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

void obtainImageBuffer(Image * image, uint8_t * buffer, const uint8_t columns, const uint8_t rows){

    PixelPacket * pixels = GetImagePixels(image, 0, 0, image->columns, image->rows);

    for (int x = 0;x < columns;x++){
        for (int y = 0;y < rows;y++){

            int readXPixel = image->columns * x / columns;
            int readYPixel = image->rows * y / rows;

            PixelPacket * pixel = pixels + readXPixel + readYPixel * image->columns;

            int index = 3 * (x + y * columns);

            uint8_t red = pixel->red;
            uint8_t green = pixel->green;
            uint8_t blue = pixel->blue;
            uint8_t alpha = 255 - pixel->opacity;

            buffer[index + 0] = alpha * red / 255;
            buffer[index + 1] = alpha * green / 255;
            buffer[index + 2] = alpha * blue / 255;
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

    led_canvas_clear(offscreenCanvas);

    char buffer[25];

    unsigned long frame;
    unsigned long maximumFrame;
    obtainFrameIndex(1000, &frame, &maximumFrame);

    float animationFactor = (float)frame / maximumFrame;
    float xOffset = cosf(animationFactor * 2 * M_PI);
    float yOffset = sinf(animationFactor * 2 * M_PI);

    {
        uint8_t imageWidth = 24;
        uint8_t imageHeight = 14;
        uint8_t buffer[imageWidth * imageHeight * 3];

        obtainImageBuffer(eyeImage, buffer, imageWidth, imageHeight);

        set_image(offscreenCanvas, 40 + (int)(2 * xOffset), (int)(2 * yOffset), buffer, sizeof(buffer), imageWidth, imageHeight, 0);
    }
    {
        uint8_t imageWidth = 60;
        uint8_t imageHeight = 16;
        uint8_t buffer[imageWidth * imageHeight * 3];

        obtainImageBuffer(mouthImage, buffer, imageWidth, imageHeight);

        set_image(offscreenCanvas, (int)(10 * xOffset), 16 + (int)(10 * yOffset), buffer, sizeof(buffer), imageWidth, imageHeight, 0);
    }
    {
        uint8_t imageWidth = 10;
        uint8_t imageHeight = 10;
        uint8_t buffer[imageWidth * imageHeight * 3];

        obtainImageBuffer(noseImage, buffer, imageWidth, imageHeight);

        set_image(offscreenCanvas, (int)(15 * xOffset), 7 + (int)(15 * yOffset), buffer, sizeof(buffer), imageWidth, imageHeight, 0);
    }
}

void render(){
    offscreenCanvas = led_matrix_swap_on_vsync(ledMatricies, offscreenCanvas);
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
    // options.disable_hardware_pulsing = true;

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
    char size[] = "720x360";
    imageInfo.size = size;
    offscreenImage = AllocateImage(&imageInfo);

    load();
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

    }
}