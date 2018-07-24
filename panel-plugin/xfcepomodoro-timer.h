#ifndef __XFCEPOMODORO_TIMER_H__
#define __XFCEPOMODORO_TIMER_H__
G_BEGIN_DECLS

gboolean update_timer(gpointer data);
void start_timer(GtkWidget *pbar, PomodoroPlugin *pd);
void stop_timer(GtkWidget *pbar, PomodoroPlugin *pd);

G_END_DECLS
#endif
