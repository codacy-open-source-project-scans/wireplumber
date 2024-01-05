-- WirePlumber
--
-- Copyright © 2022 Collabora Ltd.
--
-- SPDX-License-Identifier: MIT

-- Device settings manager

local settings_manager = require ("settings-manager")

local defaults = {
  ["restore-profile"] = true,
  ["restore-routes"] = true,
  ["routes.default-sink-volume"] = 0.4 ^ 3,
  ["routes.default-source-volume"] = 1.0,
}

return settings_manager.new ("device.", defaults)
