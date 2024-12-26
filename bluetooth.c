// #include <dbus/dbus.h>


// int main(void){
//     DBusConnection * systemConnection;
//     DBusError error;

//     dbus_error_init(&error);

//     systemConnection = dbus_bus_get(DBUS_BUS_SYSTEM, &error);

//     if (systemConnection == 0){
//         return 1;
//     }

    

// }

#include <dbus/dbus.h>
#include <stdio.h>

int main() {
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

    dbus_connection_flush(connection);
    dbus_message_unref(message);

    message = dbus_message_new_method_call("org.bluez", "/org/bluez/hci0", "org.freedesktop.DBus.Properties", "Get");

    const char * interfaceName2 = "org.bluez.Adapter1";
    const char * memberName2 = "Powered";

    dbus_message_iter_init_append(message, &iter);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &interfaceName2);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &memberName2);

    dbus_connection_read_write_dispatch(connection, 1000);

    DBusMessage * receivedMessage = dbus_connection_send_with_reply_and_block(connection, message, 1000, &error);

    if (dbus_error_is_set(&error)){
        printf("%s", error.message);
        return 1;
    }

    DBusMessageIter rIter;

    dbus_message_iter_init(receivedMessage, &rIter);

    dbus_bool_t outputValue = 0;

    char * signature = dbus_message_iter_get_signature(&rIter);


    DBusMessageIter rSub;
    dbus_message_iter_recurse(&rIter, &rSub);

    dbus_message_iter_get_basic(&rSub, &outputValue);
    printf("%s\n", signature);

    printf("%d\n", outputValue);

    return 0;
}
