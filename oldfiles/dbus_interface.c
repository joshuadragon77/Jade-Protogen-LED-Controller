#include "macros.h"

#include "dbus_interface.h"

DBusConnection * connection;

int dbus_interface_connectBus(DBusBusType busType){
    DBusConnection * connection;
    DBusError error;

    dbus_error_init(&error);

    connection = dbus_bus_get(busType, &error);

    if (dbus_error_is_set(&error)){
        printf("Failed to connect to DBus with error %s\n", error.message);

        return 1;
    }

    if (!connection){
        printf("Failed to connect to DBus with unknown error");

        return 1;
    }

    return 0;
}

int dbus_interface_disconnectBus(){
    if (!connection){
        printf("Closing already closed DBus Connection.");
        return 1;
    }
    dbus_connection_close(connection);
    connection = 0;

    return 0;
}

size_t dbus_interface_obtainArgumentsBasicLength(DBusMessage * message){
    DBusMessageIter messageIterator;
    dbus_message_iter_init(message, &messageIterator);

    if (dbus_message_iter_get_arg_type(&messageIterator) == DBUS_TYPE_INVALID){
        return 0;
    };

    size_t argumentLength = 1;

    while (dbus_message_iter_has_next(&messageIterator)){
        dbus_message_iter_next(&messageIterator);
        argumentLength += 1;
    };

    return argumentLength;
}

DBus_Interface_ReturnArgs * dbus_interface_obtainArgumentsRecursive(DBusMessageIter * messageIterator, size_t length){
    DBus_Interface_ReturnArgs * argumentList = (DBus_Interface_ReturnArgs * )malloc(sizeof(DBus_Interface_ReturnArgs));

    argumentList->length = length;

    argumentList->cachedArguments = (void **)malloc(sizeof(void * ) * argumentList->length);
    argumentList->cachedArgumentsType = (int *)malloc(sizeof(int * ) * argumentList->length);

    for (size_t index = 0;index < argumentList->length;index ++){
        switch(argumentList->cachedArgumentsType[index] = dbus_message_iter_get_arg_type(messageIterator)){
            case DBUS_TYPE_VARIANT:{
                DBusMessageIter recursedIterator;
                dbus_message_iter_recurse(messageIterator, &recursedIterator);
                argumentList->cachedArguments[index] = dbus_interface_obtainArgumentsRecursive(&recursedIterator, 1);
                break;
            }
            default:{
                dbus_message_iter_get_basic(messageIterator, &argumentList->cachedArguments[index]);
            }
        }

        dbus_message_iter_next(messageIterator);
    };

    return argumentList;
}

DBus_Interface_ReturnArgs * dbus_interface_obtainArgumentList(DBusMessage * message){
    size_t argumentLength = dbus_interface_obtainArgumentsBasicLength(message);

    DBusMessageIter messageIterator;
    dbus_message_iter_init(message, &messageIterator);

    return dbus_interface_obtainArgumentsRecursive(&messageIterator, argumentLength);
}

// DBus_Interface_ReturnArgs * dbus_interface_method_call(
//     const char * busName, 
//     const char * objectPath,
//     const char * interfaceName,
//     const char * memberName,
//     DBus_Interface_MethodArg * arguments,
    
//     ){

//     DBusMessage * message = dbus_message_new_method_call(
//         busName,
//         objectPath,
//         interfaceName,
//         memberName
//     );
// }

void * dbus_interface_unpackageVariantValue(DBus_Interface_ReturnArgs * returnArgs, unsigned int index){
    if (returnArgs->cachedArgumentsType[index] == 'v'){
        return dbus_interface_unpackageVariantValue((DBus_Interface_ReturnArgs *)returnArgs->cachedArguments[index], index);
    }

    return returnArgs->cachedArguments[index];
}

int dbus_interface_unpackageVariantType(DBus_Interface_ReturnArgs * returnArgs, unsigned int index){
    if (returnArgs->cachedArgumentsType[index] == 'v'){
        return dbus_interface_unpackageVariantType((DBus_Interface_ReturnArgs *)returnArgs->cachedArguments[index], index);
    }

    return returnArgs->cachedArgumentsType[index];
}

void * dbus_interface_readArgumentValue(DBus_Interface_ReturnArgs * returnArgs, unsigned int index){
    return dbus_interface_unpackageVariantValue(returnArgs, index);
}

int dbus_interface_readArgumentType(DBus_Interface_ReturnArgs * returnArgs, unsigned int index){
    return dbus_interface_unpackageVariantType(returnArgs, index);
}




// void getDBusProperty(const char * busName, const char * objectPath, const char * interface, const char * propertyName, void *){
//     DBusMessageIter messageIterator;

//     DBusMessage * message = dbus_message_new_method_call(busName, objectPath, "org.freedesktop.DBus.Properties", "Get");

// }

// DBusArgumentsContainer * initializeArgumentContainer(DBusArgumentsContainer * argumentContainer, size_t argumentLength){
//     argumentContainer->arguments = malloc(sizeof(void *) * argumentLength);
//     argumentContainer->argumentTypes = malloc(sizeof(int) * argumentLength);
//     argumentContainer->argumentLength = argumentLength;
// }

int main_a() {

    // initializeDBUSConnection(DBUS_BUS_SYSTEM);
    #if 1
    DBusConnection *connection;
    DBusError error;
    DBusMessageIter iter;

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

    DBusMessage * message = dbus_message_new_method_call("org.bluez", "/org/bluez/hci0", "org.freedesktop.DBus.Properties", "Set");

    const char * interfaceName = "org.bluez.Adapter1";
    const char * memberName = "Powered";

    dbus_message_iter_init_append(message, &iter);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &interfaceName);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &memberName);
    
    DBusMessageIter sub;
    dbus_message_iter_open_container(&iter, DBUS_TYPE_VARIANT, "b", &sub);
    dbus_bool_t value = 1;
    dbus_message_iter_append_basic(&sub, DBUS_TYPE_BOOLEAN, &value);
    dbus_message_iter_close_container(&iter, &sub);

    if (!message){
        printf("Something bad happened here. #1");
        return 1;
    }
    
    if (!dbus_connection_send(connection, message, 0)){
        printf("Something bad happened here. #2");
        return 1;
    }

    dbus_message_unref(message);

    message = dbus_message_new_method_call("org.bluez", "/org/bluez/hci0", "org.freedesktop.DBus.Properties", "Get");

    const char * interfaceName2 = "org.bluez.Adapter1";
    const char * memberName2 = "Powered";

    dbus_message_iter_init_append(message, &iter);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &interfaceName2);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &memberName2);

    dbus_connection_read_write_dispatch(connection, 1000);

    DBusPendingCall * pending;

    dbus_connection_send_with_reply(connection, message, &pending, 1000);

    while (!dbus_pending_call_get_completed(pending)){
        dbus_connection_read_write_dispatch(connection, 1000);
    }

    DBusMessage * receivedMessage = dbus_pending_call_steal_reply(pending);
    DBus_Interface_ReturnArgs * argumentList = dbus_interface_obtainArgumentList(receivedMessage);

    
    printf("%d\n", dbus_interface_readArgumentValue(argumentList, 0));
    printf("%c\n", dbus_interface_readArgumentType(argumentList, 0));

    printf("Size: %u\n", argumentList->length);

    #endif

    return 0;
}
