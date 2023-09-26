// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>

/*
 * This generator should be run if there is a new version of the NodeIds.csv or StatusCode.csv file
 * available in https://opcfoundation.org/UA/schemas/1.05/
 *
 * Building
 * Qt OPC UA must be configured with FEATURE_internalgenerator to enable building the generator.
 *
 * Usage:
 * qtopcua-internal-generator -n NodeIds.csv -s StatusCode.csv -o /path/to/qtopcua
 *
 * This updates qopcuanodeids.h, qopcuanodeids.cpp, qopcuatype.h, qopcuatype.cpp,
 * opcuastatus_p.h and opcuastatus.cpp with the most recent status codes and node ids.
 * The Qt OPC UA module must be rebuilt after running the generator.
*/

bool patchFile(const QString &path, const QString &startMarker, const QString &endMarker, const QString &newContent)
{
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open" << path << "for reading";
        return false;
    }

    const auto content = QString::fromUtf8(file.readAll());
    file.close();

    const auto startIndex = content.indexOf(startMarker);
    if (startIndex == -1) {
        qWarning() << "Failed to parse" << path << "- has the structure changed?";
        return false;
    }

    const auto endIndex = content.indexOf(endMarker, startIndex);
    if (endIndex == -1) {
        qWarning() << "Failed to parse" << path << "- has the structure changed?";
        return false;
    }

    if (!file.open(QIODevice::WriteOnly | QFile::Truncate)) {
        qWarning() << "Failed to open" << path << "for writing";
        return false;
    }

    QTextStream out(&file);
    out << content.left(startIndex + startMarker.length());

    out << newContent;

    out << content.right(content.size() - endIndex);

    return true;
}

class NodeId {
public:
    QString name;
    QString value;
};

class StatusCode {
public:
    QString name;
    QString value;
    QString comment;
};

QList<NodeId> parseNodeIds(const QString &path)
{
    QList<NodeId> result;

    QFile file(path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open" << path << "for reading";
        return result;
    }

    QTextStream in(&file);
    while (!in.atEnd())
    {
        const auto split = in.readLine().split(QChar::fromLatin1(','));

        if (split.length() != 3)
            continue;

        result.push_back({ split.at(0), split.at(1) });
    }

    if (!result.isEmpty())
        result.push_front({ QStringLiteral("Unknown"), QString::number(0) });

    return result;
}

bool patchNodeIdsHeader(const QList<NodeId> &nodeIds, const QString &path)
{
    const auto startMarker = QStringLiteral("enum class Namespace0 : quint32 {\n");
    const auto endMarker = QStringLiteral("        };");

    QString newContent;
    QTextStream out(&newContent);

    for (const auto &id : nodeIds) {
        out << "            " << id.name << " = " << id.value << "," << "\n";
    }

    return patchFile(path, startMarker, endMarker, newContent);
}

bool patchNodeIdsSource(const QList<NodeId> &nodeIds, const QString &path)
{
    const auto startMarker = QStringLiteral("RootFolder)));\n    \\endcode\n\n");
    const auto endMarker = QStringLiteral("*/");

    QString newContent;
    QTextStream out(&newContent);

    for (const auto &id : nodeIds) {
        out << "    " << "\\value " << id.name << "\n";
    }

    return patchFile(path, startMarker, endMarker, newContent);
}

QList<StatusCode> parseStatusCodes(const QString &path)
{
    QList<StatusCode> result;

    QFile file(path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open" << path << "for reading";
        return result;
    }

    QTextStream in(&file);
    while (!in.atEnd())
    {
        const auto split = in.readLine().split(QChar::fromLatin1(','));

        if (split.length() < 3)
            continue;

        QString comment = split.size() == 3 ? split.at(2) : split.mid(2).join(QStringLiteral(","));
        comment = comment.remove(QChar::fromLatin1('"'));
        result.push_back({ split.at(0), split.at(1), comment });
    }

    return result;
}

bool patchStatusCodeHeader(const QList<StatusCode> &statusCodes, const QString &path)
{
    const auto startMarker = QStringLiteral("enum UaStatusCode : quint32\n{\n");
    const auto endMarker = QStringLiteral("};");

    QString newContent;
    QTextStream out(&newContent);

    for (const auto &statusCode : statusCodes) {
        out << "    " << statusCode.name << " = " << statusCode.value << "," << "\n";
    }

    return patchFile(path, startMarker, endMarker, newContent);
}

bool patchStatusCodeSource(const QList<StatusCode> &statusCodes, const QString &path)
{
    const auto startMarker = QStringLiteral("Opc.Ua.StatusCodes.csv}\n\n");
    const auto endMarker = QStringLiteral("*/");

    QString newContent;
    QTextStream out(&newContent);

    for (const auto &statusCode : statusCodes) {
        out << "    " << "\\value " << statusCode.name << " " << statusCode.comment << "\n";
    }

    return patchFile(path, startMarker, endMarker, newContent);
}

bool patchQmlStatusCodeHeader(const QList<StatusCode> &statusCodes, const QString &path)
{
    const auto startMarker = QStringLiteral("enum class Status {\n");
    const auto endMarker = QStringLiteral("    };");

    QString newContent;
    QTextStream out(&newContent);

    for (const auto &statusCode : statusCodes) {
        out << "        " << statusCode.name << " = " << statusCode.value.left(6) << "," << "\n";
    }

    return patchFile(path, startMarker, endMarker, newContent);
}

bool patchQmlStatusCodeSource(const QList<StatusCode> &statusCodes, const QString &path)
{
    auto startMarker = QStringLiteral("Attributes of a status\n\n");
    auto endMarker = QStringLiteral("*/");

    QString newContent;
    QTextStream out(&newContent);

    for (const auto &statusCode : statusCodes) {
        out << "   " << "\\value Status." << statusCode.name << " " << statusCode.comment << "\n";
    }

    const auto success = patchFile(path, startMarker, endMarker, newContent);

    if (!success)
        return false;

    startMarker = QStringLiteral("switch (static_cast<QOpcUa::UaStatusCode>(m_statusCode)) {\n");
    endMarker = QStringLiteral("    }");

    out.seek(0);
    newContent.clear();

    for (const auto &statusCode : statusCodes) {
        out << "    " << "case QOpcUa::" << statusCode.name << ":\n";
        out << "        " << "return Status::" << statusCode.name << ";\n";
    }

    return patchFile(path, startMarker, endMarker, newContent);
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.setApplicationDescription("\nThis application applies changes from the NodeIds.csv and StatusCode.csv files distributed by the OPC Foundation.");
    const QCommandLineOption nOption("n", "The NodeIds.csv file path", "input file");
    const QCommandLineOption sOption("s", "The StatusCode.csv file path", "input file");
    const QCommandLineOption oOption("o", "The base directory of the Qt OPC UA repository", "output base directory");

    parser.addOption(nOption);
    parser.addOption(sOption);
    parser.addOption(oOption);

    parser.process(app);

    if (!parser.isSet(oOption)) {
        qWarning() << "Unable to run without -o specified";
        return EXIT_FAILURE;
    }

    if (!parser.isSet(nOption) && !parser.isSet(sOption)) {
        qWarning() << "Unable to run without any of -n and -s specified";
        return EXIT_FAILURE;
    }

    bool success = false;

    if (parser.isSet(nOption)) {
        const auto nodeIdsHeaderPath = QStringLiteral("%1/src/opcua/client/qopcuanodeids.h").arg(parser.value(oOption));
        const auto nodeIdsSourcePath = QStringLiteral("%1/src/opcua/client/qopcuanodeids.cpp").arg(parser.value(oOption));

        const auto nodeIds = parseNodeIds(parser.value(nOption));

        if (nodeIds.isEmpty()) {
            qWarning() << "No node ids were found in the file";
            return EXIT_FAILURE;
        }

        success = patchNodeIdsHeader(nodeIds, nodeIdsHeaderPath);
        if (success)
            patchNodeIdsSource(nodeIds, nodeIdsSourcePath);
        else
            return EXIT_FAILURE;
    }

    if (parser.isSet(sOption)) {
        const auto typeHeaderPath = QStringLiteral("%1/src/opcua/client/qopcuatype.h").arg(parser.value(oOption));
        const auto typeSourcePath = QStringLiteral("%1/src/opcua/client/qopcuatype.cpp").arg(parser.value(oOption));
        const auto qmlStatusHeaderPath = QStringLiteral("%1/src/declarative_opcua/opcuastatus_p.h").arg(parser.value(oOption));
        const auto qmlStatusSourcePath = QStringLiteral("%1/src/declarative_opcua/opcuastatus.cpp").arg(parser.value(oOption));

        const auto statusCodes = parseStatusCodes(parser.value(sOption));

        if (statusCodes.isEmpty()) {
            qWarning() << "No status codes were found in the file";
            return EXIT_FAILURE;
        }

        success = patchStatusCodeHeader(statusCodes, typeHeaderPath);
        if (success)
            success = patchStatusCodeSource(statusCodes, typeSourcePath);
        if (success)
            success = patchQmlStatusCodeHeader(statusCodes, qmlStatusHeaderPath);
        if (success)
            success = patchQmlStatusCodeSource(statusCodes, qmlStatusSourcePath);
    }

    return success ? EXIT_SUCCESS : EXIT_SUCCESS;
}
