// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>


/*
 * This generator should be run if there is a new version of the NodeIds.csv file available in
 * https://opcfoundation.org/UA/schemas/1.03/
 *
 * Building
 * Qt OPC UA must be configured with -feature-ns0idgenerator to enable building the generator.
 *
 * Usage:
 * qtopcua-defaultnodeidsgenerator -i NodeIds.csv -o /path/to/qtopcua/src/opcua/client/qopcuanodeids
 *
 * This generates qopcuanodeids.h and qopcuanodeids.cpp from the content of NodeIds.csv.
 * The Qt OPC UA module must be rebuilt after running the generator.
*/


void createLicenseHeader(QTextStream &output)
{
    output << R"(
// Copyright (C) 2021 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
)";
}

void createHeader(QTextStream &output)
{
    createLicenseHeader(output);
    output << "\n\n";
    output << "#ifndef QOPCUANODEIDS_H" << "\n";
    output << "#define QOPCUANODEIDS_H" << "\n\n";
    output << "#include <QtOpcUa/qopcuaglobal.h>" << "\n\n";
    output << "#include <QtCore/qmetatype.h>" << "\n\n";
    output << "QT_BEGIN_NAMESPACE" << "\n\n";
    output << "namespace QOpcUa {" << "\n";
    output << "    " << "namespace NodeIds {" << "\n";
    output << "#ifndef QT_OPCUA_NO_NS0IDNAMES" << "\n";
    output << "        " << "Q_OPCUA_EXPORT Q_NAMESPACE" << "\n";
    output << "#endif" << "\n\n";
    output << "        " << "enum class Namespace0 : quint32 {" << "\n";
    output << "            " << "Unknown = 0," << "\n";
}

void createFooter(QTextStream &output)
{
    output << "        " << "};" << "\n";
    output << "#ifndef QT_OPCUA_NO_NS0IDNAMES" << "\n";
    output << "        " << "Q_ENUM_NS(Namespace0)" << "\n";
    output << "#endif" << "\n";
    output << "    " << "}" << "\n";
    output << "}" << "\n\n";
    output << "QT_END_NAMESPACE" << "\n\n";
    output << "#endif // QOPCUANODEIDS_H\n";
}

void createSourceHeader(QTextStream &output)
{
    createLicenseHeader(output);
    output << "\n\n";
    output << "#include \"qopcuanodeids.h\"" << "\n\n";
    output << "QT_BEGIN_NAMESPACE" << "\n\n";
    output << R"(/*!
    \namespace QOpcUa::NodeIds
    \inmodule QtOpcUa
    \inheaderfile QtOpcUa/qopcuanodeids.h
    \brief This namespace contains enums with known node ids.
*/

/*!
    \enum QOpcUa::NodeIds::Namespace0

    Contains all numeric node identifiers from namespace 0 defined in the OPC Foundation's
    \l {https://opcfoundation.org/UA/schemas/1.03/NodeIds.csv} {NodeIds.csv} file.

    The values in this enum follow the naming from the CSV file and can be converted between
    enum and node id string using \l QOpcUa::namespace0Id() and \l QOpcUa::namespace0IdFromNodeId().
    \l QOpcUa::namespace0IdName() provides a conversion from enum value to the name string from the CSV file.

    \code
    QScopedPointer<QOpcUaNode> rootNode(client->node(QOpcUa::namespace0Id(QOpcUa::NodeIds::RootFolder)));
    \endcode)" << "\n\n";
    output << "    " << "\\value Unknown" << "\n";
}

void createSourceFooter(QTextStream &output)
{
    output << "*/" << "\n\n";
    output << "QT_END_NAMESPACE" << "\n";
}

bool nodeIdFromLine(const QString &line, QTextStream &outputHeader, QTextStream &outputSource)
{
    QStringList parts = line.trimmed().split(QLatin1String(","));

    if (parts.size() != 3)
        return false;
    bool ok = false;
    int identifier = parts.at(1).toUInt(&ok);
    if (!ok)
        return false;

    outputHeader << "            " << QStringLiteral("%1 = %2,").arg(parts.at(0)).arg(identifier) << "\n";
    outputSource << "    " << QStringLiteral("\\value %1").arg(parts.at(0)) << "\n";
    return true;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.setApplicationDescription("\nThis application generates a Qt OPC UA header file from the NodeIds.csv file distributed by the OPC Foundation.");
    parser.addOption(QCommandLineOption("i", "The NodeIds.csv file", "input file"));
    parser.addOption(QCommandLineOption("o", "The output prefix for the generated .h and c++ file, defaults to qopcuanodeids.h", "output file", "qopcuanodeids"));
    parser.process(app);

    if (!parser.isSet("i")) {
        qDebug() << "Error: No input file specified";
        return EXIT_FAILURE;
    }

    QFile inputFile(parser.value("i"));

    if (!inputFile.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "Failed to open file" << inputFile.fileName();
        return EXIT_FAILURE;
    }

    QString currentLine = inputFile.readLine();

    if (currentLine.trimmed() != QLatin1String("Boolean,1,DataType")) {
        qDebug() << "Invalid content in" << inputFile.fileName();
        inputFile.close();
        return EXIT_FAILURE;
    }

    QFile outHeader(parser.value("o")+QLatin1String(".h"));
    if (!outHeader.open(QFile::WriteOnly | QFile::Text | QFile::Truncate)) {
        qDebug() << "Failed to open output file" << outHeader.fileName();
        inputFile.close();
        return EXIT_FAILURE;
    }

    QFile outSource(parser.value("o")+QLatin1String(".cpp"));
    if (!outSource.open(QFile::WriteOnly | QFile::Text | QFile::Truncate)) {
        qDebug() << "Failed to open output file" << outSource.fileName();
        inputFile.close();
        outHeader.close();
        return EXIT_FAILURE;
    }

    QTextStream outHeaderStream(&outHeader);
    QTextStream outSourceStream(&outSource);
    createHeader(outHeaderStream);
    createSourceHeader(outSourceStream);
    nodeIdFromLine(currentLine, outHeaderStream, outSourceStream);

    while (!inputFile.atEnd()) {
        currentLine = inputFile.readLine();
        nodeIdFromLine(currentLine, outHeaderStream, outSourceStream);
    }

    createFooter(outHeaderStream);
    createSourceFooter(outSourceStream);

    inputFile.close();
    outHeader.close();
    outSource.close();

    return EXIT_SUCCESS;
}
