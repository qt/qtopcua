// Copyright (C) 2019 The Qt Company Ltd.
// Copyright (C) 2014 BlackBerry Limited. All rights reserved.
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

#ifndef OPENSSL_SYMBOLS_P_H
#define OPENSSL_SYMBOLS_P_H

#include <openssl/asn1.h>
#include <openssl/bio.h>
#include <openssl/bn.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/pkcs12.h>
#include <openssl/pkcs7.h>
#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/stack.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/x509_vfy.h>
#include <openssl/dsa.h>
#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/tls1.h>

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

#include <QtNetwork/private/qtnetworkglobal_p.h>
#include <QtCore/QLoggingCategory>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#if defined(X509_NAME)
#undef X509_NAME
#endif
#endif // Q_OS_WIN

#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(lcSsl)

#define DUMMYARG

#if !defined QT_LINKED_OPENSSL
// **************** Shared declarations ******************
// ret func(arg)

#  define DEFINEFUNC(ret, func, arg, a, err, funcret) \
    typedef ret (*_q_PTR_##func)(arg); \
    static _q_PTR_##func _q_##func = 0; \
    ret q_##func(arg) { \
        if (Q_UNLIKELY(!_q_##func)) { \
            qsslSocketUnresolvedSymbolWarning(#func); \
            err; \
        } \
        funcret _q_##func(a); \
    }

// ret func(arg1, arg2)
#  define DEFINEFUNC2(ret, func, arg1, a, arg2, b, err, funcret) \
    typedef ret (*_q_PTR_##func)(arg1, arg2);         \
    static _q_PTR_##func _q_##func = 0;               \
    ret q_##func(arg1, arg2) { \
        if (Q_UNLIKELY(!_q_##func)) { \
            qsslSocketUnresolvedSymbolWarning(#func);\
            err; \
        } \
        funcret _q_##func(a, b); \
    }

// ret func(arg1, arg2, arg3)
#  define DEFINEFUNC3(ret, func, arg1, a, arg2, b, arg3, c, err, funcret) \
    typedef ret (*_q_PTR_##func)(arg1, arg2, arg3);            \
    static _q_PTR_##func _q_##func = 0;                        \
    ret q_##func(arg1, arg2, arg3) { \
        if (Q_UNLIKELY(!_q_##func)) { \
            qsslSocketUnresolvedSymbolWarning(#func); \
            err; \
        } \
        funcret _q_##func(a, b, c); \
    }

// ret func(arg1, arg2, arg3, arg4)
#  define DEFINEFUNC4(ret, func, arg1, a, arg2, b, arg3, c, arg4, d, err, funcret) \
    typedef ret (*_q_PTR_##func)(arg1, arg2, arg3, arg4);               \
    static _q_PTR_##func _q_##func = 0;                                 \
    ret q_##func(arg1, arg2, arg3, arg4) { \
         if (Q_UNLIKELY(!_q_##func)) { \
             qsslSocketUnresolvedSymbolWarning(#func); \
             err; \
         } \
         funcret _q_##func(a, b, c, d); \
    }

// ret func(arg1, arg2, arg3, arg4, arg5)
#  define DEFINEFUNC5(ret, func, arg1, a, arg2, b, arg3, c, arg4, d, arg5, e, err, funcret) \
    typedef ret (*_q_PTR_##func)(arg1, arg2, arg3, arg4, arg5);         \
    static _q_PTR_##func _q_##func = 0;                                 \
    ret q_##func(arg1, arg2, arg3, arg4, arg5) { \
        if (Q_UNLIKELY(!_q_##func)) { \
            qsslSocketUnresolvedSymbolWarning(#func); \
            err; \
        } \
        funcret _q_##func(a, b, c, d, e); \
    }

// ret func(arg1, arg2, arg3, arg4, arg6)
#  define DEFINEFUNC6(ret, func, arg1, a, arg2, b, arg3, c, arg4, d, arg5, e, arg6, f, err, funcret) \
    typedef ret (*_q_PTR_##func)(arg1, arg2, arg3, arg4, arg5, arg6);   \
    static _q_PTR_##func _q_##func = 0;                                 \
    ret q_##func(arg1, arg2, arg3, arg4, arg5, arg6) { \
        if (Q_UNLIKELY(!_q_##func)) { \
            qsslSocketUnresolvedSymbolWarning(#func); \
            err; \
        } \
        funcret _q_##func(a, b, c, d, e, f); \
    }

// ret func(arg1, arg2, arg3, arg4, arg6, arg7)
#  define DEFINEFUNC7(ret, func, arg1, a, arg2, b, arg3, c, arg4, d, arg5, e, arg6, f, arg7, g, err, funcret) \
    typedef ret (*_q_PTR_##func)(arg1, arg2, arg3, arg4, arg5, arg6, arg7);   \
    static _q_PTR_##func _q_##func = 0;                                       \
    ret q_##func(arg1, arg2, arg3, arg4, arg5, arg6, arg7) { \
        if (Q_UNLIKELY(!_q_##func)) { \
            qsslSocketUnresolvedSymbolWarning(#func); \
            err; \
        } \
        funcret _q_##func(a, b, c, d, e, f, g); \
    }

// ret func(arg1, arg2, arg3, arg4, arg6, arg7, arg8, arg9)
#  define DEFINEFUNC9(ret, func, arg1, a, arg2, b, arg3, c, arg4, d, arg5, e, arg6, f, arg7, g, arg8, h, arg9, i, err, funcret) \
    typedef ret (*_q_PTR_##func)(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);   \
    static _q_PTR_##func _q_##func = 0;                                                   \
    ret q_##func(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) { \
        if (Q_UNLIKELY(!_q_##func)) { \
            qsslSocketUnresolvedSymbolWarning(#func); \
            err; \
        }   \
        funcret _q_##func(a, b, c, d, e, f, g, h, i); \
    }
// **************** Shared declarations ******************

#else // !defined QT_LINKED_OPENSSL

// **************** Static declarations ******************

// ret func(arg)
#  define DEFINEFUNC(ret, func, arg, a, err, funcret) \
    ret q_##func(arg) { funcret func(a); }

// ret func(arg1, arg2)
#  define DEFINEFUNC2(ret, func, arg1, a, arg2, b, err, funcret) \
    ret q_##func(arg1, arg2) { funcret func(a, b); }

// ret func(arg1, arg2, arg3)
#  define DEFINEFUNC3(ret, func, arg1, a, arg2, b, arg3, c, err, funcret) \
    ret q_##func(arg1, arg2, arg3) { funcret func(a, b, c); }

// ret func(arg1, arg2, arg3, arg4)
#  define DEFINEFUNC4(ret, func, arg1, a, arg2, b, arg3, c, arg4, d, err, funcret) \
    ret q_##func(arg1, arg2, arg3, arg4) { funcret func(a, b, c, d); }

// ret func(arg1, arg2, arg3, arg4, arg5)
#  define DEFINEFUNC5(ret, func, arg1, a, arg2, b, arg3, c, arg4, d, arg5, e, err, funcret) \
    ret q_##func(arg1, arg2, arg3, arg4, arg5) { funcret func(a, b, c, d, e); }

// ret func(arg1, arg2, arg3, arg4, arg6)
#  define DEFINEFUNC6(ret, func, arg1, a, arg2, b, arg3, c, arg4, d, arg5, e, arg6, f, err, funcret) \
    ret q_##func(arg1, arg2, arg3, arg4, arg5, arg6) { funcret func(a, b, c, d, e, f); }

// ret func(arg1, arg2, arg3, arg4, arg6, arg7)
#  define DEFINEFUNC7(ret, func, arg1, a, arg2, b, arg3, c, arg4, d, arg5, e, arg6, f, arg7, g, err, funcret) \
    ret q_##func(arg1, arg2, arg3, arg4, arg5, arg6, arg7) { funcret func(a, b, c, d, e, f, g); }

// ret func(arg1, arg2, arg3, arg4, arg6, arg7, arg8, arg9)
#  define DEFINEFUNC9(ret, func, arg1, a, arg2, b, arg3, c, arg4, d, arg5, e, arg6, f, arg7, g, arg8, h, arg9, i, err, funcret) \
    ret q_##func(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) { funcret func(a, b, c, d, e, f, g, h, i); }

// **************** Static declarations ******************

#endif // !defined QT_LINKED_OPENSSL

#if QT_CONFIG(opensslv11) | QT_CONFIG(opensslv30)
#include "qsslsocket_openssl11_symbols_p.h"
#else
#include "qsslsocket_opensslpre11_symbols_p.h"
#endif // QT_CONFIG

bool q_resolveOpenSslSymbols();
long q_ASN1_INTEGER_get(ASN1_INTEGER *a);
int q_ASN1_INTEGER_cmp(const ASN1_INTEGER *x, const ASN1_INTEGER *y);
int q_ASN1_STRING_length(ASN1_STRING *a);
int q_ASN1_STRING_to_UTF8(unsigned char **a, ASN1_STRING *b);
long q_BIO_ctrl(BIO *a, int b, long c, void *d);
Q_AUTOTEST_EXPORT int q_BIO_free(BIO *a);
void q_BIO_free_all(BIO *a);
BIO *q_BIO_new_mem_buf(void *a, int b);
int q_BIO_read(BIO *a, void *b, int c);
Q_AUTOTEST_EXPORT int q_BIO_write(BIO *a, const void *b, int c);
int q_BN_num_bits(const BIGNUM *a);

#if QT_CONFIG(opensslv11) | QT_CONFIG(opensslv30)
int q_BN_is_word(BIGNUM *a, BN_ULONG w);
#else // opensslv11
// BN_is_word is implemented purely as a
// macro in OpenSSL < 1.1. It doesn't
// call any functions.
//
// The implementation of BN_is_word is
// 100% the same between 1.0.0, 1.0.1
// and 1.0.2.
//
// Users are required to include <openssl/bn.h>.
#define q_BN_is_word BN_is_word
#endif // !opensslv11

BN_ULONG q_BN_mod_word(const BIGNUM *a, BN_ULONG w);
int q_BN_set_word(const BIGNUM *a, BN_ULONG w);
BIGNUM *q_BN_new();
void q_BN_clear(BIGNUM *a);
void q_BN_free(BIGNUM *a);
void q_BN_clear_free(BIGNUM *a);

#ifndef OPENSSL_NO_EC
const EC_GROUP* q_EC_KEY_get0_group(const EC_KEY* k);
int q_EC_GROUP_get_degree(const EC_GROUP* g);
#endif
DSA *q_DSA_new();
void q_DSA_free(DSA *a);
X509 *q_d2i_X509(X509 **a, const unsigned char **b, long c);
char *q_ERR_error_string(unsigned long a, char *b);
void q_ERR_error_string_n(unsigned long e, char *buf, size_t len);
unsigned long q_ERR_get_error();
EVP_CIPHER_CTX *q_EVP_CIPHER_CTX_new();
void q_EVP_CIPHER_CTX_free(EVP_CIPHER_CTX *a);
int q_EVP_CIPHER_CTX_ctrl(EVP_CIPHER_CTX *ctx, int type, int arg, void *ptr);
int q_EVP_CIPHER_CTX_set_key_length(EVP_CIPHER_CTX *x, int keylen);
int q_EVP_CipherInit(EVP_CIPHER_CTX *ctx, const EVP_CIPHER *type, const unsigned char *key, const unsigned char *iv, int enc);
int q_EVP_CipherInit_ex(EVP_CIPHER_CTX *ctx, const EVP_CIPHER *cipher, ENGINE *impl, const unsigned char *key, const unsigned char *iv, int enc);
int q_EVP_CipherUpdate(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl, const unsigned char *in, int inl);
int q_EVP_CipherFinal(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl);
const EVP_MD *q_EVP_get_digestbyname(const char *name);
X509_REQ *q_X509_REQ_new();
void q_X509_REQ_free(X509_REQ *req);
int q_PEM_write_bio_X509_REQ(BIO *bp, X509_REQ *x);
int q_PEM_write_bio_X509_REQ_NEW(BIO *bp, X509_REQ *x);
int q_X509_REQ_add_extensions(X509_REQ *req, STACK_OF(X509_EXTENSION) *exts);
void q_X509_EXTENSION_free(X509_EXTENSION *ext);
int q_X509_EXTENSION_set_critical(X509_EXTENSION *ex, int crit);
ASN1_INTEGER *q_ASN1_INTEGER_dup(const ASN1_INTEGER *x);

#if QT_CONFIG(opensslv11) | QT_CONFIG(opensslv30)
X509_NAME *q_X509_REQ_get_subject_name(X509_REQ *req);
#define q_ERR_load_crypto_strings() \
    q_OPENSSL_init_crypto(OPENSSL_INIT_LOAD_CRYPTO_STRINGS, NULL)
#else // opensslv11
void q_ERR_load_crypto_strings();
#define q_X509_REQ_get_subject_name X509_REQ_get_subject_name
#define q_X509_get_notBefore X509_get_notBefore
#define q_X509_get_notAfter X509_get_notAfter
#endif // !opensslv11


int q_X509_NAME_add_entry_by_txt(X509_NAME *name, const char *field, int type, const unsigned char *bytes, int len, int loc, int set);
int q_X509_NAME_add_entry_by_OBJ(X509_NAME *name, const ASN1_OBJECT *obj, int type, const unsigned char *bytes, int len, int loc, int set);
ASN1_OBJECT *q_OBJ_txt2obj(const char *s, int no_name);
int q_X509_REQ_set_pubkey(X509_REQ *x, EVP_PKEY *pkey);
int q_X509_REQ_sign(X509_REQ *x, EVP_PKEY *pkey, const EVP_MD *md);
int q_X509_REQ_set_version(X509_REQ *x, long version);
X509_EXTENSION *q_X509V3_EXT_conf_nid(LHASH_OF(CONF_VALUE) *conf, X509V3_CTX *ctx, int ext_nid, char *value);

#ifndef OPENSSL_NO_DES
const EVP_CIPHER *q_EVP_des_cbc();
const EVP_CIPHER *q_EVP_des_ede3_cbc();
#endif
const EVP_CIPHER *q_EVP_aes_256_gcm();
const EVP_CIPHER *q_EVP_aes_128_cbc();
Q_AUTOTEST_EXPORT const EVP_MD *q_EVP_sha1();
const EVP_MD *q_EVP_sha256();
int q_EVP_PKEY_assign(EVP_PKEY *a, int b, char *c);
Q_AUTOTEST_EXPORT int q_EVP_PKEY_set1_RSA(EVP_PKEY *a, RSA *b);
int q_EVP_PKEY_set1_DSA(EVP_PKEY *a, DSA *b);
int q_EVP_PKEY_set1_DH(EVP_PKEY *a, DH *b);
#ifndef OPENSSL_NO_EC
int q_EVP_PKEY_set1_EC_KEY(EVP_PKEY *a, EC_KEY *b);
#endif
Q_AUTOTEST_EXPORT void q_EVP_PKEY_free(EVP_PKEY *a);
RSA *q_EVP_PKEY_get1_RSA(EVP_PKEY *a);
DSA *q_EVP_PKEY_get1_DSA(EVP_PKEY *a);
DH *q_EVP_PKEY_get1_DH(EVP_PKEY *a);
#ifndef OPENSSL_NO_EC
EC_KEY *q_EVP_PKEY_get1_EC_KEY(EVP_PKEY *a);
#endif
int q_EVP_PKEY_type(int a);
Q_AUTOTEST_EXPORT EVP_PKEY *q_EVP_PKEY_new();
int q_i2d_X509(X509 *a, unsigned char **b);
const char *q_OBJ_nid2sn(int a);
const char *q_OBJ_nid2ln(int a);
int q_OBJ_sn2nid(const char *s);
int q_OBJ_ln2nid(const char *s);
int q_i2t_ASN1_OBJECT(char *buf, int buf_len, ASN1_OBJECT *obj);
int q_OBJ_obj2txt(char *buf, int buf_len, ASN1_OBJECT *obj, int no_name);
int q_OBJ_obj2nid(const ASN1_OBJECT *a);
#define q_EVP_get_digestbynid(a) q_EVP_get_digestbyname(q_OBJ_nid2sn(a))
#ifdef SSLEAY_MACROS
// ### verify
void *q_PEM_ASN1_read_bio(d2i_of_void *a, const char *b, BIO *c, void **d, pem_password_cb *e,
                          void *f);
// ### ditto for write
#else
Q_AUTOTEST_EXPORT EVP_PKEY *q_PEM_read_bio_PrivateKey(BIO *a, EVP_PKEY **b, pem_password_cb *c, void *d);
DSA *q_PEM_read_bio_DSAPrivateKey(BIO *a, DSA **b, pem_password_cb *c, void *d);
RSA *q_PEM_read_bio_RSAPrivateKey(BIO *a, RSA **b, pem_password_cb *c, void *d);
#ifndef OPENSSL_NO_EC
EC_KEY *q_PEM_read_bio_ECPrivateKey(BIO *a, EC_KEY **b, pem_password_cb *c, void *d);
#endif
DH *q_PEM_read_bio_DHparams(BIO *a, DH **b, pem_password_cb *c, void *d);
int q_PEM_write_bio_DSAPrivateKey(BIO *a, DSA *b, const EVP_CIPHER *c, unsigned char *d,
                                  int e, pem_password_cb *f, void *g);
int q_PEM_write_bio_RSAPrivateKey(BIO *a, RSA *b, const EVP_CIPHER *c, unsigned char *d,
                                  int e, pem_password_cb *f, void *g);
int q_PEM_write_bio_PrivateKey(BIO *a, EVP_PKEY *b, const EVP_CIPHER *c, unsigned char *d,
                               int e, pem_password_cb *f, void *g);
int q_PEM_write_bio_PKCS8PrivateKey(BIO *a, EVP_PKEY *b, const EVP_CIPHER *c, char *d,
                                int e, pem_password_cb *f, void *g);
#ifndef OPENSSL_NO_EC
int q_PEM_write_bio_ECPrivateKey(BIO *a, EC_KEY *b, const EVP_CIPHER *c, unsigned char *d,
                                  int e, pem_password_cb *f, void *g);
#endif
#endif // SSLEAY_MACROS
Q_AUTOTEST_EXPORT EVP_PKEY *q_PEM_read_bio_PUBKEY(BIO *a, EVP_PKEY **b, pem_password_cb *c, void *d);
DSA *q_PEM_read_bio_DSA_PUBKEY(BIO *a, DSA **b, pem_password_cb *c, void *d);
RSA *q_PEM_read_bio_RSA_PUBKEY(BIO *a, RSA **b, pem_password_cb *c, void *d);
#ifndef OPENSSL_NO_EC
EC_KEY *q_PEM_read_bio_EC_PUBKEY(BIO *a, EC_KEY **b, pem_password_cb *c, void *d);
#endif
int q_PEM_write_bio_DSA_PUBKEY(BIO *a, DSA *b);
int q_PEM_write_bio_RSA_PUBKEY(BIO *a, RSA *b);
int q_PEM_write_bio_PUBKEY(BIO *a, EVP_PKEY *b);
#ifndef OPENSSL_NO_EC
int q_PEM_write_bio_EC_PUBKEY(BIO *a, EC_KEY *b);
#endif
void q_RAND_seed(const void *a, int b);
int q_RAND_status();
int q_RAND_bytes(unsigned char *b, int n);
RSA *q_RSA_new();
void q_RSA_free(RSA *a);
int q_RSA_generate_key_ex(RSA *rsa, int bits, BIGNUM *e, BN_GENCB *cb);
#if OPENSSL_VERSION_NUMBER >= 0x10001000L && !defined(OPENSSL_NO_PSK)
typedef unsigned int (*q_psk_client_callback_t)(SSL *ssl, const char *hint, char *identity, unsigned int max_identity_len, unsigned char *psk, unsigned int max_psk_len);
void q_SSL_set_psk_client_callback(SSL *ssl, q_psk_client_callback_t callback);
typedef unsigned int (*q_psk_server_callback_t)(SSL *ssl, const char *identity, unsigned char *psk, unsigned int max_psk_len);
#endif // OPENSSL_VERSION_NUMBER >= 0x10001000L && !defined(OPENSSL_NO_PSK)
int q_SSL_write(SSL *a, const void *b, int c);
int q_X509_cmp(X509 *a, X509 *b);
#ifdef SSLEAY_MACROS
void *q_ASN1_dup(i2d_of_void *i2d, d2i_of_void *d2i, char *x);
#define q_X509_dup(x509) (X509 *)q_ASN1_dup((i2d_of_void *)q_i2d_X509, \
                (d2i_of_void *)q_d2i_X509,(char *)x509)
#else
X509 *q_X509_dup(X509 *a);
#endif
void q_X509_print(BIO *a, X509*b);
int q_X509_digest(const X509 *x509, const EVP_MD *type, unsigned char *md, unsigned int *len);
ASN1_OBJECT *q_X509_EXTENSION_get_object(X509_EXTENSION *a);
Q_AUTOTEST_EXPORT void q_X509_free(X509 *a);
Q_AUTOTEST_EXPORT ASN1_TIME *q_X509_gmtime_adj(ASN1_TIME *s, long adj);
Q_AUTOTEST_EXPORT void q_ASN1_TIME_free(ASN1_TIME *t);
X509_EXTENSION *q_X509_get_ext(X509 *a, int b);
int q_X509_get_ext_count(X509 *a);
void *q_X509_get_ext_d2i(X509 *a, int b, int *c, int *d);
const X509V3_EXT_METHOD *q_X509V3_EXT_get(X509_EXTENSION *a);
void *q_X509V3_EXT_d2i(X509_EXTENSION *a);
int q_X509_EXTENSION_get_critical(X509_EXTENSION *a);
ASN1_OCTET_STRING *q_X509_EXTENSION_get_data(X509_EXTENSION *a);
void q_BASIC_CONSTRAINTS_free(BASIC_CONSTRAINTS *a);
void q_AUTHORITY_KEYID_free(AUTHORITY_KEYID *a);
int q_ASN1_STRING_print(BIO *a, const ASN1_STRING *b);
int q_X509_check_issued(X509 *a, X509 *b);
X509_NAME *q_X509_get_issuer_name(X509 *a);
X509_NAME *q_X509_get_subject_name(X509 *a);
ASN1_INTEGER *q_X509_get_serialNumber(X509 *a);
int q_X509_verify_cert(X509_STORE_CTX *ctx);
int q_X509_NAME_entry_count(X509_NAME *a);
X509_NAME_ENTRY *q_X509_NAME_get_entry(X509_NAME *a,int b);
ASN1_STRING *q_X509_NAME_ENTRY_get_data(X509_NAME_ENTRY *a);
ASN1_OBJECT *q_X509_NAME_ENTRY_get_object(X509_NAME_ENTRY *a);
EVP_PKEY *q_X509_PUBKEY_get(X509_PUBKEY *a);
void q_X509_STORE_free(X509_STORE *store);
X509_STORE *q_X509_STORE_new();
int q_X509_STORE_add_cert(X509_STORE *ctx, X509 *x);
void q_X509_STORE_CTX_free(X509_STORE_CTX *storeCtx);
int q_X509_STORE_CTX_init(X509_STORE_CTX *ctx, X509_STORE *store,
                          X509 *x509, STACK_OF(X509) *chain);
X509_STORE_CTX *q_X509_STORE_CTX_new();
int q_X509_STORE_CTX_set_purpose(X509_STORE_CTX *ctx, int purpose);
int q_X509_STORE_CTX_get_error(X509_STORE_CTX *ctx);
int q_X509_STORE_CTX_get_error_depth(X509_STORE_CTX *ctx);
X509 *q_X509_STORE_CTX_get_current_cert(X509_STORE_CTX *ctx);
X509 *q_X509_REQ_to_X509(X509_REQ *r, int days, EVP_PKEY *pkey);
int q_PEM_write_bio_X509(BIO *bp, X509 *x);
X509 *q_X509_new();
int q_ASN1_INTEGER_set(ASN1_INTEGER *a, long v);
int q_X509_set_pubkey(X509 *x, EVP_PKEY *key);
int q_X509_set_issuer_name(X509 *x, X509_NAME *name);
int q_X509_sign(X509 *x, EVP_PKEY *key, const EVP_MD *md);
int q_X509_add_ext(X509 *x, X509_EXTENSION *ex, int location);
int q_X509_set_version(X509 *x, long version);
int q_X509_set_subject_name(X509 *x, X509_NAME *name);
ASN1_OCTET_STRING *q_ASN1_OCTET_STRING_new();
int q_X509_pubkey_digest(const X509 *data, const EVP_MD *type, unsigned char *md, unsigned int *len);
int q_ASN1_OCTET_STRING_set(ASN1_OCTET_STRING *str, const unsigned char *data, int len);
int q_X509_add1_ext_i2d(X509 *x, int nid, void *value, int crit, unsigned long flags);
void q_ASN1_OCTET_STRING_free(ASN1_OCTET_STRING *a);
int q_X509_NAME_digest(const X509_NAME *data, const EVP_MD *type, unsigned char *md, unsigned int *len);
ASN1_INTEGER *q_ASN1_INTEGER_new();
GENERAL_NAMES *q_GENERAL_NAMES_new();
GENERAL_NAME *q_GENERAL_NAME_new();
X509_NAME *q_X509_NAME_dup(X509_NAME *xn);
int q_X509_set_serialNumber(X509 *x, ASN1_INTEGER *serial);
AUTHORITY_KEYID *q_AUTHORITY_KEYID_new();
void q_ASN1_INTEGER_free(ASN1_INTEGER *a);
int q_i2d_X509_REQ_bio(BIO *bp, X509_REQ *req);
int q_i2d_X509_bio(BIO *bp, X509 *x509);

// Diffie-Hellman support
DH *q_DH_new();
void q_DH_free(DH *dh);
DH *q_d2i_DHparams(DH **a, const unsigned char **pp, long length);
int q_i2d_DHparams(DH *a, unsigned char **p);
int q_DH_check(DH *dh, int *codes);

BIGNUM *q_BN_bin2bn(const unsigned char *s, int len, BIGNUM *ret);

#ifndef OPENSSL_NO_EC
// EC Diffie-Hellman support
EC_KEY *q_EC_KEY_dup(const EC_KEY *src);
EC_KEY *q_EC_KEY_new_by_curve_name(int nid);
void q_EC_KEY_free(EC_KEY *ecdh);

// EC curves management
size_t q_EC_get_builtin_curves(EC_builtin_curve *r, size_t nitems);
#if OPENSSL_VERSION_NUMBER >= 0x10002000L
int q_EC_curve_nist2nid(const char *name);
#endif // OPENSSL_VERSION_NUMBER >= 0x10002000L
#endif // OPENSSL_NO_EC
#if OPENSSL_VERSION_NUMBER >= 0x10002000L
#define q_SSL_get_server_tmp_key(ssl, key) q_SSL_ctrl((ssl), SSL_CTRL_GET_SERVER_TMP_KEY, 0, (char *)key)
#endif // OPENSSL_VERSION_NUMBER >= 0x10002000L

// PKCS#12 support
int q_PKCS12_parse(PKCS12 *p12, const char *pass, EVP_PKEY **pkey, X509 **cert, STACK_OF(X509) **ca);
PKCS12 *q_d2i_PKCS12_bio(BIO *bio, PKCS12 **pkcs12);
void q_PKCS12_free(PKCS12 *pkcs12);

#define q_BIO_get_mem_data(b, pp) (int)q_BIO_ctrl(b,BIO_CTRL_INFO,0,(char *)pp)
#define q_BIO_pending(b) (int)q_BIO_ctrl(b,BIO_CTRL_PENDING,0,NULL)
#define q_SSL_CTX_set_mode(ctx,op) q_SSL_CTX_ctrl((ctx),SSL_CTRL_MODE,(op),NULL)
#define q_sk_GENERAL_NAME_num(st) q_SKM_sk_num(GENERAL_NAME, (st))
#define q_sk_GENERAL_NAME_value(st, i) q_SKM_sk_value(GENERAL_NAME, (st), (i))
#if QT_CONFIG(opensslv11) | QT_CONFIG(opensslv30)
#define q_sk_GENERAL_NAME_push(st, val) q_OPENSSL_sk_push((st), (val))
#else
#define q_sk_GENERAL_NAME_push(st, val) q_SKM_sk_push(GENERAL_NAME, (st), (val))
#endif

void q_GENERAL_NAME_free(GENERAL_NAME *a);

#define q_sk_X509_num(st) q_SKM_sk_num(X509, (st))
#define q_sk_X509_value(st, i) q_SKM_sk_value(X509, (st), (i))
#define q_sk_SSL_CIPHER_num(st) q_SKM_sk_num(SSL_CIPHER, (st))
#define q_sk_SSL_CIPHER_value(st, i) q_SKM_sk_value(SSL_CIPHER, (st), (i))
#define q_SSL_CTX_add_extra_chain_cert(ctx,x509) \
        q_SSL_CTX_ctrl(ctx,SSL_CTRL_EXTRA_CHAIN_CERT,0,(char *)x509)
#define q_EVP_PKEY_assign_RSA(pkey,rsa) q_EVP_PKEY_assign((pkey),EVP_PKEY_RSA,\
                                        (char *)(rsa))
#define q_EVP_PKEY_assign_DSA(pkey,dsa) q_EVP_PKEY_assign((pkey),EVP_PKEY_DSA,\
                                        (char *)(dsa))
#define q_OpenSSL_add_all_algorithms() q_OPENSSL_add_all_algorithms_conf()
int q_i2d_SSL_SESSION(SSL_SESSION *in, unsigned char **pp);

void q_BIO_set_flags(BIO *b, int flags);
void q_BIO_clear_flags(BIO *b, int flags);
void *q_BIO_get_ex_data(BIO *b, int idx);
int q_BIO_set_ex_data(BIO *b, int idx, void *data);

#define q_BIO_set_retry_read(b) q_BIO_set_flags(b, (BIO_FLAGS_READ|BIO_FLAGS_SHOULD_RETRY))
#define q_BIO_set_retry_write(b) q_BIO_set_flags(b, (BIO_FLAGS_WRITE|BIO_FLAGS_SHOULD_RETRY))
#define q_BIO_clear_retry_flags(b) q_BIO_clear_flags(b, (BIO_FLAGS_RWS|BIO_FLAGS_SHOULD_RETRY))
#define q_BIO_set_app_data(s,arg) q_BIO_set_ex_data(s,0,arg)
#define q_BIO_get_app_data(s) q_BIO_get_ex_data(s,0)

// Helper function
class QDateTime;
QDateTime q_getTimeFromASN1(const ASN1_TIME *aTime);

void *q_CRYPTO_malloc(size_t num, const char *file, int line);
#define q_OPENSSL_malloc(num) q_CRYPTO_malloc(num, "", 0)

QT_END_NAMESPACE

#endif
