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

#include "GpgTOFUInfo.h"
#include "core/model/GpgKeySignature.h"
#include "core/typedef/CoreTypedef.h"

namespace GpgFrontend {
/**
 * @brief
 *
 */
class GF_CORE_EXPORT GpgUID {
 public:
  /**
   * @brief
   *
   * @return QString
   */
  [[nodiscard]] auto GetName() const -> QString;

  /**
   * @brief
   *
   * @return QString
   */
  [[nodiscard]] auto GetEmail() const -> QString;

  /**
   * @brief
   *
   * @return QString
   */
  [[nodiscard]] auto GetComment() const -> QString;

  /**
   * @brief
   *
   * @return QString
   */
  [[nodiscard]] auto GetUID() const -> QString;

  /**
   * @brief
   *
   * @return true
   * @return false
   */
  [[nodiscard]] auto GetRevoked() const -> bool;

  /**
   * @brief
   *
   * @return true
   * @return false
   */
  [[nodiscard]] auto GetInvalid() const -> bool;

  /**
   * @brief
   *
   * @return std::unique_ptr<QContainer<GpgTOFUInfo>>
   */
  [[nodiscard]] auto GetTofuInfos() const
      -> std::unique_ptr<QContainer<GpgTOFUInfo>>;

  /**
   * @brief
   *
   * @return std::unique_ptr<QContainer<GpgKeySignature>>
   */
  [[nodiscard]] auto GetSignatures() const
      -> std::unique_ptr<QContainer<GpgKeySignature>>;

  /**
   * @brief Construct a new Gpg U I D object
   *
   */
  GpgUID();

  /**
   * @brief Construct a new Gpg U I D object
   *
   * @param uid
   */
  explicit GpgUID(QSharedPointer<struct _gpgme_key> key_ref,
                  gpgme_user_id_t uid);

  /**
   * @brief
   *
   */
  GpgUID(const GpgUID &);

  /**
   * @brief
   *
   * @return GpgUID&
   */
  auto operator=(const GpgUID &) -> GpgUID &;

 private:
  QSharedPointer<struct _gpgme_key> key_ref_;
  gpgme_user_id_t uid_ref_ = nullptr;  ///<
};

}  // namespace GpgFrontend
