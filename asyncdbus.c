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

void parseIntrospectable(const char * busName, const char * objectPath, const char * introspectOutput){

}

// To store the members of an interface
HashableTable interfaceCache;
// To store what interfaces are supported by an object
HashableTable objectInterfaceCache;

int main_a() {
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

    DBusMessage * message = dbus_message_new_method_call("org.bluez", "/org/bluez/hci0", "org.freedesktop.DBus.Introspectable", "Introspect");

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

    printf("Ready!");

    DBusMessageIter iter;
    dbus_message_iter_init(asyncMessage.replyMessage, &iter);

    char * output;

    dbus_message_iter_get_basic(&iter, &output);

    printf("%s", output);

    parseIntrospectable(output);

    // dbus_connection_send_with_reply()
    return 0;
}
