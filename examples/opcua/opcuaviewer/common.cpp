/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt OPC UA module.
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

#include "common.h"

#include <QHash>
#include <QFile>
#include <QDebug>
#include <QString>

QT_BEGIN_NAMESPACE

DataTypeInfo dataTypeInfo(QString nodeId)
{
    static QHash<uint, DataTypeInfo> dataTypeInfoHash;

    if (dataTypeInfoHash.isEmpty()) {
        QFile data(":/NodeIds.csv");
        if (!data.open(QFile::ReadOnly)) {
            qFatal("Could not open %s", data.fileName().toLocal8Bit().constData());
        }

        do {
            auto line = data.readLine();
            if (line.isEmpty())
                break;
            auto token = line.split(',');
            if (token.size() != 3) {
                qWarning() << "Ignoring invalid node info line" << line;
                continue;
            }

            bool ok;
            DataTypeInfo dti;
            dti.name = token[0];
            dti.id = token[1].toUInt(&ok);
            dti.type = token[2];

            if (dti.name.size() == 0 || !ok || dti.type.size() == 0) {
                qWarning() << "Ignoring invalid node info data" << token;
                continue;
            }

            dataTypeInfoHash.insert(dti.id, dti);
        } while (true);
    }

    QString prefix("ns=0;i=");
    if (!nodeId.startsWith(prefix))
        return DataTypeInfo();

    bool ok;
    auto id = nodeId.mid(prefix.size()).toUInt(&ok);
    if (!ok)
        return DataTypeInfo();
    return dataTypeInfoHash[id];
}

QT_END_NAMESPACE
