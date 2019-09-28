/****************************************************************************
**
** Copyright (C) 2018 Unified Automation GmbH
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "certificatedialog.h"
#include "ui_certificatedialog.h"
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
