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

#include <dbus/dbus-glib.h>
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

#if 0
    // Define the characteristic path and value to write
    const char *char_path = "/org/bluez/hci0/dev_XX_XX_XX_XX_XX_XX/serviceXX/charYYYY";
    unsigned char value[] = { 0x01, 0x02, 0x03, 0x04 };

    // Write the value to the characteristic
    write_value(connection, char_path, value, sizeof(value) / sizeof(value[0]));
#else

    #if 1
    DBusMessage * message = dbus_message_new_method_call("org.bluez", "/org/bluez/hci0", "org.freedesktop.DBus.Properties", "Set");

    dbus_message_iter_init_append(message, &iter);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &"org.bluez.Adapter1\0");
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &"Powered\0");
    
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
    #else
    DBusMessage * message = dbus_message_new_method_call("org.bluez", "/org/bluez/hci0", "org.bluez.Adapter1", "StartDiscovery");


    if (!message){
        printf("Something bad happened here. #1");
        return 1;
    }
    
    if (!dbus_connection_send(connection, message, 0)){
        printf("Something bad happened here. #2");
        return 1;
    }

    dbus_connection_flush(connection);
    #endif

    // DBusPendingCall *pending;

    // dbus_pending_call_block(pending);
    // // Get the reply message
    // message = dbus_pending_call_steal_reply(pending);
    // if (message == NULL) {
    //     fprintf(stderr, "Reply Null\n");
    //     return 1;
    // }

    // // Check for errors
    // if (dbus_message_get_type(message) == DBUS_MESSAGE_TYPE_ERROR) {
    //     fprintf(stderr, "Error: %s\n", dbus_message_get_error_name(message));
    // }


#endif
    return 0;
}
