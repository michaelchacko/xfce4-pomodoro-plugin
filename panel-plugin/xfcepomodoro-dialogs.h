#ifndef __XFCEPOMODORO_DIALOGS_H__
#define __XFCEPOMODORO_DIALOGS_H__
G_BEGIN_DECLS

void plugin_configure(XfcePanelPlugin *plugin, PomodoroPlugin *pomodoroPlugin);

static void configdialog_ticking_toggled(GtkWidget *widget, PomodoroPlugin *pd);
static void configdialog_alarms_toggled(GtkWidget *widget, PomodoroPlugin *pd);

void pomodoro_timer_finished_dialog (XfcePanelPlugin *plugin, PomodoroPlugin *pd);
static void pomodoro_timer_finished_dialog_response (GtkWidget *dialog, gint response,PomodoroPlugin *pd);
 
void plugin_about(XfcePanelPlugin *plugin);

G_END_DECLS
#endif
