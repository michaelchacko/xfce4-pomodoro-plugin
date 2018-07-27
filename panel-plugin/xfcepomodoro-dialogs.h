#ifndef __XFCEPOMODORO_DIALOGS_H__
#define __XFCEPOMODORO_DIALOGS_H__
G_BEGIN_DECLS

void plugin_configure(XfcePanelPlugin *plugin, PomodoroPlugin *pomodoroPlugin);

static void configdialog_ticking_toggled(GtkWidget *widget, PomodoroPlugin *pd);
static void configdialog_alarms_toggled(GtkWidget *widget, PomodoroPlugin *pd);

void plugin_about(XfcePanelPlugin *plugin);

G_END_DECLS
#endif
