/* E-Jamz first native slice: local queue, single GStreamer owner, optional rack. */
#include <Elementary.h>
#include <Ecore_X.h>
#include <gst/gst.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <glib/gstdio.h>
#include "theme-palette.h"

#define UI_WIDTH 400
#define PLAYER_HEIGHT 215
#define RACK_HEIGHT 183
#define QUEUE_HEIGHT 287

typedef struct {
    GstElement *player, *eq, *gain;
    GstBus *bus;
    Evas_Object *win, *face, *seek, *volume, *rack, *queue_win, *queue_face, *rack_face, *stack, *main_layout;
    Eina_List *paths;
    GArray *visible;
    char *filter;
    Evas_Object *filter_entry, *dialog_entry;
    int current, queue_page, preset;
    Evas_Object *preset_menu;
    Eina_Bool updating, bypass, rack_open, queue_open, transport_grabbed;
    double bands[10];
    double volume_level;
    GPid decoder;
    int rawfd;
    Eina_Bool tracker, decoder_failed;
    unsigned generation;
    double tracker_duration;
} App;
static App app;
static Elm_Theme *controls_theme;
static int skin;
static char *playlist_directory;
static Eina_Bool remember_audio=EINA_TRUE,closing;
#define DRAWER_WIDTH 280
static Evas_Object *settings_popup,*drawer_face,*drawer_clip,*drawer_slot;
static Evas_Object *settings_checks[3],*settings_radio,*settings_radios[3],*rack_seams[2];
static Ecore_Animator *drawer_anim;
static int drawer_width,drawer_target,drawer_from,window_height=PLAYER_HEIGHT;
static void drawer_position(void) {
    if(!settings_popup)return;
    /* This unscaled rack occupies the left 400 canvas pixels. Box geometry
       can be transient while the WM acknowledges animated window resizes. */
    evas_object_move(settings_popup,UI_WIDTH,0);
    evas_object_resize(settings_popup,DRAWER_WIDTH,MAX(420,window_height));
    evas_object_move(drawer_clip,UI_WIDTH,0);
    evas_object_resize(drawer_clip,drawer_width,MAX(420,window_height));
}
static const char *skin_names[]={"Blue Steel","Midnight","Amber Steel"};
static char *settings_path(void) {
    const char *override=getenv("EJAMZ_CONFIG_FILE");
    return override?g_strdup(override):g_build_filename(g_get_user_config_dir(),"ejamz","settings.ini",NULL);
}
static void settings_load(void) {
    if(getenv("EJAMZ_VERIFY")||getenv("EJAMZ_VERIFY_DROP"))return;
    char *path=settings_path();GKeyFile *config=g_key_file_new();
    if(g_key_file_load_from_file(config,path,G_KEY_FILE_NONE,NULL)) {
        GError *error=NULL;int value=g_key_file_get_integer(config,"view","theme",&error);
        if(!error&&value>=0&&value<3)skin=value;
        g_clear_error(&error);
        if(g_key_file_has_key(config,"view","playlist",NULL))app.queue_open=g_key_file_get_boolean(config,"view","playlist",NULL);
        if(g_key_file_has_key(config,"view","equalizer",NULL))app.rack_open=g_key_file_get_boolean(config,"view","equalizer",NULL);
        if(g_key_file_has_key(config,"audio","remember",NULL))remember_audio=g_key_file_get_boolean(config,"audio","remember",NULL);
        if(remember_audio) {
            double volume=g_key_file_get_double(config,"audio","volume",&error);
            if(!error&&isfinite(volume)&&volume>=0&&volume<=1)app.volume_level=volume;
            g_clear_error(&error);
            gsize count=0;double *bands=g_key_file_get_double_list(config,"audio","bands",&count,&error);
            Eina_Bool valid=!error&&count==10;
            for(gsize i=0;valid&&i<count;i++)if(!isfinite(bands[i])||bands[i]<-12||bands[i]>6)valid=EINA_FALSE;
            if(valid) { memcpy(app.bands,bands,sizeof(app.bands));app.preset=-1; }
            g_free(bands);g_clear_error(&error);
            if(g_key_file_has_key(config,"audio","bypass",NULL))app.bypass=g_key_file_get_boolean(config,"audio","bypass",NULL);
        }
    }
    playlist_directory=g_key_file_get_string(config,"library","playlist_directory",NULL);
    g_key_file_unref(config);g_free(path);
}
static Eina_Bool settings_save(void) {
    if(getenv("EJAMZ_VERIFY")||getenv("EJAMZ_VERIFY_DROP"))return EINA_TRUE;
    GKeyFile *config=g_key_file_new();
    g_key_file_set_integer(config,"view","theme",skin);
    if(playlist_directory)g_key_file_set_string(config,"library","playlist_directory",playlist_directory);
    g_key_file_set_boolean(config,"view","playlist",app.queue_open);
    g_key_file_set_boolean(config,"view","equalizer",app.rack_open);
    g_key_file_set_boolean(config,"audio","remember",remember_audio);
    if(remember_audio) {
        g_key_file_set_double(config,"audio","volume",app.volume_level);
        g_key_file_set_double_list(config,"audio","bands",app.bands,10);
        g_key_file_set_boolean(config,"audio","bypass",app.bypass);
    }
    char *contents=g_key_file_to_data(config,NULL,NULL),*path=settings_path(),*dir=g_path_get_dirname(path);
    GError *error=NULL;Eina_Bool okay=g_mkdir_with_parents(dir,0700)==0;
    if(okay)okay=g_file_set_contents_full(path,contents,-1,G_FILE_SET_CONTENTS_CONSISTENT,0600,&error);
    if(okay&&g_chmod(path,0600)!=0)okay=EINA_FALSE;
    if(!okay)fprintf(stderr,"E-Jamz settings: %s: %s\n",path,error?error->message:strerror(errno));
    g_clear_error(&error);g_free(contents);g_free(path);g_free(dir);g_key_file_unref(config);return okay;
}
static void theme_apply(void) {
    /* All panels share one ARGB surface; the skin, not Elementary's
       unrelated standard-window background, owns transparent regions. */
    if(app.win&&app.face){const char *alpha=edje_object_data_get(app.face,"window.alpha");elm_win_alpha_set(app.win,alpha&&!strcmp(alpha,"1"));}
    Evas_Object *faces[]={app.face,app.queue_face,app.rack_face,drawer_face};
    for(unsigned j=0;j<sizeof(faces)/sizeof(faces[0]);j++)if(faces[j])
        for(unsigned i=0;i<sizeof(palette)/sizeof(palette[0]);i++) {
            int r=palette[i].r,g=palette[i].g,b=palette[i].b,a=palette[i].a;
            if(skin) {
                if(r==255&&g==255&&b==255) { r=skin==1?140:235;g=skin==1?160:205;b=skin==1?180:165; }
                else if(r==189&&g==214&&b==228) { r=skin==1?170:238;g=skin==1?211:207;b=skin==1?198:140; }
                else if(r==23&&g==60&&b==82) { r=skin==1?19:66;g=skin==1?49:43;b=skin==1?43:17; }
                else if(r==50&&g==90&&b==110) { r=skin==1?35:100;g=skin==1?78:66;b=skin==1?63:27; }
                else { int l=(r+g+b)/3;r=l;g=l;b=l; }
            }
            edje_object_color_class_set(faces[j],palette[i].name,r,g,b,a,255,255,255,255,255,255,255,255);
        }
    /* Native check/radio labels are TEXT parts, not markup-capable labels. */
    const char *states[]={"normal","pressed","disabled"};
    int ink[3]={skin==1?19:skin==2?66:23,skin==1?49:skin==2?43:60,skin==1?43:skin==2?17:82};
    for(int i=0;i<3;i++)for(int j=0;j<3;j++) {
        char name[80];snprintf(name,sizeof(name),"/fg/%s/radio/text",states[j]);
        if(settings_radios[i])edje_object_color_class_set(elm_layout_edje_get(settings_radios[i]),name,ink[0],ink[1],ink[2],j==2?140:255,0,0,0,0,0,0,0,0);
        snprintf(name,sizeof(name),"/fg/%s/check/text",states[j]);
        if(settings_checks[i])edje_object_color_class_set(elm_layout_edje_get(settings_checks[i]),name,ink[0],ink[1],ink[2],j==2?140:255,0,0,0,0,0,0,0,0);
    }
    if(app.face) {
        int r,g,b,a;
        if(edje_object_color_class_get(app.face,"ejamz.16_39_59_245",&r,&g,&b,&a,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL))
            for(int i=0;i<2;i++)if(rack_seams[i])evas_object_color_set(rack_seams[i],r*a/255,g*a/255,b*a/255,a);
    }
    /* The editable filter sits on the LCD and follows its dark ink color. */
    if(app.filter_entry) {
        elm_entry_text_style_user_pop(app.filter_entry);
        elm_entry_text_style_user_push(app.filter_entry,skin==1?"DEFAULT='font=DejaVu Sans font_size=12 color=#13312b'":skin==2?"DEFAULT='font=DejaVu Sans font_size=12 color=#422b11'":"DEFAULT='font=DejaVu Sans font_size=12 color=#173c52'");
    }
}
static void window_deleted(void *data,Evas *canvas,Evas_Object *obj,void *event) {
    (void)data;(void)canvas;(void)obj;(void)event;closing=EINA_TRUE;app.win=NULL;
}
static void quit_app(void *data, Evas_Object *obj, void *event) {
    (void)data; (void)obj; (void)event; closing=EINA_TRUE;settings_save();elm_exit();
}
static const char *band_names[] = {"29", "59", "119", "237", "474", "947", "1.9k", "3.8k", "7.5k", "15k"};

static void stop_decoder(void) {
    if (!app.decoder) return;
    int value; pid_t result=waitpid(app.decoder,&value,WNOHANG);
    if(result==0) {
        kill(app.decoder,SIGTERM);
        for(int i=0;i<20 && result==0;i++) { g_usleep(10000);result=waitpid(app.decoder,&value,WNOHANG); }
        if(result==0) { kill(app.decoder,SIGKILL);waitpid(app.decoder,&value,0); }
    }
    g_spawn_close_pid(app.decoder);app.decoder=0;
}
static void dispose_player(void) {
    /* End the PCM producer first so a pipe reader can always finish on teardown. */
    stop_decoder();
    if(app.player)gst_element_set_state(app.player,GST_STATE_NULL);
    if(app.rawfd>=0){close(app.rawfd);app.rawfd=-1;}
    if(app.bus)gst_object_unref(app.bus);
    if(app.eq)gst_object_unref(app.eq);
    if(app.gain && app.gain!=app.player)gst_object_unref(app.gain);
    if(app.player)gst_object_unref(app.player);
    app.player=NULL;app.bus=NULL;app.eq=NULL;app.gain=NULL;
}
static void read_tracker_duration(const char *path);
static Eina_Bool create_player(const char *path) {
    app.generation++;app.tracker_duration=0;
    GError *error=NULL;const char *suffix=path?strrchr(path,'.'):NULL;
    app.tracker=suffix && (!strcasecmp(suffix,".mod")||!strcasecmp(suffix,".s3m")||
        !strcasecmp(suffix,".xm")||!strcasecmp(suffix,".med")||!strcasecmp(suffix,".it")||
        !strcasecmp(suffix,".mptm")||!strcasecmp(suffix,".mtm")||!strcasecmp(suffix,".stm")||
        !strcasecmp(suffix,".669")||!strcasecmp(suffix,".okt"));
    app.decoder_failed=EINA_FALSE;
    if(app.tracker) {
        read_tracker_duration(path);
        gchar *args[]={"ffmpeg","-nostdin","-hide_banner","-loglevel","error",
            "-filter_threads","1","-threads","1","-f","libopenmpt","-i",(gchar *)path,
            "-f","s16le","-acodec","pcm_s16le","-ac","2","-ar","48000","pipe:1",NULL};
        if(!g_spawn_async_with_pipes(NULL,args,NULL,G_SPAWN_SEARCH_PATH|G_SPAWN_DO_NOT_REAP_CHILD,
                NULL,NULL,&app.decoder,NULL,&app.rawfd,NULL,&error))goto fail;
        app.player=gst_parse_launch("fdsrc name=pcm ! rawaudioparse format=pcm pcm-format=s16le sample-rate=48000 num-channels=2 ! audioconvert ! equalizer-10bands name=eq ! volume name=gain ! audioconvert ! audioresample ! pulsesink name=output",&error);
        if(error || !app.player)goto fail;
        GstElement *source=gst_bin_get_by_name(GST_BIN(app.player),"pcm");
        g_object_set(source,"fd",app.rawfd,NULL);gst_object_unref(source);
        app.eq=gst_bin_get_by_name(GST_BIN(app.player),"eq");
        app.gain=gst_bin_get_by_name(GST_BIN(app.player),"gain");
        GstElement *output=gst_bin_get_by_name(GST_BIN(app.player),"output");
        g_object_set(output,"client-name","E-Jamz",NULL);gst_object_unref(output);
    } else {
        app.player=gst_element_factory_make("playbin3","ejamz-playback");
        app.eq=gst_element_factory_make("equalizer-10bands","ejamz-eq");
        GstElement *output=gst_element_factory_make("pulsesink","ejamz-output");
        if(!app.player||!app.eq||!output) { if(output)gst_object_unref(output);goto fail; }
        gst_object_ref_sink(app.eq);
        gst_object_ref_sink(output);
        g_object_set(output,"client-name","E-Jamz",NULL);
        /* Audio only: do not probe/init video sinks in an EFL streaming thread. */
        g_object_set(app.player,"flags",(guint)2,"audio-filter",app.eq,"audio-sink",output,NULL);
        gst_object_unref(output);
        app.gain=app.player;
        if(path) { gchar *uri=gst_filename_to_uri(path,&error);if(!uri)goto fail;
            g_object_set(app.player,"uri",uri,NULL);g_free(uri); }
    }
    if(!app.eq || !app.gain)goto fail;
    for(int i=0;i<10;i++){char property[16];snprintf(property,sizeof(property),"band%d",i);
        if(app.eq)g_object_set(app.eq,property,app.bypass?0.0:app.bands[i],NULL);}
    g_object_set(app.gain,"volume",app.volume_level,NULL);
    app.bus=gst_element_get_bus(app.player);
    if(app.seek) {
        app.updating=EINA_TRUE;
        elm_slider_min_max_set(app.seek,0,1);
        elm_slider_value_set(app.seek,0);
        elm_object_disabled_set(app.seek,EINA_TRUE);
        app.updating=EINA_FALSE;
    }
    return EINA_TRUE;
fail:
    fprintf(stderr,"E-Jamz backend: %s\n",error?error->message:"Required audio element unavailable");
    g_clear_error(&error);dispose_player();return EINA_FALSE;
}

static void status(const char *value) {
    edje_object_part_text_set(app.face, "status", value);
}
static void refresh_queue(void) {
    char value[512];int total=eina_list_count(app.paths),index=0;
    if(!app.visible)app.visible=g_array_new(FALSE,FALSE,sizeof(int));
    g_array_set_size(app.visible,0);
    Eina_List *item;const char *path;
    EINA_LIST_FOREACH(app.paths,item,path) {
        char *fold=g_utf8_validate(path,-1,NULL)?g_utf8_casefold(path,-1):g_ascii_strdown(path,-1);
        if(!app.filter||!*app.filter||strstr(fold,app.filter))g_array_append_val(app.visible,index);
        g_free(fold);index++;
    }
    int count=app.visible->len;
    if(app.queue_page*8>=count)app.queue_page=count?(count-1)/8:0;
    snprintf(value,sizeof(value),"%d/%d  PAGE %d",count,total,app.queue_page+1);
    edje_object_part_text_set(app.queue_face,"summary",value);
    if(app.current<0)edje_object_part_text_set(app.face,"title",total?"SELECT A TRACK":"ADD MUSIC");
    edje_object_part_text_set(app.queue_face,"label.save",app.filter&&*app.filter?"SAVE MATCHES":"SAVE LIST");
    for(int i=0;i<8;i++) {
        int row=app.queue_page*8+i;char part[32];snprintf(part,sizeof(part),"row.%d",i);
        if(row<count) {
            int original=g_array_index(app.visible,int,row);path=eina_list_nth(app.paths,original);
            const char *name=strrchr(path,'/');
            snprintf(value,sizeof(value),"%s %02d  %s",original==app.current?">":" ",original+1,name?name+1:path);
        } else value[0]=0;
        edje_object_part_text_set(app.queue_face,part,value);
    }
}
static void transport_grabs(Eina_Bool enabled) {
    if(!app.win||!app.main_layout||app.transport_grabbed==enabled)return;
    const char *keys[]={"z","x","c","v","b","space"};
    for(unsigned i=0;i<sizeof(keys)/sizeof(keys[0]);i++) {
        if(enabled)evas_object_key_grab(app.main_layout,keys[i],0,0,EINA_FALSE);
        else evas_object_key_ungrab(app.main_layout,keys[i],0,0);
    }
    app.transport_grabbed=enabled;
}
static void text_focused(void *data,Evas_Object *obj,void *event) {
    (void)data;(void)obj;(void)event;transport_grabs(EINA_FALSE);
}
static void text_unfocused(void *data,Evas_Object *obj,void *event) {
    (void)data;(void)event;
    Eina_Bool other=(obj!=app.filter_entry&&app.filter_entry&&elm_object_focus_get(app.filter_entry))||
        (obj!=app.dialog_entry&&app.dialog_entry&&elm_object_focus_get(app.dialog_entry));
    transport_grabs(!other);
}
static void filter_focus(void *data,Evas *canvas,Evas_Object *obj,void *event) {
    (void)data;(void)canvas;(void)event;elm_object_focus_set(obj,EINA_TRUE);
}
static void filter_changed(void *data,Evas_Object *obj,void *event) {
    (void)data;(void)event;char *plain=elm_entry_markup_to_utf8(elm_entry_entry_get(obj));
    g_free(app.filter);app.filter=g_utf8_casefold(plain?plain:"",-1);free(plain);
    app.queue_page=0;refresh_queue();
}
static void queue_wheel(void *data,Evas *canvas,Evas_Object *obj,void *event) {
    (void)data;(void)canvas;(void)obj;Evas_Event_Mouse_Wheel *wheel=event;
    if(wheel->direction||!app.visible||!app.visible->len)return;
    int page=app.queue_page+wheel->z,maximum=((int)app.visible->len-1)/8;
    app.queue_page=MAX(0,MIN(page,maximum));refresh_queue();
    wheel->event_flags|=EVAS_EVENT_FLAG_ON_HOLD;
}
static void select_track(int index) {
    const char *path = eina_list_nth(app.paths, index);
    if (!path) { status("Queue is empty — Open a local file"); return; }
    dispose_player();
    if(!create_player(path)){status("Audio backend unavailable — see log");return;}
    app.current = index;
    refresh_queue();
    const char *name = strrchr(path, '/');
    char *display=g_utf8_strup(name?name+1:path,-1);
    edje_object_part_text_set(app.face,"title",display);g_free(display);
    edje_object_part_text_set(app.face,"format",app.tracker?"TRACKER / OPENMPT / SEEK UNAVAILABLE":"LOCAL AUDIO / DESKTOP OUTPUT");
    fprintf(stderr,"E-Jamz opening: %s [%s]\n",path,app.tracker?"OpenMPT PCM":"GStreamer");
    status("Opening…");
    gst_element_set_state(app.player, GST_STATE_PLAYING);
}
static void play(void *data, Evas_Object *obj, const char *emission, const char *source) {
    (void)data; (void)obj; (void)emission; (void)source;
    GstState state=GST_STATE_NULL,pending=GST_STATE_VOID_PENDING;
    if(app.player)gst_element_get_state(app.player,&state,&pending,0);
    if(app.current<0)select_track(0);
    else if(!app.player || (app.tracker && state<=GST_STATE_READY && pending==GST_STATE_VOID_PENDING))select_track(app.current);
    else gst_element_set_state(app.player,GST_STATE_PLAYING);
}
static void pause_track(void *data, Evas_Object *obj, const char *emission, const char *source) {
    (void)data; (void)obj; (void)emission; (void)source;
    if (app.current >= 0 && app.player) gst_element_set_state(app.player, GST_STATE_PAUSED);
}
static void stop_track(void *data, Evas_Object *obj, const char *emission, const char *source) {
    (void)data; (void)obj; (void)emission; (void)source;
    if(app.player)gst_element_set_state(app.player,GST_STATE_READY);
    if(app.tracker)stop_decoder();
    status("Stopped");
    edje_object_part_text_set(app.face,"time","00:00");
}
static void skip(void *data, Evas_Object *obj, const char *emission, const char *source) {
    (void)obj; (void)emission; (void)source;
    int index = app.current + (int)(intptr_t)data;
    if (index >= 0 && index < (int)eina_list_count(app.paths)) select_track(index);
}
static void volume_changed(void *data, Evas_Object *obj, void *event) {
    (void)data; (void)event;
    app.volume_level=elm_slider_value_get(obj);
    if(app.gain)g_object_set(app.gain,"volume",app.volume_level,NULL);
}
static void seek_changed(void *data, Evas_Object *obj, void *event) {
    (void)data; (void)event;
    if (app.updating || app.current < 0 || !app.player || app.tracker) return;
    if (!gst_element_seek_simple(app.player, GST_FORMAT_TIME,
            GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT,
            (gint64)(elm_slider_value_get(obj) * GST_SECOND))) status("This track cannot seek");
}
static Eina_Bool tick(void *data) {
    (void)data;
    GstMessage *message;
    if(app.decoder) {
        int result;pid_t done=waitpid(app.decoder,&result,WNOHANG);
        if(done==app.decoder){app.decoder_failed=!WIFEXITED(result)||WEXITSTATUS(result)!=0;
            g_spawn_close_pid(app.decoder);app.decoder=0;}
    }
    while (app.bus && (message = gst_bus_pop(app.bus))) {
        switch (GST_MESSAGE_TYPE(message)) {
        case GST_MESSAGE_ERROR: {
            GError *error = NULL; gchar *debug = NULL;
            gst_message_parse_error(message, &error, &debug);
            status(error ? error->message : "Playback error");
            fprintf(stderr, "E-Jamz playback: %s\n%s\n", error ? error->message : "unknown error",debug?debug:"");
            g_clear_error(&error); g_free(debug);
            gst_element_set_state(app.player, GST_STATE_NULL);
            stop_decoder();
            break;
        }
        case GST_MESSAGE_EOS:
            if(app.decoder_failed){gst_element_set_state(app.player,GST_STATE_NULL);status("Tracker decoder failed — queue preserved");}
            else if (app.current + 1 < (int)eina_list_count(app.paths)) select_track(app.current + 1);
            else { gst_element_set_state(app.player, GST_STATE_READY); status("Queue finished"); }
            break;
        case GST_MESSAGE_STATE_CHANGED:
            if (GST_MESSAGE_SRC(message) == GST_OBJECT(app.player)) {
                GstState before, after, pending;
                gst_message_parse_state_changed(message, &before, &after, &pending);
                if (after == GST_STATE_PLAYING) status("Playing");
                if (after == GST_STATE_PAUSED) status("Paused");
            }
            break;
        default: break;
        }
        gst_message_unref(message);
    }
    /* Bus handling continues hidden; visual updates only while the player is shown. */
    if (app.player && evas_object_visible_get(app.win) && !elm_win_iconified_get(app.win)) {
        gint64 position = 0, duration = 0;
        if (gst_element_query_position(app.player, GST_FORMAT_TIME, &position)) {
            char value[32]; long seconds = position / GST_SECOND;
            snprintf(value, sizeof(value), "%02ld:%02ld", seconds / 60, seconds % 60);
            edje_object_part_text_set(app.face, "time", value);
            app.updating = EINA_TRUE;
            if(app.tracker && app.tracker_duration>0) {
                elm_slider_min_max_set(app.seek,0,app.tracker_duration);
                elm_slider_value_set(app.seek,MIN((double)position/GST_SECOND,app.tracker_duration));
            }
            if (!app.tracker && gst_element_query_duration(app.player, GST_FORMAT_TIME, &duration) && duration > 0) {
                GstQuery *query=gst_query_new_seeking(GST_FORMAT_TIME);
                gboolean seekable=FALSE;
                if(gst_element_query(app.player,query))gst_query_parse_seeking(query,NULL,&seekable,NULL,NULL);
                gst_query_unref(query);
                elm_object_disabled_set(app.seek,!seekable);
                elm_slider_min_max_set(app.seek,0,(double)duration/GST_SECOND);
                elm_slider_value_set(app.seek,(double)MIN(MAX(position,0),duration)/GST_SECOND);
            }
            app.updating = EINA_FALSE;
        }
    }
    return ECORE_CALLBACK_RENEW;
}
static void row_selected(void *data, Evas_Object *obj, const char *emission, const char *source) {
    (void)data; (void)obj; (void)emission;
    int row=app.queue_page*8+atoi(source+10);
    if(app.visible && row<(int)app.visible->len)select_track(g_array_index(app.visible,int,row));
}
static void queue_page(void *data, Evas_Object *obj, const char *emission, const char *source) {
    (void)obj; (void)emission; (void)source;
    int page=app.queue_page+(int)(intptr_t)data;
    if(page>=0 && app.visible && page*8<(int)app.visible->len) { app.queue_page=page;refresh_queue(); }
}
static void add_file(const char *path) {
    if (!path) return;
    char *absolute = realpath(path, NULL);
    if (!absolute) { status("File not found; queue preserved"); return; }
    app.paths = eina_list_append(app.paths, absolute);
    refresh_queue();
    status("Added to up next");
}
/* Ecore owns probe children so its SIGCHLD handling cannot race GLib waitpid. */
static GHashTable *duration_probes;
static Ecore_Event_Handler *duration_data_handler,*duration_exit_handler;
static Eina_Bool duration_data(void *data,int type,void *event) {
    (void)data;(void)type;Ecore_Exe_Event_Data *e=event;
    unsigned *generation=duration_probes?g_hash_table_lookup(duration_probes,e->exe):NULL;
    if(generation && *generation==app.generation && e->data && e->size>0) {
        char *value=g_strndup(e->data,e->size),*end=NULL;double duration=g_ascii_strtod(value,&end);
        if(end!=value && duration>0 && duration<31536000)app.tracker_duration=duration;
        g_free(value);
    }
    return ECORE_CALLBACK_PASS_ON;
}
static Eina_Bool duration_exit(void *data,int type,void *event) {
    (void)data;(void)type;Ecore_Exe_Event_Del *e=event;
    if(duration_probes)g_hash_table_remove(duration_probes,e->exe);
    return ECORE_CALLBACK_PASS_ON;
}
static void read_tracker_duration(const char *path) {
    if(!duration_probes) {
        duration_probes=g_hash_table_new_full(g_direct_hash,g_direct_equal,NULL,g_free);
        duration_data_handler=ecore_event_handler_add(ECORE_EXE_EVENT_DATA,duration_data,NULL);
        duration_exit_handler=ecore_event_handler_add(ECORE_EXE_EVENT_DEL,duration_exit,NULL);
    }
    char *quoted=g_shell_quote(path);
    char *command=g_strdup_printf("ffprobe -v error -f libopenmpt -i %s -show_entries format=duration -of default=noprint_wrappers=1:nokey=1",quoted);
    Ecore_Exe *exe=ecore_exe_pipe_run(command,ECORE_EXE_PIPE_READ|ECORE_EXE_PIPE_READ_LINE_BUFFERED,NULL);
    if(exe) { unsigned *generation=g_new(unsigned,1);*generation=app.generation;g_hash_table_insert(duration_probes,exe,generation); }
    g_free(command);g_free(quoted);
}
static void duration_shutdown(void) {
    if(!duration_probes)return;
    GHashTableIter iterator;gpointer exe;
    g_hash_table_iter_init(&iterator,duration_probes);
    while(g_hash_table_iter_next(&iterator,&exe,NULL)){ecore_exe_terminate(exe);ecore_exe_free(exe);}
    ecore_event_handler_del(duration_data_handler);ecore_event_handler_del(duration_exit_handler);
    g_hash_table_unref(duration_probes);duration_probes=NULL;
}
/* URI-list drops are read-only imports; scanning never blocks the UI thread. */
typedef struct { GPtrArray *roots, *tracks; unsigned skipped; } DropScan;
static gboolean playable_path(const char *path) {
    const char *ext=strrchr(path,'.');
    const char *types[]={".mp3",".flac",".ogg",".opus",".wav",".m4a",".aac",".aiff",".aif",".wma",
        ".mod",".med",".s3m",".xm",".it",".mptm",".mtm",".stm",".669",".okt",NULL};
    for(int i=0;ext && types[i];i++)if(!strcasecmp(ext,types[i]))return TRUE;
    return FALSE;
}
static gint path_order(gconstpointer a,gconstpointer b) {
    return g_strcmp0(*(char *const *)a,*(char *const *)b);
}
static gboolean playlist_path(const char *path) {
    const char *suffix=strrchr(path,'.');return suffix && (!strcasecmp(suffix,".m3u")||!strcasecmp(suffix,".m3u8"));
}
static void scan_playlist(DropScan *scan,Ecore_Thread *thread,const char *path) {
    FILE *file=fopen(path,"rb");if(!file){scan->skipped++;return;}
    if(fseek(file,0,SEEK_END)!=0||ftell(file)>8*1024*1024){scan->skipped++;fclose(file);return;}
    rewind(file);Eina_Bool first=EINA_TRUE;
    char *line=NULL;size_t capacity=0;ssize_t length;size_t total=0;
    char *folder=g_path_get_dirname(path);
    while(!ecore_thread_check(thread)&&(length=getline(&line,&capacity,file))>=0) {
        total+=length;if(total>8*1024*1024)break;
        while(length>0&&(line[length-1]=='\n'||line[length-1]=='\r'))line[--length]=0;
        const char *entry=line;
        if(first && g_str_has_prefix(entry,"\357\273\277"))entry+=3;
        first=EINA_FALSE;
        if(!*entry||*entry=='#')continue;
        char *candidate=NULL;
        if(g_str_has_prefix(entry,"file://")) {
            char *hostname=NULL;candidate=g_filename_from_uri(entry,&hostname,NULL);
            if(hostname&&*hostname&&g_ascii_strcasecmp(hostname,"localhost")){g_free(candidate);candidate=NULL;}
            g_free(hostname);
        } else if(!strstr(entry,"://"))candidate=g_path_is_absolute(entry)?g_strdup(entry):g_build_filename(folder,entry,NULL);
        if(candidate && g_file_test(candidate,G_FILE_TEST_IS_REGULAR)&&playable_path(candidate)) {
            char *absolute=realpath(candidate,NULL);if(absolute)g_ptr_array_add(scan->tracks,absolute);else scan->skipped++;
        } else scan->skipped++;
        g_free(candidate);
    }
    free(line);g_free(folder);fclose(file);
}
static void scan_path(DropScan *scan,Ecore_Thread *thread,const char *path,unsigned depth) {
    if(ecore_thread_check(thread)||depth>64)return;
    if(g_file_test(path,G_FILE_TEST_IS_DIR)) {
        /* Do not follow directory symlinks into cycles or unrelated collections. */
        if(g_file_test(path,G_FILE_TEST_IS_SYMLINK))return;
        GDir *dir=g_dir_open(path,0,NULL);if(!dir)return;
        GPtrArray *entries=g_ptr_array_new_with_free_func(g_free);
        const char *name;
        while(!ecore_thread_check(thread)&&(name=g_dir_read_name(dir)))
            if(name[0]!='.')g_ptr_array_add(entries,g_build_filename(path,name,NULL));
        g_dir_close(dir);g_ptr_array_sort(entries,path_order);
        for(guint i=0;i<entries->len&&!ecore_thread_check(thread);i++)
            scan_path(scan,thread,g_ptr_array_index(entries,i),depth+1);
        g_ptr_array_unref(entries);
    } else if(g_file_test(path,G_FILE_TEST_IS_REGULAR)&&playlist_path(path)&&depth==0) {
        scan_playlist(scan,thread,path);
    } else if(g_file_test(path,G_FILE_TEST_IS_REGULAR)&&playable_path(path)) {
        char *resolved=realpath(path,NULL);
        if(resolved)g_ptr_array_add(scan->tracks,resolved);
    }
}
static void scan_worker(void *data,Ecore_Thread *thread) {
    DropScan *scan=data;
    for(guint i=0;i<scan->roots->len&&!ecore_thread_check(thread);i++)
        scan_path(scan,thread,g_ptr_array_index(scan->roots,i),0);
}
static void scan_free(DropScan *scan) {
    g_ptr_array_unref(scan->roots);g_ptr_array_unref(scan->tracks);g_free(scan);
}
static void scan_done(void *data,Ecore_Thread *thread) {
    (void)thread;DropScan *scan=data;
    if(!app.win){scan_free(scan);return;}
    for(guint i=0;i<scan->tracks->len;i++)
        app.paths=eina_list_append(app.paths,strdup(g_ptr_array_index(scan->tracks,i)));
    refresh_queue();
    char message[80];snprintf(message,sizeof(message),"Added %u tracks / %u skipped",scan->tracks->len,scan->skipped);
    status(message);
    if(getenv("EJAMZ_VERIFY_DROP")) {
        printf("VERIFY DROP: %u tracks / %u skipped\n",scan->tracks->len,scan->skipped);
        for(guint i=0;i<scan->tracks->len;i++)printf("VERIFY IMPORT: %s\n",(char *)g_ptr_array_index(scan->tracks,i));
        fflush(stdout);
        elm_exit();
    }
    scan_free(scan);
}
static void scan_cancel(void *data,Ecore_Thread *thread) { (void)thread;scan_free(data); }
static Eina_Bool start_scan(DropScan *scan) {
    if(!scan->roots->len){scan_free(scan);return EINA_FALSE;}
    status("Scanning audio files…");
    if(!ecore_thread_run(scan_worker,scan_done,scan_cancel,scan)) {
        scan_free(scan);status("Unable to start folder scan");return EINA_FALSE;
    }
    return EINA_TRUE;
}
static void import_path(const char *path) {
    if(!path)return;
    DropScan *scan=g_new0(DropScan,1);
    scan->roots=g_ptr_array_new_with_free_func(g_free);scan->tracks=g_ptr_array_new_with_free_func(free);
    g_ptr_array_add(scan->roots,g_strdup(path));start_scan(scan);
}
static Eina_Bool playlist_drop(void *data,Evas_Object *obj,Elm_Selection_Data *event) {
    (void)data;(void)obj;
    if(!event||!event->data||!event->len)return EINA_FALSE;
    char *payload=g_strndup(event->data,event->len);
    gchar **uris=g_uri_list_extract_uris(payload);g_free(payload);
    DropScan *scan=g_new0(DropScan,1);
    scan->roots=g_ptr_array_new_with_free_func(g_free);
    scan->tracks=g_ptr_array_new_with_free_func(free);
    for(int i=0;uris[i];i++) {
        gchar *hostname=NULL,*path=uris[i][0]=='/'?g_strdup(uris[i]):g_filename_from_uri(uris[i],&hostname,NULL);
        if(path&&(!hostname||!*hostname||!g_ascii_strcasecmp(hostname,"localhost")))
            g_ptr_array_add(scan->roots,path);
        else g_free(path);
        g_free(hostname);
    }
    g_strfreev(uris);
    fprintf(stderr,"E-Jamz drop: %u local paths, format=%d\n",scan->roots->len,event->format);
    return start_scan(scan);
}
/* X11 prototype: own the URI transfer rather than Edje hit-test ancestry. */
static Ecore_X_Window drop_window;
static Eina_Bool drop_offered,drop_pending;
static Eina_Bool native_drop_event(void *data,int type,void *event) {
    (void)data;
    if(type==ECORE_X_EVENT_XDND_ENTER) {
        Ecore_X_Event_Xdnd_Enter *e=event;if(e->win!=drop_window)return ECORE_CALLBACK_PASS_ON;
        drop_offered=EINA_FALSE;
        for(int i=0;i<e->num_types;i++)if(!strcmp(e->types[i],"text/uri-list"))drop_offered=EINA_TRUE;
    } else if(type==ECORE_X_EVENT_XDND_POSITION) {
        Ecore_X_Event_Xdnd_Position *e=event;if(e->win!=drop_window)return ECORE_CALLBACK_PASS_ON;
        ecore_x_dnd_send_status(drop_offered,EINA_FALSE,(Ecore_X_Rectangle){0,0,0,0},ECORE_X_ATOM_XDND_ACTION_COPY);
    } else if(type==ECORE_X_EVENT_XDND_DROP) {
        Ecore_X_Event_Xdnd_Drop *e=event;if(e->win!=drop_window)return ECORE_CALLBACK_PASS_ON;
        if(drop_offered){drop_pending=EINA_TRUE;ecore_x_selection_xdnd_request(drop_window,"text/uri-list");}
        else ecore_x_dnd_send_finished();
    } else if(type==ECORE_X_EVENT_SELECTION_NOTIFY) {
        Ecore_X_Event_Selection_Notify *e=event;
        if(e->win!=drop_window||e->selection!=ECORE_X_SELECTION_XDND||!drop_pending)return ECORE_CALLBACK_PASS_ON;
        Ecore_X_Selection_Data *content=e->data;GString *payload=g_string_new(NULL);
        if(content && content->content==ECORE_X_SELECTION_CONTENT_FILES) {
            Ecore_X_Selection_Data_Files *files=e->data;
            for(int i=0;i<files->num_files;i++){g_string_append(payload,files->files[i]);g_string_append(payload,"\r\n");}
        } else if(content && content->content==ECORE_X_SELECTION_CONTENT_TEXT) {
            Ecore_X_Selection_Data_Text *text=e->data;if(text->text)g_string_append(payload,text->text);
        }
        Elm_Selection_Data selection={0};selection.data=payload->str;selection.len=payload->len;selection.format=ELM_SEL_FORMAT_URILIST;
        if(!playlist_drop(NULL,app.win,&selection))status("Drop contained no local audio paths");
        g_string_free(payload,TRUE);drop_pending=EINA_FALSE;ecore_x_dnd_send_finished();
    }
    return ECORE_CALLBACK_DONE;
}
static void delete_later(void *data) { evas_object_del(data); }
static void chooser_done(void *data,Evas_Object *obj,void *event) {
    (void)obj;
    if(event)import_path(event);
    ecore_job_add(delete_later,data);
}
static void chooser_add_folder(void *data,Evas_Object *obj,void *event) {
    (void)obj;(void)event;Evas_Object *files=data;
    const char *selected=elm_fileselector_selected_get(files);
    import_path(selected&&g_file_test(selected,G_FILE_TEST_IS_DIR)?selected:elm_fileselector_path_get(files));
}
static void open_audio(void *data,Evas_Object *obj,const char *emission,const char *source) {
    (void)data;(void)obj;(void)emission;(void)source;
    Evas_Object *win=elm_win_util_standard_add("ejamz-open","Add audio files or folder");
    elm_win_autodel_set(win,EINA_TRUE);
    Evas_Object *outer=elm_box_add(win);
    evas_object_size_hint_weight_set(outer,EVAS_HINT_EXPAND,EVAS_HINT_EXPAND);
    elm_win_resize_object_add(win,outer);evas_object_show(outer);
    Evas_Object *files=elm_fileselector_add(win);
    elm_fileselector_expandable_set(files,EINA_FALSE);
    elm_fileselector_path_set(files,getenv("EJAMZ_CHOOSER_PATH")?getenv("EJAMZ_CHOOSER_PATH"):getenv("HOME"));
    evas_object_size_hint_weight_set(files,EVAS_HINT_EXPAND,EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(files,EVAS_HINT_FILL,EVAS_HINT_FILL);
    elm_box_pack_end(outer,files);evas_object_show(files);
    evas_object_smart_callback_add(files,"done",chooser_done,win);
    Evas_Object *folder=elm_button_add(win);elm_object_text_set(folder,"Add selected / current folder");
    evas_object_smart_callback_add(folder,"clicked",chooser_add_folder,files);
    elm_box_pack_end(outer,folder);evas_object_show(folder);
    evas_object_resize(win,650,450);evas_object_show(win);
}

typedef struct { Evas_Object *win,*entry; char *contents; unsigned count; Eina_Bool load; } PlaylistSave;
static char *filtered_playlist(unsigned *count) {
    GString *text=g_string_new("#EXTM3U\n");*count=0;
    for(guint i=0;app.visible&&i<app.visible->len;i++) {
        const char *path=eina_list_nth(app.paths,g_array_index(app.visible,int,i));
        if(strchr(path,'\n')||strchr(path,'\r')){g_string_free(text,TRUE);return NULL;}
        g_string_append(text,path);g_string_append_c(text,'\n');(*count)++;
    }
    return g_string_free(text,FALSE);
}
static Eina_Bool write_playlist(const char *path,const char *contents) {
    FILE *file=fopen(path,"wx");
    if(!file){
        char message[140];snprintf(message,sizeof(message),"Cannot save: %s",strerror(errno));
        fprintf(stderr,"E-Jamz playlist write: %s: %s\n",path,strerror(errno));
        status(errno==EEXIST?"Playlist exists; choose another name":message);return EINA_FALSE;
    }
    size_t length=strlen(contents);Eina_Bool okay=fwrite(contents,1,length,file)==length;
    if(fclose(file)!=0)okay=EINA_FALSE;
    if(!okay){unlink(path);status("Playlist write failed");}
    return okay;
}
static void save_window_deleted(void *data,Evas *canvas,Evas_Object *obj,void *event) {
    (void)canvas;(void)obj;(void)event;PlaylistSave *save=data;
    if(app.dialog_entry==save->entry)app.dialog_entry=NULL;
    transport_grabs(!(app.filter_entry&&elm_object_focus_get(app.filter_entry)));
    g_free(save->contents);g_free(save);
}
static void remember_playlist_directory(const char *path) {
    g_free(playlist_directory);playlist_directory=g_path_get_dirname(path);
    settings_save();
}
static Eina_Bool playlist_browser_filter(const char *path,Eina_Bool directory,void *data) {
    (void)data;return directory||playlist_path(path);
}
static void playlist_browser_done(void *data,Evas_Object *obj,void *event) {
    (void)obj;PlaylistSave *save=data;const char *path=event;
    if(!path){ecore_job_add(delete_later,save->win);return;}
    if(!playlist_path(path)||!g_file_test(path,G_FILE_TEST_IS_REGULAR)) {
        status("Choose an existing M3U / M3U8 playlist");return;
    }
    import_path(path);remember_playlist_directory(path);ecore_job_add(delete_later,save->win);
}
static char *playlist_start_directory(void) {
    if(playlist_directory&&g_file_test(playlist_directory,G_FILE_TEST_IS_DIR))return g_strdup(playlist_directory);
    char *documents=g_build_filename(g_get_home_dir(),"Documents",NULL);
    if(g_file_test(documents,G_FILE_TEST_IS_DIR))return documents;
    g_free(documents);return g_strdup(g_get_home_dir());
}
static void save_done(void *data,Evas_Object *obj,void *event) {
    (void)obj;PlaylistSave *save=data;
    if(!event){ecore_job_add(delete_later,save->win);return;}
    const char *chosen=event;char *path=playlist_path(chosen)?g_strdup(chosen):g_strconcat(chosen,".m3u8",NULL);
    if(write_playlist(path,save->contents)) {
        remember_playlist_directory(path);
        char message[80];snprintf(message,sizeof(message),"Saved %u tracks to playlist",save->count);status(message);
        ecore_job_add(delete_later,save->win);
    }
    g_free(path);
}
static void playlist_dialog_clicked(void *data,Evas_Object *obj,void *event) {
    (void)obj;(void)event;PlaylistSave *save=data;
    char *plain=elm_entry_markup_to_utf8(elm_entry_entry_get(save->entry));
    if(!plain||!*plain){free(plain);status("Enter a playlist filename");return;}
    char *path;
    if(g_str_has_prefix(plain,"~/"))path=g_build_filename(getenv("HOME"),plain+2,NULL);
    else path=g_path_is_absolute(plain)?g_strdup(plain):g_build_filename(getenv("HOME"),plain,NULL);
    free(plain);
    if(save->load) {
        if(playlist_path(path)&&g_file_test(path,G_FILE_TEST_IS_REGULAR)) {
            import_path(path);ecore_job_add(delete_later,save->win);
        } else status("Choose an existing M3U / M3U8 playlist");
    } else save_done(save,NULL,path);
    g_free(path);
}
static void playlist_dialog_cancel(void *data,Evas_Object *obj,void *event) {
    (void)obj;(void)event;PlaylistSave *save=data;ecore_job_add(delete_later,save->win);
}
static void playlist_dialog(Eina_Bool load) {
    PlaylistSave *save=g_new0(PlaylistSave,1);save->load=load;
    if(!load) {
        save->contents=filtered_playlist(&save->count);
        if(!save->contents||!save->count){g_free(save->contents);g_free(save);status("No matching tracks to save");return;}
    }
    save->win=elm_popup_add(app.win);
    elm_object_part_text_set(save->win,"title,text",load?"Load Playlist":"Save Playlist");
    evas_object_event_callback_add(save->win,EVAS_CALLBACK_DEL,save_window_deleted,save);
    if(load) {
        Evas_Object *files=elm_fileselector_add(save->win);
        elm_fileselector_expandable_set(files,EINA_FALSE);
        elm_fileselector_is_save_set(files,EINA_FALSE);
        elm_fileselector_custom_filter_append(files,playlist_browser_filter,NULL,"Playlists (*.m3u, *.m3u8)");
        char *directory=playlist_start_directory();elm_fileselector_path_set(files,directory);g_free(directory);
        evas_object_size_hint_min_set(files,500,330);
        evas_object_smart_callback_add(files,"done",playlist_browser_done,save);
        elm_object_content_set(save->win,files);evas_object_show(files);evas_object_show(save->win);
        transport_grabs(EINA_FALSE);return;
    }
    Evas_Object *box=elm_box_add(save->win),*label=elm_label_add(box);
    char message[120];
    if(load)snprintf(message,sizeof(message),"M3U / M3U8 file — append its tracks");
    else snprintf(message,sizeof(message),"Save %u matching tracks as M3U8",save->count);
    elm_object_text_set(label,message);elm_box_pack_end(box,label);evas_object_show(label);
    save->entry=elm_entry_add(box);
    evas_object_smart_callback_add(save->entry,"focused",text_focused,NULL);
    evas_object_smart_callback_add(save->entry,"unfocused",text_unfocused,NULL);
    elm_entry_single_line_set(save->entry,EINA_TRUE);elm_entry_scrollable_set(save->entry,EINA_TRUE);
    elm_object_part_text_set(save->entry,"guide","Filename or full path");
    if(!load) {
        char *directory=playlist_start_directory(),*path=g_build_filename(directory,"playlist.m3u8",NULL);
        char *markup=elm_entry_utf8_to_markup(path);elm_entry_entry_set(save->entry,markup);
        free(markup);g_free(path);g_free(directory);
    }
    evas_object_size_hint_min_set(save->entry,330,30);
    evas_object_size_hint_weight_set(save->entry,EVAS_HINT_EXPAND,0);evas_object_size_hint_align_set(save->entry,EVAS_HINT_FILL,EVAS_HINT_FILL);
    elm_box_pack_end(box,save->entry);evas_object_show(save->entry);
    elm_object_content_set(save->win,box);evas_object_show(box);
    Evas_Object *okay=elm_button_add(save->win),*cancel=elm_button_add(save->win);
    elm_object_text_set(okay,load?"Load":"Save");elm_object_text_set(cancel,"Cancel");
    evas_object_smart_callback_add(okay,"clicked",playlist_dialog_clicked,save);
    evas_object_smart_callback_add(cancel,"clicked",playlist_dialog_cancel,save);
    evas_object_smart_callback_add(save->entry,"activated",playlist_dialog_clicked,save);
    elm_object_part_content_set(save->win,"button1",okay);elm_object_part_content_set(save->win,"button2",cancel);
    app.dialog_entry=save->entry;evas_object_show(save->win);elm_object_focus_set(save->entry,EINA_TRUE);
}
static void save_playlist(void *data,Evas_Object *obj,const char *emission,const char *source) {
    (void)data;(void)obj;(void)emission;(void)source;playlist_dialog(EINA_FALSE);
}
static void load_playlist(void *data,Evas_Object *obj,const char *emission,const char *source) {
    (void)data;(void)obj;(void)emission;(void)source;playlist_dialog(EINA_TRUE);
}
static void pack(Evas_Object *box_obj, Evas_Object *obj);
static void panel_labels(void) {
    edje_object_signal_emit(app.face,app.queue_open?"panel,queue,on":"panel,queue,off","app");
    edje_object_signal_emit(app.face,app.rack_open?"panel,rack,on":"panel,rack,off","app");
}
static void fit_window(void *data,Evas *canvas,Evas_Object *obj,void *event) {
    (void)data;(void)canvas;(void)obj;(void)event;
    if(!app.win||!evas_object_visible_get(app.win))return;
    int sx,sy,sw,sh,wx,wy,ww,wh,minw,minh;
    elm_win_screen_size_get(app.win,&sx,&sy,&sw,&sh);
    elm_win_screen_position_get(app.win,&wx,&wy);
    evas_object_geometry_get(app.win,NULL,NULL,&ww,&wh);
    evas_object_size_hint_min_get(app.win,&minw,&minh);ww=MAX(ww,minw);wh=MAX(wh,minh);
    if(sw>0&&sh>0) {
        /* Leave space for the host shelf, including native-widget minimum sizes. */
        int x=MAX(sx,MIN(wx,sx+MAX(0,sw-ww)));
        int y=MAX(sy,MIN(wy,sy+MAX(0,sh-wh-32)));
        if(x!=wx||y!=wy)evas_object_move(app.win,x,y);
    }
}
static void arrange_panels(void) {
    elm_box_unpack_all(app.stack);
    pack(app.stack,app.main_layout);
    if(app.rack_open)pack(app.stack,app.rack);else evas_object_hide(app.rack);
    if(app.queue_open)pack(app.stack,app.queue_win);else evas_object_hide(app.queue_win);
    int height=PLAYER_HEIGHT+(app.rack_open?RACK_HEIGHT:0)+(app.queue_open?QUEUE_HEIGHT:0);
    evas_smart_objects_calculate(evas_object_evas_get(app.win));
    int mw,mh,total=0;Evas_Object *panels[]={app.main_layout,app.rack,app.queue_win};
    Eina_Bool shown[]={EINA_TRUE,app.rack_open,app.queue_open};
    for(int i=0;i<3;i++)if(shown[i]) { evas_object_size_hint_min_get(panels[i],&mw,&mh);total+=MAX(0,mh); }
    height=MAX(height,total);
    if(drawer_target||drawer_width)height=MAX(height,420);
    window_height=height;
    evas_object_size_hint_min_set(app.stack,UI_WIDTH,height);
    evas_object_resize(app.win,UI_WIDTH+drawer_width,height);
    drawer_position();
    fit_window(NULL,NULL,NULL,NULL);
    panel_labels();
}
static void panel_signal(void *data, Evas_Object *obj, const char *emission, const char *source) {
    (void)obj; (void)emission; (void)source;
    if(data==app.rack)app.rack_open=!app.rack_open;else app.queue_open=!app.queue_open;
    arrange_panels();
}
static void window_signal(void *data,Evas_Object *obj,const char *emission,const char *source) {
    (void)obj;(void)emission;
    if(!strcmp(source,"event.drag")) elm_win_move_resize_start(app.win,ELM_WIN_MOVE_RESIZE_MOVE);
    else if(!strcmp(source,"event.minimize")) elm_win_iconified_set(app.win,EINA_TRUE);
    else if(data==app.win) quit_app(NULL,NULL,NULL);
    else { if(data==app.rack)app.rack_open=EINA_FALSE;else app.queue_open=EINA_FALSE;arrange_panels(); }
}
static void settings_open(void *data,Evas_Object *obj,const char *emission,const char *source);
static void wire_chrome(Evas_Object *owner,Evas_Object *face) {
    edje_object_signal_callback_add(face,"mouse,clicked,1","event.settings",settings_open,NULL);
    edje_object_signal_callback_add(face,"mouse,down,1","event.drag",window_signal,owner);
    edje_object_signal_callback_add(face,"mouse,clicked,1","event.minimize",window_signal,owner);
    edje_object_signal_callback_add(face,"mouse,clicked,1","event.close",window_signal,owner);
}
static Evas_Object *box(Evas_Object *win) {
    Evas_Object *b = elm_box_add(win);
    evas_object_size_hint_weight_set(b, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(win, b); evas_object_show(b); return b;
}
static void pack(Evas_Object *box_obj, Evas_Object *obj) {
    evas_object_size_hint_weight_set(obj, EVAS_HINT_EXPAND, 0);
    evas_object_size_hint_align_set(obj, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_box_pack_end(box_obj, obj); evas_object_show(obj);
}
/* A stereo rack has contiguous sections. Do not distribute spare window
   height between children (for example while the settings drawer is open). */
static void rack_layout(Evas_Object *box_obj,Evas_Object_Box_Data *priv,void *data) {
    (void)priv;(void)data;
    int x,y,total=0,seam=0;evas_object_geometry_get(box_obj,&x,&y,NULL,NULL);
    for(int i=0;i<2;i++)if(rack_seams[i])evas_object_hide(rack_seams[i]);
    Eina_List *children=evas_object_box_children_get(box_obj),*node;
    Evas_Object *child;
    EINA_LIST_FOREACH(children,node,child) {
        int h;evas_object_size_hint_min_get(child,NULL,&h);
        int nominal=child==app.main_layout?PLAYER_HEIGHT:child==app.rack?RACK_HEIGHT:QUEUE_HEIGHT;
        h=MAX(h,nominal);
        /* Bridge the skin texture's transparent top/bottom inset only at
           interior joins, keeping the outer corners and unused canvas clear. */
        if(total&&seam<2&&rack_seams[seam]) {
            evas_object_move(rack_seams[seam],x,y+total-2);
            evas_object_resize(rack_seams[seam],UI_WIDTH,4);
            evas_object_show(rack_seams[seam++]);
        }
        evas_object_move(child,x,y+total);evas_object_resize(child,UI_WIDTH,h);total+=h;
    }
    eina_list_free(children);
    evas_object_size_hint_min_set(box_obj,UI_WIDTH,total);
}
typedef struct { const char *name; double bands[10]; } EqPreset;
/* Relative tonal emphasis using cuts, matched to our GStreamer bands. */
static const EqPreset presets[]={
    {"Flat",{0,0,0,0,0,0,0,0,0,0}},
    {"Bass Focus",{0,0,-1,-2,-3,-4,-4,-4,-4,-4}},
    {"Vocal",{-4,-4,-3,-2,-1,0,0,-1,-3,-4}},
    {"Bright",{-4,-4,-3,-3,-2,-2,-1,0,0,0}},
    {"Warm",{-1,0,0,-1,-1,-2,-3,-4,-5,-6}}
};
static void eq_labels(void) {
    char value[64];snprintf(value,sizeof(value),"%s v",app.preset>=0?presets[app.preset].name:"Custom");
    edje_object_part_text_set(app.rack_face,"label.presets",value);
    edje_object_part_text_set(app.rack_face,"label.bypass",app.bypass?"EQ: OFF":"EQ: ON");
}
static void apply_eq_preset(int index) {
    if(index<0 || index>=(int)(sizeof(presets)/sizeof(presets[0])))return;
    app.preset=index;
    for(int i=0;i<10;i++) {
        char part[16],property[16];app.bands[i]=presets[index].bands[i];
        snprintf(part,sizeof(part),"eq.%d",i);snprintf(property,sizeof(property),"band%d",i);
        edje_object_part_drag_value_set(app.rack_face,part,0,(6.0-app.bands[i])/18.0);
        if(app.eq)g_object_set(app.eq,property,app.bypass?0.0:app.bands[i],NULL);
    }
    eq_labels();edje_object_part_text_set(app.rack_face,"eq.status","10 BAND EQ");
}
static void preset_selected(void *data,Evas_Object *obj,void *event) {
    (void)obj;(void)event;apply_eq_preset((int)(intptr_t)data);
    if(app.preset_menu)evas_object_hide(app.preset_menu);
}
static void presets_open(void *data,Evas_Object *obj,const char *emission,const char *source) {
    (void)data;(void)obj;(void)emission;(void)source;
    if(app.preset_menu)evas_object_del(app.preset_menu);
    app.preset_menu=elm_ctxpopup_add(app.win);
    for(unsigned i=0;i<sizeof(presets)/sizeof(presets[0]);i++)
        elm_ctxpopup_item_append(app.preset_menu,presets[i].name,NULL,preset_selected,(void *)(intptr_t)i);
    const Evas_Object *button=edje_object_part_object_get(app.rack_face,"button.presets");
    int x,y,w,h;evas_object_geometry_get(button,&x,&y,&w,&h);
    evas_object_move(app.preset_menu,x+w/2,y+h);
    evas_object_show(app.preset_menu);
}
static void eq_changed(void *data,Evas_Object *obj,const char *emission,const char *source) {
    (void)data;(void)emission;int index=atoi(source+3);double x,y;char property[16],value[64];
    if(index<0||index>=10)return;
    edje_object_part_drag_value_get(obj,source,&x,&y);app.bands[index]=6.0-18.0*y;app.preset=-1;
    snprintf(property,sizeof(property),"band%d",index);
    if(!app.bypass && app.eq)g_object_set(app.eq,property,app.bands[index],NULL);
    snprintf(value,sizeof(value),"%s Hz %+.1f",band_names[index],app.bands[index]);
    edje_object_part_text_set(app.rack_face,"eq.status",value);eq_labels();
}
static void bypass_changed(void *data,Evas_Object *obj,const char *emission,const char *source) {
    (void)data;(void)obj;(void)emission;(void)source;app.bypass=!app.bypass;
    for(int i=0;i<10;i++){char property[16];snprintf(property,sizeof(property),"band%d",i);if(app.eq)g_object_set(app.eq,property,app.bypass?0.0:app.bands[i],NULL);}
    eq_labels();
}
static void flat_eq(void *data,Evas_Object *obj,const char *emission,const char *source) {
    (void)data;(void)obj;(void)emission;(void)source;apply_eq_preset(0);
}
static Eina_Bool drawer_slide(void *data,double position) {
    (void)data;
    double eased=ecore_animator_pos_map(position,ECORE_POS_MAP_DECELERATE,0,0);
    drawer_width=drawer_from+(int)((drawer_target-drawer_from)*eased);
    evas_object_size_hint_min_set(drawer_slot,drawer_width,0);
    evas_object_size_hint_max_set(drawer_slot,drawer_width,-1);
    evas_object_resize(app.win,UI_WIDTH+drawer_width,window_height);
    drawer_position();fit_window(NULL,NULL,NULL,NULL);
    if(position>=1) {
        drawer_anim=NULL;
        if(!drawer_target)evas_object_hide(settings_popup);
        arrange_panels();return ECORE_CALLBACK_CANCEL;
    }
    return ECORE_CALLBACK_RENEW;
}
static void drawer_set(Eina_Bool open) {
    if(drawer_anim){ecore_animator_del(drawer_anim);drawer_anim=NULL;}
    drawer_from=drawer_width;drawer_target=open?DRAWER_WIDTH:0;
    if(open)evas_object_show(settings_popup);
    arrange_panels();
    drawer_anim=ecore_animator_timeline_add(.18,drawer_slide,NULL);
}
static void settings_close(void *data,Evas_Object *obj,void *event) {
    (void)data;(void)obj;(void)event;
    if(!settings_save()){status("Could not save settings");return;}
    drawer_set(EINA_FALSE);
}
static void settings_theme(void *data,Evas_Object *obj,void *event) {
    (void)data;(void)event;skin=elm_radio_value_get(obj);theme_apply();
}
static void settings_toggle(void *data,Evas_Object *obj,void *event) {
    (void)event;Eina_Bool value=elm_check_state_get(obj);
    switch((intptr_t)data) {
    case 0:app.queue_open=value;arrange_panels();break;
    case 1:app.rack_open=value;arrange_panels();break;
    case 2:remember_audio=value;break;
    }
}
static void settings_open(void *data,Evas_Object *obj,const char *emission,const char *source) {
    (void)data;(void)obj;(void)emission;(void)source;
    if(drawer_target){settings_close(NULL,NULL,NULL);return;}
    if(!settings_popup) {
        settings_popup=elm_layout_add(app.win);
        const char *theme=getenv("EJAMZ_THEME");
        if(!elm_layout_file_set(settings_popup,theme?theme:"player.edj","ejamz/settings")) {
            evas_object_del(settings_popup);settings_popup=NULL;status("Settings skin is missing");return;
        }
        drawer_face=elm_layout_edje_get(settings_popup);
        drawer_clip=evas_object_rectangle_add(evas_object_evas_get(app.win));
        evas_object_color_set(drawer_clip,255,255,255,255);evas_object_show(drawer_clip);
        evas_object_clip_set(settings_popup,drawer_clip);
        edje_object_signal_callback_add(drawer_face,"mouse,clicked,1","event.close",settings_open,NULL);
        edje_object_signal_callback_add(drawer_face,"mouse,down,1","event.drag",window_signal,app.win);
        Evas_Object *scroll=elm_scroller_add(settings_popup),*box=elm_box_add(scroll),*group=NULL;
        elm_scroller_policy_set(scroll,ELM_SCROLLER_POLICY_OFF,ELM_SCROLLER_POLICY_AUTO);
        evas_object_size_hint_weight_set(box,EVAS_HINT_EXPAND,0);
        evas_object_size_hint_align_set(box,EVAS_HINT_FILL,0);
        for(int i=0;i<3;i++) {
            Evas_Object *radio=elm_radio_add(box);settings_radios[i]=radio;elm_object_text_set(radio,skin_names[i]);elm_radio_state_value_set(radio,i);
            if(group)elm_radio_group_add(radio,group);else group=radio;
            elm_box_pack_end(box,radio);evas_object_show(radio);
            evas_object_smart_callback_add(radio,"changed",settings_theme,NULL);
        }
        settings_radio=group;
        const char *labels[]={"Show playlist","Show equalizer","Remember volume and EQ"};
        for(int i=0;i<3;i++) {
            settings_checks[i]=elm_check_add(box);elm_object_text_set(settings_checks[i],labels[i]);
            elm_box_pack_end(box,settings_checks[i]);evas_object_show(settings_checks[i]);
            evas_object_smart_callback_add(settings_checks[i],"changed",settings_toggle,(void *)(intptr_t)i);
        }
        Evas_Object *label=elm_label_add(box);elm_object_text_set(label,"Theme changes apply live.<br/>Playback stays manual.");
        elm_box_pack_end(box,label);evas_object_show(label);
        Evas_Object *close=elm_button_add(box);elm_object_text_set(close,"Save & Close");
        evas_object_smart_callback_add(close,"clicked",settings_close,NULL);
        elm_box_pack_end(box,close);evas_object_show(close);
        elm_object_content_set(scroll,box);evas_object_show(box);
        elm_object_part_content_set(settings_popup,"swallow.settings",scroll);evas_object_show(scroll);
    }
    elm_radio_value_set(settings_radio,skin);
    elm_check_state_set(settings_checks[0],app.queue_open);
    elm_check_state_set(settings_checks[1],app.rack_open);
    elm_check_state_set(settings_checks[2],remember_audio);
    theme_apply();drawer_set(EINA_TRUE);
}
static Evas_Object *panel_layout(const char *group,int height) {
    Evas_Object *layout=elm_layout_add(app.win);const char *theme=getenv("EJAMZ_THEME");
    if(!elm_layout_file_set(layout,theme?theme:"player.edj",group)){fprintf(stderr,"Missing panel skin %s\n",group);exit(1);}
    evas_object_size_hint_min_set(layout,UI_WIDTH,height);
    evas_object_size_hint_max_set(layout,-1,height);
    wire_chrome(layout,elm_layout_edje_get(layout));return layout;
}
static Eina_Bool keyboard(void *data,Evas_Object *obj,Evas_Object *source,Evas_Callback_Type type,void *event) {
    (void)data;(void)obj;(void)source;
    if(type!=EVAS_CALLBACK_KEY_DOWN || (app.filter_entry && elm_object_focus_get(app.filter_entry)) || (app.dialog_entry && elm_object_focus_get(app.dialog_entry)))return EINA_FALSE;
    Evas_Event_Key_Down *key=event;
    if(evas_key_modifier_is_set(key->modifiers,"Control")||evas_key_modifier_is_set(key->modifiers,"Alt"))return EINA_FALSE;
    if(!strcasecmp(key->key,"z"))skip((void *)(intptr_t)-1,NULL,NULL,NULL);
    else if(!strcasecmp(key->key,"x"))play(NULL,NULL,NULL,NULL);
    else if(!strcasecmp(key->key,"c"))pause_track(NULL,NULL,NULL,NULL);
    else if(!strcasecmp(key->key,"v"))stop_track(NULL,NULL,NULL,NULL);
    else if(!strcasecmp(key->key,"b"))skip((void *)(intptr_t)1,NULL,NULL,NULL);
    else if(!strcmp(key->key,"space")) {
        GstState state=GST_STATE_NULL;if(app.player)gst_element_get_state(app.player,&state,NULL,0);
        if(state==GST_STATE_PLAYING)pause_track(NULL,NULL,NULL,NULL);else play(NULL,NULL,NULL,NULL);
    } else return EINA_FALSE;
    key->event_flags|=EVAS_EVENT_FLAG_ON_HOLD;return EINA_TRUE;
}
static void grabbed_key(void *data,Evas *canvas,Evas_Object *obj,void *event) {
    (void)data;(void)canvas;
    keyboard(NULL,app.win,obj,EVAS_CALLBACK_KEY_DOWN,event);
}
static int verification_failed;
static Eina_Bool verify_rack_layout(void *data) {
    (void)data;
    static int phase;
    if(phase%2==0) {
        int mask=phase/2;
        if(mask==0)settings_open(NULL,NULL,NULL,NULL);
        if(mask==8) {
            printf("VERIFY RACK: eight panel/drawer combinations; contiguous geometry PASS\n");
            fflush(stdout);elm_exit();return ECORE_CALLBACK_CANCEL;
        }
        app.rack_open=!!(mask&1);app.queue_open=!!(mask&2);
        drawer_set(!!(mask&4));arrange_panels();
        /* Spare height must remain below the rack, never between sections. */
        evas_object_resize(app.win,UI_WIDTH+drawer_width,window_height+60);
    } else {
        evas_smart_objects_calculate(evas_object_evas_get(app.win));
        Evas_Object *panels[]={app.main_layout,app.rack,app.queue_win};
        Eina_Bool shown[]={EINA_TRUE,app.rack_open,app.queue_open};
        int bottom=-1;
        for(int i=0;i<3;i++)if(shown[i]) {
            int x,y,w,h;evas_object_geometry_get(panels[i],&x,&y,&w,&h);
            if(x!=0||w!=UI_WIDTH||(bottom>=0&&y!=bottom)) {
                fprintf(stderr,"VERIFY RACK FAIL: panel %d at %d,%d %dx%d; expected y %d\n",i,x,y,w,h,bottom);
                verification_failed=1;elm_exit();return ECORE_CALLBACK_CANCEL;
            }
            bottom=y+h;
        }
    }
    phase++;return ECORE_CALLBACK_RENEW;
}
static Eina_Bool verify_controls(void) {
    double saved[10];memcpy(saved,app.bands,sizeof(saved));
    for(unsigned preset=0;preset<sizeof(presets)/sizeof(presets[0]);preset++) {
        apply_eq_preset(preset);
        for(int i=0;i<10;i++) {
            char property[16],part[16];double gain,x,y;
            snprintf(property,sizeof(property),"band%d",i);snprintf(part,sizeof(part),"eq.%d",i);
            g_object_get(app.eq,property,&gain,NULL);edje_object_part_drag_value_get(app.rack_face,part,&x,&y);
            double delta=y-(6.0-presets[preset].bands[i])/18.0;
            if(gain!=presets[preset].bands[i]||delta>0.000001||delta< -0.000001)return EINA_FALSE;
        }
    }
    bypass_changed(NULL,NULL,NULL,NULL);apply_eq_preset(2);
    for(int i=0;i<10;i++){char property[16];double gain;snprintf(property,sizeof(property),"band%d",i);g_object_get(app.eq,property,&gain,NULL);if(gain!=0)return EINA_FALSE;}
    bypass_changed(NULL,NULL,NULL,NULL);
    for(int i=0;i<10;i++){char property[16];double gain;snprintf(property,sizeof(property),"band%d",i);g_object_get(app.eq,property,&gain,NULL);if(gain!=presets[2].bands[i])return EINA_FALSE;}
    flat_eq(NULL,NULL,NULL,NULL);
    for(int i=0;i<10;i++){char property[16];app.bands[i]=saved[i];snprintf(property,sizeof(property),"band%d",i);g_object_set(app.eq,property,saved[i],NULL);}
    printf("VERIFY PRESETS: all curves, faders, bypass/restore PASS\n");fflush(stdout);
    GstElement *owner=app.player;Eina_List *paths=app.paths;
    for(int t=0;t<3;t++) {
        skin=t;theme_apply();
        if(!elm_win_alpha_get(app.win))return EINA_FALSE;
        Evas_Object *faces[]={app.face,app.queue_face,app.rack_face,drawer_face};
        for(unsigned j=0;j<sizeof(faces)/sizeof(faces[0]);j++)if(faces[j])
            for(unsigned k=0;k<sizeof(palette)/sizeof(palette[0]);k++){
                int r,g,b,a;if(!edje_object_color_class_get(faces[j],palette[k].name,&r,&g,&b,&a,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)||a!=palette[k].a)return EINA_FALSE;
            }
        if(app.player!=owner||app.paths!=paths||memcmp(app.bands,saved,sizeof(saved)))return EINA_FALSE;
        for(int i=0;i<10;i++){char property[16];double gain;snprintf(property,sizeof(property),"band%d",i);g_object_get(app.eq,property,&gain,NULL);if(gain!=saved[i])return EINA_FALSE;}
    }
    skin=0;theme_apply();printf("VERIFY THEMES: three palettes; shared window/panel alpha, engine, queue and DSP preserved PASS\n");fflush(stdout);
    const char *output=getenv("EJAMZ_VERIFY_IO");
    if(output) {
        app.filter=g_strdup("centerfield");refresh_queue();unsigned count;char *contents=filtered_playlist(&count);
        if(!contents||count!=1||!write_playlist(output,contents)){g_free(contents);return EINA_FALSE;}
        if(write_playlist(output,"SHOULD NOT REPLACE")){g_free(contents);return EINA_FALSE;}
        char *readback=NULL;g_file_get_contents(output,&readback,NULL,NULL);
        Eina_Bool equal=readback&&!strcmp(contents,readback);g_free(contents);g_free(readback);
        g_free(app.filter);app.filter=NULL;refresh_queue();
        if(!equal)return EINA_FALSE;
        printf("VERIFY PLAYLIST EXPORT: one filtered track; existing file preserved PASS\n");fflush(stdout);
    }
    return EINA_TRUE;
}
static Eina_Bool verify_playback(void *data) {
    (void)data;
    static int index,phase;
    static double since;
    GstState state=GST_STATE_NULL;
    if(app.player)gst_element_get_state(app.player,&state,NULL,0);
    if(phase==0) {
        if(index==(int)eina_list_count(app.paths)) {
            flat_eq(NULL,NULL,NULL,NULL);
            double band=1;g_object_get(app.eq,"band0",&band,NULL);
            verification_failed=band!=0.0;
            printf("VERIFY COMPLETE: %d files; Flat=%s\n",index,band==0.0?"PASS":"FAIL");fflush(stdout);
            elm_exit();return ECORE_CALLBACK_CANCEL;
        }
        select_track(index);since=ecore_time_get();phase=1;
    } else if(phase==1 && state==GST_STATE_PLAYING) {
        gint64 position=0;double band;
        gst_element_query_position(app.player,GST_FORMAT_TIME,&position);
        if(position<150*GST_MSECOND)return ECORE_CALLBACK_RENEW;
        g_object_get(app.eq,"band0",&band,NULL);
        if(band!=-3.0) { verification_failed=1;elm_exit();return ECORE_CALLBACK_CANCEL; }
        printf("VERIFY PLAYING: %d position=%.3f EQ=%+.1f\n",index,(double)position/GST_SECOND,band);fflush(stdout);
        pause_track(NULL,NULL,NULL,NULL);phase=2;since=ecore_time_get();
    } else if(phase==2 && state==GST_STATE_PAUSED) {
        printf("VERIFY PAUSED: %d\n",index);fflush(stdout);
        play(NULL,NULL,NULL,NULL);phase=3;since=ecore_time_get();
    } else if(phase==3 && state==GST_STATE_PLAYING && ecore_time_get()-since>.2) {
        stop_track(NULL,NULL,NULL,NULL);index++;phase=0;
    } else if(phase && ecore_time_get()-since>10) {
        printf("VERIFY FAIL: %d phase=%d state=%s\n",index,phase,gst_element_state_get_name(state));fflush(stdout);
        verification_failed=1;elm_exit();return ECORE_CALLBACK_CANCEL;
    }
    return ECORE_CALLBACK_RENEW;
}
EAPI_MAIN int elm_main(int argc, char **argv) {
    /* This static software-X11 UI avoids the host async-render wait stalls. */
    setenv("ECORE_EVAS_FORCE_SYNC_RENDER","1",0);
    gst_init(NULL, NULL);
    app.current = -1;
    Eina_Bool verify=getenv("EJAMZ_VERIFY")!=NULL;
    app.rawfd=-1;app.volume_level=verify?.05:.65;
    settings_load();
    if(verify)app.bands[0]=-3.0;
    if(!create_player(NULL))return 1;
    app.win = elm_win_add(NULL,"ejamz",ELM_WIN_BASIC);
    /* Our rack owns its skin. Keep native controls on a private stock theme
       so desktop theme changes cannot replace their surfaces or geometry. */
    controls_theme=elm_theme_new();elm_theme_set(controls_theme,"default");
    elm_object_theme_set(app.win,controls_theme);
    elm_win_title_set(app.win,getenv("EJAMZ_TITLE")?getenv("EJAMZ_TITLE"):(verify?"E-Jamz — verification":"E-Jamz — prototype"));
    elm_win_autodel_set(app.win, EINA_TRUE);
    evas_object_event_callback_add(app.win,EVAS_CALLBACK_DEL,window_deleted,NULL);
    evas_object_event_callback_add(app.win,EVAS_CALLBACK_RESIZE,fit_window,NULL);
    evas_object_smart_callback_add(app.win, "delete,request", quit_app, NULL);
    elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);
    for(int i=0;i<2;i++) {
        rack_seams[i]=evas_object_rectangle_add(evas_object_evas_get(app.win));
        evas_object_pass_events_set(rack_seams[i],EINA_TRUE);
    }
    Evas_Object *shell=box(app.win);elm_box_horizontal_set(shell,EINA_TRUE);elm_box_padding_set(shell,0,0);
    Evas_Object *outer=elm_box_add(shell),*layout=elm_layout_add(app.win);
    elm_box_align_set(outer,0,0);elm_box_padding_set(outer,0,0);
    elm_box_layout_set(outer,rack_layout,NULL,NULL);
    evas_object_size_hint_weight_set(outer,0,EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(outer,0,0);
    evas_object_size_hint_max_set(outer,UI_WIDTH,-1);
    elm_box_pack_end(shell,outer);evas_object_show(outer);
    drawer_slot=evas_object_rectangle_add(evas_object_evas_get(app.win));
    evas_object_color_set(drawer_slot,0,0,0,0);
    evas_object_size_hint_weight_set(drawer_slot,0,EVAS_HINT_EXPAND);
    evas_object_size_hint_min_set(drawer_slot,0,0);evas_object_size_hint_max_set(drawer_slot,0,-1);
    elm_box_pack_end(shell,drawer_slot);evas_object_show(drawer_slot);
    app.stack=outer;app.main_layout=layout;
    const char *theme = getenv("EJAMZ_THEME");
    if (!elm_layout_file_set(layout, theme ? theme : "player.edj", "ejamz/player")) {
        fprintf(stderr, "Cannot load player.edj; set EJAMZ_THEME\n"); return 1;
    }
    pack(outer, layout); app.face = elm_layout_edje_get(layout);
    evas_object_size_hint_min_set(layout, UI_WIDTH, PLAYER_HEIGHT);
    evas_object_size_hint_max_set(layout,-1,PLAYER_HEIGHT);
    app.seek = elm_slider_add(layout); elm_slider_min_max_set(app.seek, 0, 1);
    elm_object_scale_set(app.seek,0.65);
    elm_object_part_content_set(layout, "swallow.seek", app.seek);
    evas_object_smart_callback_add(app.seek, "slider,drag,stop", seek_changed, NULL);
    app.volume = elm_slider_add(layout); elm_slider_min_max_set(app.volume, 0, 1);
    elm_object_scale_set(app.volume,0.65);
    elm_slider_value_set(app.volume, app.volume_level);
    elm_object_part_content_set(layout, "swallow.volume", app.volume);
    evas_object_smart_callback_add(app.volume, "changed", volume_changed, NULL);
    edje_object_signal_callback_add(app.face, "mouse,clicked,1", "event.play", play, NULL);
    edje_object_signal_callback_add(app.face, "mouse,clicked,1", "event.pause", pause_track, NULL);
    edje_object_signal_callback_add(app.face, "mouse,clicked,1", "event.stop", stop_track, NULL);
    edje_object_signal_callback_add(app.face, "mouse,clicked,1", "event.previous", skip, (void *)(intptr_t)-1);
    edje_object_signal_callback_add(app.face, "mouse,clicked,1", "event.next", skip, (void *)(intptr_t)1);
    elm_win_borderless_set(app.win,EINA_TRUE);
    wire_chrome(app.win,app.face);
    app.queue_win=panel_layout("ejamz/queue",QUEUE_HEIGHT);
    app.queue_face=elm_layout_edje_get(app.queue_win);
    app.filter_entry=elm_entry_add(app.queue_win);
    elm_entry_single_line_set(app.filter_entry,EINA_TRUE);elm_entry_scrollable_set(app.filter_entry,EINA_FALSE);
    elm_object_scale_set(app.filter_entry,0.85);
    elm_entry_text_style_user_push(app.filter_entry,"DEFAULT='font=DejaVu Sans font_size=12 color=#173c52'");
    elm_entry_editable_set(app.filter_entry,EINA_TRUE);elm_object_focus_allow_set(app.filter_entry,EINA_TRUE);
    elm_object_part_text_set(app.filter_entry,"guide","Filter artist / album / title paths");
    elm_object_part_content_set(app.queue_win,"swallow.filter",app.filter_entry);
    evas_object_smart_callback_add(app.filter_entry,"changed,user",filter_changed,NULL);
    evas_object_smart_callback_add(app.filter_entry,"focused",text_focused,NULL);
    evas_object_smart_callback_add(app.filter_entry,"unfocused",text_unfocused,NULL);
    evas_object_event_callback_add(app.filter_entry,EVAS_CALLBACK_MOUSE_DOWN,filter_focus,NULL);
    evas_object_event_callback_add(app.queue_face,EVAS_CALLBACK_MOUSE_WHEEL,queue_wheel,NULL);
    edje_object_signal_callback_add(app.queue_face,"mouse,clicked,1","event.row.*",row_selected,NULL);
    edje_object_signal_callback_add(app.queue_face,"mouse,clicked,1","event.back",queue_page,(void *)(intptr_t)-1);
    edje_object_signal_callback_add(app.queue_face,"mouse,clicked,1","event.forward",queue_page,(void *)(intptr_t)1);
    edje_object_signal_callback_add(app.queue_face,"mouse,clicked,1","event.open",open_audio,NULL);
    edje_object_signal_callback_add(app.queue_face,"mouse,clicked,1","event.save",save_playlist,NULL);
    edje_object_signal_callback_add(app.queue_face,"mouse,clicked,1","event.load",load_playlist,NULL);
    app.rack=panel_layout("ejamz/rack",RACK_HEIGHT);
    app.rack_face=elm_layout_edje_get(app.rack);
    for(int i=0;i<10;i++){char part[16];snprintf(part,sizeof(part),"eq.%d",i);edje_object_part_drag_size_set(app.rack_face,part,1.0,0.1);edje_object_part_drag_value_set(app.rack_face,part,0,(6.0-app.bands[i])/18.0);}
    edje_object_signal_callback_add(app.rack_face,"drag","eq.*",eq_changed,NULL);
    edje_object_signal_callback_add(app.rack_face,"mouse,clicked,1","event.bypass",bypass_changed,NULL);
    edje_object_signal_callback_add(app.rack_face,"mouse,clicked,1","event.flat",flat_eq,NULL);
    edje_object_signal_callback_add(app.rack_face,"mouse,clicked,1","event.presets",presets_open,NULL);
    if(!verify) {
        for(unsigned i=0;i<sizeof(presets)/sizeof(presets[0]);i++)if(!memcmp(app.bands,presets[i].bands,sizeof(app.bands)))app.preset=i;
    }
    eq_labels();theme_apply();
    transport_grabs(EINA_TRUE);
    evas_object_event_callback_add(layout,EVAS_CALLBACK_KEY_DOWN,grabbed_key,NULL);
    elm_object_focus_allow_set(layout,EINA_TRUE);
    elm_object_focus_set(layout,EINA_TRUE);
    arrange_panels();
    refresh_queue();
    edje_object_signal_callback_add(app.face, "mouse,clicked,1", "event.open", open_audio, NULL);
    edje_object_signal_callback_add(app.face, "mouse,clicked,1", "event.queue", panel_signal, app.queue_win);
    edje_object_signal_callback_add(app.face, "mouse,clicked,1", "event.rack", panel_signal, app.rack);
    for (int i = 1; i < argc; i++) {
        if(g_file_test(argv[i],G_FILE_TEST_IS_DIR))import_path(argv[i]);
        else if(playlist_path(argv[i]))import_path(argv[i]);
        else if(playable_path(argv[i]))add_file(argv[i]);
    }
    if(verify&&!verify_controls()){fprintf(stderr,"VERIFY CONTROLS FAIL\n");return 1;}
    Ecore_Timer *timer = ecore_timer_add(0.25, tick, NULL);
    if(getenv("EJAMZ_VERIFY_LAYOUT"))ecore_timer_add(.5,verify_rack_layout,NULL);
    else if(verify)ecore_timer_add(.1,verify_playback,NULL);
    if(getenv("EJAMZ_PREVIEW_SETTINGS"))settings_open(NULL,NULL,NULL,NULL);
    if(getenv("EJAMZ_PREVIEW_PLAYLISTS"))playlist_dialog(EINA_TRUE);
    if(getenv("EJAMZ_CHOOSER_PATH"))open_audio(NULL,NULL,NULL,NULL);
    const char *drop_test=getenv("EJAMZ_VERIFY_DROP");
    if(drop_test) {
        Elm_Selection_Data event={0};event.format=ELM_SEL_FORMAT_URILIST;
        event.data=(void *)drop_test;event.len=strlen(drop_test);
        if(!playlist_drop(NULL,app.queue_win,&event))return 1;
    }
    evas_object_resize(app.win, UI_WIDTH, PLAYER_HEIGHT); evas_object_show(app.win);
    drop_window=elm_win_xwindow_get(app.win);ecore_x_dnd_aware_set(drop_window,EINA_TRUE);
    const int drop_types[]={ECORE_X_EVENT_XDND_ENTER,ECORE_X_EVENT_XDND_POSITION,ECORE_X_EVENT_XDND_DROP,ECORE_X_EVENT_SELECTION_NOTIFY};
    Ecore_Event_Handler *drop_handlers[4];
    for(int i=0;i<4;i++)drop_handlers[i]=ecore_event_handler_prepend(drop_types[i],native_drop_event,NULL);
    elm_run();
    settings_save();
    for(int i=0;i<4;i++)ecore_event_handler_del(drop_handlers[i]);
    if(drawer_anim)ecore_animator_del(drawer_anim);
    app.win=NULL;
    ecore_timer_del(timer);
    dispose_player();duration_shutdown();
    char *path; EINA_LIST_FREE(app.paths, path) free(path);
    if(app.visible)g_array_unref(app.visible);
    g_free(app.filter);
    elm_theme_free(controls_theme);g_free(playlist_directory);
    return verification_failed?1:0;
}
ELM_MAIN()
