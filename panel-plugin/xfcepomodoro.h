#ifndef __XFCEPOMODORO_H__
#define __XFCEPOMODORO_H__
G_BEGIN_DECLS

/* plugin structure */
typedef struct {
    XfcePanelPlugin *xfcePlugin;
    
    /* panel widgets */
    GtkWidget *ebox;  //event box
    GtkWidget *hvbox; //horizontal/vertial box seen on panel
    GtkWidget *pbar;  //progress bar to see time remaining
    GtkWidget *label; //

    /*configure menu widgets*/
    GtkWidget *checkbox_play_ticking;
    GtkWidget *checkbox_play_alarms;

    /* icon  */
    GtkWidget *tomato_icon;
    GdkPixbuf *tomato_icon_pixbuf;

    /* panel menu */
    GtkWidget *menu;
    GtkWidget *mi_start_pomodoro;
    GtkWidget *mi_start_break;
    GtkWidget *mi_stop_pomodoro;

    /* booleans to determine program logic */
    gboolean   timer_is_pomodoro;
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
    
    gboolean   timer_on; /* TRUE if countdown is in progress*/

    GTimer    *timer;    /* The timer. */
    

} PomodoroPlugin;

/* function prototypes */

static void pomodoro_construct(XfcePanelPlugin *plugin); 
static PomodoroPlugin *pomodoroPlugin_new(XfcePanelPlugin *plugin); 
static void plugin_free (XfcePanelPlugin *plugin, PomodoroPlugin *pd);

static void make_menu(PomodoroPlugin *pd);
static void make_pbar(PomodoroPlugin *pd);
static void plugin_clicked(GtkWidget *pbar, GdkEventButton *event, PomodoroPlugin *pd);

static void plugin_orientation_changed(XfcePanelPlugin *plugin,
                                       GtkOrientation   orientation,
                                       PomodoroPlugin  *pd);
static gboolean plugin_size_changed(XfcePanelPlugin *plugin,                              
                                    gint size,                                            
                                    PomodoroPlugin *pd);

G_END_DECLS
#endif /* !__SAMPLE_H__ */
