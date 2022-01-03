#include <stdio.h>
#include <stdlib.h>

#include <json.h>
#include <gtk/gtk.h>
#include <sqlite3.h>
#include <curl/curl.h>

// GtkWidget
GtkBuilder * builder;
GtkWidget * window;

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);
    builder = gtk_builder_new_from_file("ui.glade");
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));

    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);

    gtk_widget_show(window);       
    gtk_main();

    return 0;
}