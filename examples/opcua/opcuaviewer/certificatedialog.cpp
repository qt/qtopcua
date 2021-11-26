/****************************************************************************
**
** Copyright (C) 2018 Unified Automation GmbH
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "certificatedialog.h"
#include "ui_certificatedialog.h"
#include <QFile>
#include <QPushButton>

QT_BEGIN_NAMESPACE

CertificateDialog::CertificateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CertificateDialog)
{
    ui->setupUi(this);
    connect(ui->btnTrust, &QPushButton::clicked, this, &CertificateDialog::saveCertificate);
}

CertificateDialog::~CertificateDialog()
{
    delete ui;
}

/** Returns 0 if the connect should be aborted, 1 if it should be resumed. */
int CertificateDialog::showCertificate(const QString &message, const QByteArray &der, const QString &trustListDirectory)
{
    QList<QSslCertificate> certs = QSslCertificate::fromData(der, QSsl::Der);

    m_trustListDirectory = trustListDirectory;

    // if it is a unstrusted self-signed certificate we can allow to trust it
    if (certs.count() == 1 && certs[0].isSelfSigned()) {
        m_cert = certs[0];
        ui->btnTrust->setEnabled(true);
    } else {
        ui->btnTrust->setEnabled(false);
    }

    for (const QSslCertificate &cert : qAsConst(certs))
        ui->certificate->appendPlainText(cert.toText());

    ui->message->setText(message);
    ui->certificate->moveCursor(QTextCursor::Start);
    ui->certificate->ensureCursorVisible();

    return exec();
}

void CertificateDialog::saveCertificate()
{
   const QByteArray digest = m_cert.digest();
   const QString path = m_trustListDirectory + QLatin1Char('/')
       + QLatin1String(digest.toHex()) + QLatin1String(".der");

   QFile file(path);
   if (file.open(QIODevice::WriteOnly)) {
       file.write(m_cert.toDer());
       file.close();
   }
}

QT_END_NAMESPACE
