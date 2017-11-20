/****************************************************************************
**
** Copyright (C) 2017 basysKom GmbH, opensource@basyskom.com
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

#ifndef QOPCUABINARYDATAENCODING_H
#define QOPCUABINARYDATAENCODING_H

#include "qopcuaglobal.h"
#include "qopcuatype.h"

#include <QtCore/qmetatype.h>

QT_BEGIN_NAMESPACE

// This class implements a subset of the OPC UA Binary DataEncoding defined in OPC-UA part 6, 5.2.
// Only the types needed to handle the data types in OPC-UA part 8, 5.6 have been implemented.
class Q_OPCUA_EXPORT QOpcUaBinaryDataEncoding
{
public:

    // The Ids in this enum are the numeric ids of the _Encoding_DefaultBinary nodes for the respective types
    // as listed in https://opcfoundation.org/UA/schemas/1.03/NodeIds.csv
    enum class TypeEncodingId {
        Range = 886,
        EUInformation = 889,
        ComplexNumber = 12181,
        DoubleComplexNumber = 12182,
        AxisInformation = 12089,
        XV = 12090
    };

    template <typename T>
    static T decode(const char *&ptr, size_t &bufferSize, bool &success);
    template <typename T>
    static QVector<T> decodeArray(const char *&ptr, size_t &bufferSize, bool &success);

    template <typename T>
    static void encode(const T &src, QByteArray &dst);
    template <typename T>
    static void encodeArray(const QVector<T> &src, QByteArray &dst);
};

QT_END_NAMESPACE

#endif // QOPCUABINARYDATAENCODING_H
