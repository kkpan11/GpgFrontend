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

#include "CommonUtils.h"

namespace GpgFrontend {

auto BeautifyFingerprint(QString fingerprint) -> QString {
  auto len = fingerprint.size();
  QString buffer;
  QTextStream out(&buffer);
  decltype(len) count = 0;
  while (count < len) {
    if ((count != 0U) && ((count % 5) == 0)) out << " ";
    out << fingerprint[count];
    count++;
  }
  return out.readAll();
}

auto GFCompareSoftwareVersion(const QString& a, const QString& b) -> int {
  auto remove_prefix = [](const QString& version) {
    return version.startsWith('v') ? version.mid(1) : version;
  };

  auto real_version_a = remove_prefix(a);
  auto real_version_b = remove_prefix(b);

  QStringList split_a = real_version_a.split('.');
  QStringList split_b = real_version_b.split('.');

  const auto min_depth = std::min(split_a.size(), split_b.size());

  for (int i = 0; i < min_depth; ++i) {
    int const num_a = split_a[i].toInt();
    int const num_b = split_b[i].toInt();

    if (num_a != num_b) {
      return (num_a > num_b) ? 1 : -1;
    }
  }

  if (split_a.size() != split_b.size()) {
    return (split_a.size() > split_b.size()) ? 1 : -1;
  }

  return 0;
}

auto GFStrDup(const QString& str) -> char* {
  auto utf8_str = str.toUtf8();
  auto* c_str =
      static_cast<char*>(SecureMalloc((utf8_str.size() + 1) * sizeof(char)));

  memcpy(c_str, utf8_str.constData(), utf8_str.size());
  c_str[utf8_str.size()] = '\0';
  return c_str;
}

auto GPGFRONTEND_CORE_EXPORT GFUnStrDup(char* str) -> QString {
  auto qt_str = QString::fromUtf8(str);
  SecureFree(static_cast<void*>(const_cast<char*>(str)));
  return qt_str;
}

auto GPGFRONTEND_CORE_EXPORT GFUnStrDup(const char* str) -> QString {
  return GFUnStrDup(const_cast<char*>(str));
}

}  // namespace GpgFrontend