// show_license_dialog.c

#include <gtk/gtk.h>
#include "show_license_dialog.h"

GtkWidget *license_window = NULL;

void close_license_dialog(GtkWidget *widget, gpointer data)
{
    gtk_widget_destroy(license_window);
    license_window = NULL;
}

void show_license_dialog(GtkWidget *widget, gpointer data)
{
    if (license_window != NULL)
        return;

    license_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(license_window), "LGPL許可證");
    gtk_window_set_resizable(GTK_WINDOW(license_window), TRUE);
    gtk_window_set_default_size(GTK_WINDOW(license_window), 500, 600);
    gtk_window_set_position(GTK_WINDOW(license_window), GTK_WIN_POS_CENTER);

    // 建立一個垂直容器
    GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(license_window), vbox);

    // 建立一個 GtkTextView 元件
    GtkWidget *text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);

    // 建立一個捲軸視窗以包含文字視圖
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);

    // 將捲軸視窗加入垂直容器
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);

    // 從檔案載入授權文字
    const gchar *filename = "lgpl-3.0.txt";
    gchar *license_text = NULL;
    g_file_get_contents(filename, &license_text, NULL, NULL);

    // 將授權文字設定到文字視圖中
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_text_buffer_set_text(buffer, license_text, -1);

    // 將文字視圖置中對齊
    GtkTextTag *tag = gtk_text_buffer_create_tag(buffer, NULL, "justification", GTK_JUSTIFY_CENTER, NULL);
    gtk_text_buffer_get_tag_table(buffer);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_justification(GTK_TEXT_VIEW(text_view), GTK_JUSTIFY_CENTER);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(text_view), 10);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(text_view), 10);

    g_free(license_text);

    GtkWidget *close_button = gtk_button_new_with_label("關閉");
    g_signal_connect(close_button, "clicked",
                     G_CALLBACK(close_license_dialog), NULL);

    GtkWidget *button_box = gtk_hbutton_box_new();
    gtk_button_box_set_layout(GTK_BUTTON_BOX(button_box), GTK_BUTTONBOX_CENTER);
    gtk_box_pack_end(GTK_BOX(button_box), close_button, FALSE, FALSE, 5);

    // 將按鈕容器加入垂直容器
    gtk_box_pack_start(GTK_BOX(vbox), button_box, FALSE, FALSE, 0);

    g_signal_connect(license_window, "destroy", G_CALLBACK(gtk_widget_destroyed), &license_window);

    gtk_widget_show_all(license_window);
}