// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "treeitem.h"
#include "opcuamodel.h"

#include <QOpcUaArgument>
#include <QOpcUaAxisInformation>
#include <QOpcUaClient>
#include <QOpcUaComplexNumber>
#include <QOpcUaDoubleComplexNumber>
#include <QOpcUaEUInformation>
#include <QOpcUaExtensionObject>
#include <QOpcUaGenericStructHandler>
#include <QOpcUaGenericStructValue>
#include <QOpcUaLocalizedText>
#include <QOpcUaNode>
#include <QOpcUaQualifiedName>
#include <QOpcUaRange>
#include <QOpcUaXValue>

#include <QMetaEnum>
#include <QPixmap>

using namespace Qt::Literals::StringLiterals;

const int numberOfDisplayColumns = 8; // NodeId, Value, NodeClass, DataType, BrowseName, DisplayName, Description, Historizing

TreeItem::TreeItem(OpcUaModel *model)
    : QObject(nullptr)
    , mModel(model)
{
}

TreeItem::TreeItem(QOpcUaNode *node, OpcUaModel *model, TreeItem *parent)
    : QObject(parent)
    , mOpcNode(node)
    , mModel(model)
    , mParentItem(parent)
{
    connect(mOpcNode.get(), &QOpcUaNode::attributeRead, this, &TreeItem::handleAttributes);
    connect(mOpcNode.get(), &QOpcUaNode::attributeUpdated, this, &TreeItem::handleAttributes);
    connect(mOpcNode.get(), &QOpcUaNode::browseFinished, this, &TreeItem::browseFinished);

    if (!mOpcNode->readAttributes( QOpcUa::NodeAttribute::Value
                            | QOpcUa::NodeAttribute::NodeClass
                            | QOpcUa::NodeAttribute::Description
                            | QOpcUa::NodeAttribute::DataType
                            | QOpcUa::NodeAttribute::BrowseName
                            | QOpcUa::NodeAttribute::DisplayName
                            | QOpcUa::NodeAttribute::Historizing
                            )) {
        qWarning() << "Reading attributes" << mOpcNode->nodeId() << "failed";
    }
}

TreeItem::TreeItem(QOpcUaNode *node, OpcUaModel *model, const QOpcUaReferenceDescription &browsingData, TreeItem *parent) : TreeItem(node, model, parent)
{
    mNodeBrowseName = browsingData.browseName().name();
    mNodeClass = browsingData.nodeClass();
    mNodeId = browsingData.targetNodeId().nodeId();
    mNodeDisplayName = browsingData.displayName().text();
    mHistorizing = false;
}

TreeItem::~TreeItem()
{
    qDeleteAll(mChildItems);
}

TreeItem *TreeItem::child(int row)
{
    if (row >= mChildItems.size())
        qCritical() << "TreeItem in row" << row << "does not exist.";
    return mChildItems[row];
}

int TreeItem::childIndex(const TreeItem *child) const
{
    return mChildItems.indexOf(const_cast<TreeItem *>(child));
}

int TreeItem::childCount()
{
    startBrowsing();
    return mChildItems.size();
}

int TreeItem::columnCount() const
{
    return numberOfDisplayColumns;
}

QVariant TreeItem::data(int column)
{
    if (column == 0)
        return mNodeBrowseName;
    if (column == 1) {
        if (!mAttributesReady)
            return tr("Loading ...");

        const auto type = mOpcNode->attribute(QOpcUa::NodeAttribute::DataType).toString();
        const auto value = mOpcNode->attribute(QOpcUa::NodeAttribute::Value);

        return variantToString(value, type);
    }
    if (column == 2) {
        QMetaEnum metaEnum = QMetaEnum::fromType<QOpcUa::NodeClass>();
        QString name = metaEnum.valueToKey(int(mNodeClass));
        return name + " (" + QString::number(int(mNodeClass)) + ')';
    }
    if (column == 3) {
        if (!mAttributesReady)
            return tr("Loading ...");

        const QString typeId = mOpcNode->attribute(QOpcUa::NodeAttribute::DataType).toString();
        auto enumEntry = QOpcUa::namespace0IdFromNodeId(typeId);
        if (enumEntry == QOpcUa::NodeIds::Namespace0::Unknown)
            return typeId;
        return QOpcUa::namespace0IdName(enumEntry) + " (" + typeId + ")";
    }
    if (column == 4)
        return mNodeId;
    if (column == 5)
        return mNodeDisplayName;
    if (column == 6) {
        return mAttributesReady
            ? mOpcNode->attribute(QOpcUa::NodeAttribute::Description).value<QOpcUaLocalizedText>().text()
            : tr("Loading ...");
    }
    if (column == 7) {
        return mAttributesReady
            ? mHistorizing ? QString("true") : QString("false") : tr("Loading ...");
    }
    return QVariant();
}

int TreeItem::row() const
{
    if (!mParentItem)
        return 0;
    return mParentItem->childIndex(this);
}

TreeItem *TreeItem::parentItem()
{
    return mParentItem;
}

void TreeItem::appendChild(TreeItem *child)
{
    if (!child)
        return;

    if (!hasChildNodeItem(child->mNodeId)) {
        mChildItems.append(child);
        mChildNodeIds.insert(child->mNodeId);
    } else {
        child->deleteLater();
    }
}

static QPixmap createPixmap(const QColor &c)
{
    QPixmap p(10,10);
    p.fill(c);
    return p;
}

QPixmap TreeItem::icon(int column) const
{
    if (column != 0 || !mOpcNode)
        return QPixmap();

    static const QPixmap objectPixmap = createPixmap(Qt::darkGreen);
    static const QPixmap variablePixmap = createPixmap(Qt::darkBlue);
    static const QPixmap methodPixmap = createPixmap(Qt::darkRed);
    static const QPixmap defaultPixmap = createPixmap(Qt::gray);

    switch (mNodeClass) {
    case QOpcUa::NodeClass::Object:
        return objectPixmap;
    case QOpcUa::NodeClass::Variable:
        return variablePixmap;
    case QOpcUa::NodeClass::Method:
        return methodPixmap;
    default:
        break;
    }

    return defaultPixmap;
}

bool TreeItem::hasChildNodeItem(const QString &nodeId) const
{
    return mChildNodeIds.contains(nodeId);
}

void TreeItem::setMonitoringEnabled(bool active)
{
    if (!supportsMonitoring())
        return;

    if (active) {
        mOpcNode->enableMonitoring(QOpcUa::NodeAttribute::Value, QOpcUaMonitoringParameters(500));
    } else {
        mOpcNode->disableMonitoring(QOpcUa::NodeAttribute::Value);
    }
}

bool TreeItem::monitoringEnabled() const
{
    QOpcUaMonitoringParameters monitoring = mOpcNode.get()->monitoringStatus(QOpcUa::NodeAttribute::Value);
    return  monitoring.statusCode() == QOpcUa::UaStatusCode::Good &&
            monitoring.monitoringMode() == QOpcUaMonitoringParameters::MonitoringMode::Reporting;
}

bool TreeItem::supportsMonitoring() const
{
    return mNodeClass == QOpcUa::NodeClass::Variable;
}

void TreeItem::startBrowsing()
{
    if (mBrowseStarted)
        return;

    if (!mOpcNode->browseChildren())
        qWarning() << "Browsing node" << mOpcNode->nodeId() << "failed";
    else
        mBrowseStarted = true;
}

void TreeItem::handleAttributes(QOpcUa::NodeAttributes attr)
{
    if (attr & QOpcUa::NodeAttribute::NodeClass)
        mNodeClass = mOpcNode->attribute(QOpcUa::NodeAttribute::NodeClass).value<QOpcUa::NodeClass>();
    if (attr & QOpcUa::NodeAttribute::BrowseName)
        mNodeBrowseName = mOpcNode->attribute(QOpcUa::NodeAttribute::BrowseName).value<QOpcUaQualifiedName>().name();
    if (attr & QOpcUa::NodeAttribute::DisplayName)
        mNodeDisplayName = mOpcNode->attribute(QOpcUa::NodeAttribute::DisplayName).value<QOpcUaLocalizedText>().text();
    if (attr & QOpcUa::NodeAttribute::Historizing) {
        mHistorizing = mOpcNode->attribute(QOpcUa::NodeAttribute::Historizing).value<bool>();
    }

    mAttributesReady = true;
    emit mModel->dataChanged(mModel->createIndex(row(), 0, this), mModel->createIndex(row(), numberOfDisplayColumns - 1, this));
}

void TreeItem::browseFinished(const QList<QOpcUaReferenceDescription> &children, QOpcUa::UaStatusCode statusCode)
{
    if (statusCode != QOpcUa::Good) {
        qWarning() << "Browsing node" << mOpcNode->nodeId() << "finally failed:" << statusCode;
        return;
    }

    auto index = mModel->createIndex(row(), 0, this);

    for (const auto &item : children) {
        if (hasChildNodeItem(item.targetNodeId().nodeId()))
            continue;

        auto node = mModel->opcUaClient()->node(item.targetNodeId());
        if (!node) {
            qWarning() << "Failed to instantiate node:" << item.targetNodeId().nodeId();
            continue;
        }

        mModel->beginInsertRows(index, mChildItems.size(), mChildItems.size() + 1);
        appendChild(new TreeItem(node, mModel, item, this));
        mModel->endInsertRows();
    }

    emit mModel->dataChanged(mModel->createIndex(row(), 0, this), mModel->createIndex(row(), numberOfDisplayColumns - 1, this));
}

QString TreeItem::variantToString(const QVariant &value, const QString &typeNodeId) const
{
    if (value.metaType().id() == QMetaType::QVariantList) {
        const auto list = value.toList();
        QString concat;
        for (int i = 0, size = list.size(); i < size; ++i) {
            if (i)
                concat.append('\n'_L1);
            concat.append(variantToString(list.at(i), typeNodeId));
        }
        return concat;
    }

    if (typeNodeId == "ns=0;i=19"_L1) { // StatusCode
        const char *name = QMetaEnum::fromType<QOpcUa::UaStatusCode>().valueToKey(value.toInt());
        return name ? QLatin1StringView(name) : "Unknown StatusCode"_L1;
    }
    if (typeNodeId == "ns=0;i=2"_L1) // Char
        return QString::number(value.toInt());
    if (typeNodeId == "ns=0;i=3"_L1) // SChar
        return QString::number(value.toUInt());
    if (typeNodeId == "ns=0;i=4"_L1) // Int16
        return QString::number(value.toInt());
    if (typeNodeId == "ns=0;i=5"_L1) // UInt16
        return QString::number(value.toUInt());
    if (value.metaType().id() == QMetaType::QByteArray)
        return "0x%1"_L1.arg(QString::fromLatin1(value.toByteArray().toHex()));
    if (value.metaType().id() == QMetaType::QDateTime)
        return value.toDateTime().toString(Qt::ISODate);
    if (value.canConvert<QOpcUaQualifiedName>()) {
        const auto name = value.value<QOpcUaQualifiedName>();
        return u"[NamespaceIndex: %1, Name: \"%2\"]"_s.arg(name.namespaceIndex()).arg(name.name());
    }
    if (value.canConvert<QOpcUaLocalizedText>()) {
        const auto text = value.value<QOpcUaLocalizedText>();
        return localizedTextToString(text);
    }
    if (value.canConvert<QOpcUaRange>()) {
        const auto range = value.value<QOpcUaRange>();
        return rangeToString(range);
    }
    if (value.canConvert<QOpcUaComplexNumber>()) {
        const auto complex = value.value<QOpcUaComplexNumber>();
        return u"[Real: %1, Imaginary: %2]"_s.arg(complex.real()).arg(complex.imaginary());
    }
    if (value.canConvert<QOpcUaDoubleComplexNumber>()) {
        const auto complex = value.value<QOpcUaDoubleComplexNumber>();
        return u"[Real: %1, Imaginary: %2]"_s.arg(complex.real()).arg(complex.imaginary());
    }
    if (value.canConvert<QOpcUaXValue>()) {
        const auto xv = value.value<QOpcUaXValue>();
        return u"[X: %1, Value: %2]"_s.arg(xv.x()).arg(xv.value());
    }
    if (value.canConvert<QOpcUaEUInformation>()) {
        const auto info = value.value<QOpcUaEUInformation>();
        return euInformationToString(info);
    }
    if (value.canConvert<QOpcUaAxisInformation>()) {
        const auto info = value.value<QOpcUaAxisInformation>();
        return u"[EUInformation: %1, EURange: %2, Title: %3 , AxisScaleType: %4, AxisSteps: %5]"_s.arg(
            euInformationToString(info.engineeringUnits()), rangeToString(info.eURange()), localizedTextToString(info.title()),
            info.axisScaleType() == QOpcUa::AxisScale::Linear ? "Linear" : (info.axisScaleType() == QOpcUa::AxisScale::Ln) ? "Ln" : "Log",
            numberArrayToString(info.axisSteps()));
    }
    if (value.canConvert<QOpcUaExpandedNodeId>()) {
        const auto id = value.value<QOpcUaExpandedNodeId>();
        return u"[NodeId: \"%1\", ServerIndex: \"%2\", NamespaceUri: \"%3\"]"_s.arg(
                    id.nodeId()).arg(id.serverIndex()).arg(id.namespaceUri());
    }
    if (value.canConvert<QOpcUaArgument>()) {
        const auto a = value.value<QOpcUaArgument>();

        return u"[Name: \"%1\", DataType: \"%2\", ValueRank: \"%3\", ArrayDimensions: %4, Description: %5]"_s.arg(
                    a.name(), a.dataTypeId()).arg(a.valueRank()).arg(numberArrayToString(a.arrayDimensions()),
                    localizedTextToString(a.description()));
    }
    if (value.canConvert<QOpcUaExtensionObject>()) {
        auto obj = value.value<QOpcUaExtensionObject>();

        if (mModel->genericStructHandler() &&
            mModel->genericStructHandler()->dataTypeKindForTypeId(mModel->genericStructHandler()->typeIdForBinaryEncodingId(obj.encodingTypeId()))
                                                  == QOpcUaGenericStructHandler::DataTypeKind::Struct) {
            const auto decodedValue = mModel->genericStructHandler()->decode(obj);
            if (decodedValue)
                return decodedValue->toString();
        }

        return u"[TypeId: \"%1\", Encoding: %2, Body: 0x%3]"_s.arg(obj.encodingTypeId(),
                    obj.encoding() == QOpcUaExtensionObject::Encoding::NoBody ?
                        "NoBody" : (obj.encoding() == QOpcUaExtensionObject::Encoding::ByteString ?
                            "ByteString" : "XML")).arg(obj.encodedBody().isEmpty() ? "0" : QString(obj.encodedBody().toHex()));
    }

    if (value.canConvert<QString>())
        return value.toString();

    return QString();
}

QString TreeItem::localizedTextToString(const QOpcUaLocalizedText &text) const
{
    return u"[Locale: \"%1\", Text: \"%2\"]"_s.arg(text.locale(), text.text());
}

QString TreeItem::rangeToString(const QOpcUaRange &range) const
{
    return u"[Low: %1, High: %2]"_s.arg(range.low()).arg(range.high());
}

QString TreeItem::euInformationToString(const QOpcUaEUInformation &info) const
{
    return u"[UnitId: %1, NamespaceUri: \"%2\", DisplayName: %3, Description: %4]"_s.arg(info.unitId()).arg(
                info.namespaceUri(), localizedTextToString(info.displayName()), localizedTextToString(info.description()));
}
