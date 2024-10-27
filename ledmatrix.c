#include <math.h>

#include "color.h"
#include "ledmatrix.h"


struct RGBLedMatrix * ledMatricies;
struct LedCanvas * ledCanvas;
struct LedCanvas * offscreenCanvas;

Image * offscreenImage;
PixelPacket * pixelsOffscreenImage;
ImageInfo imageInfo;

struct LedFont * primaryFont;

void obtainImageBuffer(PixelPacket * pixels, unsigned int imageWidth, unsigned int imageHeight, uint8_t * buffer, const uint8_t columns, const uint8_t rows, uint8_t flipMode){

    float maximumMagnitude = powf(powf(columns, 2) + powf(rows, 2), 0.5f);

    for (uint8_t x = 0;x < columns;x++){
        for (uint8_t y = 0;y < rows;y++){

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
            uint8_t alpha = pixel->opacity;

            uint8_t * mainOpacityFactor = &blue;

            float colorFactor = powf(powf(x, 2) + powf(y, 2), 0.5f) / maximumMagnitude;

            const JColor * color = determineColorAtIndex(&colorFactor);

            buffer[index + 0] = *mainOpacityFactor * color->red / 255;
            buffer[index + 1] = *mainOpacityFactor * color->green / 255;
            buffer[index + 2] = *mainOpacityFactor * color->blue / 255;

        }
    }
}


void renderToMatrix(){
    uint8_t matrixWidth = 64;
    uint8_t matrixHeight = 32;
    uint8_t buffer[matrixWidth * matrixHeight * 3];

    obtainImageBuffer(pixelsOffscreenImage, offscreenImage->columns, offscreenImage->rows, buffer, matrixWidth, matrixHeight, HORIZONTAL_FLIP);

    set_image(offscreenCanvas, 0, 0, buffer, sizeof(buffer), matrixWidth, matrixHeight, 0);

    obtainImageBuffer(pixelsOffscreenImage, offscreenImage->columns, offscreenImage->rows, buffer, matrixWidth, matrixHeight, NO_FLIP );

    set_image(offscreenCanvas, 64, 0, buffer, sizeof(buffer), matrixWidth, matrixHeight, 0);

    offscreenCanvas = led_matrix_swap_on_vsync(ledMatricies, offscreenCanvas);

    PixelPacket resetColor = {0, 0, 0};
    SetImageColor(offscreenImage, &resetColor);
}

void clearMatrix(){
    led_canvas_clear(ledCanvas);
}

void requestRender(){
    renderToBuffer(offscreenImage);
    renderToMatrix();
}


int initLedMatrix(int argc, char ** argv){

    struct RGBLedMatrixOptions options;
    memset(&options, 0, sizeof(options));

    options.brightness = 100;
    options.rows = 32;       
    options.chain_length = 2;
    options.cols = 64;
    options.pwm_bits = 11;
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

    ImageInfo imageInfo;
    GetImageInfo(&imageInfo);

    imageInfo.background_color.red = 0;
    imageInfo.background_color.green = 0;
    imageInfo.background_color.blue = 0;
    imageInfo.background_color.opacity = 0;

    char size[] = "720x360";

    imageInfo.size = size;

    offscreenImage = AllocateImage(&imageInfo);
    pixelsOffscreenImage = GetImagePixels(offscreenImage, 0, 0, offscreenImage->columns, offscreenImage->rows);

    return 0;
}
