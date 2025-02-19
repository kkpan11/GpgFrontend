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

#include "Wizard.h"

#include "core/function/GlobalSettingStation.h"
#include "ui/GpgFrontendUI.h"

namespace GpgFrontend::UI {

Wizard::Wizard(QWidget* parent) : QWizard(parent) {
  setPage(kPAGE_INTRO, new IntroPage(this));
  setPage(kPAGE_CHOOSE, new ChoosePage(this));
  setPage(kPAGE_CONCLUSION, new ConclusionPage(this));
#ifndef Q_WS_MAC
  setWizardStyle(ModernStyle);
#endif
  setWindowTitle(tr("First Start Wizard"));

  setPixmap(QWizard::LogoPixmap,
            QPixmap(":/icons/gpgfrontend_logo.png").scaled(64, 64));

  int next_page_id = GetSettings().value("wizard.next_page", -1).toInt();
  setStartId(next_page_id);

  connect(this, &Wizard::accepted, this, &Wizard::slot_wizard_accepted);
}

void Wizard::slot_wizard_accepted() {
  auto settings = GetSettings();
  settings.setValue("wizard/show_wizard", !field("showWizard").toBool());
  settings.setValue("network/prohibit_update_checking",
                    !field("checkUpdate").toBool());
}

IntroPage::IntroPage(QWidget* parent) : QWizardPage(parent) {
  setTitle(tr("Getting Started..."));
  setSubTitle(tr("... with GpgFrontend"));

  auto* top_label = new QLabel(
      tr("Welcome to GpgFrontend for decrypting and signing text or files!") +
      " <br><br><a href='https://gpgfrontend.bktus.com'>GpgFrontend</a> " +
      tr("is a Powerful, Easy-to-Use, Compact, Cross-Platform, and "
         "Installation-Free OpenPGP Crypto Tool. ") +
      tr("To get started, be sure to check out the") +
      " <a href='https://gpgfrontend.bktus.com/overview/glance'>" +
      tr("Overview") + "</a> (" +
      tr("by clicking the link, the page will open in your web browser") +
      "). <br>");
  top_label->setTextFormat(Qt::RichText);
  top_label->setTextInteractionFlags(Qt::TextBrowserInteraction);
  top_label->setOpenExternalLinks(true);
  top_label->setWordWrap(true);

  // QComboBox for language selection
  auto* lang_label =
      new QLabel(tr("If it supports the language currently being used in your "
                    "system, GpgFrontend will automatically set it."));
  lang_label->setWordWrap(true);

  // set layout and add widgets
  auto* layout = new QVBoxLayout;
  layout->addWidget(top_label);
  layout->addStretch();
  layout->addWidget(lang_label);

  setLayout(layout);
}

auto IntroPage::nextId() const -> int { return Wizard::kPAGE_CHOOSE; }

ChoosePage::ChoosePage(QWidget* parent) : QWizardPage(parent) {
  setTitle(tr("Choose your action..."));
  setSubTitle(tr("...by clicking on the appropriate link."));

  auto* keygen_label = new QLabel(
      tr("If you have never used GpgFrontend before and also don't own a gpg "
         "key yet you may possibly want to read how to") +
      " <a href=\"https://gpgfrontend.bktus.com/guides/generate-key\">" +
      tr("Generate Key") + "</a><hr>");
  keygen_label->setTextFormat(Qt::RichText);
  keygen_label->setTextInteractionFlags(Qt::TextBrowserInteraction);
  keygen_label->setOpenExternalLinks(true);
  keygen_label->setWordWrap(true);

  auto* encr_decy_text_label = new QLabel(
      tr("If you want to learn how to encrypt, decrypt, sign and verify text, "
         "you can read ") +
      "<a href=\"https://gpgfrontend.bktus.com/guides/encrypt-decrypt-text\">" +
      tr("Encrypt & Decrypt Text") + "</a> " + tr("or") +
      " <a href=\"https://gpgfrontend.bktus.com/guides/sign-verify-text\">" +
      tr("Sign & Verify Text") + "</a><hr>");

  encr_decy_text_label->setTextFormat(Qt::RichText);
  encr_decy_text_label->setTextInteractionFlags(Qt::TextBrowserInteraction);
  encr_decy_text_label->setOpenExternalLinks(true);
  encr_decy_text_label->setWordWrap(true);

  auto* sign_verify_text_label = new QLabel(
      tr("If you want to operate file, you can read ") +
      "<a href=\"https://gpgfrontend.bktus.com/guides/encrypt-decrypt-file\">" +
      tr("Encrypt & Sign File") + "</a> " + tr("or") +
      " <a href=\"https://gpgfrontend.bktus.com/guides/sign-verify-file\">" +
      tr("Sign & Verify File") + "</a><hr>");
  sign_verify_text_label->setTextFormat(Qt::RichText);
  sign_verify_text_label->setTextInteractionFlags(Qt::TextBrowserInteraction);
  sign_verify_text_label->setOpenExternalLinks(true);
  sign_verify_text_label->setWordWrap(true);

  auto* layout = new QVBoxLayout();
  layout->addWidget(keygen_label);
  layout->addWidget(encr_decy_text_label);
  layout->addWidget(sign_verify_text_label);
  setLayout(layout);
  next_page_ = Wizard::kPAGE_CONCLUSION;
}

auto ChoosePage::nextId() const -> int { return next_page_; }

void ChoosePage::slot_jump_page(const QString& page) {
  QMetaObject const qmo = Wizard::staticMetaObject;
  int const index = qmo.indexOfEnumerator("WizardPages");
  QMetaEnum const m = qmo.enumerator(index);

  next_page_ = m.keyToValue(page.toUtf8().data());
  wizard()->next();
}

ConclusionPage::ConclusionPage(QWidget* parent) : QWizardPage(parent) {
  setTitle(tr("Ready."));
  setSubTitle(tr("Have fun with GpgFrontend!"));

  auto* bottom_label = new QLabel(
      tr("You are ready to use GpgFrontend now.<br><br>") +
      "<a href=\"https://gpgfrontend.bktus.com/guides/fundamental-concepts\">" +
      tr("The Online Document") + "</a>" +
      tr(" will get you started with GpgFrontend. Anytime you encounter "
         "problems, please try to find help from the documentation") +
      "<br>");

  bottom_label->setTextFormat(Qt::RichText);
  bottom_label->setTextInteractionFlags(Qt::TextBrowserInteraction);
  bottom_label->setOpenExternalLinks(true);
  bottom_label->setWordWrap(true);

  open_help_check_box_ = new QCheckBox(tr("Open offline help."));
  open_help_check_box_->setChecked(true);

  dont_show_wizard_checkbox_ =
      new QCheckBox(tr("Don't show the wizard again."));
  dont_show_wizard_checkbox_->setChecked(true);

  check_updates_checkbox_ =
      new QCheckBox(tr("Check for updates at each startup."));
  check_updates_checkbox_->setChecked(true);

  registerField("showWizard", dont_show_wizard_checkbox_);
  registerField("checkUpdate", check_updates_checkbox_);

  auto* layout = new QVBoxLayout;
  layout->addWidget(bottom_label);
  layout->addWidget(dont_show_wizard_checkbox_);
  layout->addWidget(check_updates_checkbox_);
  setLayout(layout);
  setVisible(true);
}

auto ConclusionPage::nextId() const -> int { return -1; }

}  // namespace GpgFrontend::UI
