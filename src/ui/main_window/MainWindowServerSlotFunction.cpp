/**
 * This file is part of GpgFrontend.
 *
 * GpgFrontend is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <https://www.gnu.org/licenses/>.
 *
 * The initial version of the source code is inherited from gpg4usb-team.
 * Their source code version also complies with GNU General Public License.
 *
 * The source code version of this software was modified and released
 * by Saturneric<eric@bktus.com> starting on May 12, 2021.
 *
 */

#include "MainWindow.h"

#ifdef SERVER_SUPPORT
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "server/ComUtils.h"
#endif
#include "ui/ShowCopyDialog.h"

namespace GpgFrontend::UI {

#ifdef SERVER_SUPPORT

/**
 * get full size crypt text from server using short crypto text
 * @param shortenCryptoText short crypto text([GpgFrontend_ShortCrypto]://)
 * @return
 */
QString MainWindow::getCryptText(const QString& shortenCryptoText) {
  QString ownKeyId = settings.value("general/ownKeyId").toString();

  GpgKey key = mCtx->getKeyRefById(ownKeyId);
  if (!key.good) {
    QMessageBox::critical(this, _("Invalid Own Key"),
                          _("Own Key can not be use to do any operation. "
                            "Please go to the setting interface to select an "
                            "OwnKey and get a ServiceToken."));
    return {};
  }

  auto utils = new ComUtils(this);

  QString serviceToken = settings.value("general/serviceToken").toString();
  if (serviceToken.isEmpty() || !utils->checkServiceTokenFormat(serviceToken)) {
    QMessageBox::critical(
        this, _("Error"),
        _("Please obtain a Service Token from the server in the settings."));
    return {};
  }

  QUrl reqUrl(utils->getUrl(ComUtils::GetFullCryptText));
  QNetworkRequest request(reqUrl);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  // Sign Shorten Text
  auto outSignTextBase64 =
      ComUtils::getSignStringBase64(mCtx, shortenCryptoText, key);

  rapidjson::Document doc;
  doc.SetObject();

  rapidjson::Value s, t;

  // Signature
  s.SetString(outSignTextBase64.constData(), outSignTextBase64.count());
  // Service Token
  const auto t_byte_array = serviceToken.toUtf8();
  t.SetString(t_byte_array.constData(), t_byte_array.count());

  rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

  doc.AddMember("signature", s, allocator);
  doc.AddMember("serviceToken", t, allocator);

  rapidjson::StringBuffer sb;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
  doc.Accept(writer);

  QByteArray postData(sb.GetString());
  qDebug() << "postData" << QString::fromUtf8(postData);

  QNetworkReply* reply = utils->getNetworkManager().post(request, postData);

  auto dialog = new WaitingDialog(_("Getting Cpt From Server"), this);
  dialog->show();

  while (reply->isRunning()) QApplication::processEvents();

  dialog->close();

  QByteArray replyData = reply->readAll().constData();
  if (utils->checkServerReply(replyData)) {
    /**
     * {
     *      "cryptoText" : ...
     *      "sha": ...
     *      "serviceToken": ...
     *      "date": ...
     * }
     */

    if (!utils->checkDataValueStr("cryptoText") ||
        !utils->checkDataValueStr("sha") ||
        !utils->checkDataValueStr("serviceToken")) {
      QMessageBox::critical(this, _("Error"),
                            _("The communication content with the server does "
                              "not meet the requirements"));
      return {};
    }

    auto cryptoText = utils->getDataValueStr("cryptoText");
    auto sha = utils->getDataValueStr("sha");
    auto serviceTokenFromServer = utils->getDataValueStr("serviceToken");

    QCryptographicHash sha_generator(QCryptographicHash::Sha256);
    sha_generator.addData(cryptoText.toUtf8());

    if (sha_generator.result().toHex() == sha &&
        serviceToken == serviceTokenFromServer) {
      return cryptoText;
    } else
      QMessageBox::critical(this, _("Error"), _("Invalid short ciphertext"));

    return {};
  }

  return {};
}

#endif

#ifdef SERVER_SUPPORT

void MainWindow::shortenCryptText() {
  // gather information
  QString serviceToken = settings.value("general/serviceToken").toString();
  QString ownKeyId = settings.value("general/ownKeyId").toString();
  QByteArray cryptoText = edit->curTextPage()->toPlainText().toUtf8();

  auto utils = new ComUtils(this);

  if (serviceToken.isEmpty() || !utils->checkServiceTokenFormat(serviceToken)) {
    QMessageBox::critical(
        this, _("Invalid Service Token"),
        _("Please go to the setting interface to get a ServiceToken."));
    return;
  }

  QUrl reqUrl(utils->getUrl(ComUtils::ShortenCryptText));
  QNetworkRequest request(reqUrl);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  GpgKey key = mCtx->getKeyRefById(ownKeyId);
  if (!key.good) {
    QMessageBox::critical(this, _("Invalid Own Key"),
                          _("Own Key can not be use to do any operation."));
    return;
  }

  QCryptographicHash ch(QCryptographicHash::Md5);
  ch.addData(cryptoText);
  QString md5 = ch.result().toHex();

  qDebug() << "md5" << md5;

  QByteArray signText = QString("[%1][%2]").arg(serviceToken, md5).toUtf8();

  QCryptographicHash sha(QCryptographicHash::Sha256);
  sha.addData(signText);
  QString shaText = sha.result().toHex();

  qDebug() << "shaText" << shaText;

  QByteArray outSignTextBase64 =
      ComUtils::getSignStringBase64(mCtx, signText, key);

  rapidjson::Value c, s, m, t;

  rapidjson::Document doc;
  doc.SetObject();

  c.SetString(cryptoText.constData(), cryptoText.count());
  auto m_byte_array = shaText.toUtf8();
  m.SetString(m_byte_array.constData(), m_byte_array.count());
  s.SetString(outSignTextBase64.constData(), outSignTextBase64.count());
  auto t_byte_array = serviceToken.toUtf8();
  t.SetString(t_byte_array.constData(), t_byte_array.count());

  rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

  doc.AddMember("cryptoText", c, allocator);
  doc.AddMember("sha", m, allocator);
  doc.AddMember("sign", s, allocator);
  doc.AddMember("serviceToken", t, allocator);

  rapidjson::StringBuffer sb;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
  doc.Accept(writer);

  QByteArray postData(sb.GetString());
  qDebug() << "postData" << QString::fromUtf8(postData);

  QNetworkReply* reply = networkAccessManager->post(request, postData);

  auto* dialog = new WaitingDialog(_("Getting Scpt From Server"), this);
  dialog->show();
  while (reply->isRunning()) QApplication::processEvents();
  dialog->close();

  if (utils->checkServerReply(reply->readAll().constData())) {
    /**
     * {
     *      "shortenText" : ...
     *      "md5": ...
     * }
     */

    if (!utils->checkDataValueStr("shortenText") ||
        !utils->checkDataValueStr("md5")) {
      QMessageBox::critical(this, _("Error"),
                            _("The communication content with the server does "
                              "not meet the requirements"));
      return;
    }

    QString shortenText = utils->getDataValueStr("shortenText");

    QCryptographicHash md5_generator(QCryptographicHash::Md5);
    md5_generator.addData(shortenText.toUtf8());
    if (md5_generator.result().toHex() == utils->getDataValueStr("md5")) {
      auto* dialog =
          new ShowCopyDialog(shortenText,
                             _("Notice: Use Decrypt & Verify operation to "
                               "decrypt this short crypto text."),
                             this);
      dialog->show();
    } else {
      QMessageBox::critical(
          this, _("Error"),
          _("There is a problem with the communication with the server"));
      return;
    }
  }
}

#endif

}  // namespace GpgFrontend::UI