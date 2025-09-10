// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtOpcUa/qopcuadiagnosticinfo.h>

#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaDiagnosticInfo
    \inmodule QtOpcUa
    \since 6.7

    \brief The QOpcUaDiagnosticInfo class models the OPC UA built-in type DiagnosticInfo.

    The DiagnosticInfo type is used to convey diagnostics for some operations
    on the server. The \l qint32 type members refer to an index in the stringTable
    field of the OPC UA response header which is currently not supported by
    Qt OPC UA.
*/

class QOpcUaDiagnosticInfoData : public QSharedData
{
public:
    qint32 symbolicId = 0;
    bool hasSymbolicId = false;
    qint32 namespaceUri = 0;
    bool hasNamespaceUri = false;
    qint32 locale = 0;
    bool hasLocale = false;
    qint32 localizedText = 0;
    bool hasLocalizedText = false;
    QString additionalInfo;
    bool hasAdditionalInfo = false;
    QOpcUa::UaStatusCode innerStatusCode = QOpcUa::UaStatusCode::Good;
    bool hasInnerStatusCode = false;
    std::optional<QOpcUaDiagnosticInfo> innerDiagnosticInfo;
    bool hasInnerDiagnosticInfo = false;
};

QT_DEFINE_QESDP_SPECIALIZATION_DTOR(QOpcUaDiagnosticInfoData)

/*!
   Constructs a diagnostic info.
*/
QOpcUaDiagnosticInfo::QOpcUaDiagnosticInfo()
    : data(new QOpcUaDiagnosticInfoData())
{
}

/*!
    Destroys this diagnostic info object.
*/
QOpcUaDiagnosticInfo::~QOpcUaDiagnosticInfo()
{
}

/*!
    Constructs a diagnostic info from \a other.
*/
QOpcUaDiagnosticInfo::QOpcUaDiagnosticInfo(const QOpcUaDiagnosticInfo &other)
    : data(other.data)
{
}

/*!
    Sets the value of \a rhs in this diagnostic info.
*/
QOpcUaDiagnosticInfo &QOpcUaDiagnosticInfo::operator=(const QOpcUaDiagnosticInfo &rhs)
{
    if (this != &rhs)
        this->data = rhs.data;

    return *this;
}

/*!
    \fn QOpcUaDiagnosticInfo::QOpcUaDiagnosticInfo(QOpcUaDiagnosticInfo &&other)

    Move-constructs a new diagnostic info from \a other.

    \note The moved-from object \a other is placed in a
    partially-formed state, in which the only valid operations are
    destruction and assignment of a new value.
*/

/*!
    \fn QOpcUaDiagnosticInfo &QOpcUaDiagnosticInfo::operator=(QOpcUaDiagnosticInfo &&other)

    Move-assigns \a other to this QOpcUaDiagnosticInfo instance.

    \note The moved-from object \a other is placed in a
    partially-formed state, in which the only valid operations are
    destruction and assignment of a new value.
*/

/*!
    \fn void QOpcUaDiagnosticInfo::swap(QOpcUaDiagnosticInfo &other)

    Swaps diagnostic info object \a other with this diagnostic info
    object. This operation is very fast and never fails.
*/

/*!
    \fn bool QOpcUaDiagnosticInfo::operator!=(const QOpcUaDiagnosticInfo &lhs, const QOpcUaDiagnosticInfo &rhs)

    Returns \c true if \a lhs is not equal to \a rhs.
*/

/*!
    \fn bool QOpcUaDiagnosticInfo::operator==(const QOpcUaDiagnosticInfo &lhs, const QOpcUaDiagnosticInfo &rhs)

    Returns \c true if \a lhs is equal to \a rhs.
*/
bool comparesEqual(const QOpcUaDiagnosticInfo &lhs, const QOpcUaDiagnosticInfo &rhs) noexcept
{
    if (lhs.hasSymbolicId() != rhs.hasSymbolicId() ||
        (lhs.hasSymbolicId() && lhs.symbolicId() != rhs.symbolicId()))
        return false;

    if (lhs.hasNamespaceUri() != rhs.hasNamespaceUri() ||
        (lhs.hasNamespaceUri() && lhs.namespaceUri() != rhs.namespaceUri()))
        return false;

    if (lhs.hasLocale() != lhs.hasLocale() ||
        (lhs.hasLocale() && lhs.locale() != rhs.locale()))
        return false;

    if (lhs.hasLocalizedText() != rhs.hasLocalizedText() ||
        (lhs.hasLocalizedText() && lhs.localizedText() != rhs.localizedText()))
        return false;

    if (lhs.hasAdditionalInfo() != lhs.hasAdditionalInfo() ||
        (lhs.hasAdditionalInfo() && lhs.additionalInfo() != rhs.additionalInfo()))
        return false;

    if (lhs.hasInnerStatusCode() != lhs.hasInnerStatusCode() ||
        (lhs.hasInnerStatusCode() && lhs.innerStatusCode() != rhs.innerStatusCode()))
        return false;

    if (lhs.hasInnerDiagnosticInfo() != lhs.hasInnerDiagnosticInfo()
        || (lhs.hasInnerDiagnosticInfo()
            && lhs.data->innerDiagnosticInfo != rhs.data->innerDiagnosticInfo))
        return false;

    return true;
}

/*!
    Returns a \l QVariant containing this diagnostic info.
*/
QOpcUaDiagnosticInfo::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

/*!
    Returns the symbolic id of this diagnostic info.
 */
qint32 QOpcUaDiagnosticInfo::symbolicId() const
{
    return data->symbolicId;
}

/*!
    Sets the symbolic id of this diagnostic info to \a newSymbolicId.
 */
void QOpcUaDiagnosticInfo::setSymbolicId(qint32 newSymbolicId)
{
    if (newSymbolicId != data->symbolicId) {
        data.detach();
        data->symbolicId = newSymbolicId;
    }
}

/*!
    Returns \c true if this diagnostic info has a symbolic id.
 */
bool QOpcUaDiagnosticInfo::hasSymbolicId() const
{
    return data->hasSymbolicId;
}

/*!
    Sets the information whether this diagnostic info has a symbolic id to \a newHasSymbolicId.
 */
void QOpcUaDiagnosticInfo::setHasSymbolicId(bool newHasSymbolicId)
{
    if (newHasSymbolicId != data->hasSymbolicId) {
        data.detach();
        data->hasSymbolicId = newHasSymbolicId;
    }
}

/*!
    Returns the namespace URI of this diagnostic info.
 */
qint32 QOpcUaDiagnosticInfo::namespaceUri() const
{
    return data->namespaceUri;
}

/*!
    Sets the namespace URI of this diagnostic info to \a newNamespaceUri.
 */
void QOpcUaDiagnosticInfo::setNamespaceUri(qint32 newNamespaceUri)
{
    if (newNamespaceUri != data->namespaceUri) {
        data.detach();
        data->namespaceUri = newNamespaceUri;
    }
}

/*!
   Returns \c true if this diagnostic info has a namespace URI.
 */
bool QOpcUaDiagnosticInfo::hasNamespaceUri() const
{
    return data->hasNamespaceUri;
}

/*!
   Sets the information whether this diagnostic info has a namespace URI to \a newHasNamespaceUri.
 */
void QOpcUaDiagnosticInfo::setHasNamespaceUri(bool newHasNamespaceUri)
{
    if (newHasNamespaceUri != data->hasNamespaceUri) {
        data.detach();
        data->hasNamespaceUri = newHasNamespaceUri;
    }
}

/*!
   Returns the locale of this diagnostic info.
 */
qint32 QOpcUaDiagnosticInfo::locale() const
{
    return data->locale;
}

/*!
   Sets the locale of this diagnostic info to \a newLocale.
 */
void QOpcUaDiagnosticInfo::setLocale(qint32 newLocale)
{
    if (newLocale != data->locale) {
        data.detach();
        data->locale = newLocale;
    }
}

/*!
   Returns \c true if this diagnostic info has a locale.
 */
bool QOpcUaDiagnosticInfo::hasLocale() const
{
    return data->hasLocale;
}

/*!
   Sets the information whether this diagnostic info has a locale to \a newHasLocale.
 */
void QOpcUaDiagnosticInfo::setHasLocale(bool newHasLocale)
{
    if (newHasLocale != data->hasLocale) {
        data.detach();
        data->hasLocale = newHasLocale;
    }
}

/*!
   Returns the localized text of this diagnostic info.
 */
qint32 QOpcUaDiagnosticInfo::localizedText() const
{
    return data->localizedText;
}

/*!
   Sets the localized text of this diagnostic info to \a newLocalizedText.
 */
void QOpcUaDiagnosticInfo::setLocalizedText(qint32 newLocalizedText)
{
    if (newLocalizedText != data->localizedText) {
        data.detach();
        data->localizedText = newLocalizedText;
    }
}

/*!
   Returns \c true if this diagnostic info has a localized text.
 */
bool QOpcUaDiagnosticInfo::hasLocalizedText() const
{
    return data->hasLocalizedText;
}

/*!
   Sets the information whether this diagnostic info has a localized text to \a newHasLocalizedText.
 */
void QOpcUaDiagnosticInfo::setHasLocalizedText(bool newHasLocalizedText)
{
    if (newHasLocalizedText != data->hasLocalizedText) {
        data.detach();
        data->hasLocalizedText = newHasLocalizedText;
    }
}

/*!
   Returns the additional information of this diagnostic info.
 */
QString QOpcUaDiagnosticInfo::additionalInfo() const
{
    return data->additionalInfo;
}

/*!
   Sets the additional information of this diagnostic info to \a newAdditionalInfo.
 */
void QOpcUaDiagnosticInfo::setAdditionalInfo(const QString &newAdditionalInfo)
{
    if (newAdditionalInfo != data->additionalInfo) {
        data.detach();
        data->additionalInfo = newAdditionalInfo;
    }
}

/*!
   Returns \c true if this diagnostic info has additional information.
 */
bool QOpcUaDiagnosticInfo::hasAdditionalInfo() const
{
    return data->hasAdditionalInfo;
}

/*!
   Sets the information whether this diagnostic info has additional information to \a newHasAdditionalInfo.
 */
void QOpcUaDiagnosticInfo::setHasAdditionalInfo(bool newHasAdditionalInfo)
{
    if (newHasAdditionalInfo != data->hasAdditionalInfo) {
        data.detach();
        data->hasAdditionalInfo = newHasAdditionalInfo;
    }
}

/*!
   Returns the inner status code of this diagnostic info.
 */
QOpcUa::UaStatusCode QOpcUaDiagnosticInfo::innerStatusCode() const
{
    return data->innerStatusCode;
}

/*!
   Sets the inner status code of this diagnostic info to \a newInnerStatusCode.
 */
void QOpcUaDiagnosticInfo::setInnerStatusCode(QOpcUa::UaStatusCode newInnerStatusCode)
{
    if (newInnerStatusCode != data->innerStatusCode) {
        data.detach();
        data->innerStatusCode = newInnerStatusCode;
    }
}

/*!
   Returns \c true if this diagnostic info has an inner status code.
 */
bool QOpcUaDiagnosticInfo::hasInnerStatusCode() const
{
    return data->hasInnerStatusCode;
}

/*!
   Sets the information whether this diagnostic info has an inner status code to \a newHasInnerStatusCode.
 */
void QOpcUaDiagnosticInfo::setHasInnerStatusCode(bool newHasInnerStatusCode)
{
    if (newHasInnerStatusCode != data->hasInnerStatusCode) {
        data.detach();
        data->hasInnerStatusCode = newHasInnerStatusCode;
    }
}

/*!
   Returns the inner diagnostic info of this diagnostic info.
 */
QOpcUaDiagnosticInfo QOpcUaDiagnosticInfo::innerDiagnosticInfo() const
{
    return data->innerDiagnosticInfo.value_or(QOpcUaDiagnosticInfo());
}

/*!
   Returns a reference to the inner diagnostic info of this diagnostic info.
 */
QOpcUaDiagnosticInfo &QOpcUaDiagnosticInfo::innerDiagnosticInfoRef()
{
    if (!data->innerDiagnosticInfo.has_value()) {
        data.detach();
        data->innerDiagnosticInfo = QOpcUaDiagnosticInfo();
    }

    return data->innerDiagnosticInfo.value();
}

/*!
   Sets the inner diagnostic info of this diagnostic info to \a newInnerDiagnosticInfo.
 */
void QOpcUaDiagnosticInfo::setInnerDiagnosticInfo(const QOpcUaDiagnosticInfo &newInnerDiagnosticInfo)
{
    if (newInnerDiagnosticInfo != data->innerDiagnosticInfo) {
        data.detach();
        data->innerDiagnosticInfo = newInnerDiagnosticInfo;
    }
}

/*!
   Returns \c true if this diagnostic info has an inner diagnostic info.
 */
bool QOpcUaDiagnosticInfo::hasInnerDiagnosticInfo() const
{
    return data->hasInnerDiagnosticInfo;
}

/*!
   Sets the information whether this diagnostic info has an inner diagnostic info to \a newHasInnerDiagnosticInfo.
 */
void QOpcUaDiagnosticInfo::setHasInnerDiagnosticInfo(bool newHasInnerDiagnosticInfo)
{
    if (newHasInnerDiagnosticInfo != data->hasInnerDiagnosticInfo) {
        data.detach();
        data->hasInnerDiagnosticInfo = newHasInnerDiagnosticInfo;
    }
}

QT_END_NAMESPACE
