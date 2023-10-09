// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuaerrorstate.h"

QT_BEGIN_NAMESPACE

class QOpcUaErrorStateData : public QSharedData
{
public:
    QOpcUaErrorStateData() {}

    QOpcUaErrorState::ConnectionStep m_connectionStep = QOpcUaErrorState::ConnectionStep::CertificateValidation;
    QOpcUa::UaStatusCode m_errorCode = QOpcUa::Good;
    bool m_clientSideError = false;
    bool m_ignoreError = false;
};

/*!
    \class QOpcUaErrorState
    \inmodule QtOpcUa
    \since QtOpcUa 5.13
    \brief QOpcUaErrorState allows investigation and interaction with error state from backends.

    There is not need to instantiate this class in your code.
    A client will emit an error state via \l QOpcUaClient::connectError in case an error has happened
    while establishing a connection.

    The error can be caused by the backend itself or by the server rejecting the connection.
    If case of errors issued by the local backend, they can be ignored by calling the function
    \l setIgnoreError().
*/

/*!
    \enum QOpcUaErrorState::ConnectionStep

    Specifies at which step during the connection establishment the error occurred.

    \value Unknown The connection step is unknown.
    \value CertificateValidation Error happened in the certificate validation step.
    \value OpenSecureChannel Error happened when opening the secure channel.
    \value CreateSession Error happened when creating the session.
    \value ActivateSession Error happened during session acivation.
*/

/*!
    Default constructs an error state with no parameters set.
*/
QOpcUaErrorState::QOpcUaErrorState()
    : data(new QOpcUaErrorStateData())
{
}

/*!
    Constructs an error state from \a other.
*/
QOpcUaErrorState::QOpcUaErrorState(const QOpcUaErrorState &other)
    : data(other.data)
{}

/*!
    Sets the values of \a rhs in this error state.
*/
QOpcUaErrorState &QOpcUaErrorState::operator=(const QOpcUaErrorState &rhs)
{
    if (this != &rhs)
        data = rhs.data;
    return *this;
}

QOpcUaErrorState::~QOpcUaErrorState()
{

}

/*!
    Returns the connection step in which the error occurred.
*/
QOpcUaErrorState::ConnectionStep QOpcUaErrorState::connectionStep() const
{
    return data->m_connectionStep;
}

/*!
    Sets the connection step in which the error occurred to \a step.
*/
void QOpcUaErrorState::setConnectionStep(QOpcUaErrorState::ConnectionStep step)
{
    data->m_connectionStep = step;
}

/*!
    Returns the OPC UA status code of the error occurred.
*/
QOpcUa::UaStatusCode QOpcUaErrorState::errorCode() const
{
    return data->m_errorCode;
}

/*!
    Sets the OPC UA status code of the error occurred to \a error.
*/
void QOpcUaErrorState::setErrorCode(QOpcUa::UaStatusCode error)
{
    data->m_errorCode = error;
}

/*!
    Returns if the occurred error is a client side error.
*/
bool QOpcUaErrorState::isClientSideError() const
{
    return data->m_clientSideError;
}

/*!
    Sets if the occurred error is a client side error to \a clientSideError.
*/
void QOpcUaErrorState::setClientSideError(bool clientSideError)
{
    data->m_clientSideError = clientSideError;
}

/*!
    Sets if this client side error should be ignored to \a ignore.

    Setting this flag does only work if the error is actually a client side error.
*/
void QOpcUaErrorState::setIgnoreError(bool ignore)
{
    data->m_ignoreError = ignore;
}

/*!
    Returns if this client side error should be ignored.
*/
bool QOpcUaErrorState::ignoreError() const
{
    return data->m_ignoreError;
}

QT_END_NAMESPACE
