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

/* website url */
#define PLUGIN_WEBSITE "http://chackotaco.github.io/"

void plugin_about (XfcePanelPlugin *plugin) {

  /* about dialog code. you can use the GtkAboutDialog
   * or the XfceAboutInfo widget */
  GdkPixbuf *icon;

  const gchar *auth[] =
    {
      "Michael Chacko <xfce4-dev@xfce.org>",
      NULL
    };

  icon = xfce_panel_pixbuf_from_source ("xfce4-pomodoro-plugin", NULL, 32);
  gtk_show_about_dialog (NULL,
                         "logo",         icon,
                         "license",      xfce_get_license_text (XFCE_LICENSE_TEXT_GPL),
                         "version",      PACKAGE_VERSION,
                         "program-name", PACKAGE_NAME,
                         "comments",     _("A plugin for the Pomodoro time management technique"),
                         "website",      PLUGIN_WEBSITE,
                         "copyright",    _("Copyright \xc2\xa9 2018 Michael Chacko\n"),
                         "authors",      auth,
                         NULL);

  if (icon)
    g_object_unref (G_OBJECT (icon));
}

static void
plugin_configure_response (GtkWidget    *dialog,
                           gint          response,
                           PomodoroPlugin *pd) {
    gboolean result;
    
    if(response == GTK_RESPONSE_HELP) //if HELP button is pressed
    {
        /* show help */
        result = g_spawn_command_line_async ("exo-open --launch WebBrowser " PLUGIN_WEBSITE, NULL);

        if (G_UNLIKELY (result == FALSE))
        g_warning(_("Unable to open the following url: %s"), PLUGIN_WEBSITE);
    }
    else //else Close button was pressed
    {
        /* remove the dialog data from the plugin */
        g_object_set_data (G_OBJECT (pd->xfcePlugin), "dialog", NULL);

        /* unlock the panel menu */
        xfce_panel_plugin_unblock_menu (pd->xfcePlugin);

        /* save the plugin */
        config_save(pd->xfcePlugin, pd);

        /* destroy the properties dialog */
        gtk_widget_destroy(dialog);
    }
}

void plugin_configure (XfcePanelPlugin *plugin,
                       PomodoroPlugin    *pd) {
  GtkWidget *dialog;
  GtkWidget *content_area;

  /* block the plugin menu */
  xfce_panel_plugin_block_menu (plugin);

  /* create the dialog */
  dialog = xfce_titled_dialog_new_with_buttons (_("Pomodoro Plugin Settings"),
                                                GTK_WINDOW (gtk_widget_get_toplevel (GTK_WIDGET (plugin))),
                                                GTK_DIALOG_DESTROY_WITH_PARENT,
                                                "gtk-help", GTK_RESPONSE_HELP,
                                                "gtk-close", GTK_RESPONSE_OK,
                                                NULL);

  content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));

  /* center dialog on the screen */
  gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_CENTER);

  /* set dialog icon */
  gtk_window_set_icon_name (GTK_WINDOW (dialog), "xfce4-settings");

  /* link the dialog to the plugin, so we can destroy it when the plugin
   * is closed, but the dialog is still open */
  g_object_set_data (G_OBJECT (plugin), "dialog", dialog);

  /* connect the reponse signal to the dialog */
  g_signal_connect (G_OBJECT (dialog), "response",
                    G_CALLBACK(plugin_configure_response), pd);

  /* checkbox for ticking sound*/
  pd->checkbox_play_ticking = gtk_check_button_new_with_label("Play ticking sound?");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pd->checkbox_play_ticking),
                               pd->play_ticking); 
  g_signal_connect(G_OBJECT(pd->checkbox_play_ticking), 
                            "toggled", 
                            G_CALLBACK(configdialog_ticking_toggled), pd);
  gtk_container_add (GTK_CONTAINER (content_area), pd->checkbox_play_ticking);

  /* checkbox for alarm sound*/
  pd->checkbox_play_alarms = gtk_check_button_new_with_label("End of session / break alarms");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pd->checkbox_play_alarms),
                               pd->play_alarms); 
  g_signal_connect(G_OBJECT(pd->checkbox_play_alarms), 
                            "toggled", 
                            G_CALLBACK(configdialog_alarms_toggled), pd);
  gtk_container_add (GTK_CONTAINER (content_area), pd->checkbox_play_alarms);

  /* show the entire dialog */
  gtk_widget_show_all (dialog);

}

/* callback functions controlling behaviour for configure settings */
static void configdialog_ticking_toggled(GtkWidget *widget, PomodoroPlugin *pd)
{
    pd->play_ticking = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pd->checkbox_play_ticking)); 
}

static void configdialog_alarms_toggled(GtkWidget *widget, PomodoroPlugin *pd)
{
    pd->play_alarms = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pd->checkbox_play_alarms)); 
}

void pomodoro_timer_finished_dialog (XfcePanelPlugin *plugin,
                                     PomodoroPlugin    *pd) {
    GtkWidget *dialog;
    gchar *dialog_title, *dialog_message;

    dialog_message = g_strdup_printf("\nYour pomodoro is finished! Take your mind off work for a bit.");

    /* Display the name of the alarm when the countdown ends */
    dialog_title = g_strdup_printf("Pomodoro Finished!");

    dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL,
                                    GTK_MESSAGE_WARNING,
                                    GTK_BUTTONS_NONE, dialog_message);
                           
    gtk_window_set_title ((GtkWindow *) dialog, dialog_title);                                    
 
    gtk_dialog_add_button ( (GtkDialog *) dialog, "_Close", 0);
    gtk_dialog_add_button ( (GtkDialog *) dialog, _("5 minute break"), 1);

    g_signal_connect (dialog, "response",
                      G_CALLBACK (pomodoro_timer_finished_dialog_response),
                      pd);
                           
    g_free(dialog_title);
    g_free(dialog_message);
    
    gtk_widget_show(dialog);
}

static void
pomodoro_timer_finished_dialog_response (GtkWidget    *dialog,
                                         gint          response,
                                         PomodoroPlugin *pd) 
{
    gboolean result;
    
    if(response == 1) //5 min break was pressed
    {
        pd->timer_is_pomodoro = FALSE;
        start_timer(NULL, pd);
        gtk_widget_destroy(dialog);
    }
    else //destroy dialog
    {
        gtk_widget_destroy(dialog);
    }
}
