/* WirePlumber
 *
 * Copyright © 2022 Collabora Ltd.
 *    @author Ashok Sidipotu <ashok.sidipotu@collabora.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __WIREPLUMBER_SETTINGS_H__
#define __WIREPLUMBER_SETTINGS_H__

#include "object.h"
#include "spa-json.h"

G_BEGIN_DECLS

/*!
 * \brief Flags to be used as WpObjectFeatures on WpSettings subclasses.
 * \ingroup wpsettings
 */
typedef enum { /*< flags >*/
  /*! Loads the settings */
  WP_SETTINGS_LOADED = (1 << 0),
} WpSettingsFeatures;

/*!
 * \brief The WpSettings GType
 * \ingroup wpsettings
 */
#define WP_TYPE_SETTINGS (wp_settings_get_type ())

WP_API
G_DECLARE_FINAL_TYPE (WpSettings, wp_settings, WP, SETTINGS, WpObject)

WP_API
WpSettings * wp_settings_get_instance (WpCore * core,
    const gchar *metadata_name);

/*!
 * \brief callback conveying the changed setting and its json value
 *
 * \ingroup wpsettings
 * \param obj the wpsettings object
 * \param setting the changed setting
 * \param value json value of the the changed setting
 * \param user_data data passed in the \a wp_settings_subscribe
 */
typedef void (*WpSettingsChangedCallback) (WpSettings *obj,
    const gchar *setting, WpSpaJson *value, gpointer user_data);

WP_API
guintptr wp_settings_subscribe (WpSettings *self,
    const gchar *pattern, WpSettingsChangedCallback callback,
    gpointer user_data);

WP_API
guintptr wp_settings_subscribe_closure (WpSettings *self,
    const gchar *pattern,  GClosure * closure);

WP_API
gboolean wp_settings_unsubscribe (WpSettings *self,
    guintptr subscription_id);

WP_API
WpSpaJson * wp_settings_get (WpSettings *self, const gchar *setting);

G_END_DECLS

#endif
