#include "consolescript.h"

void systemNormaLog(const char * message){
    printf("\x1B[102m\x1B[30m[JadeOS][%s]\x1B[0m %s\n", "✅", message);
}

void systemWarningLog(const char * message){
    printf("\x1B[103m\x1B[30m[JadeOS][%s]\x1B[0m %s\n", "⚠️", message);
}

void systemVerboseLog(const char * message){
    printf("\x1B[106m\x1B[30m[JadeOS][%s]\x1B[0m %s\n", "📣", message);
}

void systemErrorLog(const char * message){
    printf("\x1B[101m\x1B[30m[JadeOS][%s]\x1B[0m %s\n", "❌", message);
}