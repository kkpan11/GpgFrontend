/**
 * Copyright (C) 2021 Saturneric
 *
 * This file is part of GpgFrontend.
 *
 * GpgFrontend is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GpgFrontend is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GpgFrontend. If not, see <https://www.gnu.org/licenses/>.
 *
 * The initial version of the source code is inherited from
 * the gpg4usb project, which is under GPL-3.0-or-later.
 *
 * All the source code of GpgFrontend was modified and released by
 * Saturneric<eric@bktus.com> starting on May 12, 2021.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "core/function/GlobalSettingStation.h"

/**
 * @brief Get the files of a given directory
 *
 * @param _path target directory
 * @return std::vector<std::filesystem::path>
 */
std::vector<std::filesystem::path> get_files_of_directory(
    const std::filesystem::path& _path) {
  namespace fs = std::filesystem;
  std::vector<fs::path> path_list;
  if (!_path.empty()) {
    fs::recursive_directory_iterator end;

    for (fs::recursive_directory_iterator i(_path); i != end; ++i) {
      const fs::path cp = (*i);
      path_list.push_back(cp);
    }
  }
  return path_list;
}

/**
 * @brief setup the locale and load the translations
 *
 */
void init_locale() {
  // get the instance of the GlobalSettingStation
  auto& settings =
      GpgFrontend::GlobalSettingStation::GetInstance().GetUISettings();

  // create general settings if not exist
  if (!settings.exists("general") ||
      settings.lookup("general").getType() != libconfig::Setting::TypeGroup)
    settings.add("general", libconfig::Setting::TypeGroup);

  // set system default at first
  auto& general = settings["general"];
  if (!general.exists("lang"))
    general.add("lang", libconfig::Setting::TypeString) = "";

  // sync the settings to the file
  GpgFrontend::GlobalSettingStation::GetInstance().SyncSettings();

  LOG(INFO) << "current system locale" << setlocale(LC_ALL, nullptr);

  // read from settings file
  std::string lang;
  if (!general.lookupValue("lang", lang)) {
    LOG(ERROR) << _("could not read properly from configure file");
  };

  LOG(INFO) << "lang from settings" << lang;
  LOG(INFO) << "project name" << PROJECT_NAME;
  LOG(INFO) << "locales path"
            << GpgFrontend::GlobalSettingStation::GetInstance()
                   .GetLocaleDir()
                   .c_str();

#ifndef WINDOWS
  if (!lang.empty()) {
    std::string lc = lang.empty() ? "" : lang + ".UTF-8";

    // set LC_ALL
    auto* locale_name = setlocale(LC_ALL, lc.c_str());
    if (locale_name == nullptr) LOG(WARNING) << "set LC_ALL failed" << lc;
    auto language = getenv("LANGUAGE");
    // set LANGUAGE
    std::string language_env = language == nullptr ? "en" : language;
    language_env.insert(0, lang + ":");
    LOG(INFO) << "language env" << language_env;
    if (setenv("LANGUAGE", language_env.c_str(), 1)) {
      LOG(WARNING) << "set LANGUAGE failed" << language_env;
    };
  }
#else
  if (!lang.empty()) {
    std::string lc = lang.empty() ? "" : lang;

    // set LC_ALL
    auto* locale_name = setlocale(LC_ALL, lc.c_str());
    if (locale_name == nullptr) LOG(WARNING) << "set LC_ALL failed" << lc;

    auto language = getenv("LANGUAGE");
    // set LANGUAGE
    std::string language_env = language == nullptr ? "en" : language;
    language_env.insert(0, lang + ":");
    language_env.insert(0, "LANGUAGE=");
    LOG(INFO) << "language env" << language_env;
    if (putenv(language_env.c_str())) {
      LOG(WARNING) << "set LANGUAGE failed" << language_env;
    };
  }
#endif

  bindtextdomain(PROJECT_NAME,
                 GpgFrontend::GlobalSettingStation::GetInstance()
                     .GetLocaleDir()
                     .u8string()
                     .c_str());
  bind_textdomain_codeset(PROJECT_NAME, "utf-8");
  textdomain(PROJECT_NAME);
}
