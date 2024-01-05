/* WirePlumber
 *
 * Copyright © 2022 Collabora Ltd.
 *    @author Ashok Sidipotu <ashok.sidipotu@collabora.com>
 *
 * SPDX-License-Identifier: MIT
 */
#include "../common/base-test-fixture.h"

typedef struct {
  WpBaseTestFixture base;

  WpProperties *settings;

  WpImplMetadata *impl_metadata;
  WpMetadata *metadata;

  WpSettings *s;

  gchar *triggered_setting;
  WpSpaJson *triggered_setting_value;
  gboolean triggered_callback;
} TestSettingsFixture;

static void
test_conf_file_setup (TestSettingsFixture *self, gconstpointer user_data)
{
  self->base.conf_file = g_strdup_printf ("%s/settings/wireplumber.conf",
      g_getenv ("G_TEST_SRCDIR"));

  wp_base_test_fixture_setup (&self->base, WP_BASE_TEST_FLAG_CLIENT_CORE);
}

static void
test_conf_file_teardown (TestSettingsFixture *self, gconstpointer user_data)
{
  wp_base_test_fixture_teardown (&self->base);
}

struct data {
  int count;
  WpProperties *settings;
};

static WpProperties *
do_parse_settings (WpSpaJson *json)
{
  g_autoptr (WpProperties) settings = wp_properties_new_empty ();
  g_autoptr (WpIterator) iter = wp_spa_json_new_iterator (json);
  g_auto (GValue) item = G_VALUE_INIT;

  if (!wp_spa_json_is_object (json))
    return NULL;

  while (wp_iterator_next (iter, &item)) {
    WpSpaJson *j = g_value_get_boxed (&item);
    g_autofree gchar *name = wp_spa_json_parse_string (j);
    g_autofree gchar *value = NULL;

    g_value_unset (&item);
    wp_iterator_next (iter, &item);
    j = g_value_get_boxed (&item);

    value = wp_spa_json_to_string (j);
    g_value_unset (&item);

    if (name && value)
      wp_properties_set (settings, name, value);
  }

  return g_steal_pointer (&settings);
}

static void
test_parsing_setup (TestSettingsFixture *self, gconstpointer user_data)
{
  test_conf_file_setup (self, user_data);

  {
    g_autoptr (WpConf) conf = wp_conf_get_instance (self->base.core);
    g_assert_nonnull (conf);
    g_autoptr (WpSpaJson) json = wp_conf_get_section (conf,
        "wireplumber.settings", NULL);
    g_assert_nonnull (json);

    self->settings = do_parse_settings (json);
    g_assert_nonnull (self->settings);
  }

}

static void
test_parsing_teardown (TestSettingsFixture *self, gconstpointer user_data)
{
  g_clear_pointer (&self->settings, wp_properties_unref);

  test_conf_file_teardown (self, user_data);
}

static void
test_parsing (TestSettingsFixture *self, gconstpointer data)
{
  /* total no.of settings in the conf file */
  g_assert_cmpint (wp_properties_get_count(self->settings), ==, 10);
}

static void
on_metadata_activated (WpMetadata * m, GAsyncResult * res, gpointer user_data)
{
  TestSettingsFixture *self = user_data;
  g_autoptr (GError) error = NULL;
  g_autoptr (WpIterator) it = NULL;
  g_auto (GValue) item = G_VALUE_INIT;

  g_assert_true(wp_object_activate_finish (WP_OBJECT (m), res, NULL));

  for (it = wp_properties_new_iterator (self->settings);
        wp_iterator_next (it, &item);
        g_value_unset (&item)) {
    WpPropertiesItem *pi = g_value_get_boxed (&item);

    const gchar *setting = wp_properties_item_get_key (pi);
    const gchar *value = wp_properties_item_get_value (pi);

    wp_metadata_set (m, 0, setting, "Spa:String:JSON", value);
  }
  g_debug ("loaded settings(%d) to \"sm-settings\" metadata\n",
      wp_properties_get_count (self->settings));

  self->metadata = g_object_ref(m);
  g_main_loop_quit(self->base.loop);
}

static void
test_metadata_setup (TestSettingsFixture *self, gconstpointer user_data)
{
  test_parsing_setup (self, user_data);

  {
    self->impl_metadata = wp_impl_metadata_new_full (self->base.core,
        "sm-settings", NULL);

    wp_object_activate (WP_OBJECT (self->impl_metadata),
        WP_OBJECT_FEATURES_ALL,
        NULL,
        (GAsyncReadyCallback)on_metadata_activated,
        self);

    g_main_loop_run (self->base.loop);
  }

}

static void
test_metadata_teardown (TestSettingsFixture *self, gconstpointer user_data)
{
  test_parsing_teardown (self, user_data);

  g_clear_object (&self->impl_metadata);
  g_clear_object (&self->metadata);
}

static void
test_metadata (TestSettingsFixture *self, gconstpointer data)
{
  g_autoptr (WpProperties) settings = wp_properties_new_empty();

  g_autoptr (WpIterator) it = wp_metadata_new_iterator
      (WP_METADATA (self->metadata), 0);
  g_auto (GValue) val = G_VALUE_INIT;

  for (; wp_iterator_next (it, &val); g_value_unset (&val)) {
      const gchar *setting, *value;
      wp_metadata_iterator_item_extract (&val, NULL, &setting, NULL, &value);
      wp_properties_set (settings, setting, value);
      g_debug ("%s(%lu) = %s\n", setting, strlen(value), value);
  }

  /* match the settings loaded from conf file and metadata */
  g_assert_true (wp_properties_matches (self->settings, settings));

}

static void
on_settings_ready (WpSettings *s, GAsyncResult *res, gpointer data)
{
  TestSettingsFixture *self = data;

  g_assert_true(wp_object_activate_finish (WP_OBJECT (s), res, NULL));

  g_main_loop_quit(self->base.loop);
}

static void
test_wpsettings_setup (TestSettingsFixture *self, gconstpointer user_data)
{
  test_metadata_setup (self, user_data);

  {
    self->s = wp_settings_get_instance (self->base.core, "sm-settings");

    wp_object_activate (WP_OBJECT (self->s),
        WP_OBJECT_FEATURES_ALL,
        NULL,
        (GAsyncReadyCallback)on_settings_ready,
        self);
    g_main_loop_run (self->base.loop);
  }

}

static void
test_wpsettings_teardown (TestSettingsFixture *self, gconstpointer user_data)
{
  test_metadata_teardown (self, user_data);
  g_clear_object (&self->s);
}

static void
test_wpsettings (TestSettingsFixture *self, gconstpointer data)
{
  WpSettings *s = self->s;

  {
    /* undefined */
    g_autoptr (WpSpaJson) j = wp_settings_get (s, "test-setting-undefined");
    g_assert_null (j);
  }

  {
    gboolean value = FALSE;

    /* _get_boolean */
    g_autoptr (WpSpaJson) j = wp_settings_get (s, "test-setting1");
    g_assert_nonnull (j);
    g_assert_true (wp_spa_json_parse_boolean (j, &value));
    g_assert_false (value);

    g_autoptr (WpSpaJson) j2 = wp_settings_get (s, "test-setting2");
    g_assert_nonnull (j2);
    g_assert_true (wp_spa_json_parse_boolean (j2, &value));
    g_assert_true (value);
  }

  {
    gint value = 0;

    /* _get_int () */
    g_autoptr (WpSpaJson) j = wp_settings_get (s, "test-setting3-int");
    g_assert_nonnull (j);
    g_assert_true (wp_spa_json_parse_int (j, &value));
    g_assert_cmpint (value, ==, -20);
  }

  {
    /* _get_string () */
    {
      g_autofree gchar *value = NULL;
      g_autoptr (WpSpaJson) j = wp_settings_get (s, "test-setting4-string");
      g_assert_nonnull (j);
      value = wp_spa_json_parse_string (j);
      g_assert_nonnull (value);
      g_assert_cmpstr (value, ==, "blahblah");
    }

    {
      g_autofree gchar *value = NULL;
      g_autoptr (WpSpaJson) j = NULL;
      j = wp_settings_get (s, "test-setting5-string-with-quotes");
      g_assert_nonnull (j);
      value = wp_spa_json_parse_string (j);
      g_assert_nonnull (value);
      g_assert_cmpstr (value, ==, "a string with \"quotes\"");
    }
  }

  {
    gfloat value = 0.0;

    g_autoptr (WpSpaJson) j = wp_settings_get (s, "test-setting-float1");
    g_assert_nonnull (j);
    g_assert_true (wp_spa_json_parse_float (j, &value));
    g_assert_cmpfloat_with_epsilon (value, 3.14, 0.001);

    g_autoptr (WpSpaJson) j2 = wp_settings_get (s, "test-setting-float2");
    g_assert_nonnull (j2);
    g_assert_true (wp_spa_json_parse_float (j2, &value));
    g_assert_cmpfloat_with_epsilon (value, 0.4, 0.001);
  }

  /* test the wp_settings_get_instance () API */
  {
    g_autoptr (WpSettings) s1 =
        wp_settings_get_instance (self->base.core, "test-settings");
    g_autoptr (WpSettings) s2 =
        wp_settings_get_instance (self->base.core, "test-settings");
    g_autoptr (WpSettings) s3 =
        wp_settings_get_instance (self->base.core, "blah-blah");

    g_assert_false (s == s1);
    g_assert_true (s1 == s2);
    g_assert_false (s1 == s3);

  }

  {
    /* _get_json () */
    {
      g_autoptr (WpSpaJson) value = NULL;
      value = wp_settings_get (s, "test-setting-json");
      g_assert_nonnull (value);
      g_assert_true (wp_spa_json_is_array (value));
      g_assert_cmpstr (wp_spa_json_get_data (value), ==, "[1, 2, 3]");
    }

    {
      g_autoptr (WpSpaJson) value = NULL;
      value = wp_settings_get (s, "test-setting-json2");
      g_assert_nonnull (value);
      g_assert_true (wp_spa_json_is_array (value));

      {
        g_autofree gchar *s1 = NULL;
        g_autofree gchar *s2 = NULL;
        g_autofree gchar *s3 = NULL;
        g_autofree gchar *s4 = NULL;

        wp_spa_json_parse_array
            (value, "s", &s1, "s", &s2, "s", &s3, "s", &s4, NULL);
        g_assert_cmpstr (s1, ==, "test1");
        g_assert_cmpstr (s2, ==, "test 2");
        g_assert_cmpstr (s3, ==, "test three");
        g_assert_cmpstr (s4, ==, "test-four");
      }

      {
        gchar *sample_str[] = {
          "test1",
          "test 2",
          "test three",
          "test-four"
        };
        g_autoptr (WpIterator) it = wp_spa_json_new_iterator (value);
        g_auto (GValue) item = G_VALUE_INIT;

        for (int i = 0; wp_iterator_next (it, &item);
            g_value_unset (&item), i++) {
          WpSpaJson *s = g_value_get_boxed (&item);
          g_autofree gchar *str = wp_spa_json_parse_string (s);
          g_assert_cmpstr (str, ==, sample_str[i]);
        }
      }
    }

    {
      g_autoptr (WpSpaJson) value = NULL;
      value = wp_settings_get (s, "test-setting-json3");
      g_assert_nonnull (value);
      g_assert_true (wp_spa_json_is_object (value));
      g_assert_cmpstr (wp_spa_json_get_data(value), ==,
        "{ key1: \"value\", key2: 2, key3: true }");

      g_autofree gchar *value1 = NULL;
      gint value2 = 0;
      gboolean value3 = FALSE;
      g_assert_true (wp_spa_json_object_get (value,
          "key1", "s", &value1,
          "key2", "i", &value2,
          "key3", "b", &value3,
          NULL));

      g_assert_cmpstr (value1, ==, "value");
      g_assert_cmpint (value2, ==, 2);
      g_assert_true (value3);
    }
  }

  {
    g_autoptr (WpSettings) s4 =
        wp_settings_get_instance (self->base.core, NULL);

    g_auto (GValue) value = G_VALUE_INIT;
    g_object_get_property (G_OBJECT(s4), "metadata-name", &value);

    g_assert_cmpstr (g_value_get_string (&value), ==, "sm-settings");

  }
}

void wp_settings_changed_callback (WpSettings *obj, const gchar *setting,
    WpSpaJson *json, gpointer user_data)
{
  TestSettingsFixture *self = user_data;
  g_assert_cmpstr (setting, ==, self->triggered_setting);
  self->triggered_callback = true;
  g_assert_nonnull (json);

  if (wp_spa_json_is_boolean (json)) {
    gboolean value = FALSE, expected = FALSE;
    g_assert_true (wp_spa_json_parse_boolean (json, &value));
    g_assert_true (wp_spa_json_parse_boolean (self->triggered_setting_value,
        &expected));
    g_assert_cmpint (value, ==, expected);
  } else if (wp_spa_json_is_int (json)) {
    gint value = 0, expected = 0;
    g_assert_true (wp_spa_json_parse_int (json, &value));
    g_assert_true (wp_spa_json_parse_int (self->triggered_setting_value,
        &expected));
    g_assert_cmpint (value, ==, expected);
  } else if (wp_spa_json_is_string (json)) {
    g_autofree gchar *value = wp_spa_json_parse_string (json);
    g_autofree gchar *expected = wp_spa_json_parse_string (json);
    g_assert_nonnull (value);
    g_assert_nonnull (expected);
    g_assert_cmpstr (value, ==, expected);
  }
}

static void
test_callbacks (TestSettingsFixture *self, gconstpointer data)
{
  WpSettings *s = self->s;
  guintptr sub_id;

  /* register callback */
  sub_id = wp_settings_subscribe (s, "test*",
      wp_settings_changed_callback, (gpointer)self);

  {
    g_autoptr (WpSpaJson) json = wp_spa_json_new_boolean (TRUE);
    self->triggered_setting = "test-setting1";
    self->triggered_setting_value = json;
    self->triggered_callback = FALSE;
    wp_metadata_set (self->metadata, 0, self->triggered_setting,
        "Spa:String:JSON", wp_spa_json_get_data (json));
    g_assert_cmpint (self->triggered_callback, ==, TRUE);
  }

  {
    g_autoptr (WpSpaJson) json = wp_spa_json_new_boolean (TRUE);
    self->triggered_setting = "test-setting1";
    self->triggered_setting_value = json;
    self->triggered_callback = FALSE;
    wp_metadata_set (self->metadata, 0, self->triggered_setting,
        "Spa:String:JSON", wp_spa_json_get_data (json));
    g_assert_cmpint (self->triggered_callback, ==, FALSE);
  }

  {
    g_autoptr (WpSpaJson) json = wp_spa_json_new_int (99);
    self->triggered_setting = "test-setting3-int";
    self->triggered_setting_value = json;
    self->triggered_callback = TRUE;
    wp_metadata_set (self->metadata, 0, self->triggered_setting,
        "Spa:String:JSON", wp_spa_json_get_data (json));
    g_assert_cmpint (self->triggered_callback, ==, TRUE);
  }

  {
    g_autoptr (WpSpaJson) json = wp_spa_json_new_string ("lets not blabber");
    self->triggered_setting = "test-setting4-string";
    self->triggered_setting_value = json;
    self->triggered_callback = TRUE;
    wp_metadata_set (self->metadata, 0, self->triggered_setting,
        "Spa:String:JSON", wp_spa_json_get_data (json));
    g_assert_cmpint (self->triggered_callback, ==, TRUE);
  }

  {
    g_autoptr (WpSpaJson) json = wp_spa_json_new_string ("lets blabber");
    self->triggered_setting = "test-setting4-string";
    self->triggered_setting_value = json;
    self->triggered_callback = FALSE;
    g_assert_cmpint (wp_settings_unsubscribe (s, sub_id), ==,
        true);
    g_assert_cmpint (wp_settings_unsubscribe (s, (sub_id-1)), ==,
        false);
    wp_metadata_set (self->metadata, 0, self->triggered_setting,
        "Spa:String:JSON", wp_spa_json_get_data (json));
    g_assert_cmpint (self->triggered_callback, ==, FALSE);
  }
}

gint
main (gint argc, gchar *argv[])
{
  g_test_init (&argc, &argv, NULL);
  wp_init (WP_INIT_ALL);

  /* take a close look at .conf file that is loaded, all the test work on it */
  g_test_add ("/wp/settings/parsing", TestSettingsFixture, NULL,
      test_parsing_setup, test_parsing, test_parsing_teardown);
  g_test_add ("/wp/settings/metadata-creation", TestSettingsFixture, NULL,
      test_metadata_setup, test_metadata, test_metadata_teardown);
  g_test_add ("/wp/settings/wpsettings-creation-get", TestSettingsFixture, NULL,
      test_wpsettings_setup, test_wpsettings, test_wpsettings_teardown);
  g_test_add ("/wp/settings/wpsettings-callbacks", TestSettingsFixture, NULL,
      test_wpsettings_setup, test_callbacks, test_wpsettings_teardown);

  return g_test_run ();
}
