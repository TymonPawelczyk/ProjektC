#include <gtk/gtk.h>

// Global variables for the entry widget and text attributes
static GtkWidget *entry;

static void
save_to_file (GtkWidget *widget, gpointer user_data)
{
    GtkTextBuffer *buffer;
    GtkTextIter start, end;
    const gchar *text;

    
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(entry));

    
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);

    
    text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    
    GtkWidget *dialog = gtk_file_chooser_dialog_new ("Save File",
                                                      GTK_WINDOW(user_data),
                                                      GTK_FILE_CHOOSER_ACTION_SAVE,
                                                      "Cancel",
                                                      GTK_RESPONSE_CANCEL,
                                                      "Save",
                                                      GTK_RESPONSE_ACCEPT,
                                                      NULL);

    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);

    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
        char *filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));

        // Write the text to the selected file
        FILE *file = fopen(filename, "w");
        if (file != NULL)
        {
            fprintf(file, "%s", text);
            fclose(file);
        }

        g_free (filename);
    }

    gtk_widget_destroy (dialog);
}


static void
font_changed (GtkFontButton *fontbutton, gpointer user_data)
{
    PangoFontDescription *font_desc = pango_font_description_from_string(gtk_font_button_get_font_name(fontbutton));
    
    // Set the font for the entry widget
    gtk_widget_override_font(entry, font_desc);

    // Free the allocated font description
    pango_font_description_free(font_desc);
}

static void
color_changed (GtkColorButton *colorbutton, gpointer user_data)
{
    GdkRGBA color;
    gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(colorbutton), &color);
    
    // Set the text color for the entry widget
    gtk_widget_override_color(entry, GTK_STATE_FLAG_NORMAL, &color);
}

static void
activate (GtkApplication* app,
          gpointer        user_data)
{
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *button_box;
    GtkWidget *save_button;
    GtkWidget *font_button;
    GtkWidget *color_button;

    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), "Text Editor");
    gtk_window_set_default_size (GTK_WINDOW (window), 400, 400);

    vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add (GTK_CONTAINER (window), vbox);

    // Create text view for multiline input
    entry = gtk_text_view_new();
    
    // Set wrap mode for multiline input
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(entry), GTK_WRAP_WORD_CHAR);

    gtk_container_add(GTK_CONTAINER(vbox), entry);
    
    // Set the size request for the text view
    gtk_widget_set_size_request(entry, -1, 350);  // Adjust the height as needed

    // Create button box
    button_box = gtk_button_box_new (GTK_ORIENTATION_HORIZONTAL);
    gtk_container_add(GTK_CONTAINER(vbox), button_box);

    // Create save button
    save_button = gtk_button_new_with_label ("Zapisz");
    g_signal_connect (save_button, "clicked", G_CALLBACK (save_to_file), window);
    gtk_container_add (GTK_CONTAINER (button_box), save_button);

    // Create font button
    font_button = gtk_font_button_new();
    g_signal_connect(font_button, "font-set", G_CALLBACK(font_changed), NULL);
    gtk_container_add(GTK_CONTAINER(button_box), font_button);

    // Create color button
    color_button = gtk_color_button_new();
    g_signal_connect(color_button, "color-set", G_CALLBACK(color_changed), NULL);
    gtk_container_add(GTK_CONTAINER(button_box), color_button);

    gtk_widget_show_all(window);
    gtk_window_present (GTK_WINDOW (window));
}



int
main (int    argc,
      char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new ("org.gtk.example", G_APPLICATION_REPLACE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}
