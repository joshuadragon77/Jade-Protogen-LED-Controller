#include "macros.h"

#include "asyncdbus.h"

typedef struct _DBusAsyncMessage{
    DBusMessage * replyMessage;

    int ready;
} DBusAsyncMessage;

DBusPendingCall * pendingDBusCalls[255];
DBusAsyncMessage * pendingDBusAsnycMessages[255];

int callsCount = 0;

void updateLoopEvent(DBusConnection * connection, int timeout_ms){
    dbus_connection_read_write_dispatch(connection, timeout_ms);

    int completedCallsCount = 0;

    for (int i = 0;i<callsCount;i++){
        if (dbus_pending_call_get_completed(pendingDBusCalls[i])){
            pendingDBusAsnycMessages[i]->ready = 1;
            pendingDBusAsnycMessages[i]->replyMessage = dbus_pending_call_steal_reply(pendingDBusCalls[i]);

            completedCallsCount += 1;
        }else{
            pendingDBusCalls[i - completedCallsCount] = pendingDBusCalls[i];  
            pendingDBusAsnycMessages[i - completedCallsCount] = pendingDBusAsnycMessages[i];  
        }
    }

    callsCount -= completedCallsCount;
}

void sendAsynchronousDBusMessage(DBusConnection * connection, DBusMessage * message, int timeout_ms, DBusAsyncMessage * asyncMessage){
    dbus_connection_send_with_reply(connection, message, &pendingDBusCalls[callsCount], timeout_ms);
    pendingDBusAsnycMessages[callsCount ++] = asyncMessage;

    asyncMessage->replyMessage = 0;
    asyncMessage->ready = 0;
}

int matchCharacter(char ** readHead, const char targetCharacter, unsigned int maxSearchCount){
    unsigned int count = 0;
    while (**readHead != targetCharacter){
        if (**readHead == 0 || count >= maxSearchCount){
            return -1;
        }
        *readHead += 1;
        count ++;
    }

    return 0 ;
}

int matchElement(char ** readHead, char ** start, char ** end){
    *end = *start = 0;

    if (matchCharacter(readHead, '<', -1) == -1){
        return -1;
    }
    *start = *readHead;
    *readHead += 1;

    if (matchCharacter(readHead, '>', -1) == -1){
        return -1;
    }

    *end = *readHead;
    *readHead += 1;
    
    return 0;
}

char getElementType(const char * readHead){
    char type = *(readHead + 1);

    switch(type){
        case '/':
        case '!':{
            return type;
        }
        default:{
            return '\0';
        }
    }
}

void copyStringSub(char * dest, char * start, char * end){
    unsigned int length = end - start;
    for (unsigned int index = 0;index<length;index++){
        *(dest + index) = *(start + index);
    }
    *(dest + length) = 0;
}

int getElementName(char * readHead, char * maximumReadHead, char ** start, char ** end){
    if (getElementType(readHead)){
        ++ readHead;
    }

    *start = ++ readHead;
    if (matchCharacter(&readHead, ' ', maximumReadHead - readHead) == -1){
        *end = maximumReadHead;
        return -1;
    };

    *end = readHead;

    return 0;
}

int getAttributeValue(char * readHead, char * maximumReadHead, const char * attributeName, char ** start, char ** end){

    unsigned int totalMatchesRequired = strlen(attributeName);

    while (readHead < maximumReadHead){

        unsigned int matchIndex = 0;
        for (;matchIndex < totalMatchesRequired;matchIndex++){
            readHead ++;
            if (matchCharacter(&readHead, *(attributeName + matchIndex), 1) == -1){
                break;
            }
        }


        if (matchIndex == totalMatchesRequired && matchCharacter(&readHead, '=', 1) == 0){
            matchCharacter(&readHead, '"', 1);
            *start = readHead += 1;
            matchCharacter(&readHead, '"', maximumReadHead - readHead);
            *end = readHead;
            return 0;
        }
    }

    return -1;
}

typedef struct _DBusInterface{
    HashableTable methods;
    HashableTable properties;
} DBusInterface;

typedef struct _DBusProperty{
    char propertyType[6];
} DBusProperty;

typedef enum _DBusArgumentDirection {
    DBUSArgumentDir_Out,
    DBUSArgumentDir_In
} DBusArgumentDirection;

typedef struct _DBusArgument{
    char type[6];
    DBusArgumentDirection direction;
} DBusArgument;


typedef struct _DBusMethod{
    DBusArgument arguments[6];
    unsigned int argumentLength;
} DBusMethod;

// To store the members of an interface
HashableTable interfaceCache;

void cacheIntrospectXML(char * introspectOutput){
    char * readHead = introspectOutput;
    unsigned int introspectOutputLength = strlen(introspectOutput);

    char * start, * end;

    char * startBuffer, * endBuffer;
    char buffer[255];
    
    while (matchElement(&readHead, &start, &end) != -1){

        getElementName(start, end, &startBuffer, &endBuffer);
        copyStringSub(buffer, startBuffer, endBuffer);

        if (getElementType(start) == '!'){
            continue;
        }

        if (getElementType(start) != '/'){

            if (strcmp("interface", buffer) == 0){
                char buffer3[255];

                getAttributeValue(start, end, "name", &startBuffer, &endBuffer);
                copyStringSub(buffer, startBuffer, endBuffer);


                printf("Interface: %s\n", buffer);

                if (hasValueFromHashableTable(&interfaceCache, buffer) == -1){

                    DBusInterface * interface = (DBusInterface *)malloc(sizeof(DBusInterface));

                    initHashableTable(&interface->methods, 100);
                    initHashableTable(&interface->properties, 100);

                    setValueToHashableTable(&interfaceCache, buffer, interface);

                    while (matchElement(&readHead, &start, &end) != -1){
                        getElementName(start, end, &startBuffer, &endBuffer);
                        copyStringSub(buffer, startBuffer, endBuffer);

                        if (getElementType(start) != '/'){
                            if (strcmp("method", buffer) == 0){
                                getAttributeValue(start, end, "name", &startBuffer, &endBuffer);
                                copyStringSub(buffer, startBuffer, endBuffer);

                                DBusMethod * method = (DBusMethod *)malloc(sizeof(DBusMethod));

                                setValueToHashableTable(&interface->methods, buffer, method);

                                printf("Method: %s\n", buffer);

                                unsigned int argumentSize = 0;

                                while (matchElement(&readHead, &start, &end) != -1){

                                    getElementName(start, end, &startBuffer, &endBuffer);
                                    copyStringSub(buffer, startBuffer, endBuffer);

                                    if (strcmp("arg", buffer) == 0){

                                        getAttributeValue(start, end, "name", &startBuffer, &endBuffer);
                                        copyStringSub(buffer, startBuffer, endBuffer);

                                        printf("Argument: %s\n", buffer);

                                        getAttributeValue(start, end, "type", &startBuffer, &endBuffer);
                                        copyStringSub(buffer, startBuffer, endBuffer);

                                        strcpy(method->arguments[argumentSize].type, buffer);

                                        printf("Argument Type: %s\n", buffer);

                                        getAttributeValue(start, end, "direction", &startBuffer, &endBuffer);
                                        copyStringSub(buffer, startBuffer, endBuffer);

                                        method->arguments[argumentSize].direction = strcmp(buffer, "out") == 0 ? DBUSArgumentDir_Out : DBUSArgumentDir_In;

                                        printf("Argument Direction: %s\n", buffer);

                                        argumentSize += 1;
                                    }else{
                                        break;
                                    }
                                }

                                method->argumentLength = argumentSize;
                            }
                            if (strcmp("property", buffer) == 0){

                                getAttributeValue(start, end, "name", &startBuffer, &endBuffer);
                                copyStringSub(buffer, startBuffer, endBuffer);

                                DBusProperty * property = (DBusProperty *)malloc(sizeof(DBusProperty));

                                setValueToHashableTable(&interface->properties, buffer, property);

                                getAttributeValue(start, end, "type", &startBuffer, &endBuffer);
                                copyStringSub(buffer, startBuffer, endBuffer);

                                strcpy(property->propertyType, buffer);
                            }
                        }else{
                            getElementName(start, end, &startBuffer, &endBuffer);
                            copyStringSub(buffer, startBuffer, endBuffer);

                            if (strcmp("interface", buffer) == 0){
                                break;
                            }
                        }
                    }
                }

            }
        };
    }
    
    DBusInterface * interface = 0;
    DBusMethod * method = 0;
}

int main() {
    initHashableTable(&interfaceCache, 500);

    DBusConnection *connection;
    DBusError error;
    DBusAsyncMessage asyncMessage;

    // Initialize the D-Bus error structure
    dbus_error_init(&error);

    // Connect to the system bus
    connection = dbus_bus_get(DBUS_BUS_SYSTEM, &error);
    if (dbus_error_is_set(&error)) {
        fprintf(stderr, "Connection Error (%s)\n", error.message);
        dbus_error_free(&error);
        return 1;
    }
    if (connection == NULL) {
        return 1;
    }

    DBusMessage * message = dbus_message_new_method_call("org.bluez", "/org/bluez/hci1", "org.freedesktop.DBus.Introspectable", "Introspect");

    // const char * interfaceName2 = "org.bluez.Adapter1";
    // const char * memberName2 = "Powered";

    // {
    //     DBusMessageIter iter;
    //     dbus_message_iter_init_append(message, &iter);
    //     dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &interfaceName2);
    //     dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &memberName2);
    // }

    sendAsynchronousDBusMessage(connection, message, 1000, &asyncMessage);

    while (!asyncMessage.ready){
        updateLoopEvent(connection, 1000);
    }

    printf("Ready!\n");

    DBusMessageIter iter;
    dbus_message_iter_init(asyncMessage.replyMessage, &iter);

    char * output;

    dbus_message_iter_get_basic(&iter, &output);

    // printf("%s", output);

    cacheIntrospectXML(output);

    // dbus_connection_send_with_reply()
    return 0;
}
