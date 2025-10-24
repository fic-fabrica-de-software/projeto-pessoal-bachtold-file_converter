#include <gtk/gtk.h>
#include "converter.h"
#include "fileutils.h"
#include <string.h>

static char *input_file_path = NULL;
static GtkWidget *file_label;
static GtkWidget *csv_button;
static GtkWidget *tsv_button;
static GtkWidget *txt_button;
static GtkWidget *json_button;

static void update_conversion_buttons(int file_type) {
    switch (file_type) {
        case FILE_TYPE_TXT:
            gtk_widget_set_sensitive(csv_button, TRUE);
            gtk_widget_set_sensitive(tsv_button, TRUE);
            gtk_widget_set_sensitive(txt_button, FALSE); 
            gtk_widget_set_sensitive(json_button, FALSE); 
            break;
        case FILE_TYPE_BMP:
            gtk_widget_set_sensitive(csv_button, FALSE);
            gtk_widget_set_sensitive(tsv_button, FALSE);
            gtk_widget_set_sensitive(txt_button, TRUE);
            gtk_widget_set_sensitive(json_button, FALSE);
            break;
        case FILE_TYPE_CSV:
            gtk_widget_set_sensitive(csv_button, FALSE);
            gtk_widget_set_sensitive(tsv_button, FALSE);
            gtk_widget_set_sensitive(txt_button, FALSE);
            gtk_widget_set_sensitive(json_button, TRUE);
            break;
        case FILE_TYPE_JSON:
            gtk_widget_set_sensitive(csv_button, TRUE);
            gtk_widget_set_sensitive(tsv_button, FALSE);
            gtk_widget_set_sensitive(txt_button, FALSE);
            gtk_widget_set_sensitive(json_button, FALSE);
            break;
        case FILE_TYPE_UNSUPPORTED:
        default:
            gtk_widget_set_sensitive(csv_button, FALSE);
            gtk_widget_set_sensitive(tsv_button, FALSE);
            gtk_widget_set_sensitive(txt_button, FALSE);
            gtk_widget_set_sensitive(json_button, FALSE);
            break;
    }
}

static void update_file_label(const char *filename) {
    char label_text[256];
    if (filename) {
        int file_type = get_file_type(filename);
        const char *type_desc = "";
        
        switch (file_type) {
            case FILE_TYPE_TXT:
                type_desc = " (Text file)";
                break;
            case FILE_TYPE_BMP:
                type_desc = " (Bitmap image)";
                break;
            case FILE_TYPE_CSV:
                type_desc = " (Comma-Separated Values file)";
                break;
            case FILE_TYPE_JSON:
                type_desc = " (JavaScript Object Notation file)";
                break;
            default:
                type_desc = " (Unsupported format)";
                break;
        }
        
        snprintf(label_text, sizeof(label_text), "Selected file: %s%s", filename, type_desc);
        update_conversion_buttons(file_type);
    } else {
        strcpy(label_text, "No file selected");
        update_conversion_buttons(FILE_TYPE_UNSUPPORTED);
    }
    gtk_label_set_text(GTK_LABEL(file_label), label_text);
}

static void on_drag_data_received(GtkWidget *widget, GdkDragContext *context, 
                                 gint x, gint y, GtkSelectionData *data, 
                                 guint info, guint time, gpointer user_data) {
    gchar **uris = gtk_selection_data_get_uris(data);
    if (uris) {
        gchar *file_uri = uris[0];
        gchar *file_path = g_filename_from_uri(file_uri, NULL, NULL);
        
        if (file_path) {
            if (input_file_path) {
                g_free(input_file_path);
            }
            
            input_file_path = file_path;
            update_file_label(input_file_path);
        }
        
        g_strfreev(uris);
    }
    
    gtk_drag_finish(context, TRUE, FALSE, time);
}

static void on_csv_conversion(GtkWidget *widget, gpointer data) {
    if (!input_file_path) return;
    
    int file_type = get_file_type(input_file_path);
    char *base_name = remove_extension(input_file_path);
    char output_path[256];
    int result = 1;
    const char *message = "";
    
    switch (file_type) {
        case FILE_TYPE_TXT:
            snprintf(output_path, sizeof(output_path), "%s.csv", base_name);
            result = TXTtoCSV(input_file_path, output_path);
            message = "Text file converted successfully to CSV!";
            break;
            
        case FILE_TYPE_JSON:
            snprintf(output_path, sizeof(output_path), "%s.csv", base_name);
            result = JSONtoCSV(input_file_path, output_path);
            message = "JSON file converted successfully to CSV!";
            break;
            
        default:
            message = "Unsupported file format for CSV conversion!";
            break;
    }
    
    free(base_name);
    
    if (result == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "%s\nSaved as: %s", message, output_path);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    } else {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Error: %s", message);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}

static void on_tsv_conversion(GtkWidget *widget, gpointer data) {
    if (!input_file_path) return;
    
    char *base_name = remove_extension(input_file_path);
    char output_path[256];
    snprintf(output_path, sizeof(output_path), "%s.tsv", base_name);
    
    int result = TXTtoTSV(input_file_path, output_path);
    
    free(base_name);
    if (result == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "File converted successfully to TSV!\nSaved as: %s",
            output_path);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    } else {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Error converting file to TSV!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}

static void on_txt_conversion(GtkWidget *widget, gpointer data) {
    if (!input_file_path) return;
    
    char *base_name = remove_extension(input_file_path);
    char output_path[256];
    snprintf(output_path, sizeof(output_path), "%s.txt", base_name);
    
    int result = BMPtoTXT(input_file_path, output_path);
    
    free(base_name);
    if (result == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "File converted successfully to TXT!\nSaved as: %s",
            output_path);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    } else {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Error converting file to TXT!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}

static void on_json_conversion(GtkWidget *widget, gpointer data) {
    if (!input_file_path) return;
    
    char *base_name = remove_extension(input_file_path);
    char output_path[256];
    snprintf(output_path, sizeof(output_path), "%s.json", base_name);
    
    int result = CSVtoJSON(input_file_path, output_path);
    
    free(base_name);
    if (result == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "File converted successfully to JSON!\nSaved as: %s",
            output_path);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    } else {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Error converting file to JSON!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}


static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *drop_area;
    GtkWidget *instructions;
    GtkWidget *button_box;
    GtkCssProvider *provider;
    
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "File Converter");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    
    provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "drop-area { background-color: #E0E0E0; }", -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    
    instructions = gtk_label_new("Drag and drop a text file to convert");
    gtk_box_pack_start(GTK_BOX(vbox), instructions, FALSE, FALSE, 0);
    
    drop_area = gtk_event_box_new();
    gtk_widget_set_size_request(drop_area, 300, 100);
    gtk_widget_set_name(drop_area, "drop-area"); 
    
    GtkWidget *drop_label = gtk_label_new("Drop file here");
    gtk_container_add(GTK_CONTAINER(drop_area), drop_label);
    
    gtk_drag_dest_set(drop_area, GTK_DEST_DEFAULT_ALL, NULL, 0, GDK_ACTION_COPY);
    gtk_drag_dest_add_uri_targets(drop_area);
    g_signal_connect(drop_area, "drag-data-received", G_CALLBACK(on_drag_data_received), NULL);
    
    gtk_box_pack_start(GTK_BOX(vbox), drop_area, FALSE, FALSE, 0);
    
    file_label = gtk_label_new("No file selected");
    gtk_box_pack_start(GTK_BOX(vbox), file_label, FALSE, FALSE, 0);
    
    button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10); 
    gtk_box_set_homogeneous(GTK_BOX(button_box), TRUE); 
    
    csv_button = gtk_button_new_with_label("Convert to CSV");
    tsv_button = gtk_button_new_with_label("Convert to TSV");
    txt_button = gtk_button_new_with_label("Convert to TXT");
    json_button = gtk_button_new_with_label("Convert to JSON");
    
    gtk_widget_set_sensitive(csv_button, FALSE);
    gtk_widget_set_sensitive(tsv_button, FALSE);
    gtk_widget_set_sensitive(txt_button, FALSE);
    gtk_widget_set_sensitive(json_button, FALSE);
    
    g_signal_connect(csv_button, "clicked", G_CALLBACK(on_csv_conversion), NULL);
    g_signal_connect(tsv_button, "clicked", G_CALLBACK(on_tsv_conversion), NULL);
    g_signal_connect(txt_button, "clicked", G_CALLBACK(on_txt_conversion), NULL);
    g_signal_connect(json_button, "clicked", G_CALLBACK(on_json_conversion), NULL);
    
    gtk_container_add(GTK_CONTAINER(button_box), csv_button);
    gtk_container_add(GTK_CONTAINER(button_box), tsv_button);
    gtk_container_add(GTK_CONTAINER(button_box), txt_button);
    gtk_container_add(GTK_CONTAINER(button_box), json_button);
    
    gtk_box_pack_start(GTK_BOX(vbox), button_box, FALSE, FALSE, 0);
    
    gtk_widget_show_all(window);
    g_object_unref(provider); 
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;
    
    app = gtk_application_new("com.example.fileconverter", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    
    if (input_file_path) {
        g_free(input_file_path);
    }
    
    return status;
}