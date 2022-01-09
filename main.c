#include <stdio.h>
#include <stdlib.h>

#include <json.h>
#include <gtk/gtk.h>
#include <sqlite3.h>
#include <curl/curl.h>

#include "db.h"
#include "list.h"
#include "element.h"
#include "httpget.h"

List carrier_list;

// GtkWidget
GtkBuilder *builder;
GtkWidget *window;

GtkTextBuffer *buffer;
GtkTextIter start, end;

GtkTextBuffer *prev_buffer;
GtkTextIter prev_start, prev_end;

GtkEntryBuffer *entry_buffer;

GtkComboBoxText *carrier_combo;
GtkListBox *latest_progress_listbox;

GtkWidget *add_dialog;
GtkListBox *add_listbox;
GtkEntryBuffer *add_entry_buffer;
GtkComboBoxText *add_carrier_combo;

GtkWidget *del_dialog;
GtkListBox *del_listbox;
GtkTextBuffer *del_buffer;
GtkTextIter del_start, del_end;

GtkWidget *track_dialog;

char *find_carrier_id(char *name)
{
    ListElmt *element;
    Carrier *carrier;

    element = list_head(&carrier_list);
    while (1)
    {
        carrier = list_data(element);

        if (strcmp(carrier->name, name) == 0)
            return carrier->id;
        else
            element = list_next(element);
    }
    return NULL;
}

char *find_carrier_name(char *id)
{
    ListElmt *element;
    Carrier *carrier;

    element = list_head(&carrier_list);
    while (1)
    {
        carrier = list_data(element);

        if (strcmp(carrier->id, id) == 0)
            return carrier->name;
        else
            element = list_next(element);
    }
    return NULL;
}

int trim(char *str)
{
    int p = 0;
    int len = 0;
    char temp[BUFSIZ];

    len = strlen(str);
    if (len >= BUFSIZ)
        return -1;

    strcpy(temp, str);

    for (int i = 0; i < len; i++)
    {
        if (temp[i] == ' ' || temp[i] == '\t' || temp[i] == '\n')
            p++;
        else
            break;
    }

    for (int i = len - 2; i >= 0; i--)
    {
        if (temp[i] == ' ' || temp[i] == '\t' || temp[i] == '\n')
            temp[i] = '\0';
        else
            break;
    }

    strcpy(str, temp + p);

    return strlen(str);
}

void make_progress_form(TrackingData tracking_data, GtkTextBuffer *out_buffer)
{
    for (int i = 0; i < tracking_data.progress_count; i++)
    {
        char form[BUFSIZ];
        trim(tracking_data.progresses[i].location.name);
        sprintf(form, "단계\r\t%s\n시간\n\t%d-%02d-%02d %02d:%02d\n현재위치\n\t%s\n처리현황\n\t%s\n\n==============================\n\n",
                tracking_data.progresses[i].status.text, tracking_data.progresses[i].time.tm_year + 1900, tracking_data.progresses[i].time.tm_mon + 1, tracking_data.progresses[i].time.tm_mday, tracking_data.progresses[i].time.tm_hour, tracking_data.progresses[i].time.tm_min, tracking_data.progresses[i].location.name, tracking_data.progresses[i].description);

        gtk_text_buffer_insert(out_buffer, &start, form, -1);
        gtk_text_buffer_get_end_iter(out_buffer, &start);
    }
}

int track_invoice(char *invoice, char *carrier, GtkTextBuffer *out_buffer, int mode)
{
    // mode: 0(carrier.name), 1(carrier.id)

    char *response;
    char url[BUFSIZ];

    puts(invoice);
    puts(carrier);

    if (mode)
        sprintf(url, "https://apis.tracker.delivery/carriers/%s/tracks/%s", carrier, invoice);
    else
        sprintf(url, "https://apis.tracker.delivery/carriers/%s/tracks/%s", find_carrier_id(carrier), invoice);
    HttpGet(url, &response);

    puts(response);

    TrackingData tracking_data;
    GetTrackingData(response, &tracking_data);

    gtk_text_buffer_get_bounds(out_buffer, &start, &end);
    gtk_text_buffer_delete(out_buffer, &start, &end);

    gtk_text_buffer_get_bounds(out_buffer, &start, &end);
    make_progress_form(tracking_data, out_buffer);
}

void init_add_listbox()
{
    open_db("prev_track_table.db");
    select_table("test_table");
    close_db();

    ListElmt * element;
    TrackingOption * option;

    element = list_head(&prev_track_list);
    for(int i = 0; i < list_size(&prev_track_list); i++)
    {
        GtkLabel * temp_label;
        GtkWidget * separator;

        option = list_data(element);

        char buf[512];
        sprintf(buf, "%2d |\t%s\t|\t%s", i, option->tracking_number, option->carrier_id);
        
        // label create et config
        temp_label = GTK_LABEL(gtk_label_new(buf));
        gtk_label_set_selectable(temp_label, TRUE);
        gtk_label_set_xalign(temp_label, 0);
        gtk_widget_show(temp_label);

        separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
        gtk_widget_show(separator);

        gtk_list_box_insert(add_listbox, GTK_WIDGET(temp_label), -1);
        gtk_list_box_insert(add_listbox, GTK_WIDGET(separator), -1);

        element = list_next(element);
    }
}

void remove_all_add_listbox_rows()
{
    GList * children = gtk_container_get_children(add_listbox);
    int add_list_count = g_list_length(children);
    printf("%d\r\n", add_list_count);
    
    for(; add_list_count > 0; add_list_count--){
        gtk_container_remove(add_listbox, gtk_list_box_get_row_at_index(add_listbox, add_list_count-1));
    }
}

void open_add_dialog()
{
    init_add_listbox();

    gtk_widget_show(add_dialog);
}

/* =============================================== */
#if defined(_WIN32) || defined(_WIN64)
G_MODULE_EXPORT
#endif
void on_search_button_clicked(GtkButton *self)
{
    gchar *invoice;
    gchar *carrier;

    invoice = gtk_entry_buffer_get_text(entry_buffer);
    carrier = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(carrier_combo));

    if (strlen(invoice) == 0 || carrier == NULL)
    {
        printf("invoice number or carrier is empty!!\r\n");
        return;
    }

    puts("tracking start!");
    puts(invoice);
    puts(carrier);

    track_invoice(invoice, carrier, buffer, 0);
}

#if defined(_WIN32) || defined(_WIN64)
G_MODULE_EXPORT
#endif
void carrier_combo_changed(GtkComboBoxText *self)
{
    puts("carrier combobox changed");
}

// delete dialog
#if defined(_WIN32) || defined(_WIN64)
G_MODULE_EXPORT
#endif
void delete_delete_button_clicked(GtkWidget *self)
{
    puts("delete delete button");
}

#if defined(_WIN32) || defined(_WIN64)
G_MODULE_EXPORT
#endif
void delete_close_button_clicked(GtkWidget *self)
{
    puts("delete close button");
}

#if defined(_WIN32) || defined(_WIN64)
G_MODULE_EXPORT
#endif
void del_prev_track_menu_activate(GtkWidget *self)
{
    puts("del menu");
}

// add dialog
#if defined(_WIN32) || defined(_WIN64)
G_MODULE_EXPORT
#endif
void add_add_button_clicked(GtkWidget *self)
{
    gchar * invoice;
    gchar * carrier;

    invoice = gtk_entry_buffer_get_text(add_entry_buffer);
    carrier = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(add_carrier_combo));

    if(strlen(invoice) == 0 || carrier == NULL){
        printf("strlen : %d\r\n", strlen(invoice));
        printf("invoice number or carrier is empty!!\r\n");    
        return;
    }

    char invoice_p[32];
    char carrier_p[32];
    sprintf(invoice_p, "'%s'", invoice);
    sprintf(carrier_p, "'%s'", find_carrier_id(carrier));

    open_db("prev_track_table.db");
    int ret = insert_values("test_table", 2, "Invoice", "Carrier", invoice_p, carrier_p);
    // select_table("test_table");
    close_db();

    if(ret)
    {
        // hide dialog or select refresh
        // gtk_widget_hide(add_dialog);
        remove_all_add_listbox_rows();
        init_add_listbox();
    }
    else {
        // show error dialog
        GtkWidget * dialog = gtk_message_dialog_new (NULL, 0,
                                                    GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                                    "%s", ("SQL Error"));
        gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog),
                                                    ("SQL insert value error.\r\nreturn value: %d"), ret);

        gtk_dialog_run (GTK_DIALOG (dialog));

        gtk_widget_destroy (dialog);

    }
}

#if defined(_WIN32) || defined(_WIN64)
G_MODULE_EXPORT
#endif
void add_close_button_clicked(GtkWidget *self)
{
    puts("add close button");
}

#if defined(_WIN32) || defined(_WIN64)
G_MODULE_EXPORT
#endif
void add_prev_track_menu_activate(GtkWidget *self)
{
    open_add_dialog();
}

// track dialog
#if defined(_WIN32) || defined(_WIN64)
G_MODULE_EXPORT
#endif
void track_track_button_clicked(GtkWidget *self)
{
    puts("track track button");
}

#if defined(_WIN32) || defined(_WIN64)
G_MODULE_EXPORT
#endif
void track_close_button_clicked(GtkWidget *self)
{
    puts("track close button");
}

#if defined(_WIN32) || defined(_WIN64)
G_MODULE_EXPORT
#endif
void track_track_menu_activate(GtkWidget *self)
{
    puts("track menu");
}

int set_carrier_combobox_elements(GtkComboBoxText *combo)
{
    ListElmt *element;
    Carrier *carrier;

    gtk_combo_box_text_remove_all(combo);

    element = list_head(&carrier_list);
    puts("List Elements");
    while (1)
    {
        carrier = list_data(element);

        puts(carrier->id);
        puts(carrier->name);
        gtk_combo_box_text_append(combo, carrier->id, carrier->name);

        if (list_is_tail(element))
            break;
        else
            element = list_next(element);
    }

    return carrier_list.size;
}

int db_test_main()
{
    open_db("prev_track_table.db");
    create_table("test_table", 1, 3, "Id integer primary key autoincrement", "Invoice text", "Carrier text");
    insert_values("test_table", 2, "Invoice", "Carrier", "'1234567890'", "'kr.epost'");
    select_table("test_table");
    close_db();

    printf("%d\r\n", db_available());

    ListElmt * element;
    TrackingOption * option;

    element = list_head(&prev_track_list);
    while (1)
    {
        option = list_data(element);
        
        puts(option->tracking_number);
        puts(option->carrier_id);

        if(list_is_tail(element))
            break;
        else 
            element = list_next(element);
    }   
}

void init()
{
    // db_test_main();

    // http request carrier list
    char *response;

    HttpGet("https://apis.tracker.delivery/carriers", &response);
    puts(response);

    list_init(&carrier_list);

    for (int i = 0; i < GetCarrierCount(response); i++)
    {
        Carrier *temp = (Carrier *)malloc(sizeof(Carrier));
        memcpy(temp, &(GetCarriers(response)[i]), sizeof(Carrier));
        if (!list_size(&carrier_list))
            list_insert_next(&carrier_list, NULL, temp);
        else
            list_insert_next(&carrier_list, list_tail(&carrier_list), temp);
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
    add_listbox = GTK_LIST_BOX(gtk_builder_get_object(builder, "add_listbox"));
    add_entry_buffer = GTK_ENTRY_BUFFER(gtk_builder_get_object(builder, "add_entry_buffer"));
    add_carrier_combo = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder, "add_carrier_combobox"));

    del_dialog = GTK_WIDGET(gtk_builder_get_object(builder, "delete_dialog"));
    del_listbox = GTK_LIST_BOX(gtk_builder_get_object(builder, "delete_listbox"));
    del_buffer = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "delete_buffer"));

    int combo_size = set_carrier_combobox_elements(carrier_combo);
    printf("combo_size: %d\r\n", combo_size);

    set_carrier_combobox_elements(add_carrier_combo);

    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);

    gtk_widget_show(window);
    gtk_main();

    return 0;
}

#if defined(_WIN32) || defined(_WIN64)
G_MODULE_EXPORT
#endif
void on_destroy(GtkWidget *self)
{
    gtk_main_quit();
}