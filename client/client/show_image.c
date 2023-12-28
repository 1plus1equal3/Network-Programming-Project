#include <stdlib.h>
#include <gtk/gtk.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <math.h>
#include <ctype.h>
#include <gtk/gtkx.h>

GtkBuilder *builder;
GtkWidget *window;
GtkWidget *stack;
GtkWidget *image;
GtkWidget *question;
GtkWidget *nextBtn, *backBtn, *playBtn;

int counter = 0;
int viewerOpen = 0;

question_image **question_list;
int question_num = 0;
char full_media_path[1000][1000];
int media_type; // 0: image, 1: sound

void getFullImagePath(){
    if(media_type == 0){
    for(int i = 0; i < question_num; i++){
        char *media_path = (*question_list)[i].image_path;
        sprintf((full_media_path)[i], "/home/duy/NetworkProgramming/Project/main/main_reconstructx2/client/source/image/%s", media_path);
    }}
    else if (media_type == 1){
        for(int i = 0; i < question_num; i++){
        char *media_path = (*question_list)[i].image_path;
        sprintf((full_media_path)[i], "/home/duy/NetworkProgramming/Project/main/main_reconstructx2/client/source/soundtrack/%s", media_path);
    }}
}

void resize_image(char *media_path) {
    char cmd[1000];
    sprintf(cmd, "/home/duy/NetworkProgramming/Project/main/main_reconstructx2/client/resize_img.sh %s", media_path);
    system(cmd);
}

void deleteImageAfterUsed() {
    char cmd[1000];
    sprintf(cmd, "rm -r %s", "/home/duy/NetworkProgramming/Project/main/main_reconstructx2/client/source/image/*");
    system(cmd);
}

void deleteSoundAfterUsed() {
    char cmd[1000];
    sprintf(cmd, "rm -r %s", "/home/duy/NetworkProgramming/Project/main/main_reconstructx2/client/source/soundtrack/*");
    system(cmd);
}

void update_image(char *media_path) {
    //gtk_stack_set_visible_child_name(GTK_STACK(stack), "image");
    resize_image(media_path);
    gtk_image_set_from_file(GTK_IMAGE(image), media_path);
}

void update_title(char *title) {
    gtk_label_set_text(GTK_LABEL(question), title);
}

// Button click events
void on_Next_btn_clicked() {
    if(counter < question_num - 1){
        counter++;
        if(media_type == 0)
        update_image(full_media_path[counter]);
        update_title((*question_list)[counter].question_content);
    }
}

void on_Back_btn_clicked() {
    if(counter > 0){
        counter--;
        if(media_type == 0)
        update_image(full_media_path[counter]);
        update_title((*question_list)[counter].question_content);
    }
}

void on_playBtn_clicked(){
    char cmd[2000];
    sprintf(cmd, "mpg123 %s", full_media_path[counter]);
    system(cmd);
}

void getInput(question_image **questions, int num_question, int media_t){
    question_list = questions;
    question_num = num_question;
    media_type = media_t;
    getFullImagePath();
}

void showWindow(){
    gtk_widget_show(window);
}

void destroyWindow(){
    gtk_widget_destroy(window);
    gtk_main_quit();
}

void setImageViewerState(){
    viewerOpen = 0;
}

int startImageQuestionInterface() {
    //sleep(question_num/2);
    viewerOpen = 1;
    gtk_init(NULL, NULL);
    builder = gtk_builder_new_from_file("/home/duy/NetworkProgramming/Project/main/main_reconstructx2/client/image_UI.glade");
    GtkCssProvider *provider;
    provider = gtk_css_provider_new();
    GdkDisplay *display;
    GdkScreen *screen;
    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen(display);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    //gtk_css_provider_load_from_path(provider, "style.css", NULL);
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    //g_signal_connect(window, "destroy", G_CALLBACK(deleteImageAfterUsed), NULL);
    g_signal_connect(window, "destroy", setImageViewerState, NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_builder_connect_signals(builder, NULL);
    stack = GTK_WIDGET(gtk_builder_get_object(builder, "stack"));
    image = GTK_WIDGET(gtk_builder_get_object(builder, "image"));
    question = GTK_WIDGET(gtk_builder_get_object(builder, "question"));
    nextBtn = GTK_WIDGET(gtk_builder_get_object(builder, "Next_btn"));
    backBtn = GTK_WIDGET(gtk_builder_get_object(builder, "Back_btn"));
    update_image(full_media_path[0]);
    update_title((*question_list)[0].question_content);
    gtk_widget_show(window);
    gtk_main();
    return EXIT_SUCCESS;
}


int startSoundQuestionInterface() {
    //sleep(question_num/2);
    viewerOpen = 1;
    gtk_init(NULL, NULL);
    builder = gtk_builder_new_from_file("/home/duy/NetworkProgramming/Project/main/main_reconstructx2/client/sound_UI.glade");
    GtkCssProvider *provider;
    provider = gtk_css_provider_new();
    GdkDisplay *display;
    GdkScreen *screen;
    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen(display);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    //gtk_css_provider_load_from_path(provider, "style.css", NULL);
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    //g_signal_connect(window, "destroy", G_CALLBACK(deleteImageAfterUsed), NULL);
    g_signal_connect(window, "destroy", setImageViewerState, NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_builder_connect_signals(builder, NULL);
    stack = GTK_WIDGET(gtk_builder_get_object(builder, "stack"));
    playBtn = GTK_WIDGET(gtk_builder_get_object(builder, "playBtn"));
    question = GTK_WIDGET(gtk_builder_get_object(builder, "question"));
    nextBtn = GTK_WIDGET(gtk_builder_get_object(builder, "Next_btn"));
    backBtn = GTK_WIDGET(gtk_builder_get_object(builder, "Back_btn"));
    update_title((*question_list)[0].question_content);
    gtk_widget_show(window);
    gtk_main();
    return EXIT_SUCCESS;
}