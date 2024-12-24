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

void write_value(DBusConnection *connection, const char *char_path, const unsigned char *value, int length) {
    DBusMessage *msg;
    DBusMessageIter args;
    DBusPendingCall *pending;
    DBusError error;
    dbus_error_init(&error);

    // Create a new method call message
    msg = dbus_message_new_method_call("org.bluez", char_path, "org.bluez.GattCharacteristic1", "WriteValue");
    if (!msg) {
        fprintf(stderr, "Message Null\n");
        return;
    }

    // Append arguments
    dbus_message_iter_init_append(msg, &args);
    dbus_message_iter_append_fixed_array(&args, DBUS_TYPE_BYTE, &value, length);

    // Append options dictionary (empty in this case)
    DBusMessageIter dict;
    dbus_message_iter_open_container(&args, DBUS_TYPE_ARRAY, "{sv}", &dict);
    dbus_message_iter_close_container(&args, &dict);

    // Send message and wait for a reply
    if (!dbus_connection_send_with_reply(connection, msg, &pending, -1)) {
        fprintf(stderr, "Out of memory\n");
        dbus_message_unref(msg);
        return;
    }
    if (pending == NULL) {
        fprintf(stderr, "Pending Call Null\n");
        dbus_message_unref(msg);
        return;
    }

    dbus_connection_flush(connection);
    dbus_message_unref(msg);

    // Block until we receive a reply
    dbus_pending_call_block(pending);

    // Get the reply message
    msg = dbus_pending_call_steal_reply(pending);
    if (msg == NULL) {
        fprintf(stderr, "Reply Null\n");
        return;
    }

    // Check for errors
    if (dbus_message_get_type(msg) == DBUS_MESSAGE_TYPE_ERROR) {
        fprintf(stderr, "Error: %s\n", dbus_message_get_error_name(msg));
    }

    // Clean up
    dbus_message_unref(msg);
    dbus_pending_call_unref(pending);
}

int main() {
    DBusConnection *connection;
    DBusError error;

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

    // Define the characteristic path and value to write
    const char *char_path = "/org/bluez/hci0/dev_XX_XX_XX_XX_XX_XX/serviceXX/charYYYY";
    unsigned char value[] = { 0x01, 0x02, 0x03, 0x04 };

    // Write the value to the characteristic
    write_value(connection, char_path, value, sizeof(value) / sizeof(value[0]));

    return 0;
}
