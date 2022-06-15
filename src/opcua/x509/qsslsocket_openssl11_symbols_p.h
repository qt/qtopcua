// Copyright (C) 2017 The Qt Company Ltd.
// Copyright (C) 2014 BlackBerry Limited. All rights reserved.
// Copyright (C) 2016 Richard J. Moore <rich@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

/****************************************************************************
**
** In addition, as a special exception, the copyright holders listed above give
** permission to link the code of its release of Qt with the OpenSSL project's
** "OpenSSL" library (or modified versions of the "OpenSSL" library that use the
** same license as the original version), and distribute the linked executables.
**
** You must comply with the GNU General Public License version 2 in all
** respects for all of the code used other than the "OpenSSL" code.  If you
** modify this file, you may extend this exception to your version of the file,
** but you are not obligated to do so.  If you do not wish to do so, delete
** this exception statement from your version of this file.
**
****************************************************************************/

#ifndef QSSLSOCKET_OPENSSL11_SYMBOLS_P_H
#define QSSLSOCKET_OPENSSL11_SYMBOLS_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

// Note: this file does not have QT_BEGIN_NAMESPACE/QT_END_NAMESPACE, it's done
// in qsslsocket_openssl_symbols_p.h.

#ifndef OPENSSL_SYMBOLS_P_H
#error "You are not supposed to use this header file, include openssl_symbols_p.h instead"
#endif

#include <openssl/x509.h>

const unsigned char * q_ASN1_STRING_get0_data(const ASN1_STRING *x);

Q_AUTOTEST_EXPORT BIO *q_BIO_new(const BIO_METHOD *a);
Q_AUTOTEST_EXPORT const BIO_METHOD *q_BIO_s_mem();

int q_DSA_bits(DSA *a);
int q_EVP_CIPHER_CTX_reset(EVP_CIPHER_CTX *c);
int q_EVP_PKEY_base_id(EVP_PKEY *a);
int q_RSA_bits(RSA *a);
Q_AUTOTEST_EXPORT int q_OPENSSL_sk_num(OPENSSL_STACK *a);
Q_AUTOTEST_EXPORT void q_OPENSSL_sk_pop_free(OPENSSL_STACK *a, void (*b)(void *));
Q_AUTOTEST_EXPORT OPENSSL_STACK *q_OPENSSL_sk_new_null();
Q_AUTOTEST_EXPORT void q_OPENSSL_sk_push(OPENSSL_STACK *st, void *data);
Q_AUTOTEST_EXPORT void q_OPENSSL_sk_free(OPENSSL_STACK *a);
Q_AUTOTEST_EXPORT void * q_OPENSSL_sk_value(OPENSSL_STACK *a, int b);
int q_SSL_session_reused(SSL *a);
unsigned long q_SSL_CTX_set_options(SSL_CTX *ctx, unsigned long op);
int q_OPENSSL_init_ssl(uint64_t opts, const OPENSSL_INIT_SETTINGS *settings);
size_t q_SSL_get_client_random(SSL *a, unsigned char *out, size_t outlen);
size_t q_SSL_SESSION_get_master_key(const SSL_SESSION *session, unsigned char *out, size_t outlen);
int q_CRYPTO_get_ex_new_index(int class_index, long argl, void *argp, CRYPTO_EX_new *new_func, CRYPTO_EX_dup *dup_func, CRYPTO_EX_free *free_func);
const SSL_METHOD *q_TLS_method();
const SSL_METHOD *q_TLS_client_method();
const SSL_METHOD *q_TLS_server_method();
ASN1_TIME *q_X509_getm_notBefore(X509 *a);
ASN1_TIME *q_X509_getm_notAfter(X509 *a);

Q_AUTOTEST_EXPORT void q_X509_up_ref(X509 *a);
long q_X509_get_version(X509 *a);
EVP_PKEY *q_X509_get_pubkey(X509 *a);
void q_X509_STORE_set_verify_cb(X509_STORE *ctx, X509_STORE_CTX_verify_cb verify_cb);
STACK_OF(X509) *q_X509_STORE_CTX_get0_chain(X509_STORE_CTX *ctx);
void q_DH_get0_pqg(const DH *dh, const BIGNUM **p, const BIGNUM **q, const BIGNUM **g);
int q_DH_bits(DH *dh);

# define q_SSL_load_error_strings() q_OPENSSL_init_ssl(OPENSSL_INIT_LOAD_SSL_STRINGS \
                                                       | OPENSSL_INIT_LOAD_CRYPTO_STRINGS, NULL)

#define q_SKM_sk_num(type, st) ((int (*)(const STACK_OF(type) *))q_OPENSSL_sk_num)(st)
#define q_SKM_sk_value(type, st,i) ((type * (*)(const STACK_OF(type) *, int))q_OPENSSL_sk_value)(st, i)

#define q_OPENSSL_add_all_algorithms_conf()  q_OPENSSL_init_crypto(OPENSSL_INIT_ADD_ALL_CIPHERS \
                                                                   | OPENSSL_INIT_ADD_ALL_DIGESTS \
                                                                   | OPENSSL_INIT_LOAD_CONFIG, NULL)
#define  q_OPENSSL_add_all_algorithms_noconf() q_OPENSSL_init_crypto(OPENSSL_INIT_ADD_ALL_CIPHERS \
                                                                    | OPENSSL_INIT_ADD_ALL_DIGESTS, NULL)

// We resolve q_sk_ functions, but use q_OPENSSL_sk_ macros in code to reduce
// the amount of #ifdefs and for confusing developers.
OPENSSL_STACK *q_sk_new_null();
#define q_OPENSSL_sk_new_null() q_sk_new_null()
#define q_sk_X509_EXTENSION_new_null() \
    ((OPENSSL_STACK *)q_sk_new_null())

void q_sk_push(OPENSSL_STACK *st, void *data);

#define q_sk_X509_EXTENSION_push(st, val) \
        q_OPENSSL_sk_push((st), (val))
#define q_sk_X509_EXTENSION_pop_free(st, free_func) \
              q_OPENSSL_sk_pop_free((st), (free_func))


int q_OPENSSL_init_crypto(uint64_t opts, const OPENSSL_INIT_SETTINGS *settings);
void q_CRYPTO_free(void *str, const char *file, int line);

long q_OpenSSL_version_num();
const char *q_OpenSSL_version(int type);

unsigned long q_SSL_SESSION_get_ticket_lifetime_hint(const SSL_SESSION *session);
unsigned long q_SSL_set_options(SSL *s, unsigned long op);

#ifdef TLS1_3_VERSION
int q_SSL_CTX_set_ciphersuites(SSL_CTX *ctx, const char *str);
#endif

void q_BIO_set_data(BIO *a, void *ptr);
void *q_BIO_get_data(BIO *a);
void q_BIO_set_init(BIO *a, int init);
int q_BIO_get_shutdown(BIO *a);
void q_BIO_set_shutdown(BIO *a, int shut);

EVP_PKEY_CTX* q_EVP_PKEY_CTX_new_id(int id, ENGINE *e);
void q_EVP_PKEY_CTX_free(EVP_PKEY_CTX *ctx);
int q_EVP_PKEY_keygen_init(EVP_PKEY_CTX *ctx);

int q_RSA_pkey_ctx_ctrl(EVP_PKEY_CTX *ctx, int optype, int cmd, int p1, void *p2);
#define q_EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, bits) \
    q_RSA_pkey_ctx_ctrl(ctx, EVP_PKEY_OP_KEYGEN, \
    EVP_PKEY_CTRL_RSA_KEYGEN_BITS, bits, NULL)

int q_EVP_PKEY_keygen(EVP_PKEY_CTX *ctx, EVP_PKEY **pkey);

#define q_SSL_CTX_set_min_proto_version(ctx, version) \
        q_SSL_CTX_ctrl(ctx, SSL_CTRL_SET_MIN_PROTO_VERSION, version, nullptr)

#define q_SSL_CTX_set_max_proto_version(ctx, version) \
        q_SSL_CTX_ctrl(ctx, SSL_CTRL_SET_MAX_PROTO_VERSION, version, nullptr)

#endif
