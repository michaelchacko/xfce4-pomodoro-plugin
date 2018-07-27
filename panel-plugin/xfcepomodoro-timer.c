#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <gtk/gtk.h>

#include <libxfce4ui/libxfce4ui.h>
#include <libxfce4panel/xfce-panel-plugin.h>

#include "xfcepomodoro.h"
#include "xfcepomodoro-dialogs.h"
#include "xfcepomodoro-config.h"
#include "xfcepomodoro-timer.h"

#define UPDATE_INTERVAL 2000 //frequency to update progress bar in milliseconds

/* timer periods in minutes */
#define POMODORO_PERIOD      0
#define SHORT_BREAK_PERIOD   5
#define LONG_BREAK_PERIOD    30

/*callback function to start the pomodoro timer! */
void start_timer (GtkWidget *pbar, PomodoroPlugin *pd)
{
    //TODO put in logic to determine if its a pomodoro or a break
    pd->pomodoro_is_running = TRUE;
    pd->break_is_running = TRUE;

    if(pd->pomodoro_is_running)
        pd->timeout_period_in_sec = POMODORO_PERIOD * 60;
    else if(pd->break_is_running)
        pd->timeout_period_in_sec = SHORT_BREAK_PERIOD * 60;

    //play ticking sound
    //TODO get this working without mplayer
    //TODO add check box to config dialog to control audio settings
    if(pd->play_ticking)
        system("mplayer -really-quiet ~/repos/xfce4-pomodoro-plugin/audio/ticking.flac &");

    if(pd->timer) {
        g_timer_destroy(pd->timer);
    }    
    
    pd->timer = g_timer_new();
    pd->timer_on = TRUE;

    //set progress bar to full
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(pd->pbar), 1); 

    pd->timeout = g_timeout_add(UPDATE_INTERVAL, update_timer, pd);

}

/* callback function to stop the timer */
void stop_timer (GtkWidget *pbar, PomodoroPlugin *pd)
{
    pd->pomodoro_is_running = FALSE;
    pd->break_is_running = FALSE;
    pd->timer_on = FALSE;

    /* free timer */
    if(pd->timer)
    {
        g_timer_destroy(pd->timer);
        pd->timer = NULL;
    }

    /* remove timeout source call */
    if(pd->timeout)
        g_source_remove(pd->timeout);
    pd->timeout = 0;


    /* reset progress bar */
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(pd->pbar), 0);

    return;
}

/* runs every UPDATE_INTERVAL milliseconds while timer is running */
/* returns TRUE if timer is still running */
gboolean update_timer(gpointer data)
{
    /* convert pointer parameter to plugin data */
    PomodoroPlugin *pd = (PomodoroPlugin *)data;

    gint elapsed_sec, remaining; 
    elapsed_sec = (gint) g_timer_elapsed(pd->timer, NULL);
    
    //if timer has not exceeded timer limit, update remaining time and progress bar
    if(elapsed_sec < pd->timeout_period_in_sec){
        remaining = pd->timeout_period_in_sec-elapsed_sec;
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(pd->pbar), 1.0 - ((gdouble) elapsed_sec) / pd->timeout_period_in_sec); 
        return TRUE;
    }

    /* If program reaches here, the timer has finished! */
    /* play timer finished noise (TODO make path to alert noise dynamic)*/
    if(pd->play_alarms)
        system("mplayer -really-quiet ~/repos/xfce4-pomodoro-plugin/audio/alert.wav > /dev/null 2>&1");

    pomodoro_timer_finished_dialog(pd->xfcePlugin, pd);

    /* free timer resources */
    if(pd->timer){
       g_timer_destroy(pd->timer);
    }
    pd->timer=NULL;

    /* reset progress bar */
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(pd->pbar), 0); 

    /* reset plugin variables */
    pd->timeout=0;
    pd->timer_on=FALSE;
    
    /* returning false prevents loop from continuing */
    return FALSE;
}
