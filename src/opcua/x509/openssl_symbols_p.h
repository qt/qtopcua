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

BIO *q_BIO_new(const BIO_METHOD *a);
const BIO_METHOD *q_BIO_s_mem();

int q_EVP_PKEY_get_base_id(const EVP_PKEY *pkey);
#define q_EVP_PKEY_base_id q_EVP_PKEY_get_base_id

void q_OPENSSL_sk_pop_free(OPENSSL_STACK *a, void (*b)(void *));
void q_OPENSSL_sk_push(OPENSSL_STACK *st, void *data);
ASN1_TIME *q_X509_getm_notBefore(X509 *a);
ASN1_TIME *q_X509_getm_notAfter(X509 *a);

// We resolve q_sk_ functions, but use q_OPENSSL_sk_ macros in code to reduce
// the amount of #ifdefs and for confusing developers.
OPENSSL_STACK *q_sk_new_null();
#define q_OPENSSL_sk_new_null() q_sk_new_null()
#define q_sk_X509_EXTENSION_new_null() \
    ((OPENSSL_STACK *)q_sk_new_null())
 #define q_sk_X509_EXTENSION_push(st, val) \
         q_OPENSSL_sk_push((st), (val))
 #define q_sk_X509_EXTENSION_pop_free(st, free_func) \
            q_OPENSSL_sk_pop_free((st), (free_func))

int q_OPENSSL_init_crypto(uint64_t opts, const OPENSSL_INIT_SETTINGS *settings);
EVP_PKEY_CTX* q_EVP_PKEY_CTX_new_id(int id, ENGINE *e);
void q_EVP_PKEY_CTX_free(EVP_PKEY_CTX *ctx);
int q_EVP_PKEY_keygen_init(EVP_PKEY_CTX *ctx);

int q_RSA_pkey_ctx_ctrl(EVP_PKEY_CTX *ctx, int optype, int cmd, int p1, void *p2);
#define q_EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, bits) \
    q_RSA_pkey_ctx_ctrl(ctx, EVP_PKEY_OP_KEYGEN, \
    EVP_PKEY_CTRL_RSA_KEYGEN_BITS, bits, NULL)

int q_EVP_PKEY_keygen(EVP_PKEY_CTX *ctx, EVP_PKEY **pkey);
bool q_resolveOpenSslSymbols();

long q_BIO_ctrl(BIO *a, int b, long c, void *d);
void q_BIO_free_all(BIO *a);
BIO *q_BIO_new_mem_buf(void *a, int b);
void q_ERR_error_string_n(unsigned long e, char *buf, size_t len);
unsigned long q_ERR_get_error();
X509_REQ *q_X509_REQ_new();
void q_X509_REQ_free(X509_REQ *req);
int q_PEM_write_bio_X509_REQ(BIO *bp, X509_REQ *x);
int q_X509_REQ_add_extensions(X509_REQ *req, STACK_OF(X509_EXTENSION) *exts);
void q_X509_EXTENSION_free(X509_EXTENSION *ext);
int q_X509_EXTENSION_set_critical(X509_EXTENSION *ex, int crit);
ASN1_INTEGER *q_ASN1_INTEGER_dup(const ASN1_INTEGER *x);

X509_NAME *q_X509_REQ_get_subject_name(X509_REQ *req);
#define q_ERR_load_crypto_strings() \
    q_OPENSSL_init_crypto(OPENSSL_INIT_LOAD_CRYPTO_STRINGS, NULL)

int q_X509_NAME_add_entry_by_OBJ(X509_NAME *name, const ASN1_OBJECT *obj, int type, const unsigned char *bytes, int len, int loc, int set);
ASN1_OBJECT *q_OBJ_txt2obj(const char *s, int no_name);
int q_X509_REQ_set_pubkey(X509_REQ *x, EVP_PKEY *pkey);
int q_X509_REQ_sign(X509_REQ *x, EVP_PKEY *pkey, const EVP_MD *md);
int q_X509_REQ_set_version(X509_REQ *x, long version);
X509_EXTENSION *q_X509V3_EXT_conf_nid(LHASH_OF(CONF_VALUE) *conf, X509V3_CTX *ctx, int ext_nid, char *value);

const EVP_CIPHER *q_EVP_aes_128_cbc();
const EVP_MD *q_EVP_sha1();
const EVP_MD *q_EVP_sha256();
void q_EVP_PKEY_free(EVP_PKEY *a);
EVP_PKEY *q_PEM_read_bio_PrivateKey(BIO *a, EVP_PKEY **b, pem_password_cb *c, void *d);
int q_PEM_write_bio_PKCS8PrivateKey(BIO *a, EVP_PKEY *b, const EVP_CIPHER *c, char *d,
                                int e, pem_password_cb *f, void *g);
EVP_PKEY *q_PEM_read_bio_PUBKEY(BIO *a, EVP_PKEY **b, pem_password_cb *c, void *d);
int q_PEM_write_bio_PUBKEY(BIO *a, EVP_PKEY *b);

void q_X509_free(X509 *a);
ASN1_TIME *q_X509_gmtime_adj(ASN1_TIME *s, long adj);
void *q_X509_get_ext_d2i(X509 *a, int b, int *c, int *d);
void q_AUTHORITY_KEYID_free(AUTHORITY_KEYID *a);
X509_NAME *q_X509_get_issuer_name(X509 *a);
X509_NAME *q_X509_get_subject_name(X509 *a);
ASN1_INTEGER *q_X509_get_serialNumber(X509 *a);
int q_PEM_write_bio_X509(BIO *bp, X509 *x);
X509 *q_X509_new();
int q_X509_set_pubkey(X509 *x, EVP_PKEY *key);
int q_X509_sign(X509 *x, EVP_PKEY *key, const EVP_MD *md);
int q_X509_add_ext(X509 *x, X509_EXTENSION *ex, int location);
int q_X509_set_version(X509 *x, long version);
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

#define q_BIO_get_mem_data(b, pp) (int)q_BIO_ctrl(b,BIO_CTRL_INFO,0,(char *)pp)
#define q_sk_GENERAL_NAME_push(st, val) q_OPENSSL_sk_push((st), (val))

QT_END_NAMESPACE

#endif
