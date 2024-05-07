#include <gtk/gtk.h>
#include <sys/time.h>
#include <stdlib.h>
#include <ctype.h>

// Stopwatch data structure
typedef struct {
    GtkWidget *label;
    struct timeval start_time;
    gboolean running;
    guint timer_id;
} StopwatchData;

// Timer data structure
typedef struct {
    GtkWidget *label;
    GtkWidget *hours_entry;
    GtkWidget *minutes_entry;
    GtkWidget *seconds_entry;
    struct timeval end_time;
    guint timer_id;
    gboolean running;
} TimerData;
//validate field inputs
static gboolean is_valid_integer(const char *str) {
    while (*str) {
        if (!isdigit(*str++)) return FALSE;
    }
    return TRUE;
}
///update the stopwatch label where we display the timer running. 
static void update_stopwatch_label(StopwatchData *data) {
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    long milliseconds = (current_time.tv_sec - data->start_time.tv_sec) * 1000 +
                        (current_time.tv_usec - data->start_time.tv_usec) / 1000;

    int hours = milliseconds / (1000 * 60 * 60);
    int minutes = (milliseconds / (1000 * 60)) % 60;
    int seconds = (milliseconds / 1000) % 60;
    milliseconds %= 1000;

    char buffer[100];
    sprintf(buffer, "%02d:%02d:%02d.%03ld", hours, minutes, seconds, milliseconds);
    gtk_label_set_text(GTK_LABEL(data->label), buffer);
}
//if the stopwatch is running update the label. 
static gboolean tick_stopwatch(gpointer user_data) {
    StopwatchData *data = (StopwatchData *)user_data;
    if (data->running) {
        update_stopwatch_label(data);
    }
    return G_SOURCE_CONTINUE;
}
//start or stop the stopwatch
static void start_stop_stopwatch(GtkWidget *widget, gpointer user_data) {
    StopwatchData *data = (StopwatchData *)user_data;
    if (data->running) {
        gtk_button_set_label(GTK_BUTTON(widget), "Start Stopwatch");
        g_source_remove(data->timer_id);
        data->running = FALSE;
    } else {
        gtk_button_set_label(GTK_BUTTON(widget), "Stop Stopwatch");
        gettimeofday(&data->start_time, NULL);
        data->running = TRUE;
        data->timer_id = g_timeout_add(1, tick_stopwatch, data);
    }
}
//update the timer label
static void update_timer_label(TimerData *data) {
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    long milliseconds = (data->end_time.tv_sec - current_time.tv_sec) * 1000 +
                        (data->end_time.tv_usec - current_time.tv_usec) / 1000;

    if (milliseconds < 0) milliseconds = 0;

    int hours = milliseconds / (1000 * 60 * 60);
    int minutes = (milliseconds / (1000 * 60)) % 60;
    int seconds = (milliseconds / 1000) % 60;
    milliseconds %= 1000;

    char buffer[100];
    sprintf(buffer, "%02d:%02d:%02d.%03ld", hours, minutes, seconds, milliseconds);
    gtk_label_set_text(GTK_LABEL(data->label), buffer);
}
//tick the timer
static gboolean tick_timer(gpointer user_data) {
    TimerData *data = (TimerData *)user_data;
    update_timer_label(data); // Update the display every millisecond
    if (data->running && data->end_time.tv_sec == 0 && data->end_time.tv_usec == 0) {
        gtk_button_set_label(GTK_BUTTON(data->label), "Time's up!");
        data->running = FALSE;
        return G_SOURCE_REMOVE; // Stop the timer
    }
    return G_SOURCE_CONTINUE;
}
//start or stop the timer
static void start_stop_timer(GtkWidget *widget, gpointer user_data) {
    TimerData *data = (TimerData *)user_data;
    if (data->running) {
        gtk_button_set_label(GTK_BUTTON(widget), "Start Timer");
        g_source_remove(data->timer_id);
        data->running = FALSE;
    } else {
        const char *hours_str = gtk_entry_get_text(GTK_ENTRY(data->hours_entry));
        const char *minutes_str = gtk_entry_get_text(GTK_ENTRY(data->minutes_entry));
        const char *seconds_str = gtk_entry_get_text(GTK_ENTRY(data->seconds_entry));

        if (!is_valid_integer(hours_str) || !is_valid_integer(minutes_str) || !is_valid_integer(seconds_str)) {
            gtk_label_set_text(GTK_LABEL(data->label), "Invalid input! Use only numbers.");
            return;
        }

        int hours = atoi(hours_str);
        int minutes = atoi(minutes_str);
        int seconds = atoi(seconds_str);

        struct timeval now;
        gettimeofday(&now, NULL);
        data->end_time.tv_sec = now.tv_sec + hours * 3600 + minutes * 60 + seconds;
        data->end_time.tv_usec = now.tv_usec;

        gtk_button_set_label(GTK_BUTTON(widget), "Stop Timer");
        data->running = TRUE;
        data->timer_id = g_timeout_add(1, tick_timer, data);
    }
}
//main function
int main(int argc, char *argv[]) {
    GtkWidget *window, *notebook;
    GtkWidget *stopwatch_tab, *timer_tab;
    GtkWidget *stopwatch_label, *timer_label;
    GtkWidget *stopwatch_button, *timer_button;
    GtkWidget *stopwatch_box, *timer_box, *timer_entries_box;
    GtkWidget *hours_entry, *minutes_entry, *seconds_entry;
    StopwatchData stopwatch_data = {0};
    TimerData timer_data = {0};

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Countdown App");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    notebook = gtk_notebook_new();
    gtk_container_add(GTK_CONTAINER(window), notebook);

    // Stopwatch setup
    stopwatch_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    stopwatch_label = gtk_label_new("00:00:00.000");
    stopwatch_data.label = stopwatch_label;

    // Create a horizontal box to control the size of the button
    GtkWidget *stopwatch_button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    stopwatch_button = gtk_button_new_with_label("Start Stopwatch");
    gtk_box_pack_start(GTK_BOX(stopwatch_button_box), stopwatch_button, FALSE, FALSE, 0);  // Pack button with FALSE, FALSE to not expand
    gtk_box_set_center_widget(GTK_BOX(stopwatch_box), stopwatch_button_box);  // Center the button box

    g_signal_connect(stopwatch_button, "clicked", G_CALLBACK(start_stop_stopwatch), &stopwatch_data);
    gtk_box_pack_start(GTK_BOX(stopwatch_box), stopwatch_label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(stopwatch_box), stopwatch_button_box, FALSE, FALSE, 0);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), stopwatch_box, gtk_label_new("Stopwatch"));

    // Timer setup
    timer_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    timer_entries_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    timer_label = gtk_label_new("00:00:00.000");
    timer_data.label = timer_label;

    // Entries setup
    hours_entry = gtk_entry_new();
    gtk_entry_set_width_chars(GTK_ENTRY(hours_entry), 2);
    minutes_entry = gtk_entry_new();
    gtk_entry_set_width_chars(GTK_ENTRY(minutes_entry), 2);
    seconds_entry = gtk_entry_new();
    gtk_entry_set_width_chars(GTK_ENTRY(seconds_entry), 2);
    timer_data.hours_entry = hours_entry;
    timer_data.minutes_entry = minutes_entry;
    timer_data.seconds_entry = seconds_entry;

    gtk_box_pack_start(GTK_BOX(timer_entries_box), gtk_label_new("Hours:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(timer_entries_box), hours_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(timer_entries_box), gtk_label_new("Minutes:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(timer_entries_box), minutes_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(timer_entries_box), gtk_label_new("Seconds:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(timer_entries_box), seconds_entry, FALSE, FALSE, 0);

    // Create a horizontal box to control the size of the timer button
    GtkWidget *timer_button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    timer_button = gtk_button_new_with_label("Start Timer");
    gtk_box_pack_start(GTK_BOX(timer_button_box), timer_button, FALSE, FALSE, 0);  // Pack button with FALSE, FALSE to not expand
    gtk_box_set_center_widget(GTK_BOX(timer_box), timer_button_box);  // Center the button box

    g_signal_connect(timer_button, "clicked", G_CALLBACK(start_stop_timer), &timer_data);
    gtk_box_pack_start(GTK_BOX(timer_box), timer_entries_box, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(timer_box), timer_label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(timer_box), timer_button_box, FALSE, FALSE, 0);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), timer_box, gtk_label_new("Timer"));

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
