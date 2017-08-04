/*  $Id$
 *
 *  Copyright (C) 2012 John Doo <john@foo.org>
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

#ifndef __SAMPLE_H__
#define __SAMPLE_H__

G_BEGIN_DECLS

/* plugin structure */
typedef struct {
    XfcePanelPlugin *plugin;
    
    /* panel widgets */
    GtkWidget *ebox;
    GtkWidget *hvbox; //?? box seen on panel
    GtkWidget *label;

    /*  */
    GtkWidget *tomato_icon;
    GdkPixbuf *tomato_icon_pixbuf;

    /* panel menu */
    GtkWidget *menu;
    GtkWidget *mi_start_pomodoro;
    GtkWidget *mi_start_break;
    GtkWidget *mi_stop_pomodoro;

    /* booleans to determine program logic */
    gboolean   pomodoro_is_running;
    gboolean   break_is_running;

    /* settings */
    gchar     *configfile;
    gchar     *setting1;
    gint       setting2;
    gboolean   setting3;

    gboolean   play_ticking;
    gboolean   play_alarms;

    /* alarm data */
    guint      timeout;  /* timeout IDs*/

    gint       timeout_period_in_sec;
    
    gboolean   alarm_is_pomodoro,
               alarm_is_break;

    gboolean   timer_on; /* TRUE if countdown is in progress*/

    GTimer    *timer;    /* The timer. */
    

} PomodoroPlugin;

static gboolean update_function(gpointer data);

static
void start_pomodoro (PomodoroPlugin *pomodoroPlugin);

void sample_save (XfcePanelPlugin *plugin, PomodoroPlugin    *pomodoroPlugin);

static
void pbar_clicked (GtkWidget *pbar, GdkEventButton *event, PomodoroPlugin *pomodoroPlugin);                                        
void
pomodoro_plugin_make_menu (PomodoroPlugin *pomodoroplugin);

G_END_DECLS

#endif /* !__SAMPLE_H__ */
