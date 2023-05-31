/* WirePlumber
 *
 * Copyright © 2020 Collabora Ltd.
 *    @author George Kiagiadakis <george.kiagiadakis@collabora.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "log.h"
#include "spa-pod.h"
#include "proxy.h"
#include <pipewire/pipewire.h>
#include <spa/support/log.h>

/*!
 * \defgroup wplog Debug Logging
 * \{
 */
/*!
 * \def WP_LOG_LEVEL_TRACE
 * \brief A custom GLib log level for trace messages (see GLogLevelFlags)
 */
/*!
 * \def WP_OBJECT_FORMAT
 * \brief A format string to print GObjects with WP_OBJECT_ARGS()
 * For example:
 * \code
 * GObject *myobj = ...;
 * wp_debug ("This: " WP_OBJECT_FORMAT " is an object", WP_OBJECT_ARGS (myobj));
 * \endcode
 */
/*!
 * \def WP_OBJECT_ARGS(object)
 * \brief A macro to format an object for printing with WP_OBJECT_FORMAT
 */
/*!
 * \def wp_critical(...)
 * \brief Logs a critical message to the standard log via GLib's logging system.
 * \param ... A format string, followed by format arguments in printf() style
 */
/*!
 * \def wp_critical_object(object, ...)
 * \brief Logs a critical message to the standard log via GLib's logging system.
 * \param object A GObject associated with the log; this is printed in a special
 *   way to make it easier to track messages from a specific object
 * \param ... A format string, followed by format arguments in printf() style
 */
/*!
 * \def wp_critical_boxed(type, object, ...)
 * \brief Logs a critical message to the standard log via GLib's logging system.
 * \param type The type of \a object
 * \param object A boxed object associated with the log; this is printed in a
 *   special way to make it easier to track messages from a specific object.
 *   For some object types, contents from the object are also printed (ex WpSpaPod)
 * \param ... A format string, followed by format arguments in printf() style
 */
/*!
 * \def wp_warning(...)
 * \brief Logs a warning message to the standard log via GLib's logging system.
 * \param ... A format string, followed by format arguments in printf() style
 */
/*!
 * \def wp_warning_object(object, ...)
 * \brief Logs a warning message to the standard log via GLib's logging system.
 * \param object A GObject associated with the log; this is printed in a special
 *   way to make it easier to track messages from a specific object
 * \param ... A format string, followed by format arguments in printf() style
 */
/*!
 * \def wp_warning_boxed(type, object, ...)
 * \brief Logs a warning message to the standard log via GLib's logging system.
 * \param type The type of \a object
 * \param object A boxed object associated with the log; this is printed in a
 *   special way to make it easier to track messages from a specific object.
 *   For some object types, contents from the object are also printed (ex WpSpaPod)
 * \param ... A format string, followed by format arguments in printf() style
 */
/*!
 * \def wp_message(...)
 * \brief Logs a standard message to the standard log via GLib's logging system.
 * \param ... A format string, followed by format arguments in printf() style
 */
/*!
 * \def wp_message_object(object, ...)
 * \brief Logs a standard message to the standard log via GLib's logging system.
 * \param object A GObject associated with the log; this is printed in a special
 *   way to make it easier to track messages from a specific object
 * \param ... A format string, followed by format arguments in printf() style
 */
/*!
 * \def wp_message_boxed(type, object, ...)
 * \brief Logs a standard message to the standard log via GLib's logging system.
 * \param type The type of \a object
 * \param object A boxed object associated with the log; this is printed in a
 *   special way to make it easier to track messages from a specific object.
 *   For some object types, contents from the object are also printed (ex WpSpaPod)
 * \param ... A format string, followed by format arguments in printf() style
 */
/*!
 * \def wp_info(...)
 * \brief Logs a info message to the standard log via GLib's logging system.
 * \param ... A format string, followed by format arguments in printf() style
 */
/*!
 * \def wp_info_object(object, ...)
 * \brief Logs a info message to the standard log via GLib's logging system.
 * \param object A GObject associated with the log; this is printed in a special
 *   way to make it easier to track messages from a specific object
 * \param ... A format string, followed by format arguments in printf() style
 */
/*!
 * \def wp_info_boxed(type, object, ...)
 * \brief Logs a info message to the standard log via GLib's logging system.
 * \param type The type of \a object
 * \param object A boxed object associated with the log; this is printed in a
 *   special way to make it easier to track messages from a specific object.
 *   For some object types, contents from the object are also printed (ex WpSpaPod)
 * \param ... A format string, followed by format arguments in printf() style
 */
/*!
 * \def wp_debug(...)
 * \brief Logs a debug message to the standard log via GLib's logging system.
 * \param ... A format string, followed by format arguments in printf() style
 */
/*!
 * \def wp_debug_object(object, ...)
 * \brief Logs a debug message to the standard log via GLib's logging system.
 * \param object A GObject associated with the log; this is printed in a special
 *   way to make it easier to track messages from a specific object
 * \param ... A format string, followed by format arguments in printf() style
 */
/*!
 * \def wp_debug_boxed(type, object, ...)
 * \brief Logs a debug message to the standard log via GLib's logging system.
 * \param type The type of \a object
 * \param object A boxed object associated with the log; this is printed in a
 *   special way to make it easier to track messages from a specific object.
 *   For some object types, contents from the object are also printed (ex WpSpaPod)
 * \param ... A format string, followed by format arguments in printf() style
 */
/*!
 * \def wp_trace(...)
 * \brief Logs a trace message to the standard log via GLib's logging system.
 * \param ... A format string, followed by format arguments in printf() style
 */
/*!
 * \def wp_trace_object(object, ...)
 * \brief Logs a trace message to the standard log via GLib's logging system.
 * \param object A GObject associated with the log; this is printed in a special
 *   way to make it easier to track messages from a specific object
 * \param ... A format string, followed by format arguments in printf() style
 */
/*!
 * \def wp_trace_boxed(type, object, ...)
 * \brief Logs a trace message to the standard log via GLib's logging system.
 * \param type The type of \a object
 * \param object A boxed object associated with the log; this is printed in a
 *   special way to make it easier to track messages from a specific object.
 *   For some object types, contents from the object are also printed (ex WpSpaPod)
 * \param ... A format string, followed by format arguments in printf() style
 */
/*!
 * \def wp_log(level, type, object, ...)
 * \brief The generic form of all the logging macros
 * \remark Don't use this directly, use one of the other logging macros
 */
/*! \} */

static GString *spa_dbg_str = NULL;
#define spa_debug(...) \
({ \
  g_string_append_printf (spa_dbg_str, __VA_ARGS__); \
  g_string_append_c (spa_dbg_str, '\n'); \
})

#include <spa/debug/pod.h>

static gsize initialized = 0;
static gboolean use_color = FALSE;
static gboolean output_is_journal = FALSE;
static GPatternSpec **enabled_categories = NULL;
static gint enabled_level = 4; /* MESSAGE */

struct common_fields
{
  const gchar *log_domain;
  const gchar *file;
  const gchar *line;
  const gchar *func;
  const gchar *message;
  GLogField *message_field;
  gint log_level;
  GType object_type;
  gconstpointer object;
};

/* reference: https://en.wikipedia.org/wiki/ANSI_escape_code#3/4_bit */
#define COLOR_RED            "\033[1;31m"
#define COLOR_GREEN          "\033[1;32m"
#define COLOR_YELLOW         "\033[1;33m"
#define COLOR_BLUE           "\033[1;34m"
#define COLOR_MAGENTA        "\033[1;35m"
#define COLOR_CYAN           "\033[1;36m"
#define COLOR_BRIGHT_RED     "\033[1;91m"
#define COLOR_BRIGHT_GREEN   "\033[1;92m"
#define COLOR_BRIGHT_YELLOW  "\033[1;93m"
#define COLOR_BRIGHT_BLUE    "\033[1;94m"
#define COLOR_BRIGHT_MAGENTA "\033[1;95m"
#define COLOR_BRIGHT_CYAN    "\033[1;96m"

#define RESET_COLOR          "\033[0m"

/* our palette */
#define DOMAIN_COLOR    COLOR_MAGENTA
#define LOCATION_COLOR  COLOR_BLUE

/* available colors for object printouts (the <Object:0xfoobar>) */
static const gchar *object_colors[] = {
  COLOR_RED, COLOR_GREEN, COLOR_YELLOW, COLOR_MAGENTA, COLOR_CYAN,
  COLOR_BRIGHT_RED, COLOR_BRIGHT_GREEN, COLOR_BRIGHT_YELLOW,
  COLOR_BRIGHT_MAGENTA, COLOR_BRIGHT_CYAN
};

/*
 * priority numbers are based on GLib's gmessages.c
 * reference: http://man7.org/linux/man-pages/man3/syslog.3.html#DESCRIPTION
 */
static const struct {
  GLogLevelFlags log_level;
  enum spa_log_level spa_level;
  gchar name[6];
  gchar priority[2];
  gchar color[8];
} log_level_info[] = {
  { 0,                   0,                  "U", "5", COLOR_BRIGHT_RED },
  { G_LOG_LEVEL_ERROR,   0,                  "E", "3", COLOR_RED },
  { G_LOG_LEVEL_CRITICAL,0,                  "C", "4", COLOR_BRIGHT_MAGENTA },
  { G_LOG_LEVEL_WARNING, SPA_LOG_LEVEL_ERROR,"W", "4", COLOR_BRIGHT_YELLOW },
  { G_LOG_LEVEL_MESSAGE, SPA_LOG_LEVEL_WARN, "M", "5", COLOR_BRIGHT_GREEN },
  { G_LOG_LEVEL_INFO,    SPA_LOG_LEVEL_INFO, "I", "6", COLOR_GREEN },
  { G_LOG_LEVEL_DEBUG,   SPA_LOG_LEVEL_DEBUG,"D", "7", COLOR_BRIGHT_CYAN },
  { WP_LOG_LEVEL_TRACE,  SPA_LOG_LEVEL_TRACE,"T", "7", COLOR_CYAN },
};

/* map glib's log levels, which are flags in the range (1<<2) to (1<<8),
  to the 1-7 range; first calculate the integer part of log2(log_level)
  to bring it down to 2-8 and substract 1 */
static G_GNUC_CONST inline gint
log_level_index (GLogLevelFlags log_level)
{
  gint logarithm = 0;
  while ((log_level >>= 1) != 0)
    logarithm += 1;
  return (logarithm >= 2 && logarithm <= 8) ? (logarithm - 1) : 0;
}

static G_GNUC_CONST inline gint
level_index_from_spa (gint spa_lvl)
{
  return CLAMP (spa_lvl + 2, 0, (gint) G_N_ELEMENTS (log_level_info) - 1);
}

static G_GNUC_CONST inline gint
level_index_to_spa (gint lvl_index)
{
  return CLAMP (lvl_index - 2, 0, 5);
}

static inline void
write_debug_message (FILE *s, struct common_fields *cf)
{
  gint64 now;
  time_t now_secs;
  struct tm now_tm;
  gchar time_buf[128];

  now = g_get_real_time ();
  now_secs = (time_t) (now / G_USEC_PER_SEC);
  localtime_r (&now_secs, &now_tm);
  strftime (time_buf, sizeof (time_buf), "%H:%M:%S", &now_tm);

  fprintf (s, "%s%s %s.%06d %s%18.18s %s%s:%s:%s:%s %s\n",
      /* level */
      use_color ? log_level_info[cf->log_level].color : "",
      log_level_info[cf->log_level].name,
      /* timestamp */
      time_buf,
      (gint) (now % G_USEC_PER_SEC),
      /* domain */
      use_color ? DOMAIN_COLOR : "",
      cf->log_domain,
      /* file, line, function */
      use_color ? LOCATION_COLOR : "",
      cf->file,
      cf->line,
      cf->func,
      use_color ? RESET_COLOR : "",
      /* message */
      cf->message);
  fflush (s);
}

static inline gchar *
format_message (struct common_fields *cf)
{
  g_autofree gchar *extra_message = NULL;
  g_autofree gchar *extra_object = NULL;
  const gchar *object_color = "";

  if (use_color) {
    guint h = g_direct_hash (cf->object) % G_N_ELEMENTS (object_colors);
    object_color = object_colors[h];
  }

  if (cf->object_type == WP_TYPE_SPA_POD && cf->object && !spa_dbg_str) {
    spa_dbg_str = g_string_new (cf->message);
    g_string_append (spa_dbg_str, ":\n");
    spa_debug_pod (2, NULL, wp_spa_pod_get_spa_pod (cf->object));
    extra_message = g_string_free (spa_dbg_str, FALSE);
    spa_dbg_str = NULL;
  }
  else if (cf->object && g_type_is_a (cf->object_type, WP_TYPE_PROXY) &&
      (wp_object_get_active_features ((WpObject *) cf->object) & WP_PROXY_FEATURE_BOUND)) {
    extra_object = g_strdup_printf (":%u:",
        wp_proxy_get_bound_id ((WpProxy *) cf->object));
  }

  return g_strdup_printf ("%s<%s%s%p>%s %s",
      object_color,
      cf->object_type != 0 ? g_type_name (cf->object_type) : "",
      extra_object ? extra_object : ":",
      cf->object,
      use_color ? RESET_COLOR : "",
      extra_message ? extra_message : cf->message);
}

static inline void
extract_common_fields (struct common_fields *cf, const GLogField *fields,
    gsize n_fields)
{
  for (guint i = 0; i < n_fields; i++) {
    if (g_strcmp0 (fields[i].key, "GLIB_DOMAIN") == 0) {
      cf->log_domain = fields[i].value;
    }
    else if (g_strcmp0 (fields[i].key, "MESSAGE") == 0) {
      cf->message = fields[i].value;
      cf->message_field = (GLogField *) &fields[i];
    }
    else if (g_strcmp0 (fields[i].key, "CODE_FILE") == 0) {
      cf->file = fields[i].value;
    }
    else if (g_strcmp0 (fields[i].key, "CODE_LINE") == 0) {
      cf->line = fields[i].value;
    }
    else if (g_strcmp0 (fields[i].key, "CODE_FUNC") == 0) {
      cf->func = fields[i].value;
    }
    else if (g_strcmp0 (fields[i].key, "WP_OBJECT_TYPE") == 0 &&
        fields[i].length == sizeof (GType)) {
      cf->object_type = *((GType *) fields[i].value);
    }
    else if (g_strcmp0 (fields[i].key, "WP_OBJECT") == 0 &&
        fields[i].length == sizeof (gconstpointer)) {
      cf->object = *((gconstpointer *) fields[i].value);
    }
  }
}

/*!
 * \brief Use this to figure out if a debug message is going to be printed or not,
 * so that you can avoid allocating resources just for debug logging purposes
 * \ingroup wplog
 * \param log_level a log level
 * \returns whether the log level is currently enabled
 */
gboolean
wp_log_level_is_enabled (GLogLevelFlags log_level)
{
  return log_level_index (log_level) <= enabled_level;
}

static gint
level_index_from_string (const char *str)
{
  g_return_val_if_fail (str != NULL, 0);

  for (guint i = 0; i < G_N_ELEMENTS (log_level_info); i++) {
    if (g_str_equal (str, log_level_info[i].name))
      return i;
  }

  return level_index_from_spa (atoi (str));
}

/*!
 * \brief Configures the log level and enabled categories
 * \ingroup wplog
 * \param level_str a log level description string as it would appear in the
 *   WIREPLUMBER_DEBUG environment variable "level:category1,category2"
 */
void
wp_log_set_level (const gchar * level_str)
{
  gint n_tokens = 0;
  gchar **tokens = NULL;
  gchar **categories = NULL;

  /* reset to defaults */
  enabled_level = 4; /* MESSAGE */
  if (enabled_categories) {
    GPatternSpec **pspec = enabled_categories;
    for (; *pspec != NULL; pspec++)
      g_pattern_spec_free (*pspec);
    g_clear_pointer (&enabled_categories, g_free);
  }

  if (level_str && level_str[0] != '\0') {
    /* level:category1,category2 */
    tokens = pw_split_strv (level_str, ":", 2, &n_tokens);

    /* set the log level */
    enabled_level = level_index_from_string (tokens[0]);

    /* enable filtering of debug categories */
    if (n_tokens > 1) {
      categories = pw_split_strv (tokens[1], ",", INT_MAX, &n_tokens);

      /* alloc space to hold the GPatternSpec pointers */
      enabled_categories = g_malloc_n ((n_tokens + 1), sizeof (gpointer));
      if (!enabled_categories)
        g_error ("out of memory");

      for (gint i = 0; i < n_tokens; i++)
        enabled_categories[i] = g_pattern_spec_new (categories[i]);
      enabled_categories[n_tokens] = NULL;
    }
  }

  /* set the log level also on the spa_log */
  wp_spa_log_get_instance()->level = level_index_to_spa (enabled_level);

  if (categories)
    pw_free_strv (categories);
  if (tokens)
    pw_free_strv (tokens);
}

static gboolean
is_category_enabled(const gchar *log_domain)
{
  GPatternSpec **cat = enabled_categories;
  guint len;
  g_autofree gchar *reverse_domain = NULL;

  if (!enabled_categories)
    return true;

  len = strlen (log_domain);
  reverse_domain = g_strreverse (g_strndup (log_domain, len));

  cat = enabled_categories;
  while (*cat && !g_pattern_match (*cat, len, log_domain, reverse_domain))
    cat++;

  /* NULL if we reached the end without matching */
  return (*cat != NULL);
}

/*!
 * \brief WirePlumber's GLogWriterFunc
 *
 * This is installed automatically when you call wp_init() with
 * WP_INIT_SET_GLIB_LOG set in the flags
 * \ingroup wplog
 */
GLogWriterOutput
wp_log_writer_default (GLogLevelFlags log_level,
    const GLogField *fields, gsize n_fields, gpointer user_data)
{
  struct common_fields cf = {0};
  g_autofree gchar *full_message = NULL;

  g_return_val_if_fail (fields != NULL, G_LOG_WRITER_UNHANDLED);
  g_return_val_if_fail (n_fields > 0, G_LOG_WRITER_UNHANDLED);

  /* in the unlikely event that someone messed with stderr... */
  if (G_UNLIKELY (!stderr || fileno (stderr) < 0))
    return G_LOG_WRITER_UNHANDLED;

  /* one-time initialization */
  if (g_once_init_enter (&initialized)) {
    use_color = g_log_writer_supports_color (fileno (stderr));
    output_is_journal = g_log_writer_is_journald (fileno (stderr));
    g_once_init_leave (&initialized, TRUE);
  }

  cf.log_level = log_level_index (log_level);

  /* check if debug level is enabled */
  if (cf.log_level > enabled_level)
    return G_LOG_WRITER_UNHANDLED;

  extract_common_fields (&cf, fields, n_fields);

  if (!cf.log_domain)
    cf.log_domain = "default";

  /* check if debug category is enabled */
  if (!is_category_enabled(cf.log_domain))
    return G_LOG_WRITER_UNHANDLED;

  if (G_UNLIKELY (!cf.message))
    cf.message_field->value = cf.message = "(null)";

  /* format the message to include the object */
  if (cf.object_type) {
    cf.message_field->value = cf.message = full_message =
        format_message (&cf);
  }

  /* write complete field information to the journal if we are logging to it */
  if (output_is_journal &&
      g_log_writer_journald (log_level, fields, n_fields, user_data) == G_LOG_WRITER_HANDLED)
    return G_LOG_WRITER_HANDLED;

  write_debug_message (stderr, &cf);
  return G_LOG_WRITER_HANDLED;
}

/*!
 * \brief Used internally by the debug logging macros. Avoid using it directly.
 * \ingroup wplog
 */
void
wp_log_structured_standard (
    const gchar *log_domain,
    GLogLevelFlags log_level,
    const gchar *file,
    const gchar *line,
    const gchar *func,
    GType object_type,
    gconstpointer object,
    const gchar *message_format,
    ...)
{
  g_autofree gchar *message = NULL;
  GLogField fields[8] = {
    { "PRIORITY", log_level_info[log_level_index (log_level)].priority, -1 },
    { "CODE_FILE", file, -1 },
    { "CODE_LINE", line, -1 },
    { "CODE_FUNC", func, -1 },
    { "MESSAGE", NULL, -1 },
  };
  gsize n_fields = 5;
  va_list args;

  if (log_domain != NULL) {
    fields[n_fields].key = "GLIB_DOMAIN";
    fields[n_fields].value = log_domain;
    fields[n_fields].length = -1;
    n_fields++;
  }

  if (object_type != 0) {
    fields[n_fields].key = "WP_OBJECT_TYPE";
    fields[n_fields].value = &object_type;
    fields[n_fields].length = sizeof (GType);
    n_fields++;
  }

  if (object != NULL) {
    fields[n_fields].key = "WP_OBJECT";
    fields[n_fields].value = &object;
    fields[n_fields].length = sizeof (gconstpointer);
    n_fields++;
  }

  va_start (args, message_format);
  fields[4].value = message = g_strdup_vprintf (message_format, args);
  va_end (args);

  g_log_structured_array (log_level, fields, n_fields);
}

static G_GNUC_PRINTF (7, 0) void
wp_spa_log_logtv (void *object,
    enum spa_log_level level,
    const struct spa_log_topic *topic,
    const char *file,
    int line,
    const char *func,
    const char *fmt,
    va_list args)
{
  g_autofree gchar *message = NULL;
  gchar line_str[11];
  GLogField fields[] = {
    { "PRIORITY", NULL, -1 },
    { "CODE_FILE", file, -1 },
    { "CODE_LINE", line_str, -1 },
    { "CODE_FUNC", func, -1 },
    { "MESSAGE", NULL, -1 },
    { "GLIB_DOMAIN", "pw", -1 },
  };

  gint log_level_idx = level_index_from_spa (level);
  GLogLevelFlags log_level = log_level_info[log_level_idx].log_level;
  fields[0].value = log_level_info[log_level_idx].priority;

  sprintf (line_str, "%d", line);
  fields[4].value = message = g_strdup_vprintf (fmt, args);

  if (topic)
    fields[5].value = topic->topic;

  g_log_structured_array (log_level, fields, SPA_N_ELEMENTS (fields));
}

static G_GNUC_PRINTF (7, 8) void
wp_spa_log_logt (void *object,
    enum spa_log_level level,
    const struct spa_log_topic *topic,
    const char *file,
    int line,
    const char *func,
    const char *fmt, ...)
{
  va_list args;
  va_start (args, fmt);
  wp_spa_log_logtv (object, level, topic, file, line, func, fmt, args);
  va_end (args);
}

static G_GNUC_PRINTF (6, 0) void
wp_spa_log_logv (void *object,
    enum spa_log_level level,
    const char *file,
    int line,
    const char *func,
    const char *fmt,
    va_list args)
{
  wp_spa_log_logtv (object, level, NULL, file, line, func, fmt, args);
}

static G_GNUC_PRINTF (6, 7) void
wp_spa_log_log (void *object,
    enum spa_log_level level,
    const char *file,
    int line,
    const char *func,
    const char *fmt, ...)
{
  va_list args;
  va_start (args, fmt);
  wp_spa_log_logtv (object, level, NULL, file, line, func, fmt, args);
  va_end (args);
}

static void
wp_spa_log_topic_init (void *object, struct spa_log_topic *topic)
{
  if (is_category_enabled(topic->topic)) {
    topic->has_custom_level = false;
  } else {
    topic->has_custom_level = true;
    topic->level = SPA_LOG_LEVEL_NONE;
  }
}

static const struct spa_log_methods wp_spa_log_methods = {
  SPA_VERSION_LOG_METHODS,
  .log = wp_spa_log_log,
  .logv = wp_spa_log_logv,
  .logt = wp_spa_log_logt,
  .logtv = wp_spa_log_logtv,
  .topic_init = wp_spa_log_topic_init,
};

static struct spa_log wp_spa_log = {
  .iface = { SPA_TYPE_INTERFACE_Log, SPA_VERSION_LOG, { &wp_spa_log_methods, NULL } },
  .level = SPA_LOG_LEVEL_WARN,
};

/*!
 * \brief Gets WirePlumber's instance of `spa_log`
 * \ingroup wplog
 * \returns WirePlumber's instance of `spa_log`, which can be used to redirect
 *   PipeWire's log messages to the currently installed GLogWriterFunc.
 *   This is installed automatically when you call wp_init() with
 *   WP_INIT_SET_PW_LOG set in the flags
 */
struct spa_log *
wp_spa_log_get_instance (void)
{
  return &wp_spa_log;
}
