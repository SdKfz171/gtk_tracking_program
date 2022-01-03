#include <stdio.h>
#include <stdlib.h>

#include <json.h>
#include <gtk/gtk.h>
#include <sqlite3.h>
#include <curl/curl.h>

// GtkWidget
GtkBuilder * builder;
GtkWidget * window;

void on_search_button_clicked(GtkButton * self)
{
    puts("search button clicked");
}

void carrier_combo_changed(GtkComboBoxText * self)
{
    puts("carrier combobox changed");
}

void delete_delete_button_clicked(GtkWidget * self)
{
    puts("delete delete button");
}

void delete_close_button_clicked(GtkWidget * self)
{
    puts("delete close button");
}

void del_prev_track_menu_activate(GtkWidget * self)
{
    puts("del menu");
}

void add_add_button_clicked(GtkWidget * self)
{
    puts("add add button");
}

void add_close_button_clicked(GtkWidget * self)
{
    puts("add close button");
}

void add_prev_track_menu_activate(GtkWidget * self)
{
    puts("add menu");
}

void track_track_button_clicked(GtkWidget * self)
{
    puts("track track button");
}

void track_close_button_clicked(GtkWidget * self)
{
    puts("track close button");
}

void track_track_menu_activate(GtkWidget * self)
{
    puts("track menu");
}

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

void on_destroy (GtkWidget * self)
{
    gtk_main_quit();
}