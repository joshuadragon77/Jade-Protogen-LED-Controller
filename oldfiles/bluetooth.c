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
    
    GError * error = 0;

    DBusGConnection * connection =  dbus_g_bus_get(
        DBUS_BUS_SYSTEM, 
        &error
    );


    DBusGProxy * proxy = dbus_g_proxy_new_for_name(
        connection,
        "org.bluez",
        "/org/bluez/hci0",
        "org.freedesktop.DBus.Properties"
    );

    char e []= "org.bluez.Adapter1";
    char f []= "Powered";

    dbus_g_proxy_call(
        proxy, 
        "Set",
        &error,
        G_TYPE_STRING, &e,
        G_TYPE_STRING, &f,
        G_TYPE_VARIANT, G_TYPE_BOOLEAN, 1
    );

    return 0;
}
