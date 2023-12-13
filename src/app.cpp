/**
 * Copyright (C) 2021 Saturneric <eric@bktus.com>
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
 * Saturneric <eric@bktus.com> starting on May 12, 2021.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <csetjmp>

#include "GpgFrontendContext.h"
#include "core/GpgConstants.h"
#include "core/GpgCoreInit.h"
#include "module/GpgFrontendModuleInit.h"
#include "spdlog/spdlog.h"
#include "ui/GpgFrontendUIInit.h"

// main
#include "init.h"

/**
 * \brief Store the jump buff and make it possible to recover from a crash.
 */
#ifdef FREEBSD
sigjmp_buf recover_env;
#else
jmp_buf recover_env;
#endif

namespace GpgFrontend {

constexpr int kCrashCode = ~0;  ///<

/**
 * @brief
 *
 * @param argc
 * @param argv
 * @return int
 */
auto StartApplication(const GFCxtWPtr& p_ctx) -> int {
  GFCxtSPtr ctx = p_ctx.lock();
  if (ctx == nullptr) {
    SPDLOG_ERROR("cannot get gpgfrontend context.");
    return -1;
  }

  if (!ctx->load_ui_env) {
    SPDLOG_ERROR("the loading of gui application is forbidden.");
    return -1;
  }

  auto* app = qobject_cast<QApplication*>(ctx->app.get());
  if (app == nullptr) {
    SPDLOG_ERROR("cannot get qapplication from gpgfrontend context.");
    return -1;
  }

  SPDLOG_INFO("start running gui application");

  /**
   * internationalisation. loop to restart main window
   * with changed translation when settings change.
   */
  int return_from_event_loop_code;
  int restart_count = 0;

  do {
#ifndef WINDOWS
    int r = sigsetjmp(recover_env, 1);
#else
    int r = setjmp(recover_env);
#endif
    if (!r) {
      // after that load ui totally
      GpgFrontend::UI::InitGpgFrontendUI(app);

      // finally create main window
      return_from_event_loop_code = GpgFrontend::UI::RunGpgFrontendUI(app);
    } else {
      SPDLOG_ERROR("recover from a crash");
      // when signal is caught, restart the main window
      auto* message_box = new QMessageBox(
          QMessageBox::Critical, _("A serious error has occurred"),
          _("Oh no! GpgFrontend caught a serious error in the software, so "
            "it needs to be restarted. If the problem recurs, please "
            "manually terminate the program and report the problem to the "
            "developer."),
          QMessageBox::Ok, nullptr);
      message_box->exec();
      return_from_event_loop_code = kCrashCode;
    }

    SPDLOG_DEBUG("try to destroy modules system and core");

    // first should shutdown the module system
    GpgFrontend::Module::ShutdownGpgFrontendModules();

    // then shutdown the core
    GpgFrontend::DestroyGpgFrontendCore();

    SPDLOG_DEBUG("core and modules system destroyed");

    restart_count++;

    SPDLOG_DEBUG("restart loop refresh, event loop code: {}, restart count: {}",
                 return_from_event_loop_code, restart_count);
  } while (return_from_event_loop_code == GpgFrontend::kRestartCode &&
           restart_count < 3);

  // log for debug
  SPDLOG_INFO("GpgFrontend is about to exit.");

  // deep restart mode
  if (return_from_event_loop_code == GpgFrontend::kDeepRestartCode ||
      return_from_event_loop_code == kCrashCode) {
    // log for debug
    SPDLOG_DEBUG(
        "deep restart or cash loop status caught, restart a new application");
    QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
  };

  // exit the program
  return return_from_event_loop_code;
}

}  // namespace GpgFrontend
