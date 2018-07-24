#ifndef __XFCEPOMODORO_CONFIG_H__
#define __XFCEPOMODORO_CONFIG_H__
G_BEGIN_DECLS

void config_read (PomodoroPlugin *pd);
void config_save(XfcePanelPlugin *plugin, PomodoroPlugin *pd);

G_END_DECLS
#endif
