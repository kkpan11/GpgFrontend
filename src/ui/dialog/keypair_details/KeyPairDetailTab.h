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

#ifndef GPGFRONTEND_KEYPAIRDETAILTAB_H
#define GPGFRONTEND_KEYPAIRDETAILTAB_H

#include "KeySetExpireDateDialog.h"
#include "core/GpgContext.h"
#include "ui/GpgFrontendUI.h"
#include "ui/dialog/import_export/KeyServerImportDialog.h"
#include "ui/dialog/import_export/KeyUploadDialog.h"

namespace GpgFrontend::UI {

class KeyPairDetailTab : public QWidget {
  Q_OBJECT

 private slots:

  /**
   * @details Copy the fingerprint to clipboard
   */
  void slot_copy_fingerprint();

  /**
   * @brief
   *
   */
  void slot_refresh_key_info();

  /**
   * @brief
   *
   */
  void slot_refresh_key();

 private:
  GpgKey key_;  ///<

  QGroupBox* owner_box_;        ///< Groupbox containing owner information
  QGroupBox* key_box_;          ///< Groupbox containing key information
  QGroupBox* fingerprint_box_;  ///< Groupbox containing fingerprint information
  QGroupBox* additional_uid_box_;  ///< Groupbox containing information about
                                   ///< additional uids

  QLabel* name_var_label_;      ///< Label containing the keys name
  QLabel* email_var_label_;     ///< Label containing the keys email
  QLabel* comment_var_label_;   ///< Label containing the keys comment
  QLabel* key_size_var_label_;  ///< Label containing the keys key size
  QLabel* expire_var_label_;    ///< Label containing the keys expiration date
  QLabel* created_var_label_;   ///< Label containing the keys creation date
  QLabel* last_update_var_label_;  ///<
  QLabel* algorithm_var_label_;    ///< Label containing the keys algorithm
  QLabel* key_id_var_label;        ///< Label containing the keys keyid
  QLabel* fingerprint_var_label_;  ///< Label containing the keys fingerprint
  QLabel* usage_var_label_;
  QLabel* actual_usage_var_label_;
  QLabel* primary_key_exist_var_label_;
  QLabel* owner_trust_var_label_;

  QLabel* icon_label_;  ///<
  QLabel* exp_label_;   ///<

 public:
  /**
   * @brief Construct a new Key Pair Detail Tab object
   *
   * @param key_id
   * @param parent
   */
  explicit KeyPairDetailTab(const std::string& key_id,
                            QWidget* parent = nullptr);
};

}  // namespace GpgFrontend::UI

#endif  // GPGFRONTEND_KEYPAIRDETAILTAB_H
