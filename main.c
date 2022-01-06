#include <stdio.h>
#include <stdlib.h>

#include <json.h>
#include <gtk/gtk.h>
#include <sqlite3.h>
#include <curl/curl.h>

#include "list.h"
#include "element.h"
#include "httpget.h"

List carrier_list;

// GtkWidget
GtkBuilder * builder;
GtkWidget * window;

GtkTextBuffer * buffer;
GtkTextIter start, end;

GtkTextBuffer * prev_buffer;
GtkTextIter prev_start, prev_end;

GtkEntryBuffer * entry_buffer;

GtkComboBoxText * carrier_combo;
GtkListBox * latest_progress_listbox;

GtkWidget * add_dialog;
GtkListBox * add_listbox;
GtkEntryBuffer * add_entry_buffer;
GtkComboBoxText * add_carrier_combo;

GtkWidget * del_dialog;
GtkListBox * del_listbox;
GtkTextBuffer * del_buffer;
GtkTextIter del_start, del_end;

GtkWidget * track_dialog;

void on_search_button_clicked(GtkButton * self)
{
    puts("search button clicked");
}

void carrier_combo_changed(GtkComboBoxText * self)
{
    puts("carrier combobox changed");
}


// delete dialog
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

// add dialog
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

// track dialog
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

void init()
{
    // http request carrier list
    char * response;

    HttpGet("https://apis.tracker.delivery/carriers", &response);
    puts(response);

    list_init(&carrier_list);

    for(int i = 0; i < GetCarrierCount(response); i++){
        Carrier * temp = (Carrier *)malloc(sizeof(Carrier));
        memcpy(temp, &(GetCarriers(response)[i]), sizeof(Carrier));
        if(!list_size(&carrier_list)) list_insert_next(&carrier_list, NULL, temp);
        else list_insert_next(&carrier_list, list_tail(&carrier_list),temp);   
    }
}

// main
int main(int argc, char *argv[])
{
    init();

    gtk_init(&argc, &argv);

    builder = gtk_builder_new_from_file("ui.glade");
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));

    buffer = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "s_progress_buffer"));
    prev_buffer = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "prev_buffer"));
    entry_buffer = GTK_ENTRY_BUFFER(gtk_builder_get_object(builder, "invoice_number_buffer"));
    carrier_combo = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder, "carrier_combobox"));
    latest_progress_listbox = GTK_LIST_BOX(gtk_builder_get_object(builder, "latest_progress_listbox"));

    add_dialog = GTK_WIDGET(gtk_builder_get_object(builder, "add_dialog"));
    add_listbox= GTK_LIST_BOX(gtk_builder_get_object(builder, "add_listbox"));
    add_entry_buffer = GTK_ENTRY_BUFFER(gtk_builder_get_object(builder, "add_entry_buffer"));
    add_carrier_combo = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder, "add_carrier_combobox"));

    del_dialog = GTK_WIDGET(gtk_builder_get_object(builder, "delete_dialog"));
    del_listbox = GTK_LIST_BOX(gtk_builder_get_object(builder, "delete_listbox"));
    del_buffer = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "delete_buffer"));

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