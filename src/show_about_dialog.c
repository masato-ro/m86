// show_about_dialog.c

#include <gtk/gtk.h>
#include "show_about_dialog.h"
#include "show_license_dialog.h"

GtkWidget *about_window = NULL;

void close_about_dialog(GtkWidget *widget, gpointer data)
{
    gtk_widget_destroy(about_window);
    about_window = NULL;
}

void show_about_dialog(GtkWidget *widget, gpointer data)
{
    if (about_window != NULL)
        return;

    about_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(about_window), "關於M86");
    gtk_window_set_default_size(GTK_WINDOW(about_window), 400, 300);
    gtk_window_set_position(GTK_WINDOW(about_window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(about_window), FALSE);

    GtkWidget *button_license = gtk_button_new_with_label("LGPL許可證");
    g_signal_connect(button_license, "clicked",
                     G_CALLBACK(show_license_dialog), data);

    GtkWidget *close_button = gtk_button_new_with_label("我了解了！");
    g_signal_connect(close_button, "clicked",
                     G_CALLBACK(close_about_dialog), NULL);

    GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(about_window), vbox);

    GtkWidget *label = gtk_label_new("關於M86");
    PangoFontDescription *font_desc = pango_font_description_new();
    pango_font_description_set_size(font_desc, 16 * PANGO_SCALE);
    gtk_widget_modify_font(label, font_desc);
    pango_font_description_free(font_desc);

    GtkWidget *label1 = gtk_label_new("Version 1.0");
    GtkWidget *label2 = gtk_label_new("本程式基於GNU Lesser General Public License 發布\n\n");    
    GtkWidget *label3 = gtk_label_new("Powered by PenguinBay Team\n\n");    
    GtkWidget *label4 = gtk_label_new(NULL);
    const gchar *markup = "<a href=\"http://www.penguinbay.info\">http://www.penguinbay.info</a>";
    gtk_label_set_markup(GTK_LABEL(label4), markup);
    gtk_label_set_line_wrap(GTK_LABEL(label4), TRUE);
    gtk_label_set_line_wrap_mode(GTK_LABEL(label4), PANGO_WRAP_WORD);
    gtk_label_set_selectable(GTK_LABEL(label4), FALSE);

    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), label1, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), label2, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), label3, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), label4, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(vbox), close_button, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(vbox), button_license, FALSE, FALSE, 0);

    g_signal_connect(about_window, "destroy",
                     G_CALLBACK(gtk_widget_destroyed), &about_window);

    gtk_widget_show_all(about_window);
}