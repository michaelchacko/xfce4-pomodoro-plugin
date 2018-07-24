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

#define DEFAULT_SETTING1 NULL 
#define DEFAULT_SETTING2 1
#define DEFAULT_SETTING3 FALSE

void
config_save (XfcePanelPlugin *plugin,
             PomodoroPlugin    *pd)
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
      if (pd->setting1)
        xfce_rc_write_entry    (rc, "setting1", pd->setting1);

      xfce_rc_write_int_entry  (rc, "setting2", pd->setting2);
      xfce_rc_write_bool_entry (rc, "setting3", pd->setting3);

      /* close the rc file */
      xfce_rc_close (rc);
    }
}

void
config_read (PomodoroPlugin *pd)
{
  XfceRc      *rc;
  gchar       *file;
  const gchar *value;

  /* get the plugin config file location */
  file = xfce_panel_plugin_save_location (pd->xfcePlugin, TRUE);

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
          pd->setting1 = g_strdup (value);

          pd->setting2 = xfce_rc_read_int_entry (rc, "setting2", DEFAULT_SETTING2);
          pd->setting3 = xfce_rc_read_bool_entry (rc, "setting3", DEFAULT_SETTING3);

          /* cleanup */
          xfce_rc_close (rc);

          /* leave the function, everything went well */
          return;
        }
    }

  /* something went wrong, apply default values */
  DBG ("Applying default settings");

  pd->setting1 = g_strdup (DEFAULT_SETTING1);
  pd->setting2 = DEFAULT_SETTING2;
  pd->setting3 = DEFAULT_SETTING3;
}
