// Copyright (C) 2018 Unified Automation GmbH
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef CERTIFICATEDIALOG_H
#define CERTIFICATEDIALOG_H

#include <QDialog>
#include <QSslCertificate>

QT_BEGIN_NAMESPACE
namespace Ui {
class CertificateDialog;
}
QT_END_NAMESPACE

class CertificateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CertificateDialog(QWidget *parent = nullptr);
    ~CertificateDialog();

    int showCertificate(const QString &message, const QByteArray &der, const QString &trustListDirectory);

private slots:
    void saveCertificate();

private:
    Ui::CertificateDialog *ui;
    QSslCertificate m_cert;
    QString         m_trustListDirectory;
};

#endif // CERTIFICATEDIALOG_H
