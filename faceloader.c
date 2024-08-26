#include "faceloader.h"
#include <regex.h>
#include <libxml/parser.h>

Image * eyeImage;
Image * mouthImage;
Image * noseImage;

void loadImage(const char * filePath, Image ** imageStoreLocation){
    ExceptionInfo exception;
    ImageInfo * imageInfo = CloneImageInfo(0);
    size_t imageBlobSize = 0;

    
    printf("Loading... %s\n", filePath);
    GetExceptionInfo(&exception);
    void * imageBlob = FileToBlob(filePath, &imageBlobSize, &exception);
    Image * image = BlobToImage(imageInfo, imageBlob, imageBlobSize, &exception);

    *imageStoreLocation = image;

    printf("Successfully loaded %s with bytesize of %lu\n", filePath, imageBlobSize);
}

struct _xmlNode * findFirstElement(const struct _xmlNode * node, const char * tagName, uint8_t recursive){
    struct _xmlNode * children = node->children;
    struct _xmlNode * searchingChild = children;


    while (searchingChild){
        if (strcmp((const char * )searchingChild->name, tagName) == 0){
            return searchingChild;
        }else{
            if (recursive && searchingChild->children){
                struct _xmlNode * attemptedRecursiveSearch = findFirstElement(searchingChild, tagName, recursive);

                if (attemptedRecursiveSearch){
                    return attemptedRecursiveSearch;
                }
            }
        }

        searchingChild = searchingChild->next;
    }

    return 0;
}

void findFirstPropertyValue(const struct _xmlNode * node, const char * propertyName, char * propertyValue){
    struct _xmlAttr * properties = node->properties;
    struct _xmlAttr * searchingProperty = properties;

    while (searchingProperty){
        if (strcmp((const char *)searchingProperty->name, propertyName) == 0){

            strcpy(propertyValue, (const char *)searchingProperty->children->content);

            return;
        }

        searchingProperty = searchingProperty->next;
    }
}

struct PathPoint{
    float absoluteX;
    float absoluteY;

    struct PathPoint * next;
};

void searchForCharacterPattern(char * string, const char * pattern, char ** start, char ** end){
    const char * currentCharMatch = pattern;

    *start = 0;
    *end = 0;

    while (true){
        char character = *(string ++);

        if (character == 0)
            return;

        if (character == *currentCharMatch){
            currentCharMatch ++;
            if (*start == 0){
                *start = string - 1;
            }
            if (*currentCharMatch == 0){
                *end = string - 1;
                return;
            }
        }else{
            currentCharMatch = pattern;
        }
    }
}

void readSimpleSVG(const char * filePath){
    xmlDocPtr doc = xmlReadFile(filePath, NULL, 0);

    struct _xmlNode * svgElement = doc->children->next;
    
    struct _xmlNode * group = findFirstElement(svgElement, "g", 1);
    struct _xmlNode * path = findFirstElement(svgElement, "path", 1);

    char tempStringStore[50];

    findFirstPropertyValue(svgElement, "width", tempStringStore);
    float width = atof(tempStringStore); 

    findFirstPropertyValue(svgElement, "height", tempStringStore);
    float height = atof(tempStringStore);
    
    findFirstPropertyValue(svgElement, "viewBox", tempStringStore);

    char * readPointer = tempStringStore;

    for (int i = 0;i<2;i++)
        while (*(readPointer++) != ' '){};
    

    char tempStringStore2[10];
    char * writePointer = tempStringStore2;

    while (*(readPointer) != ' '){*(writePointer ++) = *(readPointer ++);};
    float viewBoxWidth = atof(tempStringStore2);


    readPointer++;
    for (int i = 0;i<10;i++)
        tempStringStore2[i] = 0;

    writePointer = tempStringStore2;

    while (*(readPointer) != ' '){*(writePointer ++) = *(readPointer ++);};

    float viewBoxHeight = atof(tempStringStore2);

    printf("%f %f %f %f", width, height, viewBoxWidth, viewBoxHeight);

    xmlFreeDoc(doc);
}

void load(){
    loadImage("./faces/JadesFace_Eye.png", &eyeImage);
    loadImage("./faces/JadesFace_Mouth.png", &mouthImage);
    loadImage("./faces/JadesFace_Nose.png", &noseImage);
}

int main(){
    readSimpleSVG("./faces/JadesFace_Eye.svg");
    return 0;
};