// main.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <gtk/gtk.h>
#include "show_about_dialog.h"
#include "show_license_dialog.h"
char cfg_dir[256];
char exec_file[256];
char editor_file[256];

enum
{
    COLUMN_FILENAME,
    NUM_COLUMNS
};

void show_warning_dialog(const gchar *format, ...) {
    GtkWidget *dialog;
    gchar *message;
    va_list args;

    va_start(args, format);
    message = g_strdup_vprintf(format, args);
    va_end(args);

    dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    g_free(message);
}

void read_cfg_file()
{
    const char *cfg_file = "m86.config";
    const char *cfg_dir_key = "cfg_dir";
    const char *exec_file_key = "exec_file";
    const char *editor_file_key = "editor_file";
    char cfg_dir_path[256];
    char exec_file_path[256];
    char editor_file_path[256];

    FILE *cfg = fopen(cfg_file, "r");
    if (cfg == NULL) {
        show_warning_dialog("無法開啟%s檔案", cfg_file);
        return;
    }

    while (fgets(cfg_dir_path, sizeof(cfg_dir_path), cfg) != NULL) {
        if (strncmp(cfg_dir_path, cfg_dir_key, strlen(cfg_dir_key)) == 0) {
            // 移除前綴 "cfg_dir = " 並刪除換行字元
            char *path = strtok(cfg_dir_path + strlen(cfg_dir_key) + 3, "\n");            
            // 將 cfg_dir 的值複製到全局變數中
            strcpy(cfg_dir, path);
            break;
        }
    }

    rewind(cfg); // 重置文件指針以重新讀取檔案

    while (fgets(exec_file_path, sizeof(exec_file_path), cfg) != NULL) {
        if (strncmp(exec_file_path, exec_file_key, strlen(exec_file_key)) == 0) {
            char *file = strtok(exec_file_path + strlen(exec_file_key) + 3, "\n");
            strcpy(exec_file, file);
        }
    }

    rewind(cfg); // 重置文件指針以重新讀取檔案

    while (fgets(editor_file_path, sizeof(editor_file_path), cfg) != NULL) {
        if (strncmp(editor_file_path, editor_file_key, strlen(editor_file_key)) == 0) {
            char *file = strtok(editor_file_path + strlen(editor_file_key) + 3, "\n");
            strcpy(editor_file, file);
        }
    }

    fclose(cfg);
}

void refresh_tree_view(GtkWidget *widget, gpointer data)
{
    GtkListStore *list_store = GTK_LIST_STORE(data);
    gtk_list_store_clear(list_store);

    // 讀取目錄下的 *.cfg 檔案
    GDir *dir = g_dir_open(cfg_dir, 0, NULL);
    const gchar *filename;
    while ((filename = g_dir_read_name(dir)) != NULL)
    {
        if (g_str_has_suffix(filename, ".cfg"))
        {
            GtkTreeIter iter;
            gtk_list_store_append(list_store, &iter);
            gtk_list_store_set(list_store, &iter, COLUMN_FILENAME, filename, -1);
        }
    }
    g_dir_close(dir);
}

void open_file(GtkWidget *widget, gpointer data)
{
    GtkTreeView *tree_view = GTK_TREE_VIEW(data);
    GtkTreeModel *model = gtk_tree_view_get_model(tree_view);
    GtkTreeSelection *selection = gtk_tree_view_get_selection(tree_view);
    GtkTreeIter iter;
    if (gtk_tree_selection_get_selected(selection, &model, &iter))
    {
        gchar *filename;
        gtk_tree_model_get(model, &iter, COLUMN_FILENAME, &filename, -1);
        gchar *path = g_strdup_printf("%s%s", cfg_dir, filename);
        gchar *command = g_strdup_printf("%s%s \"%s\"", cfg_dir, exec_file, path);

        g_free(filename);

        GError *error = NULL;
        if (!g_spawn_command_line_async(command, &error))
        {
            if (error != NULL && g_error_matches(error, G_SPAWN_ERROR, G_SPAWN_ERROR_NOENT))
            {
                show_warning_dialog("無法開啟%s\n請檢查86Box執行檔！", exec_file);
            }
            else
            {
                g_print("執行命令失敗：%s\n", error != NULL ? error->message : "未知錯誤");
            }

            if (error != NULL)
                g_error_free(error);
        }

        g_free(path);
        g_free(command);
    }
}

void edit_file(GtkWidget *widget, gpointer data)
{
    GtkTreeView *tree_view = GTK_TREE_VIEW(data);
    GtkTreeModel *model = gtk_tree_view_get_model(tree_view);
    GtkTreeSelection *selection = gtk_tree_view_get_selection(tree_view);
    GtkTreeIter iter;
    if (gtk_tree_selection_get_selected(selection, &model, &iter))
    {
        gchar *filename;
        gtk_tree_model_get(model, &iter, COLUMN_FILENAME, &filename, -1);
        gchar *path = g_strdup_printf("%s%s", cfg_dir, filename);
        gchar *command = g_strdup_printf("%s \"%s\"" , editor_file, path);
        printf(editor_file);
        g_free(filename);

        GError *error = NULL;
        if (!g_spawn_command_line_async(command, &error))
        {
            if (error != NULL && g_error_matches(error, G_SPAWN_ERROR, G_SPAWN_ERROR_NOENT))
            {
                show_warning_dialog("無法開啟%s\n請檢查編輯程式！", editor_file);
            }
            else
            {
                g_print("執行命令失敗：%s\n", error != NULL ? error->message : "未知錯誤");
            }

            if (error != NULL)
                g_error_free(error);
        }

        g_free(path);
        g_free(command);
    }
}

void exit_application(GtkWidget *widget, gpointer data)
{
    GtkWidget *dialog = gtk_message_dialog_new(
        GTK_WINDOW(data),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_QUESTION,
        GTK_BUTTONS_YES_NO,
        "確定要退出嗎？");

    gtk_window_set_title(GTK_WINDOW(dialog), "退出確認");
    gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_NO);
    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    
    if (response == GTK_RESPONSE_YES)
    {
        gtk_main_quit();
    }
    
    gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *button_about;
    GtkWidget *button_refresh;
    GtkWidget *button_edit;
    GtkWidget *button_open;
    GtkWidget *button_exit;
    GtkWidget *button_box;
    GtkWidget *table;

    gtk_init(&argc, &argv);

    // 讀取 m86.cfg 檔案
    read_cfg_file();

    // 設定GTK主題為Raleigh
    GtkSettings *settings = gtk_settings_get_default();
    g_object_set(settings, "gtk-theme-name", "Raleigh", NULL);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "M86 - 86Box manager in Linux");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 500);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_icon_from_file(GTK_WINDOW(window), "esd.png", NULL);
    
    table = gtk_table_new(6, 3, TRUE);

    button_box = gtk_hbutton_box_new();

    button_about = gtk_button_new_with_label("關於M86");
    g_signal_connect(button_about, "clicked", G_CALLBACK(show_about_dialog), window);
    gtk_container_add(GTK_CONTAINER(button_box), button_about);

    button_refresh = gtk_button_new_with_label("重新整理");
    gtk_widget_set_tooltip_text(button_refresh, "重新載入檔案列表");
    gtk_container_add(GTK_CONTAINER(button_box), button_refresh);

    // 建立 GtkTreeView 和 GtkListStore
    GtkTreeIter iter;
    GtkListStore *list_store;
    GtkTreeView *tree_view;
    GtkTreeViewColumn *column;
    GtkCellRenderer *renderer;

    list_store = gtk_list_store_new(NUM_COLUMNS, G_TYPE_STRING);
    tree_view = GTK_TREE_VIEW(gtk_tree_view_new_with_model(GTK_TREE_MODEL(list_store)));

    // 設定列標題
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("設定檔名稱", renderer, "text", COLUMN_FILENAME, NULL);
    gtk_tree_view_append_column(tree_view, column);

    GDir *dir = g_dir_open(cfg_dir, 0, NULL);
    const gchar *filename;
    while ((filename = g_dir_read_name(dir)) != NULL)
    {
        if (g_str_has_suffix(filename, ".cfg"))
        {
            gtk_list_store_append(list_store, &iter);
            gtk_list_store_set(list_store, &iter, COLUMN_FILENAME, filename, -1);
        }
    }
    g_dir_close(dir);

    // 將 GtkTreeView 放入 GtkScrolledWindow
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);

    gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(tree_view));

    gtk_table_attach_defaults(
                 GTK_TABLE(table), scrolled_window, 0, 3, 0, 5);

    // 編輯按鈕
    button_edit = gtk_button_new_with_label("編輯設定");
    g_signal_connect(button_edit, "clicked", G_CALLBACK(edit_file), tree_view);
    gtk_container_add(GTK_CONTAINER(button_box), button_edit);
    
    // 啟動按鈕
    button_open = gtk_button_new_with_label("啟動");
    g_signal_connect(button_open, "clicked", G_CALLBACK(open_file), tree_view);
    gtk_container_add(GTK_CONTAINER(button_box), button_open);

    // 退出按鈕
    button_exit = gtk_button_new_with_label("退出");
    g_signal_connect(button_exit, "clicked", G_CALLBACK(exit_application), NULL);
    gtk_container_add(GTK_CONTAINER(button_box), button_exit);

    gtk_table_attach_defaults(
                 GTK_TABLE(table), button_box, 0, 3, 5, 6);
    
    // 顯示所有的元件
    gtk_container_add(GTK_CONTAINER(window), table);
    gtk_widget_show_all(window);
    
    // 關閉視窗時退出 GTK 主迴圈
    g_signal_connect(window, "destroy",
                     G_CALLBACK(gtk_main_quit), NULL);

    // 重新整理按鈕的回調函式
    g_signal_connect(button_refresh, "clicked", 
                     G_CALLBACK(refresh_tree_view), list_store);

    // 啟動主迴圈
    gtk_main();

    return 0;
}