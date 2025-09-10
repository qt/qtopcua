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

DEFINEFUNC2(int, OPENSSL_init_crypto, uint64_t opts, opts, const OPENSSL_INIT_SETTINGS *settings, settings, return 0, return)
DEFINEFUNC(BIO *, BIO_new, const BIO_METHOD *a, a, return nullptr, return)
DEFINEFUNC(const BIO_METHOD *, BIO_s_mem, void, DUMMYARG, return nullptr, return)
DEFINEFUNC(int, EVP_PKEY_get_base_id, const EVP_PKEY *pkey, pkey, return -1, return)
DEFINEFUNC2(void, OPENSSL_sk_pop_free, OPENSSL_STACK *a, a, void (*b)(void*), b, return, DUMMYARG)
DEFINEFUNC(OPENSSL_STACK *, OPENSSL_sk_new_null, DUMMYARG, DUMMYARG, return nullptr, return)
DEFINEFUNC2(void, OPENSSL_sk_push, OPENSSL_STACK *a, a, void *b, b, return, DUMMYARG)
DEFINEFUNC(ASN1_TIME *, X509_getm_notBefore, X509 *a, a, return nullptr, return)
DEFINEFUNC(ASN1_TIME *, X509_getm_notAfter, X509 *a, a, return nullptr, return)
DEFINEFUNC4(long, BIO_ctrl, BIO *a, a, int b, b, long c, c, void *d, d, return -1, return)
DEFINEFUNC(void, BIO_free_all, BIO *a, a, return, return)
DEFINEFUNC2(BIO *, BIO_new_mem_buf, void *a, a, int b, b, return nullptr, return)
DEFINEFUNC3(void, ERR_error_string_n, unsigned long e, e, char *b, b, size_t len, len, return, DUMMYARG)
DEFINEFUNC(unsigned long, ERR_get_error, DUMMYARG, DUMMYARG, return 0, return)
DEFINEFUNC(const EVP_MD *, EVP_sha1, DUMMYARG, DUMMYARG, return nullptr, return)
DEFINEFUNC(const EVP_MD *, EVP_sha256, DUMMYARG, DUMMYARG, return nullptr, return)
DEFINEFUNC(const EVP_CIPHER *, EVP_aes_128_cbc, DUMMYARG, DUMMYARG, return nullptr, return)
DEFINEFUNC(void, EVP_PKEY_free, EVP_PKEY *a, a, return, DUMMYARG)
DEFINEFUNC4(X509_EXTENSION* , X509V3_EXT_conf_nid, LHASH_OF(CONF_VALUE) *conf, conf, X509V3_CTX *ctx, ctx, int ext_nid, ext_nid, char *value, value, return NULL, return)
DEFINEFUNC4(EVP_PKEY *, PEM_read_bio_PrivateKey, BIO *a, a, EVP_PKEY **b, b, pem_password_cb *c, c, void *d, d, return nullptr, return)
DEFINEFUNC7(int, PEM_write_bio_PKCS8PrivateKey, BIO *a, a, EVP_PKEY *b, b, const EVP_CIPHER *c, c, char *d, d, int e, e, pem_password_cb *f, f, void *g, g, return 0, return)
DEFINEFUNC4(EVP_PKEY *, PEM_read_bio_PUBKEY, BIO *a, a, EVP_PKEY **b, b, pem_password_cb *c, c, void *d, d, return nullptr, return)
DEFINEFUNC2(int, PEM_write_bio_PUBKEY, BIO *a, a, EVP_PKEY *b, b, return 0, return)
DEFINEFUNC(void, X509_free, X509 *a, a, return, DUMMYARG)
DEFINEFUNC2(ASN1_TIME *, X509_gmtime_adj, ASN1_TIME *s, s, long adj, adj, return nullptr, return)
DEFINEFUNC4(void *, X509_get_ext_d2i, X509 *a, a, int b, b, int *c, c, int *d, d, return nullptr, return)
DEFINEFUNC(void, AUTHORITY_KEYID_free, AUTHORITY_KEYID *a, a, return, DUMMYARG)
DEFINEFUNC(X509_NAME *, X509_get_issuer_name, X509 *a, a, return nullptr, return)
DEFINEFUNC(X509_NAME *, X509_get_subject_name, X509 *a, a, return nullptr, return)
DEFINEFUNC(ASN1_INTEGER *, X509_get_serialNumber, X509 *a, a, return nullptr, return)
DEFINEFUNC(X509_REQ*, X509_REQ_new, void, DUMMYARG, return NULL, return);
DEFINEFUNC(void, X509_REQ_free, X509_REQ *req, req, return, return);
DEFINEFUNC(X509_NAME*, X509_REQ_get_subject_name, X509_REQ *req, req, return NULL, return);
DEFINEFUNC2(EVP_PKEY_CTX*, EVP_PKEY_CTX_new_id, int id, id, ENGINE *e, e, return NULL, return);
DEFINEFUNC(void, EVP_PKEY_CTX_free, EVP_PKEY_CTX *ctx, ctx, return, return);
DEFINEFUNC(int, EVP_PKEY_keygen_init, EVP_PKEY_CTX *ctx, ctx, return 0, return);
DEFINEFUNC2(int, EVP_PKEY_keygen, EVP_PKEY_CTX *ctx, ctx, EVP_PKEY **pkey, pkey, return 0, return);
DEFINEFUNC5(int, RSA_pkey_ctx_ctrl, EVP_PKEY_CTX *ctx, ctx, int optype, optype, int cmd, cmd, int p1, p1, void *p2, p2, return 0, return)
DEFINEFUNC7(int, X509_NAME_add_entry_by_OBJ, X509_NAME *name, name, const ASN1_OBJECT *obj, obj, int type, type, const unsigned char *bytes, bytes, int len, len, int loc, loc, int set, set, return 0, return);
DEFINEFUNC2(ASN1_OBJECT *, OBJ_txt2obj, const char *s, s, int no_name, no_name, return NULL, return);
DEFINEFUNC2(int, X509_REQ_set_pubkey, X509_REQ *x, x, EVP_PKEY *pkey, pkey, return 0, return);
DEFINEFUNC3(int, X509_REQ_sign, X509_REQ *x, x, EVP_PKEY *pkey, pkey, const EVP_MD *md, md, return 0, return);
DEFINEFUNC2(int, PEM_write_bio_X509_REQ, BIO *bp, bp, X509_REQ *x, x, return 0, return);
DEFINEFUNC2(int, X509_REQ_set_version, X509_REQ *x, x, long version, version, return 0, return);
DEFINEFUNC2(int, X509_REQ_add_extensions, X509_REQ *req, req, STACK_OF(X509_EXTENSION) *exts, exts, return 0, return)
DEFINEFUNC(void, X509_EXTENSION_free, X509_EXTENSION *ext, ext, return, return)
DEFINEFUNC2(int, X509_EXTENSION_set_critical, X509_EXTENSION *ex, ex, int crit, crit, return 0, return)
DEFINEFUNC2(int, PEM_write_bio_X509, BIO *bp, bp, X509 *x, x, return 0, return)
DEFINEFUNC(X509*, X509_new, void, DUMMYARG, return NULL, return)
DEFINEFUNC2(int, X509_set_pubkey,X509 *x, x, EVP_PKEY *key, key, return 0, return)
DEFINEFUNC3(int, X509_sign, X509 *x, x, EVP_PKEY *key, key, const EVP_MD *md, md, return 0, return)
DEFINEFUNC3(int, X509_add_ext, X509 *x, x, X509_EXTENSION *ex, ex, int loc, loc, return 0, return)
DEFINEFUNC2(int, X509_set_version, X509 *x, x, long version, version, return 0, return)
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
            .split(QLatin1Char(':'), Qt::SkipEmptyParts);

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

#if defined(Q_PROCESSOR_X86_64)
#define QT_SSL_SUFFIX "-x64"
#elif defined(Q_PROCESSOR_ARM_64)
#define QT_SSL_SUFFIX "-arm64"
#elif defined(Q_PROCESSOR_ARM_32)
#define QT_SSL_SUFFIX "-arm"
#else
#define QT_SSL_SUFFIX
#endif

tryToLoadOpenSslWin32Library(QLatin1String("libssl-3" QT_SSL_SUFFIX),
                             QLatin1String("libcrypto-3" QT_SSL_SUFFIX), result);

#undef QT_SSL_SUFFIX

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

#if QT_CONFIG(opensslv30)
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
    // OpenSSL 3.x.x must be suffixed in order to be able to find the right OpenSSL libraries
    auto openSSLSuffix = [](const QByteArray &defaultSuffix = {}) {
        auto suffix = qgetenv("ANDROID_OPENSSL_SUFFIX");
        if (suffix.isEmpty())
            return defaultSuffix;
        return suffix;
    };
    static QString suffix = QString::fromLatin1(openSSLSuffix("_3"));
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

    RESOLVEFUNC(X509_REQ_get_subject_name)
    RESOLVEFUNC(OPENSSL_init_crypto)
    RESOLVEFUNC(EVP_PKEY_get_base_id)
    RESOLVEFUNC(OPENSSL_sk_new_null)
    RESOLVEFUNC(OPENSSL_sk_push)
    RESOLVEFUNC(OPENSSL_sk_pop_free)
    RESOLVEFUNC(X509_getm_notBefore)
    RESOLVEFUNC(X509_getm_notAfter)
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
    RESOLVEFUNC(BIO_ctrl)
    RESOLVEFUNC(BIO_new)
    RESOLVEFUNC(BIO_new_mem_buf)
    RESOLVEFUNC(BIO_s_mem)
    RESOLVEFUNC(X509V3_EXT_conf_nid)
    RESOLVEFUNC(X509_EXTENSION_set_critical)
    RESOLVEFUNC(ERR_error_string_n)
    RESOLVEFUNC(ERR_get_error)
    RESOLVEFUNC(EVP_sha1)
    RESOLVEFUNC(EVP_sha256)
    RESOLVEFUNC(EVP_aes_128_cbc)
    RESOLVEFUNC(EVP_PKEY_free)
    RESOLVEFUNC(PEM_read_bio_PrivateKey)
    RESOLVEFUNC(PEM_write_bio_PKCS8PrivateKey)
    RESOLVEFUNC(PEM_read_bio_PUBKEY)
    RESOLVEFUNC(PEM_write_bio_PUBKEY)
    RESOLVEFUNC(X509_free)
    RESOLVEFUNC(X509_gmtime_adj)
    RESOLVEFUNC(X509_get_ext_d2i)
    RESOLVEFUNC(AUTHORITY_KEYID_free)
    RESOLVEFUNC(X509_get_issuer_name)
    RESOLVEFUNC(X509_get_subject_name)
    RESOLVEFUNC(X509_get_serialNumber)
    RESOLVEFUNC(PEM_write_bio_X509)
    RESOLVEFUNC(X509_new)
    RESOLVEFUNC(X509_set_pubkey)
    RESOLVEFUNC(X509_sign)
    RESOLVEFUNC(X509_add_ext)
    RESOLVEFUNC(X509_set_version)
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
