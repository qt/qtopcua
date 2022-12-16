// Copyright (C) 2018 Unified Automation GmbH
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "certificatedialog.h"
#include "ui_certificatedialog.h"
#include <QFile>
#include <QPushButton>

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

    for (const QSslCertificate &cert : std::as_const(certs))
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
