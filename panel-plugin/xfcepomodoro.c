/*  $Id$
 *
 *  Copyright (C) 2017 Michael Chacko <john@foo.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <stdlib.h>
#include <unistd.h>

#include <gtk/gtk.h>
#include <libxfce4util/libxfce4util.h>
#include <libxfce4panel/xfce-panel-plugin.h>

#include "xfcepomodoro.h"
#include "xfcepomodoro-dialogs.h"

/* default settings */
#define DEFAULT_SETTING1 NULL
#define DEFAULT_SETTING2 1
#define DEFAULT_SETTING3 FALSE


#define UPDATE_INTERVAL 2000 //update period interval in millisecs

//#define BREAK_PERIOD    5*60  //break period in seconds
#define POMODORO_PERIOD 25 //pomodoro period in seconds
#define BREAK_PERIOD    5  //break period in seconds

/* prototypes */
static void
pomodoro_construct (XfcePanelPlugin *plugin);


/* register the plugin */
XFCE_PANEL_PLUGIN_REGISTER_INTERNAL (pomodoro_construct);

void
sample_save (XfcePanelPlugin *plugin,
             PomodoroPlugin    *pomodoroPlugin)
{
  XfceRc *rc;
  gchar  *file;

  /* get the config file location */
  file = xfce_panel_plugin_save_location (plugin, TRUE);

  if (G_UNLIKELY (file == NULL))
    {
       DBG ("Failed to open config file");
       return;
    }

  /* open the config file, read/write */
  rc = xfce_rc_simple_open (file, FALSE);
  g_free (file);

  if (G_LIKELY (rc != NULL))
    {
      /* save the settings */
      DBG(".");
      if (pomodoroPlugin->setting1)
        xfce_rc_write_entry    (rc, "setting1", pomodoroPlugin->setting1);

      xfce_rc_write_int_entry  (rc, "setting2", pomodoroPlugin->setting2);
      xfce_rc_write_bool_entry (rc, "setting3", pomodoroPlugin->setting3);

      /* close the rc file */
      xfce_rc_close (rc);
    }
}



static void
sample_read (PomodoroPlugin *pomodoroPlugin)
{
  XfceRc      *rc;
  gchar       *file;
  const gchar *value;

  /* get the plugin config file location */
  file = xfce_panel_plugin_save_location (pomodoroPlugin->plugin, TRUE);

  if (G_LIKELY (file != NULL))
    {
      /* open the config file, readonly */
      rc = xfce_rc_simple_open (file, TRUE);

      /* cleanup */
      g_free (file);

      if (G_LIKELY (rc != NULL))
        {
          /* read the settings */
          value = xfce_rc_read_entry (rc, "setting1", DEFAULT_SETTING1);
          pomodoroPlugin->setting1 = g_strdup (value);

          pomodoroPlugin->setting2 = xfce_rc_read_int_entry (rc, "setting2", DEFAULT_SETTING2);
          pomodoroPlugin->setting3 = xfce_rc_read_bool_entry (rc, "setting3", DEFAULT_SETTING3);

          /* cleanup */
          xfce_rc_close (rc);

          /* leave the function, everything went well */
          return;
        }
    }

  /* something went wrong, apply default values */
  DBG ("Applying default settings");

  pomodoroPlugin->setting1 = g_strdup (DEFAULT_SETTING1);
  pomodoroPlugin->setting2 = DEFAULT_SETTING2;
  pomodoroPlugin->setting3 = DEFAULT_SETTING3;
}



static PomodoroPlugin *pomodoroPlugin_new (XfcePanelPlugin *plugin){
  PomodoroPlugin   *pomodoroPlugin;
  GtkOrientation  orientation;
  GtkWidget      *label;

  /* allocate memory for the plugin structure */
  pomodoroPlugin = panel_slice_new (PomodoroPlugin);

  /* pointer to plugin */
  pomodoroPlugin->plugin = plugin;

  /* read the user settings */
  sample_read (pomodoroPlugin);

  /* get the current orientation */
  orientation = xfce_panel_plugin_get_orientation (plugin);

  /* create some panel widgets */
  pomodoroPlugin->ebox = gtk_event_box_new();
  gtk_widget_show (pomodoroPlugin->ebox);

  pomodoroPlugin->hvbox = gtk_box_new (orientation, 2);
  gtk_widget_show (pomodoroPlugin->hvbox);
  gtk_container_add (GTK_CONTAINER (pomodoroPlugin->ebox), pomodoroPlugin->hvbox);

  /* some sample widgets */
  label = gtk_label_new ("P");
  gtk_widget_show (label);

  //CREATE ICON FOR PANEL
  pomodoroPlugin->tomato_icon_pixbuf = gdk_pixbuf_new_from_file_at_scale ("/home/michael/repos/xfce4-pomodoro-plugin/icons/48x48/xfce4-pomodoro-plugin.png", 20, 20, FALSE, NULL);

  pomodoroPlugin->tomato_icon = gtk_image_new_from_pixbuf(pomodoroPlugin->tomato_icon_pixbuf);

  gtk_widget_show(pomodoroPlugin->tomato_icon);

  gtk_box_pack_start (GTK_BOX (pomodoroPlugin->hvbox), label, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (pomodoroPlugin->hvbox), pomodoroPlugin->tomato_icon, FALSE, FALSE, 0);

  return pomodoroPlugin;
}



static void sample_free (XfcePanelPlugin *plugin, PomodoroPlugin *pomodoroPlugin) {
  GtkWidget *dialog;

  /* check if the dialog is still open. if so, destroy it */
  dialog = g_object_get_data (G_OBJECT (plugin), "dialog");
  if (G_UNLIKELY (dialog != NULL))
    gtk_widget_destroy (dialog);

  /* destroy the panel widgets */
  gtk_widget_destroy (pomodoroPlugin->hvbox);

  /* cleanup the settings */
  if (G_LIKELY (pomodoroPlugin->setting1 != NULL))
    g_free (pomodoroPlugin->setting1);

  /* free the plugin structure */
  panel_slice_free (PomodoroPlugin, pomodoroPlugin);
}



static void
sample_orientation_changed (XfcePanelPlugin *plugin,
                            GtkOrientation   orientation,
                            PomodoroPlugin    *pomodoroPlugin)
{
  /* change the orienation of the box */
  gtk_orientable_set_orientation(GTK_ORIENTABLE(pomodoroPlugin->hvbox), orientation);
}



static gboolean
sample_size_changed (XfcePanelPlugin *plugin,
                     gint             size,
                     PomodoroPlugin    *pomodoroPlugin)
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

static void pomodoro_construct (XfcePanelPlugin *plugin) {
  PomodoroPlugin *pomodoroPlugin;


  /* setup transation domain */
  xfce_textdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");

  /* create the plugin */
  pomodoroPlugin = pomodoroPlugin_new (plugin);

  /* add the ebox to the panel */
  gtk_container_add (GTK_CONTAINER (plugin), pomodoroPlugin->ebox);

  /* show the panel's right-click menu on this ebox */
  xfce_panel_plugin_add_action_widget (plugin, pomodoroPlugin->ebox);

  /* connect plugin signals */
  g_signal_connect (G_OBJECT (plugin), "free-data",
                    G_CALLBACK (sample_free), pomodoroPlugin);

  g_signal_connect (G_OBJECT (plugin), "save",
                    G_CALLBACK (sample_save), pomodoroPlugin);

  g_signal_connect (G_OBJECT (plugin), "size-changed",
                    G_CALLBACK (sample_size_changed), pomodoroPlugin);

  g_signal_connect (G_OBJECT (plugin), "orientation-changed",
                    G_CALLBACK (sample_orientation_changed), pomodoroPlugin);

  //WHEN PANEL IS PRESSED
  g_signal_connect  (G_OBJECT(plugin), "button_press_event",
                    G_CALLBACK(pbar_clicked), pomodoroPlugin);

  /* show the configure menu item and connect signal */
  xfce_panel_plugin_menu_show_configure (plugin);
  g_signal_connect (G_OBJECT (plugin), "configure-plugin",
                    G_CALLBACK (sample_configure), pomodoroPlugin);

  /* show the about menu item and connect signal */
  xfce_panel_plugin_menu_show_about (plugin);
  g_signal_connect (G_OBJECT (plugin), "about",
                    G_CALLBACK (sample_about), NULL);

}
static
void start_pomodoro (PomodoroPlugin *pomodoroPlugin){
  
  //TODO: CHANGING FROM 0 CAUSES ERROR. PLZ DEBUG
  pomodoroPlugin->timeout_period_in_sec = 2;
  pomodoroPlugin->pomodoro_is_running = TRUE;

  //start ticking sound
  system("mplayer ~/repos/xfce4-pomodoro-plugin/audio/ticking.flac &");
  
  /*
  pomodoroPlugin->timer = g_timer_new();
  g_timer_start(pomodoroPlugin->timer);

  
  pomodoroPlugin->timeout = g_timeout_add(UPDATE_INTERVAL, update_function, pomodoroPlugin);
  */
}

//update_function(PomodoroPlugin *pp)
//returns TRUE if timer is still running
//returns FALSE if timer is NOT running
static
gboolean update_function(gpointer data){
/*
    PomodoroPlugin *pomodoroPlugin=(PomodoroPlugin *)data;

    gint elapsed_sec, remaining; 

    elapsed_sec=(gint)g_timer_elapsed(pomodoroPlugin->timer,NULL);
    
    //if timer is still running
    if(elapsed_sec < pomodoroPlugin->timeout_period_in_sec){
        remaining=pomodoroPlugin->timeout_period_in_sec-elapsed_sec;
    
        return TRUE;
    }

    // else countdown is over, stop timer and free resources //
    if(pomodoroPlugin->timer){
       g_timer_destroy(pomodoroPlugin->timer);
    }
    pomodoroPlugin->timer=NULL;
    
    pomodoroPlugin->timeout=0;

    pomodoroPlugin->timer_on=FALSE;
    
*/
    return FALSE;
}

static
void pbar_clicked (GtkWidget *pbar, 
                   GdkEventButton *event, 
                   PomodoroPlugin *pomodoroPlugin) {                                        

    pomodoro_plugin_make_menu(pomodoroPlugin); 

    //if menu does not exist, return void
    if(!pomodoroPlugin->menu){
      return;
    }

    //check state of toggle button
    if(event->button==1){
      gtk_menu_popup_at_widget (GTK_MENU(pomodoroPlugin->menu), 
                                pomodoroPlugin->hvbox,       
                                GDK_GRAVITY_NORTH_WEST,     
                                GDK_GRAVITY_NORTH_WEST,    
                                NULL);
    } else {
      gtk_menu_popdown(GTK_MENU(pomodoroPlugin->menu));
    }
}

void
pomodoro_plugin_make_menu (PomodoroPlugin *pomodoroPlugin) {
  

    //TODO: Find out if it's neccessary to destroy the menu. it seems to 
    //      cause problems when repeating tmers 
    /* Destroy the existing one */
    //if(pomodoroPlugin->menu){
    //  gtk_widget_destroy(pomodoroPlugin->menu);
    //}

	pomodoroPlugin->menu = gtk_menu_new();

    /* Start pomodoro menu item */
	pomodoroPlugin->mi_start_pomodoro = gtk_menu_item_new_with_label ("Start a pomodoro");
	gtk_menu_shell_append (GTK_MENU_SHELL(pomodoroPlugin->menu),
	                       pomodoroPlugin->mi_start_pomodoro);
    g_signal_connect  (G_OBJECT(pomodoroPlugin->mi_start_pomodoro), "activate",
                       G_CALLBACK(start_pomodoro), pomodoroPlugin);
	gtk_widget_show(pomodoroPlugin->mi_start_pomodoro);

    /* Stop pomodoro menu item */
	pomodoroPlugin->mi_stop_pomodoro  = gtk_menu_item_new_with_label ("Stop");
	gtk_menu_shell_append (GTK_MENU_SHELL(pomodoroPlugin->menu),
	                       pomodoroPlugin->mi_stop_pomodoro);
    g_signal_connect  (G_OBJECT(pomodoroPlugin->mi_stop_pomodoro), "activate",
                       G_CALLBACK(start_pomodoro), pomodoroPlugin);
	gtk_widget_show(pomodoroPlugin->mi_stop_pomodoro);

	gtk_widget_show(pomodoroPlugin->menu);
}



