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

#define DEFAULT_PLAY_TICKING TRUE 
#define DEFAULT_PLAY_ALARMS  TRUE
#define CONFIG_PLAY_TICKING "config_play_ticking"
#define CONFIG_PLAY_ALARMS  "config_play_alarms"

void config_save(XfcePanelPlugin *plugin,
                 PomodoroPlugin  *pd)
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
      xfce_rc_write_bool_entry (rc, CONFIG_PLAY_TICKING, pd->play_ticking);
      xfce_rc_write_bool_entry (rc, CONFIG_PLAY_ALARMS, pd->play_alarms);

      /* close the rc file */
      xfce_rc_close (rc);
    }
}

void config_read (PomodoroPlugin *pd)
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
          pd->play_ticking = xfce_rc_read_bool_entry(rc, CONFIG_PLAY_TICKING, 
                                                     DEFAULT_PLAY_TICKING);
          pd->play_alarms = xfce_rc_read_bool_entry(rc, CONFIG_PLAY_ALARMS, 
                                                    DEFAULT_PLAY_ALARMS);

          /* cleanup */
          xfce_rc_close (rc);

          /* leave the function, everything went well */
          return;
        }
    }

  /* something went wrong, apply default values */
  DBG ("Applying default settings");

  pd->play_ticking = DEFAULT_PLAY_TICKING;
  pd->play_alarms  = DEFAULT_PLAY_ALARMS;
}
