// Copyright (C) 2019 The Qt Company Ltd.
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

#include "openssl_symbols_p.h"
#include <QtCore/qurl.h>
#include <QtCore/qset.h>

#ifdef Q_OS_WIN
# include <private/qsystemlibrary_p.h>
#elif QT_CONFIG(library)
# include <QtCore/qlibrary.h>
#endif
#include <QtCore/qmutex.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qtimezone.h>
#if defined(Q_OS_UNIX)
#include <QtCore/qdir.h>
#endif
#include <memory>
#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
#include <link.h>
#endif
#ifdef Q_OS_DARWIN
#include "private/qcore_mac_p.h"
#endif

#include <algorithm>

QT_BEGIN_NAMESPACE

Q_LOGGING_CATEGORY(lcSsl, "qt.opcua.ssl");


/*
    Note to maintainer:
    -------------------

    We load OpenSSL symbols dynamically. Because symbols are known to
    disappear, and signatures sometimes change, between releases, we need to
    be careful about how this is done. To ensure we don't end up dereferencing
    null function pointers, and continue running even if certain functions are
    missing, we define helper functions for each of the symbols we load from
    OpenSSL, all prefixed with "q_" (declared in
    qsslsocket_openssl_symbols_p.h). So instead of calling SSL_connect
    directly, we call q_SSL_connect, which is a function that checks if the
    actual SSL_connect fptr is null, and returns a failure if it is, or calls
    SSL_connect if it isn't.

    This requires a somewhat tedious process of declaring each function we
    want to call in OpenSSL thrice: once with the q_, in _p.h, once using the
    DEFINEFUNC macros below, and once in the function that actually resolves
    the symbols, below the DEFINEFUNC declarations below.

    There's one DEFINEFUNC macro declared for every number of arguments
    exposed by OpenSSL (feel free to extend when needed). The easiest thing to
    do is to find an existing entry that matches the arg count of the function
    you want to import, and do the same.

    The first macro arg is the function return type. The second is the
    verbatim name of the function/symbol. Then follows a list of N pairs of
    argument types with a variable name, and just the variable name (char *a,
    a, char *b, b, etc). Finally there's two arguments - a suitable return
    statement for the error case (for an int function, return 0 or return -1
    is usually right). Then either just "return" or DUMMYARG, the latter being
    for void functions.

    Note: Take into account that these macros and declarations are processed
    at compile-time, and the result depends on the OpenSSL headers the
    compiling host has installed, but the symbols are resolved at run-time,
    possibly with a different version of OpenSSL.
*/

#ifndef QT_LINKED_OPENSSL

namespace {
void qsslSocketUnresolvedSymbolWarning(const char *functionName)
{
    qCWarning(lcSsl, "QSslSocket: cannot call unresolved function %s", functionName);
}

#if QT_CONFIG(library)
void qsslSocketCannotResolveSymbolWarning(const char *functionName)
{
    qCWarning(lcSsl, "QSslSocket: cannot resolve %s", functionName);
}
#endif

}

#endif // QT_LINKED_OPENSSL

#if QT_CONFIG(opensslv11) | QT_CONFIG(opensslv30)

// Below are the functions first introduced in version 1.1:

DEFINEFUNC(const unsigned char *, ASN1_STRING_get0_data, const ASN1_STRING *a, a, return nullptr, return)
DEFINEFUNC2(int, OPENSSL_init_ssl, uint64_t opts, opts, const OPENSSL_INIT_SETTINGS *settings, settings, return 0, return)
DEFINEFUNC2(int, OPENSSL_init_crypto, uint64_t opts, opts, const OPENSSL_INIT_SETTINGS *settings, settings, return 0, return)
DEFINEFUNC(BIO *, BIO_new, const BIO_METHOD *a, a, return nullptr, return)
DEFINEFUNC(const BIO_METHOD *, BIO_s_mem, void, DUMMYARG, return nullptr, return)
DEFINEFUNC2(int, BN_is_word, BIGNUM *a, a, BN_ULONG w, w, return 0, return)
DEFINEFUNC(int, EVP_CIPHER_CTX_reset, EVP_CIPHER_CTX *c, c, return 0, return)
DEFINEFUNC(int, EVP_PKEY_base_id, EVP_PKEY *a, a, return NID_undef, return)
DEFINEFUNC(int, RSA_bits, RSA *a, a, return 0, return)
DEFINEFUNC(int, DSA_bits, DSA *a, a, return 0, return)
DEFINEFUNC(int, OPENSSL_sk_num, OPENSSL_STACK *a, a, return -1, return)
DEFINEFUNC2(void, OPENSSL_sk_pop_free, OPENSSL_STACK *a, a, void (*b)(void*), b, return, DUMMYARG)
DEFINEFUNC(OPENSSL_STACK *, OPENSSL_sk_new_null, DUMMYARG, DUMMYARG, return nullptr, return)
DEFINEFUNC2(void, OPENSSL_sk_push, OPENSSL_STACK *a, a, void *b, b, return, DUMMYARG)
DEFINEFUNC(void, OPENSSL_sk_free, OPENSSL_STACK *a, a, return, DUMMYARG)
DEFINEFUNC2(void *, OPENSSL_sk_value, OPENSSL_STACK *a, a, int b, b, return nullptr, return)
DEFINEFUNC(int, SSL_session_reused, SSL *a, a, return 0, return)
DEFINEFUNC2(unsigned long, SSL_CTX_set_options, SSL_CTX *ctx, ctx, unsigned long op, op, return 0, return)
#ifdef TLS1_3_VERSION
DEFINEFUNC2(int, SSL_CTX_set_ciphersuites, SSL_CTX *ctx, ctx, const char *str, str, return 0, return)
#endif
DEFINEFUNC3(size_t, SSL_get_client_random, SSL *a, a, unsigned char *out, out, size_t outlen, outlen, return 0, return)
DEFINEFUNC3(size_t, SSL_SESSION_get_master_key, const SSL_SESSION *ses, ses, unsigned char *out, out, size_t outlen, outlen, return 0, return)
DEFINEFUNC6(int, CRYPTO_get_ex_new_index, int class_index, class_index, long argl, argl, void *argp, argp, CRYPTO_EX_new *new_func, new_func, CRYPTO_EX_dup *dup_func, dup_func, CRYPTO_EX_free *free_func, free_func, return -1, return)
DEFINEFUNC2(unsigned long, SSL_set_options, SSL *ssl, ssl, unsigned long op, op, return 0, return)

DEFINEFUNC(const SSL_METHOD *, TLS_method, DUMMYARG, DUMMYARG, return nullptr, return)
DEFINEFUNC(const SSL_METHOD *, TLS_client_method, DUMMYARG, DUMMYARG, return nullptr, return)
DEFINEFUNC(const SSL_METHOD *, TLS_server_method, DUMMYARG, DUMMYARG, return nullptr, return)
DEFINEFUNC(void, X509_up_ref, X509 *a, a, return, DUMMYARG)
DEFINEFUNC(ASN1_TIME *, X509_getm_notBefore, X509 *a, a, return nullptr, return)
DEFINEFUNC(ASN1_TIME *, X509_getm_notAfter, X509 *a, a, return nullptr, return)
DEFINEFUNC(long, X509_get_version, X509 *a, a, return -1, return)
DEFINEFUNC(EVP_PKEY *, X509_get_pubkey, X509 *a, a, return nullptr, return)
DEFINEFUNC2(void, X509_STORE_set_verify_cb, X509_STORE *a, a, X509_STORE_CTX_verify_cb verify_cb, verify_cb, return, DUMMYARG)
DEFINEFUNC(STACK_OF(X509) *, X509_STORE_CTX_get0_chain, X509_STORE_CTX *a, a, return nullptr, return)
DEFINEFUNC3(void, CRYPTO_free, void *str, str, const char *file, file, int line, line, return, DUMMYARG)
DEFINEFUNC(long, OpenSSL_version_num, void, DUMMYARG, return 0, return)
DEFINEFUNC(const char *, OpenSSL_version, int a, a, return nullptr, return)
DEFINEFUNC(unsigned long, SSL_SESSION_get_ticket_lifetime_hint, const SSL_SESSION *session, session, return 0, return)
DEFINEFUNC4(void, DH_get0_pqg, const DH *dh, dh, const BIGNUM **p, p, const BIGNUM **q, q, const BIGNUM **g, g, return, DUMMYARG)
DEFINEFUNC(int, DH_bits, DH *dh, dh, return 0, return)

DEFINEFUNC2(void, BIO_set_data, BIO *a, a, void *ptr, ptr, return, DUMMYARG)
DEFINEFUNC(void *, BIO_get_data, BIO *a, a, return nullptr, return)
DEFINEFUNC2(void, BIO_set_init, BIO *a, a, int init, init, return, DUMMYARG)
DEFINEFUNC(int, BIO_get_shutdown, BIO *a, a, return -1, return)
DEFINEFUNC2(void, BIO_set_shutdown, BIO *a, a, int shut, shut, return, DUMMYARG)


#else // QT_CONFIG(opensslv11) | QT_CONFIG(opensslv30)

// Functions below are either deprecated or removed in OpenSSL >= 1.1:

DEFINEFUNC(unsigned char *, ASN1_STRING_data, ASN1_STRING *a, a, return nullptr, return)

#ifdef SSLEAY_MACROS
DEFINEFUNC3(void *, ASN1_dup, i2d_of_void *a, a, d2i_of_void *b, b, char *c, c, return nullptr, return)
#endif
DEFINEFUNC2(BIO *, BIO_new_file, const char *filename, filename, const char *mode, mode, return nullptr, return)
DEFINEFUNC(void, ERR_clear_error, DUMMYARG, DUMMYARG, return, DUMMYARG)
DEFINEFUNC(BIO *, BIO_new, BIO_METHOD *a, a, return nullptr, return)
DEFINEFUNC(BIO_METHOD *, BIO_s_mem, void, DUMMYARG, return nullptr, return)
DEFINEFUNC(int, CRYPTO_num_locks, DUMMYARG, DUMMYARG, return 0, return)
DEFINEFUNC(void, CRYPTO_set_locking_callback, void (*a)(int, int, const char *, int), a, return, DUMMYARG)
DEFINEFUNC(void, CRYPTO_set_id_callback, unsigned long (*a)(), a, return, DUMMYARG)
DEFINEFUNC(void, CRYPTO_free, void *a, a, return, DUMMYARG)
DEFINEFUNC(unsigned long, ERR_peek_last_error, DUMMYARG, DUMMYARG, return 0, return)
DEFINEFUNC(void, ERR_free_strings, void, DUMMYARG, return, DUMMYARG)
DEFINEFUNC(void, EVP_CIPHER_CTX_cleanup, EVP_CIPHER_CTX *a, a, return, DUMMYARG)
DEFINEFUNC(void, EVP_CIPHER_CTX_init, EVP_CIPHER_CTX *a, a, return, DUMMYARG)

#ifdef SSLEAY_MACROS
DEFINEFUNC6(void *, PEM_ASN1_read_bio, d2i_of_void *a, a, const char *b, b, BIO *c, c, void **d, d, pem_password_cb *e, e, void *f, f, return nullptr, return)
DEFINEFUNC6(void *, PEM_ASN1_write_bio, d2i_of_void *a, a, const char *b, b, BIO *c, c, void **d, d, pem_password_cb *e, e, void *f, f, return nullptr, return)
#endif // SSLEAY_MACROS

DEFINEFUNC(int, sk_num, STACK *a, a, return -1, return)
DEFINEFUNC2(void, sk_pop_free, STACK *a, a, void (*b)(void*), b, return, DUMMYARG)

DEFINEFUNC(_STACK *, sk_new_null, DUMMYARG, DUMMYARG, return nullptr, return)
DEFINEFUNC2(void, sk_push, _STACK *a, a, void *b, b, return, DUMMYARG)
DEFINEFUNC(void, sk_free, _STACK *a, a, return, DUMMYARG)
DEFINEFUNC2(void *, sk_value, STACK *a, a, int b, b, return nullptr, return)

DEFINEFUNC(int, SSL_library_init, void, DUMMYARG, return -1, return)
DEFINEFUNC(void, SSL_load_error_strings, void, DUMMYARG, return, DUMMYARG)

#if OPENSSL_VERSION_NUMBER >= 0x10001000L
DEFINEFUNC5(int, SSL_get_ex_new_index, long argl, argl, void *argp, argp, CRYPTO_EX_new *new_func, new_func, CRYPTO_EX_dup *dup_func, dup_func, CRYPTO_EX_free *free_func, free_func, return -1, return)
#endif // OPENSSL_VERSION_NUMBER >= 0x10001000L

DEFINEFUNC(STACK_OF(X509) *, X509_STORE_CTX_get_chain, X509_STORE_CTX *a, a, return nullptr, return)

#ifdef SSLEAY_MACROS
DEFINEFUNC2(int, i2d_DSAPrivateKey, const DSA *a, a, unsigned char **b, b, return -1, return)
DEFINEFUNC2(int, i2d_RSAPrivateKey, const RSA *a, a, unsigned char **b, b, return -1, return)
#ifndef OPENSSL_NO_EC
DEFINEFUNC2(int, i2d_ECPrivateKey, const EC_KEY *a, a, unsigned char **b, b, return -1, return)
#endif
DEFINEFUNC3(RSA *, d2i_RSAPrivateKey, RSA **a, a, unsigned char **b, b, long c, c, return nullptr, return)
DEFINEFUNC3(DSA *, d2i_DSAPrivateKey, DSA **a, a, unsigned char **b, b, long c, c, return nullptr, return)
#ifndef OPENSSL_NO_EC
DEFINEFUNC3(EC_KEY *, d2i_ECPrivateKey, EC_KEY **a, a, unsigned char **b, b, long c, c, return nullptr, return)
#endif
#endif

DEFINEFUNC(char *, CONF_get1_default_config_file, DUMMYARG, DUMMYARG, return nullptr, return)
DEFINEFUNC(void, OPENSSL_add_all_algorithms_noconf, void, DUMMYARG, return, DUMMYARG)
DEFINEFUNC(void, OPENSSL_add_all_algorithms_conf, void, DUMMYARG, return, DUMMYARG)
DEFINEFUNC(long, SSLeay, void, DUMMYARG, return 0, return)
DEFINEFUNC(const char *, SSLeay_version, int a, a, return nullptr, return)
DEFINEFUNC(void, ERR_load_crypto_strings, void, DUMMYARG, return, return)

#endif // QT_CONFIG(opensslv11) | QT_CONFIG(opensslv30)

DEFINEFUNC(long, ASN1_INTEGER_get, ASN1_INTEGER *a, a, return 0, return)
DEFINEFUNC2(int, ASN1_INTEGER_cmp, const ASN1_INTEGER *a, a, const ASN1_INTEGER *b, b, return 1, return)
DEFINEFUNC(int, ASN1_STRING_length, ASN1_STRING *a, a, return 0, return)
DEFINEFUNC2(int, ASN1_STRING_to_UTF8, unsigned char **a, a, ASN1_STRING *b, b, return 0, return)
DEFINEFUNC4(long, BIO_ctrl, BIO *a, a, int b, b, long c, c, void *d, d, return -1, return)
DEFINEFUNC(int, BIO_free, BIO *a, a, return 0, return)
DEFINEFUNC(void, BIO_free_all, BIO *a, a, return, return)
DEFINEFUNC2(BIO *, BIO_new_mem_buf, void *a, a, int b, b, return nullptr, return)
DEFINEFUNC3(int, BIO_read, BIO *a, a, void *b, b, int c, c, return -1, return)

DEFINEFUNC3(int, BIO_write, BIO *a, a, const void *b, b, int c, c, return -1, return)
DEFINEFUNC(int, BN_num_bits, const BIGNUM *a, a, return 0, return)
DEFINEFUNC2(BN_ULONG, BN_mod_word, const BIGNUM *a, a, BN_ULONG w, w, return static_cast<BN_ULONG>(-1), return)
DEFINEFUNC2(int, BN_set_word, BIGNUM *a, a, BN_ULONG w, w, return 0, return)
DEFINEFUNC(BIGNUM *, BN_new, void, DUMMYARG, return nullptr, return)
DEFINEFUNC(void, BN_clear, BIGNUM *bignum, bignum, return, return)
DEFINEFUNC(void, BN_free, BIGNUM *bignum, bignum, return, return)
DEFINEFUNC(void, BN_clear_free, BIGNUM *bignum, bignum, return, return)
#ifndef OPENSSL_NO_EC
DEFINEFUNC(const EC_GROUP*, EC_KEY_get0_group, const EC_KEY* k, k, return nullptr, return)
DEFINEFUNC(int, EC_GROUP_get_degree, const EC_GROUP* g, g, return 0, return)
#endif
DEFINEFUNC(DSA *, DSA_new, DUMMYARG, DUMMYARG, return nullptr, return)
DEFINEFUNC(void, DSA_free, DSA *a, a, return, DUMMYARG)
DEFINEFUNC3(X509 *, d2i_X509, X509 **a, a, const unsigned char **b, b, long c, c, return nullptr, return)
DEFINEFUNC2(char *, ERR_error_string, unsigned long a, a, char *b, b, return nullptr, return)
DEFINEFUNC3(void, ERR_error_string_n, unsigned long e, e, char *b, b, size_t len, len, return, DUMMYARG)
DEFINEFUNC(unsigned long, ERR_get_error, DUMMYARG, DUMMYARG, return 0, return)
DEFINEFUNC(EVP_CIPHER_CTX *, EVP_CIPHER_CTX_new, void, DUMMYARG, return nullptr, return)
DEFINEFUNC(void, EVP_CIPHER_CTX_free, EVP_CIPHER_CTX *a, a, return, DUMMYARG)
DEFINEFUNC4(int, EVP_CIPHER_CTX_ctrl, EVP_CIPHER_CTX *ctx, ctx, int type, type, int arg, arg, void *ptr, ptr, return 0, return)
DEFINEFUNC2(int, EVP_CIPHER_CTX_set_key_length, EVP_CIPHER_CTX *ctx, ctx, int keylen, keylen, return 0, return)
DEFINEFUNC5(int, EVP_CipherInit, EVP_CIPHER_CTX *ctx, ctx, const EVP_CIPHER *type, type, const unsigned char *key, key, const unsigned char *iv, iv, int enc, enc, return 0, return)
DEFINEFUNC6(int, EVP_CipherInit_ex, EVP_CIPHER_CTX *ctx, ctx, const EVP_CIPHER *cipher, cipher, ENGINE *impl, impl, const unsigned char *key, key, const unsigned char *iv, iv, int enc, enc, return 0, return)
DEFINEFUNC5(int, EVP_CipherUpdate, EVP_CIPHER_CTX *ctx, ctx, unsigned char *out, out, int *outl, outl, const unsigned char *in, in, int inl, inl, return 0, return)
DEFINEFUNC3(int, EVP_CipherFinal, EVP_CIPHER_CTX *ctx, ctx, unsigned char *out, out, int *outl, outl, return 0, return)
DEFINEFUNC(const EVP_MD *, EVP_get_digestbyname, const char *name, name, return nullptr, return)
DEFINEFUNC(const EVP_MD *, EVP_sha1, DUMMYARG, DUMMYARG, return nullptr, return)
DEFINEFUNC(const EVP_MD *, EVP_sha256, DUMMYARG, DUMMYARG, return nullptr, return)
DEFINEFUNC(const EVP_CIPHER *, EVP_aes_256_gcm, DUMMYARG, DUMMYARG, return nullptr, return)
DEFINEFUNC(const EVP_CIPHER *, EVP_aes_128_cbc, DUMMYARG, DUMMYARG, return nullptr, return)
DEFINEFUNC3(int, EVP_PKEY_assign, EVP_PKEY *a, a, int b, b, char *c, c, return -1, return)
DEFINEFUNC2(int, EVP_PKEY_set1_RSA, EVP_PKEY *a, a, RSA *b, b, return -1, return)
DEFINEFUNC2(int, EVP_PKEY_set1_DSA, EVP_PKEY *a, a, DSA *b, b, return -1, return)
DEFINEFUNC2(int, EVP_PKEY_set1_DH, EVP_PKEY *a, a, DH *b, b, return -1, return)
#ifndef OPENSSL_NO_EC
DEFINEFUNC2(int, EVP_PKEY_set1_EC_KEY, EVP_PKEY *a, a, EC_KEY *b, b, return -1, return)
#endif
DEFINEFUNC(void, EVP_PKEY_free, EVP_PKEY *a, a, return, DUMMYARG)
DEFINEFUNC(DSA *, EVP_PKEY_get1_DSA, EVP_PKEY *a, a, return nullptr, return)
DEFINEFUNC(RSA *, EVP_PKEY_get1_RSA, EVP_PKEY *a, a, return nullptr, return)
DEFINEFUNC(DH *, EVP_PKEY_get1_DH, EVP_PKEY *a, a, return nullptr, return)
#ifndef OPENSSL_NO_EC
DEFINEFUNC(EC_KEY *, EVP_PKEY_get1_EC_KEY, EVP_PKEY *a, a, return nullptr, return)
#endif
DEFINEFUNC(EVP_PKEY *, EVP_PKEY_new, DUMMYARG, DUMMYARG, return nullptr, return)
DEFINEFUNC(int, EVP_PKEY_type, int a, a, return NID_undef, return)
DEFINEFUNC2(int, i2d_X509, X509 *a, a, unsigned char **b, b, return -1, return)
DEFINEFUNC(const char *, OBJ_nid2sn, int a, a, return nullptr, return)
DEFINEFUNC(const char *, OBJ_nid2ln, int a, a, return nullptr, return)
DEFINEFUNC(int, OBJ_sn2nid, const char *s, s, return 0, return)
DEFINEFUNC(int, OBJ_ln2nid, const char *s, s, return 0, return)
DEFINEFUNC3(int, i2t_ASN1_OBJECT, char *a, a, int b, b, ASN1_OBJECT *c, c, return -1, return)
DEFINEFUNC4(int, OBJ_obj2txt, char *a, a, int b, b, ASN1_OBJECT *c, c, int d, d, return -1, return)

DEFINEFUNC(int, OBJ_obj2nid, const ASN1_OBJECT *a, a, return NID_undef, return)
DEFINEFUNC4(X509_EXTENSION* , X509V3_EXT_conf_nid, LHASH_OF(CONF_VALUE) *conf, conf, X509V3_CTX *ctx, ctx, int ext_nid, ext_nid, char *value, value, return NULL, return)

#ifndef SSLEAY_MACROS
DEFINEFUNC4(EVP_PKEY *, PEM_read_bio_PrivateKey, BIO *a, a, EVP_PKEY **b, b, pem_password_cb *c, c, void *d, d, return nullptr, return)
DEFINEFUNC4(DSA *, PEM_read_bio_DSAPrivateKey, BIO *a, a, DSA **b, b, pem_password_cb *c, c, void *d, d, return nullptr, return)
DEFINEFUNC4(RSA *, PEM_read_bio_RSAPrivateKey, BIO *a, a, RSA **b, b, pem_password_cb *c, c, void *d, d, return nullptr, return)
#ifndef OPENSSL_NO_EC
DEFINEFUNC4(EC_KEY *, PEM_read_bio_ECPrivateKey, BIO *a, a, EC_KEY **b, b, pem_password_cb *c, c, void *d, d, return nullptr, return)
#endif
DEFINEFUNC4(DH *, PEM_read_bio_DHparams, BIO *a, a, DH **b, b, pem_password_cb *c, c, void *d, d, return nullptr, return)
DEFINEFUNC7(int, PEM_write_bio_DSAPrivateKey, BIO *a, a, DSA *b, b, const EVP_CIPHER *c, c, unsigned char *d, d, int e, e, pem_password_cb *f, f, void *g, g, return 0, return)
DEFINEFUNC7(int, PEM_write_bio_RSAPrivateKey, BIO *a, a, RSA *b, b, const EVP_CIPHER *c, c, unsigned char *d, d, int e, e, pem_password_cb *f, f, void *g, g, return 0, return)
DEFINEFUNC7(int, PEM_write_bio_PrivateKey, BIO *a, a, EVP_PKEY *b, b, const EVP_CIPHER *c, c, unsigned char *d, d, int e, e, pem_password_cb *f, f, void *g, g, return 0, return)
DEFINEFUNC7(int, PEM_write_bio_PKCS8PrivateKey, BIO *a, a, EVP_PKEY *b, b, const EVP_CIPHER *c, c, char *d, d, int e, e, pem_password_cb *f, f, void *g, g, return 0, return)
#ifndef OPENSSL_NO_EC
DEFINEFUNC7(int, PEM_write_bio_ECPrivateKey, BIO *a, a, EC_KEY *b, b, const EVP_CIPHER *c, c, unsigned char *d, d, int e, e, pem_password_cb *f, f, void *g, g, return 0, return)
#endif
#endif // !SSLEAY_MACROS
DEFINEFUNC4(EVP_PKEY *, PEM_read_bio_PUBKEY, BIO *a, a, EVP_PKEY **b, b, pem_password_cb *c, c, void *d, d, return nullptr, return)
DEFINEFUNC4(DSA *, PEM_read_bio_DSA_PUBKEY, BIO *a, a, DSA **b, b, pem_password_cb *c, c, void *d, d, return nullptr, return)
DEFINEFUNC4(RSA *, PEM_read_bio_RSA_PUBKEY, BIO *a, a, RSA **b, b, pem_password_cb *c, c, void *d, d, return nullptr, return)
#ifndef OPENSSL_NO_EC
DEFINEFUNC4(EC_KEY *, PEM_read_bio_EC_PUBKEY, BIO *a, a, EC_KEY **b, b, pem_password_cb *c, c, void *d, d, return nullptr, return)
#endif
DEFINEFUNC2(int, PEM_write_bio_DSA_PUBKEY, BIO *a, a, DSA *b, b, return 0, return)
DEFINEFUNC2(int, PEM_write_bio_RSA_PUBKEY, BIO *a, a, RSA *b, b, return 0, return)
DEFINEFUNC2(int, PEM_write_bio_PUBKEY, BIO *a, a, EVP_PKEY *b, b, return 0, return)
#ifndef OPENSSL_NO_EC
DEFINEFUNC2(int, PEM_write_bio_EC_PUBKEY, BIO *a, a, EC_KEY *b, b, return 0, return)
#endif
DEFINEFUNC2(void, RAND_seed, const void *a, a, int b, b, return, DUMMYARG)
DEFINEFUNC(int, RAND_status, void, DUMMYARG, return -1, return)
DEFINEFUNC2(int, RAND_bytes, unsigned char *b, b, int n, n, return 0, return)
DEFINEFUNC(RSA *, RSA_new, DUMMYARG, DUMMYARG, return nullptr, return)
DEFINEFUNC4(int, RSA_generate_key_ex, RSA *rsa, rsa, int bits, bits, BIGNUM *e, e, BN_GENCB *cb, cb, return 0, return)
DEFINEFUNC(void, RSA_free, RSA *a, a, return, DUMMYARG)
DEFINEFUNC(X509_STORE *, SSL_CTX_get_cert_store, const SSL_CTX *a, a, return nullptr, return)
DEFINEFUNC2(int, X509_cmp, X509 *a, a, X509 *b, b, return -1, return)
DEFINEFUNC4(int, X509_digest, const X509 *x509, x509, const EVP_MD *type, type, unsigned char *md, md, unsigned int *len, len, return -1, return)
#ifndef SSLEAY_MACROS
DEFINEFUNC(X509 *, X509_dup, X509 *a, a, return nullptr, return)
#endif
DEFINEFUNC2(void, X509_print, BIO *a, a, X509 *b, b, return, DUMMYARG);
DEFINEFUNC(ASN1_OBJECT *, X509_EXTENSION_get_object, X509_EXTENSION *a, a, return nullptr, return)
DEFINEFUNC(void, X509_free, X509 *a, a, return, DUMMYARG)
//Q_AUTOTEST_EXPORT ASN1_TIME *q_X509_gmtime_adj(ASN1_TIME *s, long adj);
DEFINEFUNC2(ASN1_TIME *, X509_gmtime_adj, ASN1_TIME *s, s, long adj, adj, return nullptr, return)
DEFINEFUNC(void, ASN1_TIME_free, ASN1_TIME *t, t, return, DUMMYARG)
DEFINEFUNC2(X509_EXTENSION *, X509_get_ext, X509 *a, a, int b, b, return nullptr, return)
DEFINEFUNC(int, X509_get_ext_count, X509 *a, a, return 0, return)
DEFINEFUNC4(void *, X509_get_ext_d2i, X509 *a, a, int b, b, int *c, c, int *d, d, return nullptr, return)
DEFINEFUNC(const X509V3_EXT_METHOD *, X509V3_EXT_get, X509_EXTENSION *a, a, return nullptr, return)
DEFINEFUNC(void *, X509V3_EXT_d2i, X509_EXTENSION *a, a, return nullptr, return)
DEFINEFUNC(int, X509_EXTENSION_get_critical, X509_EXTENSION *a, a, return 0, return)
DEFINEFUNC(ASN1_OCTET_STRING *, X509_EXTENSION_get_data, X509_EXTENSION *a, a, return nullptr, return)
DEFINEFUNC(void, BASIC_CONSTRAINTS_free, BASIC_CONSTRAINTS *a, a, return, DUMMYARG)
DEFINEFUNC(void, AUTHORITY_KEYID_free, AUTHORITY_KEYID *a, a, return, DUMMYARG)
DEFINEFUNC(void, GENERAL_NAME_free, GENERAL_NAME *a, a, return, DUMMYARG)
DEFINEFUNC2(int, ASN1_STRING_print, BIO *a, a, const ASN1_STRING *b, b, return 0, return)
DEFINEFUNC2(int, X509_check_issued, X509 *a, a, X509 *b, b, return -1, return)
DEFINEFUNC(X509_NAME *, X509_get_issuer_name, X509 *a, a, return nullptr, return)
DEFINEFUNC(X509_NAME *, X509_get_subject_name, X509 *a, a, return nullptr, return)
DEFINEFUNC(ASN1_INTEGER *, X509_get_serialNumber, X509 *a, a, return nullptr, return)
DEFINEFUNC(int, X509_verify_cert, X509_STORE_CTX *a, a, return -1, return)
DEFINEFUNC(int, X509_NAME_entry_count, X509_NAME *a, a, return 0, return)
DEFINEFUNC2(X509_NAME_ENTRY *, X509_NAME_get_entry, X509_NAME *a, a, int b, b, return nullptr, return)
DEFINEFUNC(ASN1_STRING *, X509_NAME_ENTRY_get_data, X509_NAME_ENTRY *a, a, return nullptr, return)
DEFINEFUNC(ASN1_OBJECT *, X509_NAME_ENTRY_get_object, X509_NAME_ENTRY *a, a, return nullptr, return)
DEFINEFUNC(EVP_PKEY *, X509_PUBKEY_get, X509_PUBKEY *a, a, return nullptr, return)
DEFINEFUNC(void, X509_STORE_free, X509_STORE *a, a, return, DUMMYARG)
DEFINEFUNC(X509_STORE *, X509_STORE_new, DUMMYARG, DUMMYARG, return nullptr, return)
DEFINEFUNC2(int, X509_STORE_add_cert, X509_STORE *a, a, X509 *b, b, return 0, return)
DEFINEFUNC(void, X509_STORE_CTX_free, X509_STORE_CTX *a, a, return, DUMMYARG)
DEFINEFUNC4(int, X509_STORE_CTX_init, X509_STORE_CTX *a, a, X509_STORE *b, b, X509 *c, c, STACK_OF(X509) *d, d, return -1, return)
DEFINEFUNC2(int, X509_STORE_CTX_set_purpose, X509_STORE_CTX *a, a, int b, b, return -1, return)
DEFINEFUNC(int, X509_STORE_CTX_get_error, X509_STORE_CTX *a, a, return -1, return)
DEFINEFUNC(int, X509_STORE_CTX_get_error_depth, X509_STORE_CTX *a, a, return -1, return)
DEFINEFUNC(X509 *, X509_STORE_CTX_get_current_cert, X509_STORE_CTX *a, a, return nullptr, return)
DEFINEFUNC(X509_STORE_CTX *, X509_STORE_CTX_new, DUMMYARG, DUMMYARG, return nullptr, return)
DEFINEFUNC2(void *, X509_STORE_CTX_get_ex_data, X509_STORE_CTX *ctx, ctx, int idx, idx, return nullptr, return)
DEFINEFUNC(int, SSL_get_ex_data_X509_STORE_CTX_idx, DUMMYARG, DUMMYARG, return -1, return)
DEFINEFUNC3(int, SSL_CTX_load_verify_locations, SSL_CTX *ctx, ctx, const char *CAfile, CAfile, const char *CApath, CApath, return 0, return)
DEFINEFUNC2(int, i2d_SSL_SESSION, SSL_SESSION *in, in, unsigned char **pp, pp, return 0, return)
DEFINEFUNC3(SSL_SESSION *, d2i_SSL_SESSION, SSL_SESSION **a, a, const unsigned char **pp, pp, long length, length, return nullptr, return)
#if OPENSSL_VERSION_NUMBER >= 0x1000100fL && !defined(OPENSSL_NO_NEXTPROTONEG)
DEFINEFUNC6(int, SSL_select_next_proto, unsigned char **out, out, unsigned char *outlen, outlen,
            const unsigned char *in, in, unsigned int inlen, inlen,
            const unsigned char *client, client, unsigned int client_len, client_len,
            return -1, return)
DEFINEFUNC3(void, SSL_CTX_set_next_proto_select_cb, SSL_CTX *s, s,
            int (*cb) (SSL *ssl, unsigned char **out,
                       unsigned char *outlen,
                       const unsigned char *in,
                       unsigned int inlen, void *arg), cb,
            void *arg, arg, return, DUMMYARG)
DEFINEFUNC3(void, SSL_get0_next_proto_negotiated, const SSL *s, s,
            const unsigned char **data, data, unsigned *len, len, return, DUMMYARG)
#if OPENSSL_VERSION_NUMBER >= 0x10002000L
DEFINEFUNC3(int, SSL_set_alpn_protos, SSL *s, s, const unsigned char *protos, protos,
            unsigned protos_len, protos_len, return -1, return)
DEFINEFUNC3(void, SSL_CTX_set_alpn_select_cb, SSL_CTX *s, s,
            int (*cb) (SSL *ssl, const unsigned char **out,
                       unsigned char *outlen,
                       const unsigned char *in,
                       unsigned int inlen, void *arg), cb,
            void *arg, arg, return, DUMMYARG)
DEFINEFUNC3(void, SSL_get0_alpn_selected, const SSL *s, s, const unsigned char **data, data,
            unsigned *len, len, return, DUMMYARG)
#endif // OPENSSL_VERSION_NUMBER >= 0x10002000L ...
#endif // OPENSSL_VERSION_NUMBER >= 0x1000100fL ...

DEFINEFUNC2(void, BIO_set_flags, BIO *b, b, int flags, flags, return, DUMMYARG)
DEFINEFUNC2(void, BIO_clear_flags, BIO *b, b, int flags, flags, return, DUMMYARG)
DEFINEFUNC2(void *, BIO_get_ex_data, BIO *b, b, int idx, idx, return nullptr, return)
DEFINEFUNC3(int, BIO_set_ex_data, BIO *b, b, int idx, idx, void *data, data, return -1, return)

DEFINEFUNC3(void *, CRYPTO_malloc, size_t num, num, const char *file, file, int line, line, return nullptr, return)
DEFINEFUNC(DH *, DH_new, DUMMYARG, DUMMYARG, return nullptr, return)
DEFINEFUNC(void, DH_free, DH *dh, dh, return, DUMMYARG)
DEFINEFUNC3(DH *, d2i_DHparams, DH**a, a, const unsigned char **pp, pp, long length, length, return nullptr, return)
DEFINEFUNC2(int, i2d_DHparams, DH *a, a, unsigned char **p, p, return -1, return)
DEFINEFUNC2(int, DH_check, DH *dh, dh, int *codes, codes, return 0, return)
DEFINEFUNC3(BIGNUM *, BN_bin2bn, const unsigned char *s, s, int len, len, BIGNUM *ret, ret, return nullptr, return)
#ifndef OPENSSL_NO_EC
DEFINEFUNC(EC_KEY *, EC_KEY_dup, const EC_KEY *ec, ec, return nullptr, return)
DEFINEFUNC(EC_KEY *, EC_KEY_new_by_curve_name, int nid, nid, return nullptr, return)
DEFINEFUNC(void, EC_KEY_free, EC_KEY *ecdh, ecdh, return, DUMMYARG)
DEFINEFUNC2(size_t, EC_get_builtin_curves, EC_builtin_curve * r, r, size_t nitems, nitems, return 0, return)
#if OPENSSL_VERSION_NUMBER >= 0x10002000L
DEFINEFUNC(int, EC_curve_nist2nid, const char *name, name, return 0, return)
#endif // OPENSSL_VERSION_NUMBER >= 0x10002000L
#endif // OPENSSL_NO_EC

DEFINEFUNC5(int, PKCS12_parse, PKCS12 *p12, p12, const char *pass, pass, EVP_PKEY **pkey, pkey, \
            X509 **cert, cert, STACK_OF(X509) **ca, ca, return 1, return);
DEFINEFUNC2(PKCS12 *, d2i_PKCS12_bio, BIO *bio, bio, PKCS12 **pkcs12, pkcs12, return nullptr, return);
DEFINEFUNC(void, PKCS12_free, PKCS12 *pkcs12, pkcs12, return, DUMMYARG)

DEFINEFUNC(X509_REQ*, X509_REQ_new, void, DUMMYARG, return NULL, return);
DEFINEFUNC(void, X509_REQ_free, X509_REQ *req, req, return, return);
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
DEFINEFUNC(X509_NAME*, X509_REQ_get_subject_name, X509_REQ *req, req, return NULL, return);
DEFINEFUNC2(EVP_PKEY_CTX*, EVP_PKEY_CTX_new_id, int id, id, ENGINE *e, e, return NULL, return);
DEFINEFUNC(void, EVP_PKEY_CTX_free, EVP_PKEY_CTX *ctx, ctx, return, return);
DEFINEFUNC(int, EVP_PKEY_keygen_init, EVP_PKEY_CTX *ctx, ctx, return 0, return);
DEFINEFUNC2(int, EVP_PKEY_keygen, EVP_PKEY_CTX *ctx, ctx, EVP_PKEY **pkey, pkey, return 0, return);
DEFINEFUNC5(int, RSA_pkey_ctx_ctrl, EVP_PKEY_CTX *ctx, ctx, int optype, optype, int cmd, cmd, int p1, p1, void *p2, p2, return 0, return)

#endif
DEFINEFUNC7(int, X509_NAME_add_entry_by_txt, X509_NAME *name, name, const char *field, field, int type, type, const unsigned char *bytes, bytes, int len, len, int loc, loc, int set, set, return 0, return);
DEFINEFUNC7(int, X509_NAME_add_entry_by_OBJ, X509_NAME *name, name, const ASN1_OBJECT *obj, obj, int type, type, const unsigned char *bytes, bytes, int len, len, int loc, loc, int set, set, return 0, return);
DEFINEFUNC2(ASN1_OBJECT *, OBJ_txt2obj, const char *s, s, int no_name, no_name, return NULL, return);
DEFINEFUNC2(int, X509_REQ_set_pubkey, X509_REQ *x, x, EVP_PKEY *pkey, pkey, return 0, return);
DEFINEFUNC3(int, X509_REQ_sign, X509_REQ *x, x, EVP_PKEY *pkey, pkey, const EVP_MD *md, md, return 0, return);
DEFINEFUNC2(int, PEM_write_bio_X509_REQ, BIO *bp, bp, X509_REQ *x, x, return 0, return);
DEFINEFUNC2(int, PEM_write_bio_X509_REQ_NEW, BIO *bp, bp, X509_REQ *x, x, return 0, return);
DEFINEFUNC2(int, X509_REQ_set_version, X509_REQ *x, x, long version, version, return 0, return);
DEFINEFUNC2(int, X509_REQ_add_extensions, X509_REQ *req, req, STACK_OF(X509_EXTENSION) *exts, exts, return 0, return)
DEFINEFUNC(void, X509_EXTENSION_free, X509_EXTENSION *ext, ext, return, return)
DEFINEFUNC2(int, X509_EXTENSION_set_critical, X509_EXTENSION *ex, ex, int crit, crit, return 0, return)
DEFINEFUNC3(X509*, X509_REQ_to_X509, X509_REQ *r, r, int days, days, EVP_PKEY *pkey, pkey, return NULL, return)
DEFINEFUNC2(int, PEM_write_bio_X509, BIO *bp, bp, X509 *x, x, return 0, return)
DEFINEFUNC(X509*, X509_new, void, DUMMYARG, return NULL, return)
DEFINEFUNC2(int, ASN1_INTEGER_set, ASN1_INTEGER *a, a, long v, v, return 0, return)
DEFINEFUNC2(int, X509_set_pubkey,X509 *x, x, EVP_PKEY *key, key, return 0, return)
DEFINEFUNC2(int, X509_set_issuer_name, X509 *x, x, X509_NAME *name, name, return 0, return)
DEFINEFUNC3(int, X509_sign, X509 *x, x, EVP_PKEY *key, key, const EVP_MD *md, md, return 0, return)
DEFINEFUNC3(int, X509_add_ext, X509 *x, x, X509_EXTENSION *ex, ex, int loc, loc, return 0, return)
DEFINEFUNC2(int, X509_set_version, X509 *x, x, long version, version, return 0, return)
DEFINEFUNC2(int, X509_set_subject_name, X509 *x, x, X509_NAME *name, name, return 0, return)
DEFINEFUNC(ASN1_OCTET_STRING *, ASN1_OCTET_STRING_new, void, DUMMYARG, return NULL, return)
DEFINEFUNC4(int, X509_pubkey_digest, const X509 *data, data, const EVP_MD *type, type, unsigned char *md, md, unsigned int *len, len, return 0, return)
DEFINEFUNC3(int, ASN1_OCTET_STRING_set, ASN1_OCTET_STRING *str, str, const unsigned char *data, data, int len, len, return 0, return)
DEFINEFUNC5(int, X509_add1_ext_i2d, X509 *x, x, int nid, nid, void *value, value, int crit, crit, unsigned long flags, flags, return 0, return)
DEFINEFUNC(void, ASN1_OCTET_STRING_free, ASN1_OCTET_STRING *a, a, return, return)
DEFINEFUNC(ASN1_INTEGER *, ASN1_INTEGER_new, void, DUMMYARG, return NULL, return)
DEFINEFUNC(GENERAL_NAMES *, GENERAL_NAMES_new, void, DUMMYARG, return NULL, return)
DEFINEFUNC(GENERAL_NAME *, GENERAL_NAME_new, void, DUMMYARG, return NULL, return)
DEFINEFUNC(X509_NAME *, X509_NAME_dup, X509_NAME *xn, xn, return NULL, return)
DEFINEFUNC2(int, X509_set_serialNumber, X509 *x, x, ASN1_INTEGER *serial, serial, return 0, return)
DEFINEFUNC(AUTHORITY_KEYID *, AUTHORITY_KEYID_new, void, DUMMYARG, return NULL, return)
DEFINEFUNC(ASN1_INTEGER *, ASN1_INTEGER_dup, const ASN1_INTEGER *x, x, return NULL, return)
DEFINEFUNC4(int, X509_NAME_digest, const X509_NAME *data, data, const EVP_MD *type, type, unsigned char *md, md, unsigned int *len, len, return 0, return)
DEFINEFUNC(void, ASN1_INTEGER_free, ASN1_INTEGER *a, a, return, return)
DEFINEFUNC2(int, i2d_X509_REQ_bio, BIO *bp, bp, X509_REQ *req, req, return 0, return)
DEFINEFUNC2(int, i2d_X509_bio, BIO *bp, bp, X509 *x509, x509, return 0, return)

#define RESOLVEFUNC(func) \
    if (!(_q_##func = _q_PTR_##func(libs.ssl->resolve(#func)))     \
        && !(_q_##func = _q_PTR_##func(libs.crypto->resolve(#func)))) \
        qsslSocketCannotResolveSymbolWarning(#func);

#if !defined QT_LINKED_OPENSSL

#if !QT_CONFIG(library)
bool q_resolveOpenSslSymbols()
{
    qCWarning(lcSsl, "QSslSocket: unable to resolve symbols. Qt is configured without the "
                     "'library' feature, which means runtime resolving of libraries won't work.");
    qCWarning(lcSsl, "Either compile Qt statically or with support for runtime resolving "
                     "of libraries.");
    return false;
}
#else

# ifdef Q_OS_UNIX
struct NumericallyLess
{
    typedef bool result_type;
    result_type operator()(const QStringView &lhs, const QStringView &rhs) const
    {
        bool ok = false;
        int b = 0;
        int a = lhs.toInt(&ok);
        if (ok)
            b = rhs.toInt(&ok);
        if (ok) {
            // both toInt succeeded
            return a < b;
        } else {
            // compare as strings;
            return lhs < rhs;
        }
    }
};

struct LibGreaterThan
{
    typedef bool result_type;
    result_type operator()(const QString &lhs, const QString &rhs) const
    {
        const QList<QStringView> lhsparts = QStringView{lhs}.split(QLatin1Char('.'));
        const QList<QStringView> rhsparts = QStringView{rhs}.split(QLatin1Char('.'));
        Q_ASSERT(lhsparts.size() > 1 && rhsparts.size() > 1);

        // note: checking rhs < lhs, the same as lhs > rhs
        return std::lexicographical_compare(rhsparts.begin() + 1, rhsparts.end(),
                                            lhsparts.begin() + 1, lhsparts.end(),
                                            NumericallyLess());
    }
};

#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
static int dlIterateCallback(struct dl_phdr_info *info, size_t size, void *data)
{
    if (size < sizeof (info->dlpi_addr) + sizeof (info->dlpi_name))
        return 1;
    QSet<QString> *paths = (QSet<QString> *)data;
    QString path = QString::fromLocal8Bit(info->dlpi_name);
    if (!path.isEmpty()) {
        QFileInfo fi(path);
        path = fi.absolutePath();
        if (!path.isEmpty())
            paths->insert(path);
    }
    return 0;
}
#endif

static QStringList libraryPathList()
{
    QStringList paths;
#  ifdef Q_OS_DARWIN
    paths = QString::fromLatin1(qgetenv("DYLD_LIBRARY_PATH"))
            .split(QLatin1Char(':'), QString::SkipEmptyParts);

    // search in .app/Contents/Frameworks
    UInt32 packageType;
    CFBundleGetPackageInfo(CFBundleGetMainBundle(), &packageType, nullptr);
    if (packageType == FOUR_CHAR_CODE('APPL')) {
        QUrl bundleUrl = QUrl::fromCFURL(QCFType<CFURLRef>(CFBundleCopyBundleURL(CFBundleGetMainBundle())));
        QUrl frameworksUrl = QUrl::fromCFURL(QCFType<CFURLRef>(CFBundleCopyPrivateFrameworksURL(CFBundleGetMainBundle())));
        paths << bundleUrl.resolved(frameworksUrl).path();
    }
#  else
    paths = QString::fromLatin1(qgetenv("LD_LIBRARY_PATH"))
            .split(QLatin1Char(':'), Qt::SkipEmptyParts);
#  endif
    paths << QLatin1String("/lib") << QLatin1String("/usr/lib") << QLatin1String("/usr/local/lib");
    paths << QLatin1String("/lib64") << QLatin1String("/usr/lib64") << QLatin1String("/usr/local/lib64");
    paths << QLatin1String("/lib32") << QLatin1String("/usr/lib32") << QLatin1String("/usr/local/lib32");

#if defined(Q_OS_ANDROID)
    paths << QLatin1String("/system/lib");
#elif defined(Q_OS_LINUX)
    // discover paths of already loaded libraries
    QSet<QString> loadedPaths;
    dl_iterate_phdr(dlIterateCallback, &loadedPaths);
    paths.append(loadedPaths.values());
#endif

    return paths;
}

Q_NEVER_INLINE
static QStringList findAllLibs(QLatin1String filter)
{
    const QStringList paths = libraryPathList();
    QStringList found;
    const QStringList filters((QString(filter)));

    for (const QString &path : paths) {
        QDir dir(path);
        QStringList entryList = dir.entryList(filters, QDir::Files);

        std::sort(entryList.begin(), entryList.end(), LibGreaterThan());
        for (const QString &entry : std::as_const(entryList))
            found << path + QLatin1Char('/') + entry;
    }

    return found;
}

static QStringList findAllLibSsl()
{
    return findAllLibs(QLatin1String("libssl.*"));
}

static QStringList findAllLibCrypto()
{
    return findAllLibs(QLatin1String("libcrypto.*"));
}
# endif

#ifdef Q_OS_WIN

struct LoadedOpenSsl {
    std::unique_ptr<QSystemLibrary> ssl, crypto;
};

static bool tryToLoadOpenSslWin32Library(QLatin1String ssleay32LibName, QLatin1String libeay32LibName, LoadedOpenSsl &result)
{
    auto ssleay32 = std::make_unique<QSystemLibrary>(ssleay32LibName);
    if (!ssleay32->load(false)) {
        return FALSE;
    }

    auto libeay32 = std::make_unique<QSystemLibrary>(libeay32LibName);
    if (!libeay32->load(false)) {
        return FALSE;
    }

    result.ssl = std::move(ssleay32);
    result.crypto = std::move(libeay32);
    return TRUE;
}

static LoadedOpenSsl loadOpenSsl()
{
    LoadedOpenSsl result;

#if QT_CONFIG(opensslv11) | QT_CONFIG(opensslv30)
    // With OpenSSL 1.1 the names have changed to libssl-1_1(-x64) and libcrypto-1_1(-x64), for builds using
    // MSVC and GCC, (-x64 suffix for 64-bit builds).

#ifdef Q_PROCESSOR_X86_64
#define QT_SSL_SUFFIX "-x64"
#else // !Q_PROCESSOFR_X86_64
#define QT_SSL_SUFFIX
#endif // !Q_PROCESSOR_x86_64

#if QT_CONFIG(opensslv11)
    tryToLoadOpenSslWin32Library(QLatin1String("libssl-1_1" QT_SSL_SUFFIX),
                                 QLatin1String("libcrypto-1_1" QT_SSL_SUFFIX), result);
#elif QT_CONFIG(opensslv30)
    tryToLoadOpenSslWin32Library(QLatin1String("libssl-3" QT_SSL_SUFFIX),
                                 QLatin1String("libcrypto-3" QT_SSL_SUFFIX), result);
#endif

#undef QT_SSL_SUFFIX

#else // QT_CONFIG(opensslv11) | QT_CONFIG(opensslv30)

    // When OpenSSL is built using MSVC then the libraries are named 'ssleay32.dll' and 'libeay32'dll'.
    // When OpenSSL is built using GCC then different library names are used (depending on the OpenSSL version)
    // The oldest version of a GCC-based OpenSSL which can be detected by the code below is 0.9.8g (released in 2007)
    if (!tryToLoadOpenSslWin32Library(QLatin1String("ssleay32"), QLatin1String("libeay32"), result)) {
        if (!tryToLoadOpenSslWin32Library(QLatin1String("libssl-10"), QLatin1String("libcrypto-10"), result)) {
            if (!tryToLoadOpenSslWin32Library(QLatin1String("libssl-8"), QLatin1String("libcrypto-8"), result)) {
                tryToLoadOpenSslWin32Library(QLatin1String("libssl-7"), QLatin1String("libcrypto-7"), result);
            }
        }
    }
#endif // !QT_CONFIG(opensslv11) | QT_CONFIG(opensslv30)

    return result;
}
#else

struct LoadedOpenSsl {
    std::unique_ptr<QLibrary> ssl, crypto;
};

static LoadedOpenSsl loadOpenSsl()
{
    LoadedOpenSsl result = {std::make_unique<QLibrary>(), std::make_unique<QLibrary>()};

# if defined(Q_OS_UNIX)
    QLibrary * const libssl = result.ssl.get();
    QLibrary * const libcrypto = result.crypto.get();

    // Try to find the libssl library on the system.
    //
    // Up until Qt 4.3, this only searched for the "ssl" library at version -1, that
    // is, libssl.so on most Unix systems.  However, the .so file isn't present in
    // user installations because it's considered a development file.
    //
    // The right thing to do is to load the library at the major version we know how
    // to work with: the SHLIB_VERSION_NUMBER version (macro defined in opensslv.h)
    //
    // However, OpenSSL is a well-known case of binary-compatibility breakage. To
    // avoid such problems, many system integrators and Linux distributions change
    // the soname of the binary, letting the full version number be the soname. So
    // we'll find libssl.so.0.9.7, libssl.so.0.9.8, etc. in the system. For that
    // reason, we will search a few common paths (see findAllLibSsl() above) in hopes
    // we find one that works.
    //
    // If that fails, for OpenSSL 1.0 we also try some fallbacks -- look up
    // libssl.so with a hardcoded soname. The reason is QTBUG-68156: the binary
    // builds of Qt happen (at the time of this writing) on RHEL machines,
    // which change SHLIB_VERSION_NUMBER to a non-portable string. When running
    // those binaries on the target systems, this code won't pick up
    // libssl.so.MODIFIED_SHLIB_VERSION_NUMBER because it doesn't exist there.
    // Given that the only 1.0 supported release (at the time of this writing)
    // is 1.0.2, with soname "1.0.0", give that a try too. Note that we mandate
    // OpenSSL >= 1.0.0 with a configure-time check, and OpenSSL has kept binary
    // compatibility between 1.0.0 and 1.0.2.
    //
    // It is important, however, to try the canonical name and the unversioned name
    // without going through the loop. By not specifying a path, we let the system
    // dlopen(3) function determine it for us. This will include any DT_RUNPATH or
    // DT_RPATH tags on our library header as well as other system-specific search
    // paths. See the man page for dlopen(3) on your system for more information.

#ifdef Q_OS_OPENBSD
    libcrypto->setLoadHints(QLibrary::ExportExternalSymbolsHint);
#endif
#if defined(SHLIB_VERSION_NUMBER) && !defined(Q_OS_QNX) // on QNX, the libs are always libssl.so and libcrypto.so
    // first attempt: the canonical name is libssl.so.<SHLIB_VERSION_NUMBER>
    libssl->setFileNameAndVersion(QLatin1String("ssl"), QLatin1String(SHLIB_VERSION_NUMBER));
    libcrypto->setFileNameAndVersion(QLatin1String("crypto"), QLatin1String(SHLIB_VERSION_NUMBER));
    if (libcrypto->load() && libssl->load()) {
        // libssl.so.<SHLIB_VERSION_NUMBER> and libcrypto.so.<SHLIB_VERSION_NUMBER> found
        return result;
    } else {
        libssl->unload();
        libcrypto->unload();
    }

#if !QT_CONFIG(opensslv11) | QT_CONFIG(opensslv30)
    // first-and-half attempts: for OpenSSL 1.0 try to load some hardcoded sonames:
    // - "1.0.0" is the official upstream one
    // - "1.0.2" is found on some distributions (e.g. Debian) that patch OpenSSL
    static const QLatin1String fallbackSonames[] = {
        QLatin1String("1.0.0"),
        QLatin1String("1.0.2")
    };

    for (auto fallbackSoname : fallbackSonames) {
        libssl->setFileNameAndVersion(QLatin1String("ssl"), fallbackSoname);
        libcrypto->setFileNameAndVersion(QLatin1String("crypto"), fallbackSoname);
        if (libcrypto->load() && libssl->load()) {
            return result;
        } else {
            libssl->unload();
            libcrypto->unload();
        }
    }
#endif
#endif

#ifndef Q_OS_DARWIN
    // second attempt: find the development files libssl.so and libcrypto.so
    //
    // disabled on macOS/iOS:
    //  macOS's /usr/lib/libssl.dylib, /usr/lib/libcrypto.dylib will be picked up in the third
    //    attempt, _after_ <bundle>/Contents/Frameworks has been searched.
    //  iOS does not ship a system libssl.dylib, libcrypto.dylib in the first place.
# if defined(Q_OS_ANDROID)
    // OpenSSL 1.1.x must be suffixed otherwise it will use the system libcrypto.so libssl.so which on API-21 are OpenSSL 1.0 not 1.1
    auto openSSLSuffix = [](const QByteArray &defaultSuffix = {}) {
        auto suffix = qgetenv("ANDROID_OPENSSL_SUFFIX");
        if (suffix.isEmpty())
            return defaultSuffix;
        return suffix;
    };
#  if QT_CONFIG(opensslv11) | QT_CONFIG(opensslv30)
    static QString suffix = QString::fromLatin1(openSSLSuffix("_1_1"));
#  else
    static QString suffix = QString::fromLatin1(openSSLSuffix());
#  endif
    libssl->setFileNameAndVersion(QLatin1String("ssl") + suffix, -1);
    libcrypto->setFileNameAndVersion(QLatin1String("crypto") + suffix, -1);
# else
    libssl->setFileNameAndVersion(QLatin1String("ssl"), -1);
    libcrypto->setFileNameAndVersion(QLatin1String("crypto"), -1);
# endif
    if (libcrypto->load() && libssl->load()) {
        // libssl.so.0 and libcrypto.so.0 found
        return result;
    } else {
        libssl->unload();
        libcrypto->unload();
    }
#endif

    // third attempt: loop on the most common library paths and find libssl
    const QStringList sslList = findAllLibSsl();
    const QStringList cryptoList = findAllLibCrypto();

    for (const QString &crypto : cryptoList) {
        libcrypto->setFileNameAndVersion(crypto, -1);
        if (libcrypto->load()) {
            QFileInfo fi(crypto);
            QString version = fi.completeSuffix();

            for (const QString &ssl : sslList) {
                if (!ssl.endsWith(version))
                    continue;

                libssl->setFileNameAndVersion(ssl, -1);

                if (libssl->load()) {
                    // libssl.so.x and libcrypto.so.x found
                    return result;
                } else {
                    libssl->unload();
                }
            }
        }
        libcrypto->unload();
    }

    // failed to load anything
    result = {};
    return result;

# else
    // not implemented for this platform yet
    return result;
# endif
}
#endif

static QBasicMutex symbolResolveMutex;
static QBasicAtomicInt symbolsResolved = Q_BASIC_ATOMIC_INITIALIZER(false);
static bool triedToResolveSymbols = false;

bool q_resolveOpenSslSymbols()
{
    if (symbolsResolved.loadAcquire())
        return true;
    QMutexLocker locker(&symbolResolveMutex);
    if (symbolsResolved.loadRelaxed())
        return true;
    if (triedToResolveSymbols)
        return false;
    triedToResolveSymbols = true;

    LoadedOpenSsl libs = loadOpenSsl();
    if (!libs.ssl || !libs.crypto)
        // failed to load them
        return false;

#if QT_CONFIG(opensslv11) | QT_CONFIG(opensslv30)
    RESOLVEFUNC(X509_REQ_get_subject_name) // v1.1.0
    RESOLVEFUNC(OPENSSL_init_ssl)
    RESOLVEFUNC(OPENSSL_init_crypto)
    RESOLVEFUNC(ASN1_STRING_get0_data)
    RESOLVEFUNC(EVP_CIPHER_CTX_reset)
    RESOLVEFUNC(EVP_PKEY_base_id)
    RESOLVEFUNC(RSA_bits)
    RESOLVEFUNC(OPENSSL_sk_new_null)
    RESOLVEFUNC(OPENSSL_sk_push)
    RESOLVEFUNC(OPENSSL_sk_free)
    RESOLVEFUNC(OPENSSL_sk_num)
    RESOLVEFUNC(OPENSSL_sk_pop_free)
    RESOLVEFUNC(OPENSSL_sk_value)
    RESOLVEFUNC(DH_get0_pqg)
    RESOLVEFUNC(CRYPTO_get_ex_new_index)
    RESOLVEFUNC(X509_up_ref)
    RESOLVEFUNC(X509_STORE_CTX_get0_chain)
    RESOLVEFUNC(X509_getm_notBefore)
    RESOLVEFUNC(X509_getm_notAfter)
    RESOLVEFUNC(X509_get_version)
    RESOLVEFUNC(X509_get_pubkey)
    RESOLVEFUNC(X509_STORE_set_verify_cb)
    RESOLVEFUNC(CRYPTO_free)
    RESOLVEFUNC(OpenSSL_version_num)
    RESOLVEFUNC(OpenSSL_version)
    if (!_q_OpenSSL_version) {
        // Apparently, we were built with OpenSSL 1.1 enabled but are now using
        // a wrong library.
        qCWarning(lcSsl, "Incompatible version of OpenSSL");
        return false;
    }

    RESOLVEFUNC(DH_bits)

    RESOLVEFUNC(BIO_set_data)
    RESOLVEFUNC(BIO_get_data)
    RESOLVEFUNC(BIO_set_init)
    RESOLVEFUNC(BIO_get_shutdown)
    RESOLVEFUNC(BIO_set_shutdown)
    RESOLVEFUNC(EVP_PKEY_CTX_new_id)
    RESOLVEFUNC(EVP_PKEY_CTX_free)
    RESOLVEFUNC(EVP_PKEY_keygen_init)
    RESOLVEFUNC(EVP_PKEY_keygen);
    RESOLVEFUNC(RSA_pkey_ctx_ctrl);
    RESOLVEFUNC(BIO_free_all)
    RESOLVEFUNC(X509_REQ_new)
    RESOLVEFUNC(X509_REQ_set_version)
    RESOLVEFUNC(OBJ_txt2obj)
    RESOLVEFUNC(X509_NAME_add_entry_by_OBJ)
    RESOLVEFUNC(X509_REQ_add_extensions)
    RESOLVEFUNC(X509_EXTENSION_free)
    RESOLVEFUNC(X509_REQ_set_pubkey)
    RESOLVEFUNC(X509_REQ_sign)
    RESOLVEFUNC(PEM_write_bio_X509_REQ)
    RESOLVEFUNC(X509_REQ_free)

#else // !opensslv11

    RESOLVEFUNC(ASN1_STRING_data)

#ifdef SSLEAY_MACROS
    RESOLVEFUNC(ASN1_dup)
#endif // SSLEAY_MACROS
    RESOLVEFUNC(BIO_new_file)
    RESOLVEFUNC(ERR_clear_error)
    RESOLVEFUNC(ERR_load_crypto_strings)
    RESOLVEFUNC(CRYPTO_free)
    RESOLVEFUNC(CRYPTO_num_locks)
    RESOLVEFUNC(CRYPTO_set_id_callback)
    RESOLVEFUNC(CRYPTO_set_locking_callback)
    RESOLVEFUNC(ERR_peek_last_error)
    RESOLVEFUNC(ERR_free_strings)
    RESOLVEFUNC(EVP_CIPHER_CTX_cleanup)
    RESOLVEFUNC(EVP_CIPHER_CTX_init)

#ifdef SSLEAY_MACROS // ### verify
    RESOLVEFUNC(PEM_ASN1_read_bio)
#endif // SSLEAY_MACROS

    RESOLVEFUNC(sk_new_null)
    RESOLVEFUNC(sk_push)
    RESOLVEFUNC(sk_free)
    RESOLVEFUNC(sk_num)
    RESOLVEFUNC(sk_pop_free)
    RESOLVEFUNC(sk_value)
    RESOLVEFUNC(X509_STORE_CTX_get_chain)
#ifdef SSLEAY_MACROS
    RESOLVEFUNC(i2d_RSAPrivateKey)
    RESOLVEFUNC(d2i_RSAPrivateKey)
#endif

    RESOLVEFUNC(CONF_get1_default_config_file)
    RESOLVEFUNC(OPENSSL_add_all_algorithms_noconf)
    RESOLVEFUNC(OPENSSL_add_all_algorithms_conf)
    RESOLVEFUNC(SSLeay)
    RESOLVEFUNC(X509_REQ_new)
    RESOLVEFUNC(X509_REQ_free)
    RESOLVEFUNC(X509_NAME_add_entry_by_txt)
    RESOLVEFUNC(X509_NAME_add_entry_by_OBJ)
    RESOLVEFUNC(OBJ_txt2obj)
    RESOLVEFUNC(X509_REQ_add_extensions)

    RESOLVEFUNC(X509_REQ_set_pubkey)
    RESOLVEFUNC(X509_REQ_sign)
    RESOLVEFUNC(PEM_write_bio_X509_REQ)
    RESOLVEFUNC(PEM_write_bio_X509_REQ_NEW)
    RESOLVEFUNC(X509_REQ_set_version)
    RESOLVEFUNC(BIO_free_all)
    RESOLVEFUNC(X509_EXTENSION_free)

    if (!_q_SSLeay || q_SSLeay() >= 0x10100000L) {
        // OpenSSL 1.1 has deprecated and removed SSLeay. We consider a failure to
        // resolve this symbol as a failure to resolve symbols.
        // The right operand of '||' above is ... a bit of paranoia.
        qCWarning(lcSsl, "Incompatible version of OpenSSL");
        return false;
    }


    RESOLVEFUNC(SSLeay_version)

#ifndef OPENSSL_NO_EC
#if OPENSSL_VERSION_NUMBER >= 0x10002000L
    if (q_SSLeay() >= 0x10002000L)
        RESOLVEFUNC(EC_curve_nist2nid)
#endif // OPENSSL_VERSION_NUMBER >= 0x10002000L
#endif // OPENSSL_NO_EC


#endif // !opensslv11

    RESOLVEFUNC(ASN1_INTEGER_get)
    RESOLVEFUNC(ASN1_INTEGER_cmp)
    RESOLVEFUNC(ASN1_STRING_length)
    RESOLVEFUNC(ASN1_STRING_to_UTF8)
    RESOLVEFUNC(BIO_ctrl)
    RESOLVEFUNC(BIO_free)
    RESOLVEFUNC(BIO_new)
    RESOLVEFUNC(BIO_new_mem_buf)
    RESOLVEFUNC(BIO_read)
    RESOLVEFUNC(BIO_s_mem)
    RESOLVEFUNC(BIO_write)
    RESOLVEFUNC(BIO_set_flags)
    RESOLVEFUNC(BIO_clear_flags)
    RESOLVEFUNC(BIO_set_ex_data)
    RESOLVEFUNC(BIO_get_ex_data)
    RESOLVEFUNC(X509V3_EXT_conf_nid)
    RESOLVEFUNC(X509_EXTENSION_set_critical)

#ifndef OPENSSL_NO_EC
    RESOLVEFUNC(EC_KEY_get0_group)
    RESOLVEFUNC(EC_GROUP_get_degree)
#endif
    RESOLVEFUNC(BN_num_bits)
#if QT_CONFIG(opensslv11) | QT_CONFIG(opensslv30)
    RESOLVEFUNC(BN_is_word)
#endif
    RESOLVEFUNC(BN_mod_word)
    RESOLVEFUNC(BN_set_word)
    RESOLVEFUNC(BN_new)
    RESOLVEFUNC(BN_free)
    RESOLVEFUNC(BN_clear)
    RESOLVEFUNC(BN_clear_free)
    RESOLVEFUNC(ERR_error_string)
    RESOLVEFUNC(ERR_error_string_n)
    RESOLVEFUNC(ERR_get_error)
    RESOLVEFUNC(EVP_CIPHER_CTX_new)
    RESOLVEFUNC(EVP_CIPHER_CTX_free)
    RESOLVEFUNC(EVP_CIPHER_CTX_ctrl)
    RESOLVEFUNC(EVP_CIPHER_CTX_set_key_length)
    RESOLVEFUNC(EVP_CipherInit)
    RESOLVEFUNC(EVP_CipherInit_ex)
    RESOLVEFUNC(EVP_CipherUpdate)
    RESOLVEFUNC(EVP_CipherFinal)
    RESOLVEFUNC(EVP_get_digestbyname)
    RESOLVEFUNC(EVP_sha1)
    RESOLVEFUNC(EVP_sha256)
    RESOLVEFUNC(EVP_aes_256_gcm)
    RESOLVEFUNC(EVP_aes_128_cbc)
    RESOLVEFUNC(EVP_PKEY_assign)
    RESOLVEFUNC(EVP_PKEY_set1_RSA)
    RESOLVEFUNC(EVP_PKEY_set1_DH)
#ifndef OPENSSL_NO_EC
    RESOLVEFUNC(EVP_PKEY_set1_EC_KEY)
#endif
    RESOLVEFUNC(EVP_PKEY_free)
    RESOLVEFUNC(EVP_PKEY_get1_RSA)
    RESOLVEFUNC(EVP_PKEY_get1_DH)
#ifndef OPENSSL_NO_EC
    RESOLVEFUNC(EVP_PKEY_get1_EC_KEY)
#endif
    RESOLVEFUNC(EVP_PKEY_new)
    RESOLVEFUNC(EVP_PKEY_type)
    RESOLVEFUNC(OBJ_nid2sn)
    RESOLVEFUNC(OBJ_nid2ln)
    RESOLVEFUNC(OBJ_sn2nid)
    RESOLVEFUNC(OBJ_ln2nid)
    RESOLVEFUNC(i2t_ASN1_OBJECT)
    RESOLVEFUNC(OBJ_obj2txt)
    RESOLVEFUNC(OBJ_obj2nid)

#ifndef SSLEAY_MACROS
    RESOLVEFUNC(PEM_read_bio_PrivateKey)
    RESOLVEFUNC(PEM_write_bio_PKCS8PrivateKey)
    RESOLVEFUNC(PEM_read_bio_RSAPrivateKey)
#ifndef OPENSSL_NO_EC
    RESOLVEFUNC(PEM_read_bio_ECPrivateKey)
#endif
    RESOLVEFUNC(PEM_read_bio_DHparams)
    RESOLVEFUNC(PEM_write_bio_RSAPrivateKey)
    RESOLVEFUNC(PEM_write_bio_PrivateKey)
#ifndef OPENSSL_NO_EC
    RESOLVEFUNC(PEM_write_bio_ECPrivateKey)
#endif
#endif // !SSLEAY_MACROS

    RESOLVEFUNC(PEM_read_bio_PUBKEY)
    RESOLVEFUNC(PEM_read_bio_RSA_PUBKEY)
#ifndef OPENSSL_NO_EC
    RESOLVEFUNC(PEM_read_bio_EC_PUBKEY)
#endif
    RESOLVEFUNC(PEM_write_bio_RSA_PUBKEY)
    RESOLVEFUNC(PEM_write_bio_PUBKEY)
#ifndef OPENSSL_NO_EC
    RESOLVEFUNC(PEM_write_bio_EC_PUBKEY)
#endif
    RESOLVEFUNC(RAND_seed)
    RESOLVEFUNC(RAND_status)
    RESOLVEFUNC(RAND_bytes)
    RESOLVEFUNC(RSA_new)
    RESOLVEFUNC(RSA_generate_key_ex);
    RESOLVEFUNC(RSA_free)
    RESOLVEFUNC(X509_NAME_entry_count)
    RESOLVEFUNC(X509_NAME_get_entry)
    RESOLVEFUNC(X509_NAME_ENTRY_get_data)
    RESOLVEFUNC(X509_NAME_ENTRY_get_object)
    RESOLVEFUNC(X509_PUBKEY_get)
    RESOLVEFUNC(X509_STORE_free)
    RESOLVEFUNC(X509_STORE_new)
    RESOLVEFUNC(X509_STORE_add_cert)
    RESOLVEFUNC(X509_STORE_CTX_free)
    RESOLVEFUNC(X509_STORE_CTX_init)
    RESOLVEFUNC(X509_STORE_CTX_new)
    RESOLVEFUNC(X509_STORE_CTX_set_purpose)
    RESOLVEFUNC(X509_STORE_CTX_get_error)
    RESOLVEFUNC(X509_STORE_CTX_get_error_depth)
    RESOLVEFUNC(X509_STORE_CTX_get_current_cert)
    RESOLVEFUNC(X509_cmp)
    RESOLVEFUNC(X509_STORE_CTX_get_ex_data)

#ifndef SSLEAY_MACROS
    RESOLVEFUNC(X509_dup)
#endif
    RESOLVEFUNC(X509_print)
    RESOLVEFUNC(X509_digest)
    RESOLVEFUNC(X509_EXTENSION_get_object)
    RESOLVEFUNC(X509_free)
    RESOLVEFUNC(X509_gmtime_adj)
    RESOLVEFUNC(ASN1_TIME_free)
    RESOLVEFUNC(X509_get_ext)
    RESOLVEFUNC(X509_get_ext_count)
    RESOLVEFUNC(X509_get_ext_d2i)
    RESOLVEFUNC(X509V3_EXT_get)
    RESOLVEFUNC(X509V3_EXT_d2i)
    RESOLVEFUNC(X509_EXTENSION_get_critical)
    RESOLVEFUNC(X509_EXTENSION_get_data)
    RESOLVEFUNC(BASIC_CONSTRAINTS_free)
    RESOLVEFUNC(AUTHORITY_KEYID_free)
    RESOLVEFUNC(GENERAL_NAME_free)
    RESOLVEFUNC(ASN1_STRING_print)
    RESOLVEFUNC(X509_check_issued)
    RESOLVEFUNC(X509_get_issuer_name)
    RESOLVEFUNC(X509_get_subject_name)
    RESOLVEFUNC(X509_get_serialNumber)
    RESOLVEFUNC(X509_verify_cert)
    RESOLVEFUNC(d2i_X509)
    RESOLVEFUNC(i2d_X509)
    RESOLVEFUNC(SSL_CTX_load_verify_locations)
    RESOLVEFUNC(i2d_SSL_SESSION)
    RESOLVEFUNC(d2i_SSL_SESSION)
#if OPENSSL_VERSION_NUMBER >= 0x1000100fL && !defined(OPENSSL_NO_NEXTPROTONEG)
    RESOLVEFUNC(SSL_select_next_proto)
    RESOLVEFUNC(SSL_CTX_set_next_proto_select_cb)
    RESOLVEFUNC(SSL_get0_next_proto_negotiated)
#endif // OPENSSL_VERSION_NUMBER >= 0x1000100fL ...
#if OPENSSL_VERSION_NUMBER >= 0x10002000L
    RESOLVEFUNC(SSL_set_alpn_protos)
    RESOLVEFUNC(SSL_CTX_set_alpn_select_cb)
    RESOLVEFUNC(SSL_get0_alpn_selected)
#endif // OPENSSL_VERSION_NUMBER >= 0x10002000L ...
    RESOLVEFUNC(CRYPTO_malloc)
    RESOLVEFUNC(DH_new)
    RESOLVEFUNC(DH_free)
    RESOLVEFUNC(d2i_DHparams)
    RESOLVEFUNC(i2d_DHparams)
    RESOLVEFUNC(DH_check)
    RESOLVEFUNC(BN_bin2bn)
#ifndef OPENSSL_NO_EC
    RESOLVEFUNC(EC_KEY_dup)
    RESOLVEFUNC(EC_KEY_new_by_curve_name)
    RESOLVEFUNC(EC_KEY_free)
    RESOLVEFUNC(EC_get_builtin_curves)
#endif // OPENSSL_NO_EC
    RESOLVEFUNC(PKCS12_parse)
    RESOLVEFUNC(d2i_PKCS12_bio)
    RESOLVEFUNC(PKCS12_free)
    RESOLVEFUNC(X509_REQ_to_X509)
    RESOLVEFUNC(PEM_write_bio_X509)
    RESOLVEFUNC(X509_new)
    RESOLVEFUNC(ASN1_INTEGER_set)
    RESOLVEFUNC(X509_set_pubkey)
    RESOLVEFUNC(X509_set_issuer_name)
    RESOLVEFUNC(X509_sign)
    RESOLVEFUNC(X509_add_ext)
    RESOLVEFUNC(X509_set_version)
    RESOLVEFUNC(X509_set_subject_name)
    RESOLVEFUNC(ASN1_OCTET_STRING_new)
    RESOLVEFUNC(X509_pubkey_digest)
    RESOLVEFUNC(ASN1_OCTET_STRING_set)
    RESOLVEFUNC(X509_add1_ext_i2d)
    RESOLVEFUNC(ASN1_OCTET_STRING_free)
    RESOLVEFUNC(ASN1_INTEGER_new)
    RESOLVEFUNC(GENERAL_NAMES_new)
    RESOLVEFUNC(GENERAL_NAME_new)
    RESOLVEFUNC(X509_NAME_dup)
    RESOLVEFUNC(X509_set_serialNumber)
    RESOLVEFUNC(AUTHORITY_KEYID_new)
    RESOLVEFUNC(ASN1_INTEGER_dup)
    RESOLVEFUNC(X509_NAME_digest)
    RESOLVEFUNC(ASN1_INTEGER_free)
    RESOLVEFUNC(i2d_X509_REQ_bio)
    RESOLVEFUNC(i2d_X509_bio)

    symbolsResolved.storeRelease(true);
    return true;
}
#endif // QT_CONFIG(library)

#else // !defined QT_LINKED_OPENSSL

bool q_resolveOpenSslSymbols()
{
#ifdef QT_NO_OPENSSL
    return false;
#endif
    return true;
}
#endif // !defined QT_LINKED_OPENSSL

//==============================================================================
// contributed by Jay Case of Sarvega, Inc.; http://sarvega.com/
// Based on X509_cmp_time() for initial buffer hacking.
//==============================================================================
QDateTime q_getTimeFromASN1(const ASN1_TIME *aTime)
{
    size_t lTimeLength = aTime->length;
    char *pString = (char *) aTime->data;

    if (aTime->type == V_ASN1_UTCTIME) {

        char lBuffer[24];
        char *pBuffer = lBuffer;

        if ((lTimeLength < 11) || (lTimeLength > 17))
            return QDateTime();

        memcpy(pBuffer, pString, 10);
        pBuffer += 10;
        pString += 10;

        if ((*pString == 'Z') || (*pString == '-') || (*pString == '+')) {
            *pBuffer++ = '0';
            *pBuffer++ = '0';
        } else {
            *pBuffer++ = *pString++;
            *pBuffer++ = *pString++;
            // Skip any fractional seconds...
            if (*pString == '.') {
                pString++;
                while ((*pString >= '0') && (*pString <= '9'))
                    pString++;
            }
        }

        *pBuffer++ = 'Z';
        *pBuffer++ = '\0';

        time_t lSecondsFromUCT;
        if (*pString == 'Z') {
            lSecondsFromUCT = 0;
        } else {
            if ((*pString != '+') && (*pString != '-'))
                return QDateTime();

            lSecondsFromUCT = ((pString[1] - '0') * 10 + (pString[2] - '0')) * 60;
            lSecondsFromUCT += (pString[3] - '0') * 10 + (pString[4] - '0');
            lSecondsFromUCT *= 60;
            if (*pString == '-')
                lSecondsFromUCT = -lSecondsFromUCT;
        }

        tm lTime;
        lTime.tm_sec = ((lBuffer[10] - '0') * 10) + (lBuffer[11] - '0');
        lTime.tm_min = ((lBuffer[8] - '0') * 10) + (lBuffer[9] - '0');
        lTime.tm_hour = ((lBuffer[6] - '0') * 10) + (lBuffer[7] - '0');
        lTime.tm_mday = ((lBuffer[4] - '0') * 10) + (lBuffer[5] - '0');
        lTime.tm_mon = (((lBuffer[2] - '0') * 10) + (lBuffer[3] - '0')) - 1;
        lTime.tm_year = ((lBuffer[0] - '0') * 10) + (lBuffer[1] - '0');
        if (lTime.tm_year < 50)
            lTime.tm_year += 100; // RFC 2459

        QDate resDate(lTime.tm_year + 1900, lTime.tm_mon + 1, lTime.tm_mday);
        QTime resTime(lTime.tm_hour, lTime.tm_min, lTime.tm_sec);

        QDateTime result(resDate, resTime, QTimeZone::UTC);
        result = result.addSecs(lSecondsFromUCT);
        return result;

    } else if (aTime->type == V_ASN1_GENERALIZEDTIME) {

        if (lTimeLength < 15)
            return QDateTime(); // hopefully never triggered

        // generalized time is always YYYYMMDDHHMMSSZ (RFC 2459, section 4.1.2.5.2)
        tm lTime;
        lTime.tm_sec = ((pString[12] - '0') * 10) + (pString[13] - '0');
        lTime.tm_min = ((pString[10] - '0') * 10) + (pString[11] - '0');
        lTime.tm_hour = ((pString[8] - '0') * 10) + (pString[9] - '0');
        lTime.tm_mday = ((pString[6] - '0') * 10) + (pString[7] - '0');
        lTime.tm_mon = (((pString[4] - '0') * 10) + (pString[5] - '0'));
        lTime.tm_year = ((pString[0] - '0') * 1000) + ((pString[1] - '0') * 100) +
                        ((pString[2] - '0') * 10) + (pString[3] - '0');

        QDate resDate(lTime.tm_year, lTime.tm_mon, lTime.tm_mday);
        QTime resTime(lTime.tm_hour, lTime.tm_min, lTime.tm_sec);

        QDateTime result(resDate, resTime, QTimeZone::UTC);
        return result;

    } else {
        qCWarning(lcSsl, "unsupported date format detected");
        return QDateTime();
    }

}

QT_END_NAMESPACE
