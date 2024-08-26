#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

// #include <GraphicsMagick/magick/api.h>

#include "led-matrix-c.h"

int render_face(struct LedCanvas * ledCanvas, int frame){
    return 0;   
}

int main(int argc, char **argv) {

    struct RGBLedMatrixOptions options;
    memset(&options, 0, sizeof(options));
    options.rows = 32;            // You can set defaults if you want.
    options.chain_length = 1;
    options.cols = 64;
    options.pixel_mapper_config = "Rotate:180";
    options.led_rgb_sequence = "RBG";
    options.hardware_mapping = "adafruit-hat";
    options.multiplexing = 0;
    // options.disable_hardware_pulsing = true;

    struct RGBLedMatrix * matrix = led_matrix_create_from_options(&options,
                                                                &argc, &argv);
    if (matrix == NULL) {
        led_matrix_print_flags(stderr);
        return 1;
    }
    struct LedCanvas * ledCanvas = led_matrix_get_canvas(matrix);

    led_canvas_set_pixel(ledCanvas, 32, 16, 0, 170, 255);
    
    struct LedFont * font = load_font("./fonts/ShareTechMono.bdf");

    int height = height_font(font);

    // draw_text(ledCanvas, font, 0, height, 0, 170, 255, "Hello!", 1);

    char shit[25];

    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);

    
    size_t blobSize = 0;
    // ExceptionInfo exception;

    // InitializeMagick(NULL);
    // GetExceptionInfo(&exception);

    // ImageInfo * imageInfo = CloneImageInfo(0);
    // void * blob = FileToBlob("./faces/JadesFace_Eye.png", &blobSize, &exception);
    // Image * image = BlobToImage(imageInfo, blob, blobSize, &exception);

    // size_t blobSize2 = 0;
    // void * blob2 = FileToBlob("./Under_Starlit_Sky.png", &blobSize2, &exception);
    // Image * image2 = BlobToImage(imageInfo, blob2, blobSize2, &exception);

    // DrawInfo * drawInfo = CloneDrawInfo(0, 0);
    // DrawContext context = DrawAllocateContext(0, image);
    
    // DrawComposite(context, OverCompositeOp, 25, 25, 500, 500, image2);
    // DrawRender(context);

    // PixelPacket * pixels = GetImagePixels(image, 0, 0, image->columns, image->rows);
    
    // uint8_t imageBuffer[64 * 32 * 3];


    // for (int x = 0;x < 64;x++){
    //     for (int y = 0;y < 32;y++){

    //         int readXPixel = image->columns * x / 64;
    //         int readYPixel = image->rows * y / 32;

    //         PixelPacket * pixel = pixels + readXPixel + readYPixel * image->columns;

    //         int index = 3 * (x + y * 64);

    //         imageBuffer[index + 0] = pixel->red;
    //         imageBuffer[index + 1] = pixel->green;
    //         imageBuffer[index + 2] = pixel->blue;
    //     }
    // }
    

    // set_image(ledCanvas, 0, 0, imageBuffer, sizeof(imageBuffer), 64, 32, 0);

    while (true){
        usleep(10000);
        // sleep(1); 
        // render_face(ledCanvas, 0);
        // write(STDOUT_FILENO, "Hi!", 3);
        // printf("Hi!");
        
    }


    led_matrix_delete(matrix);
    delete_font(font);
}