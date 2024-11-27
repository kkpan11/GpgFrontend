/**
 * Copyright (C) 2021-2024 Saturneric <eric@bktus.com>
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

#pragma once

#include "GFSDKExport.h"

extern "C" {

using QObjectFactory = void* (*)(void*);

struct MetaData {
  const char* key;
  const char* value;
};

auto GPGFRONTEND_MODULE_SDK_EXPORT
GFUIMountEntry(const char* id, MetaData** meta_data_array,
               int meta_data_array_size, QObjectFactory factory) -> int;

auto GPGFRONTEND_MODULE_SDK_EXPORT GFUICreateGUIObject(QObjectFactory factory,
                                                       void* data) -> void*;

auto GPGFRONTEND_MODULE_SDK_EXPORT GFUIMainWindowPtr() -> void*;

auto GPGFRONTEND_MODULE_SDK_EXPORT GFUIActiveWindowPtr() -> void*;

auto GPGFRONTEND_MODULE_SDK_EXPORT GFUIShowDialog(void* dialog,
                                                  void* parent) -> bool;
}