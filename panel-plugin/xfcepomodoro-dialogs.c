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

static void
plugin_configure_response (GtkWidget    *dialog,
                           gint          response,
                           PomodoroPlugin *pd) {
  gboolean result;

  if (response == GTK_RESPONSE_HELP)
    {
      /* show help */
      result = g_spawn_command_line_async ("exo-open --launch WebBrowser " PLUGIN_WEBSITE, NULL);

      if (G_UNLIKELY (result == FALSE))
        g_warning (_("Unable to open the following url: %s"), PLUGIN_WEBSITE);
    }
  else
    {
      /* remove the dialog data from the plugin */
      g_object_set_data (G_OBJECT (pd->xfcePlugin), "dialog", NULL);

      /* unlock the panel menu */
      xfce_panel_plugin_unblock_menu (pd->xfcePlugin);

      /* save the plugin */
      config_save(pd->xfcePlugin, pd);

      /* destroy the properties dialog */
      gtk_widget_destroy (dialog);
    }
}

void plugin_configure (XfcePanelPlugin *plugin,
                       PomodoroPlugin    *pd) {
  GtkWidget *dialog;
  GtkWidget *content_area;

  GtkWidget *checkbox_play_ticking;
  GtkWidget *checkbox_play_alarms;

  /* block the plugin menu */
  //xfce_panel_plugin_block_menu (plugin);

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
  checkbox_play_ticking = gtk_check_button_new_with_label ("Play ticking sound?");
  gtk_container_add (GTK_CONTAINER (content_area), checkbox_play_ticking);

  /* checkbox for alarm sound*/
  checkbox_play_alarms = gtk_check_button_new_with_label ("End of session / break alarms");
  gtk_container_add (GTK_CONTAINER (content_area), checkbox_play_alarms);

  /* show the entire dialog */
  gtk_widget_show_all (dialog);
}



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

// Function to open a dialog box with a message
void
quick_message (GtkWindow *parent, gchar *message){
 GtkWidget *dialog, *label, *content_area;
 GtkDialogFlags flags;

 // Create the widgets
 flags = GTK_DIALOG_DESTROY_WITH_PARENT;
 dialog = gtk_dialog_new_with_buttons ("Message",
                                       parent,
                                       flags,
                                       _("_OK"),
                                       GTK_RESPONSE_NONE,
                                       NULL);
 content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
 label = gtk_label_new (message);

 // Ensure that the dialog box is destroyed when the user responds

 g_signal_connect_swapped (dialog,
                           "response",
                           G_CALLBACK (gtk_widget_destroy),
                           dialog);

 // Add the label, and show everything we’ve added

 gtk_container_add (GTK_CONTAINER (content_area), label);
 gtk_widget_show_all (dialog);
}
