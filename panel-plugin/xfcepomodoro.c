#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

#include <gtk/gtk.h>
#include <libxfce4util/libxfce4util.h>
#include <libxfce4ui/libxfce4ui.h>
#include <libxfce4panel/libxfce4panel.h>
#include <libxfce4panel/xfce-panel-plugin.h>

#include "xfcepomodoro.h"
#include "xfcepomodoro-dialogs.h"
#include "xfcepomodoro-config.h"
#include "xfcepomodoro-timer.h"

/* default settings */
#define ICON_PATH "/home/michael/repos/xfce4-pomodoro-plugin/icons/48x48/xfce4-pomodoro-plugin.png"
#define LABEL_TEXT ""

#define PBAR_THICKNESS 15 
#define HV_BOX_SPACING 4 

/* register pomodoro_construct() as the primary plugin function*/
XFCE_PANEL_PLUGIN_REGISTER_INTERNAL(pomodoro_construct);

/* program constructor */
static void pomodoro_construct(XfcePanelPlugin *plugin)
{
    /* create the plugin */
    PomodoroPlugin *pd = pomodoroPlugin_new(plugin);

    /* setup translation domain */
    xfce_textdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");

    /* add the ebox to the panel */
    gtk_container_add(GTK_CONTAINER (plugin), pd->ebox);

    /* show the panel's right-click menu on this ebox */
    xfce_panel_plugin_add_action_widget(plugin, pd->ebox);

    /* connect plugin signals */
    g_signal_connect(G_OBJECT (plugin), "free-data",
                     G_CALLBACK (plugin_free), pd);

    g_signal_connect(G_OBJECT (plugin), "save",
                     G_CALLBACK (config_save), pd);

    g_signal_connect(G_OBJECT (plugin), "size-changed",
                     G_CALLBACK (plugin_size_changed), pd);

    g_signal_connect(G_OBJECT (plugin), "orientation-changed",
                     G_CALLBACK (plugin_orientation_changed), pd);

    /* show the configure menu item and connect signal */
    xfce_panel_plugin_menu_show_configure (plugin);
    g_signal_connect(G_OBJECT (plugin), "configure-plugin",
                     G_CALLBACK (plugin_configure), pd);

    /* show the about menu item and connect signal */
    xfce_panel_plugin_menu_show_about (plugin);
    g_signal_connect(G_OBJECT (plugin), "about",
                     G_CALLBACK (plugin_about), NULL);

    /* signal when plugin in panel is pressed */
    g_signal_connect(G_OBJECT(plugin), "button_press_event",
                     G_CALLBACK(plugin_clicked), pd);
}

/* setup the PomodoroPlugin struct */
static PomodoroPlugin * pomodoroPlugin_new(XfcePanelPlugin *plugin)
{
    GtkOrientation  orientation;

    /* allocate memory for the plugin data (pd) */
    PomodoroPlugin *pd = g_slice_new(PomodoroPlugin);

    /* give struct access to pointer to XfcePanelPlugin */
    pd->xfcePlugin = plugin;

    /* set struct variables */
    pd->pomodoro_is_running = FALSE;
    pd->break_is_running= FALSE;
    pd->timer_on = FALSE;
    pd->timeout = 0;

    pd->play_ticking = TRUE;
    pd->play_alarms = TRUE;

    /* read the user settings */
    config_read (pd);

    /* get the current orientation */
    orientation = xfce_panel_plugin_get_orientation(plugin);

    /* create the event box */
    pd->ebox = gtk_event_box_new();
    gtk_widget_show(pd->ebox);

    /* create the hvbox */
    pd->hvbox = gtk_box_new(orientation, HV_BOX_SPACING);
    gtk_widget_show(pd->hvbox);

    /* add hvbox to ebox */
    gtk_container_add(GTK_CONTAINER (pd->ebox), pd->hvbox);

    /* add the progress bar */
    make_pbar(pd);

    /* add optional text */
    pd->label = gtk_label_new (LABEL_TEXT); //edit me to add text to plugin icon
    gtk_widget_show(pd->label);
    gtk_box_pack_start(GTK_BOX (pd->hvbox), pd->label, FALSE, FALSE, 0);

    /* create icon*/
    // TODO make image path dynamic for different users
    pd->tomato_icon_pixbuf = gdk_pixbuf_new_from_file_at_scale(ICON_PATH, 
                                                               20, 20, TRUE, NULL);
    pd->tomato_icon = gtk_image_new_from_pixbuf(pd->tomato_icon_pixbuf);
    gtk_widget_show(pd->tomato_icon);
    gtk_box_pack_start(GTK_BOX (pd->hvbox), pd->tomato_icon, FALSE, FALSE, 0);

    return pd;
}

/* free plugin resources (called when plugin is removed) */
static void plugin_free(XfcePanelPlugin *plugin, PomodoroPlugin *pd) 
{
    GtkWidget *dialog;

    /* check if the dialog is still open. if so, destroy it */
    dialog = g_object_get_data (G_OBJECT (plugin), "dialog");
    if (G_UNLIKELY (dialog != NULL))
        gtk_widget_destroy (dialog);

    /* destroy the panel widgets */
    gtk_widget_destroy (pd->hvbox);
    gtk_widget_destroy (pd->ebox);

    if(pd->timer)
        g_timer_destroy(pd->timer);

    /* cleanup the settings */
    if (G_LIKELY (pd->setting1 != NULL))
        g_free (pd->setting1);

    /* free the plugin structure */
    g_slice_free(PomodoroPlugin, pd);
}

/**
 * Adds the progressbar, NOT taking into account the orientation.
 * TODO change it to match the xfcetimer plugin to account for orientation of entire xfce panel
 * TODO make progress bar wider
**/
static void make_pbar(PomodoroPlugin *pd)
{
    /* if pbar already exists, do nothing */
    if(pd->pbar)
        return;

    pd->pbar = gtk_progress_bar_new();
    
    /* add pbar to hvbox */
    gtk_box_pack_start(GTK_BOX(pd->hvbox),pd->pbar,FALSE,FALSE,0);    

    gtk_orientable_set_orientation (GTK_ORIENTABLE(pd->pbar),
                                    GTK_ORIENTATION_VERTICAL);
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(pd->pbar), 0.0);
    gtk_progress_bar_set_inverted(GTK_PROGRESS_BAR(pd->pbar), TRUE);

    gtk_widget_show(pd->pbar);
}

static void make_menu(PomodoroPlugin *pd)
{
    /* Destroy the existing menu if it exists */
    if(pd->menu)
        gtk_widget_destroy(GTK_WIDGET(pd->menu));

	pd->menu = gtk_menu_new();

    /* Start pomodoro menu item */
    if(!pd->timer_on)
    {
        pd->mi_start_pomodoro = gtk_menu_item_new_with_label("Start a pomodoro");

        gtk_menu_shell_append(GTK_MENU_SHELL(pd->menu),
                              pd->mi_start_pomodoro);

        g_signal_connect(G_OBJECT(pd->mi_start_pomodoro), "activate",
                         G_CALLBACK(start_timer), pd);

        gtk_widget_show(pd->mi_start_pomodoro);
    }
    /* Stop pomodoro menu item */
    else 
    {
        pd->mi_stop_pomodoro = gtk_menu_item_new_with_label("Stop");

        gtk_menu_shell_append(GTK_MENU_SHELL(pd->menu),
                              pd->mi_stop_pomodoro);

        g_signal_connect(G_OBJECT(pd->mi_stop_pomodoro), "activate",
                         G_CALLBACK(stop_timer), pd);

        gtk_widget_show(pd->mi_stop_pomodoro);
        gtk_widget_show(pd->menu);
    }
}

static void plugin_clicked(GtkWidget *pbar, 
                           GdkEventButton *event, 
                           PomodoroPlugin *pd) 
{
    make_menu(pd); 

    //if menu does not exist, return void
    if(!pd->menu)
        return;

    //check state of toggle button
    guint LEFT_MOUSE_BUTTON = 1; //from gdk online api
    if(event->button==LEFT_MOUSE_BUTTON)
    {
        gtk_menu_popup_at_widget(GTK_MENU(pd->menu), 
                                 pd->hvbox,       
                                 GDK_GRAVITY_NORTH_WEST,     
                                 GDK_GRAVITY_NORTH_WEST,    
                                 (GdkEvent *) event); // Used to be NULL
    }
    else
    {
        gtk_menu_popdown(GTK_MENU(pd->menu));
    }
}

static void plugin_orientation_changed (XfcePanelPlugin *plugin,
                                        GtkOrientation   orientation,
                                        PomodoroPlugin  *pd)
{

    /* change the orienation of the box */
    gtk_orientable_set_orientation(GTK_ORIENTABLE(pd->hvbox), orientation);

}

static gboolean plugin_size_changed(XfcePanelPlugin *plugin, 
                                    gint size, 
                                    PomodoroPlugin *pd)
{
    GtkOrientation orientation;

    /* get the orientation of the plugin */
    orientation = xfce_panel_plugin_get_orientation (plugin);

    /* set the widget size */
    if (orientation == GTK_ORIENTATION_HORIZONTAL)
        gtk_widget_set_size_request (GTK_WIDGET (plugin), -1, size);
    else
        gtk_widget_set_size_request (GTK_WIDGET (plugin), size, -1);


    /* we handled the orientation */
    return TRUE;
}
