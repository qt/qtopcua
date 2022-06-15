// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <mbedtls/x509.h>
#include <mbedtls/x509_crt.h>

int main(int argc, char *argv[])
{
    mbedtls_pk_context pk;
    mbedtls_pk_init( &pk );

    mbedtls_x509_crt remoteCertificate;
    mbedtls_x509_crt_init(&remoteCertificate);

    return 0;
}
