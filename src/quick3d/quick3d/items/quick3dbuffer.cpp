// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtQml/QJSValue>
#include <QtQml/QQmlEngine>

#include <Qt3DQuick/private/quick3dbuffer_p.h>
#include <QtQml/private/qqmlengine_p.h>
#include <QtQml/private/qjsvalue_p.h>
#include <QtQml/private/qv4typedarray_p.h>
#include <QtQml/private/qv4arraybuffer_p.h>
#include <Qt3DCore/private/qurlhelper_p.h>
#include <QtCore/qfile.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
namespace Quick {

namespace {
const int jsValueTypeId = qMetaTypeId<QJSValue>();
}

Quick3DBuffer::Quick3DBuffer(Qt3DCore::QNode *parent)
    : Qt3DCore::QBuffer(parent)
    , m_engine(nullptr)
    , m_v4engine(nullptr)
{
    QObject::connect(this, &Qt3DCore::QBuffer::dataChanged, this, &Quick3DBuffer::bufferDataChanged);
}

QByteArray Quick3DBuffer::convertToRawData(const QJSValue &jsValue)
{
    initEngines();
    Q_ASSERT(m_v4engine);
    QV4::Scope scope(m_v4engine);
    QV4::Scoped<QV4::TypedArray> typedArray(scope,
                                            QJSValuePrivate::convertToReturnedValue(m_v4engine, jsValue));
    char *dataPtr = reinterpret_cast<char *>(typedArray->arrayData());
    if (!typedArray)
        return QByteArray();

    dataPtr += typedArray->d()->byteOffset;
    uint byteLength = typedArray->byteLength();
    return QByteArray(dataPtr, byteLength);
}

QVariant Quick3DBuffer::bufferData() const
{
    return QVariant::fromValue(data());
}

void Quick3DBuffer::setBufferData(const QVariant &bufferData)
{
    if (bufferData.userType() == QMetaType::QByteArray) {
        QBuffer::setData(bufferData.toByteArray());
    } else if (bufferData.userType() == jsValueTypeId) {
        QJSValue jsValue = bufferData.value<QJSValue>();
        QBuffer::setData(convertToRawData(jsValue));
    }
}

void Quick3DBuffer::updateData(int offset, const QVariant &bufferData)
{
    if (bufferData.userType() == QMetaType::QByteArray) {
        QBuffer::updateData(offset, bufferData.toByteArray());
    } else if (bufferData.userType() == jsValueTypeId) {
        QJSValue jsValue = bufferData.value<QJSValue>();
        QBuffer::updateData(offset, convertToRawData(jsValue));
    }
}

/*!
    \qmlmethod string Quick3DBuffer::readBinaryFile(url &fileUrl)

    Reads the binary at \a fileUrl and return it as a QByteArray wrapped in a
    QVariant

    \note this is provided as convenience for QML where reading files and creating
    QByteArray is not possible
 */
QVariant Quick3DBuffer::readBinaryFile(const QUrl &fileUrl)
{
    QFile f(Qt3DCore::QUrlHelper::urlToLocalFileOrQrc(fileUrl));
    QByteArray data;

    if (f.open(QIODevice::ReadOnly))
        data = f.readAll();
    return QVariant(data);
}

void Quick3DBuffer::initEngines()
{
    if (m_engine == nullptr) {
        m_engine = qmlEngine(parent());
        m_v4engine = QQmlEnginePrivate::getV4Engine(m_engine);
    }
}

} // Quick
} // Qt3DCore

QT_END_NAMESPACE

#include "moc_quick3dbuffer_p.cpp"
