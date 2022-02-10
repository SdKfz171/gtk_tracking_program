#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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

GtkImage * node_images[4];

GtkDialog *add_dialog;
GtkListBox *add_listbox;
GtkEntryBuffer *add_entry_buffer;
GtkComboBoxText *add_carrier_combo;

GtkWidget *del_dialog;
GtkListBox *del_listbox;
GtkTextBuffer *del_buffer;
GtkTextIter del_start, del_end;

GtkWidget *track_dialog;

char image_paths[4][2][BUFSIZ] = {
                                {"./Image/warehouse.png", "./Image/warehouse_red.png"}, 
                                {"./Image/box_close.png", "./Image/box_close_red.png"}, 
                                {"./Image/truck.png", "./Image/truck_red.png"}, 
                                {"./Image/box_open.png", "./Image/box_open_red.png"}};

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
    int response_code = HttpGet(url, &response);
    if(response_code == 404){
        GtkWidget * dialog = gtk_message_dialog_new (NULL, 0,
                                                    GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                                    "%s", ("Invoice Error"));
        gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog),
                                                    ("Invoice not exist!!\r\n\r\nYour query: [ %s | %s ]"), invoice, carrier);

        gtk_dialog_run (GTK_DIALOG (dialog));

        gtk_widget_destroy (dialog);
        
        return -1;
    }

    puts(response);
    puts("testestestsssss");
    TrackingData tracking_data;
    GetTrackingData(response, &tracking_data);
    puts("get tracking check");

    gtk_text_buffer_get_bounds(out_buffer, &start, &end);
    gtk_text_buffer_delete(out_buffer, &start, &end);

    gtk_text_buffer_get_bounds(out_buffer, &start, &end);
    make_progress_form(tracking_data, out_buffer);

    
    if(!mode){
        typedef union
        {
            struct 
            {
                bool start_node;
                bool middle_node;
                bool end_node;
                bool delivered;
            } type;
            bool nodes[4]; 
            int hex;
        } nodes_t;
        
        nodes_t nodes;
        nodes.hex = 0;
        
        if(strcmp(tracking_data.state.id, "at_pickup") == 0){
            nodes.type.start_node = true;
        }
        else if(strcmp(tracking_data.state.id, "in_transit") == 0){
            nodes.type.middle_node = true;
        }
        else if(strcmp(tracking_data.state.id, "out_for_delivery") == 0){
            nodes.type.end_node = true;
        }
        else if(strcmp(tracking_data.state.id, "delivered") == 0){
            nodes.type.delivered = true;
        }

        if(nodes.hex != 0){
            for(int i = 0; i < 4; i++){
                printf("path: %s\r\n", image_paths[i][nodes.nodes[i]]);
                gtk_image_clear(GTK_IMAGE(node_images[i]));
                gtk_image_set_from_file(GTK_IMAGE(node_images[i]), image_paths[i][nodes.nodes[i]]);
            }
        }
    }
}

void prev_track_invoce(char * invoice, char * carrier_id)
{
    // char header[BUFSIZ];
    // sprintf(header, "<big>-----------------------\n<b>운송장 번호: %s</b>\n<b>택배사: %s</b>\n-----------------------</big>\n\n", invoice, find_carrier_name(carrier_id));

    track_invoice(invoice, carrier_id, prev_buffer, 1);

    // gtk_text_buffer_insert_markup(prev_buffer, &start, header, strlen(header));
}

void load_prev_tracking_data()
{
    printf("loading...\r\n");

    open_db("prev_track_table.db");
    select_table("TrackData");
    close_db();

    ListElmt * element;
    TrackingOption * option;
    
    element = list_head(&prev_track_list);

    int list_box_count = 0;
    for(int i = 0; i < list_size(&prev_track_list); i++){
        GtkTextView * temp_textview;
        GtkWidget * separator;

        option = list_data(element);
        
        prev_track_invoce(option->tracking_number, option->carrier_id);
        puts("debug test1");
        char header[BUFSIZ];
        sprintf(header, "<big>-----------------------\n<b>운송장 번호: %s</b>\n<b>택배사: %s</b>\n-----------------------</big>\n\n", option->tracking_number, find_carrier_name(option->carrier_id));

        gchar * temp_str;
        GtkTextBuffer * temp_buffer;
        temp_buffer = gtk_text_buffer_new(NULL);

        gtk_text_buffer_get_bounds(prev_buffer, &start, &end);
        temp_str = gtk_text_buffer_get_text(prev_buffer, &start, &end, FALSE);
        puts("debug test3");
        gtk_text_buffer_set_text(temp_buffer, temp_str, -1);
        gtk_text_buffer_get_bounds(temp_buffer, &start, &end);

        gtk_text_buffer_insert_markup(temp_buffer, &start, header, strlen(header));

        // text view create et config
        temp_textview = GTK_TEXT_VIEW(gtk_text_view_new_with_buffer(temp_buffer));
        gtk_text_view_set_editable(temp_textview, FALSE);
        gtk_text_view_set_wrap_mode(temp_textview, GTK_WRAP_CHAR);
        gtk_text_view_set_cursor_visible(temp_textview, FALSE);
        gtk_text_view_set_left_margin(temp_textview, 2);
        gtk_text_view_set_right_margin(temp_textview, 2);
        gtk_text_view_set_top_margin(temp_textview, 5);
        gtk_text_view_set_bottom_margin(temp_textview, 5);
        gtk_text_view_set_monospace(temp_textview, TRUE);
        gtk_widget_show(temp_textview);

        separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
        gtk_widget_show(separator);

        gtk_list_box_insert(latest_progress_listbox, GTK_WIDGET(temp_textview), -1);
        gtk_list_box_insert(latest_progress_listbox, GTK_WIDGET(separator), -1);
        
        list_box_count++;
        
        element = list_next(element);
    }    
    puts("debug test2");
}

void remove_all_latest_listbox_rows()
{
    GList * children = gtk_container_get_children(latest_progress_listbox);
    int latest_list_count = g_list_length(children);
    printf("%d\r\n", latest_list_count);
    
    for(; latest_list_count > 0; latest_list_count--){
        gtk_container_remove(latest_progress_listbox, gtk_list_box_get_row_at_index(latest_progress_listbox, latest_list_count-1));
    }
}

void init_add_listbox()
{
    open_db("prev_track_table.db");
    select_table("TrackData");
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

void init_del_listbox()
{
    open_db("prev_track_table.db");
    select_table("TrackData");
    close_db();

    ListElmt * element;
    TrackingOption * option;

    element = list_head(&prev_track_list);
    // puts("List Elements");
    for(int i = 0; i < list_size(&prev_track_list); i++)
    {
        GtkLabel * temp_label;
        GtkWidget * separator;

        option = list_data(element);

        char buf[512];
        sprintf(buf, "%2d |\t%s\t|\t%s", i, option->tracking_number, option->carrier_id);
        
        // label create et config
        temp_label = GTK_LABEL(gtk_label_new(buf));
        gtk_label_set_xalign(temp_label, 0);
        gtk_widget_set_margin_bottom(temp_label, 4);
        gtk_widget_set_margin_top(temp_label, 4);
        gtk_widget_show(temp_label);

        gtk_list_box_insert(del_listbox, GTK_WIDGET(temp_label), -1);

        element = list_next(element);
    }
}

void remove_all_del_listbox_rows()
{
    GList * children = gtk_container_get_children(del_listbox);
    int del_list_count = g_list_length(children);
    printf("%d\r\n", del_list_count);
    
    for(; del_list_count > 0; del_list_count--){
        gtk_container_remove(del_listbox, gtk_list_box_get_row_at_index(del_listbox, del_list_count-1));
    }
}

void open_del_dialog()
{
    init_del_listbox();

    gtk_widget_show(del_dialog);
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
    GList * selected_rows = gtk_list_box_get_selected_rows(del_listbox);
    int del_list_count = g_list_length(selected_rows);
    printf("%d\r\n", del_list_count);

    open_db("prev_track_table.db");  
    for(; selected_rows != NULL; selected_rows = selected_rows->next){
        int id;
        char del_invoice[20];
        char del_carrier[64];
        char sql[BUFSIZ];
        
        GtkLabel * label = GTK_LABEL(gtk_bin_get_child(GTK_BIN(selected_rows->data)));
        sscanf(gtk_label_get_text(label), "%2d |\t%s\t|\t%s", &id, del_invoice, del_carrier);

        sprintf(sql, "delete from %s where Invoice='%s' and Carrier='%s';", "TrackData", del_invoice, del_carrier);
        puts(sql);
        sql_execute(sql);

        gtk_container_remove(del_listbox, GTK_WIDGET(selected_rows->data));
    }
    close_db();

    g_list_free(selected_rows);

    // load_prev_tracking_data();
    remove_all_latest_listbox_rows();
    load_prev_tracking_data();
}

#if defined(_WIN32) || defined(_WIN64)
G_MODULE_EXPORT
#endif
void delete_listbox_selected_rows_changed (GtkListBox* box)
{
    GList * selected_rows = gtk_list_box_get_selected_rows(del_listbox);
    int del_list_count = g_list_length(selected_rows);
    printf("%d\r\n", del_list_count);
    
    gtk_text_buffer_set_text(del_buffer, "", -1);
    for(; selected_rows != NULL; selected_rows = selected_rows->next){
        char temp[BUFSIZ];
        GtkLabel * label = GTK_LABEL(gtk_bin_get_child(GTK_BIN(selected_rows->data)));
        sprintf(temp, "%s\r\n", gtk_label_get_text(label));
        gtk_text_buffer_insert_at_cursor(del_buffer, temp, -1);
    }
    g_list_free(selected_rows);
}

#if defined(_WIN32) || defined(_WIN64)
G_MODULE_EXPORT
#endif
void delete_close_button_clicked(GtkWidget *self)
{
    remove_all_del_listbox_rows();

    gtk_widget_hide(del_dialog);
}

#if defined(_WIN32) || defined(_WIN64)
G_MODULE_EXPORT
#endif
void del_prev_track_menu_activate(GtkWidget *self)
{
    open_del_dialog();
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
    int ret = insert_values("TrackData", 2, "Invoice", "Carrier", invoice_p, carrier_p);
    // select_table("TrackData");
    close_db();

    if(ret)
    {
        // hide dialog or select refresh
        // gtk_widget_hide(add_dialog);
        remove_all_add_listbox_rows();
        init_add_listbox();

        // remove_all_latest_listbox_rows();
        // load_prev_tracking_data();
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
    remove_all_add_listbox_rows();
    
    gtk_widget_hide(add_dialog);
}

gboolean add_dialog_delete_event(GtkWidget* self, GdkEvent* event, gpointer user_data)
{
    remove_all_add_listbox_rows();

    gtk_widget_hide(add_dialog);
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
    // create_table("TrackData", 1, 3, "Id integer primary key autoincrement", "Invoice text", "Carrier text");
    insert_values("TrackData", 2, "Invoice", "Carrier", "'6078990235407'", "'kr.epost'");
    insert_values("TrackData", 2, "Invoice", "Carrier", "'6078990235408'", "'kr.epost'");
    insert_values("TrackData", 2, "Invoice", "Carrier", "'6078990235409'", "'kr.epost'");
    select_table("TrackData");
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
    int response_code;
    response_code = HttpGet("https://apis.tracker.delivery/carriers", &response);
    if(response_code == 200){
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

    node_images[0] = GTK_IMAGE(gtk_builder_get_object(builder, "start_node_image"));
    node_images[1] = GTK_IMAGE(gtk_builder_get_object(builder, "middle_node_image"));
    node_images[2] = GTK_IMAGE(gtk_builder_get_object(builder, "end_node_image"));
    node_images[3] = GTK_IMAGE(gtk_builder_get_object(builder, "delivered_image"));

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

    load_prev_tracking_data();

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