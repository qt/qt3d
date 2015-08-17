/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qitemmodelbuffer.h"
#include <Qt3DRenderer/private/qgraphicscontext_p.h>
#include <QDebug>
#include <QColor>

QT_BEGIN_NAMESPACE

namespace Qt3D {

void variantToBytes(void* dest, const QVariant& v, GLint type)
{
    int varType = v.type();
    switch (type) {
    case GL_FLOAT: {
        float f = v.toFloat();
        memcpy(dest, &f, sizeof(float));
        return;
    }

    case GL_INT: {
        GLint i = v.toInt();
        memcpy(dest, &i, sizeof(GLint));
        return;
    }

    case GL_UNSIGNED_INT: {
        GLuint i = v.toUInt();
        memcpy(dest, &i, sizeof(GLuint));
        return;
    }

    case GL_FLOAT_VEC3:
        if (varType == QVariant::Vector3D) {
            QVector3D v3(v.value<QVector3D>());
            memcpy(dest, &v3[0], sizeof(float) * 3);
            return;
        }

        break;

    case GL_FLOAT_VEC4:
        if (varType == QVariant::Vector4D) {
            QVector4D v4(v.value<QVector4D>());
            memcpy(dest, &v4[0], sizeof(float) * 4);
            return;
        }

        if (varType == QVariant::Color) {
            QColor c = v.value<QColor>();
            float* fptr = reinterpret_cast<float*>(dest);
            *fptr++ = c.redF();
            *fptr++ = c.greenF();
            *fptr++ = c.blueF();
            *fptr++ = c.alphaF();
            return;
        }

        if (varType == QVariant::Quaternion) {
            QVector4D qv = v.value<QQuaternion>().toVector4D();
            memcpy(dest, &qv[0], sizeof(float) * 4);
            return;
        }

        break;

    default:
        break;
    }

    qWarning() << Q_FUNC_INFO << "failed to convert" << v << "to GL type" <<
                  QString::number(type, 16);
}

namespace {

QAbstractAttribute::DataType typeFromGLType(GLint dataType, uint &dataCount)
{
    switch (dataType) {

    case GL_UNSIGNED_SHORT:
        dataCount = 1;
        return QAbstractAttribute::UnsignedShort;

    case GL_UNSIGNED_BYTE:
        dataCount = 1;
        return QAbstractAttribute::UnsignedByte;

    case GL_UNSIGNED_INT:
        dataCount = 1;
        return QAbstractAttribute::UnsignedInt;

    case GL_SHORT:
        dataCount = 1;
        return QAbstractAttribute::Short;

    case GL_BYTE:
        dataCount = 1;
        return QAbstractAttribute::Byte;

    case GL_INT:
        dataCount = 1;
        return QAbstractAttribute::Int;

    case GL_FLOAT:
        dataCount = 1;
        break;

    case GL_FLOAT_VEC2:
        dataCount = 2;
        break;

    case GL_FLOAT_VEC3:
        dataCount = 3;
        break;

    case GL_FLOAT_VEC4:
        dataCount = 4;
        break;

// TO DO: Handle doubles

    default:
        Q_UNREACHABLE();
    }

    return QAbstractAttribute::Float;
}

} // anonymous

QItemModelBuffer::QItemModelBuffer()
    : m_buffer(Q_NULLPTR)
{
}

void QItemModelBuffer::setModel(QAbstractItemModel *model)
{
    if (model == m_model)
        return;

    m_model = model;
    delete m_buffer;
    m_buffer = Q_NULLPTR;

    connect(m_model, SIGNAL(modelReset()), this, SLOT(onModelReset()));
    connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(onModelDataChanged(QModelIndex,QModelIndex)));
}

void QItemModelBuffer::setRoot(const QModelIndex &rootIndex)
{
    m_rootIndex = rootIndex;
    delete m_buffer;
    m_buffer = Q_NULLPTR;
}

void QItemModelBuffer::mapRoleName(QByteArray roleName, int elementType)
{
    mapRoleName(roleName, QString::fromLatin1(roleName), elementType);
}

void QItemModelBuffer::mapRoleName(QByteArray roleName, QString attributeName, int elementType)
{
    if (m_model) {
        if (!m_model->roleNames().values().contains(roleName)) {
            qWarning() << Q_FUNC_INFO << "unknown role name" << roleName;
        }
    }

    m_mappings.append(RoleMapping(roleName, attributeName, elementType));
    delete m_buffer;
    m_buffer = Q_NULLPTR;
}

QBuffer *QItemModelBuffer::buffer()
{
    if (!m_buffer) {
        if (!validateRoles())
            return m_buffer;

        qDeleteAll(m_attributes);
        m_attributes.clear();
        m_itemStride = 0;

        m_buffer = new QBuffer(QBuffer::VertexBuffer);
        // assume model will change
        m_buffer->setUsage(QBuffer::DynamicDraw);

        int rowCount = m_model->rowCount(m_rootIndex);
        int offset = 0;
        int mappingCount = m_mappings.count();
        for (int m=0; m<mappingCount; ++m)
            m_itemStride += m_mappings.at(m).byteSize;

        for (int m=0; m<mappingCount; ++m) {
            const RoleMapping mapping(m_mappings.at(m));
            uint dataSize = 0;
            QAttribute::DataType dataType = typeFromGLType(mapping.type, dataSize);
            QAttribute *attr(new QAttribute(m_buffer, dataType,
                                            dataSize, rowCount,
                                            offset, m_itemStride));
            m_attributes[mapping.attribute] = attr;
            offset += Render::QGraphicsContext::byteSizeFromType(mapping.type);
        } // of mappings iteration

        m_buffer->setData(computeBufferData());
    }

    return m_buffer;
}

QStringList QItemModelBuffer::attributeNames() const
{
    return m_attributes.keys();
}

QAttribute *QItemModelBuffer::attributeByName(QString nm) const
{
    return m_attributes.value(nm);
}

void QItemModelBuffer::onModelDataChanged(const QModelIndex& topLeft,
                                          const QModelIndex& bottomRight)
{
    if (!m_buffer)
        return;

    if (topLeft.parent() != m_rootIndex)
        return;

    QByteArray newBytes = m_buffer->data();
    for (int row=topLeft.row(); row<=bottomRight.row(); ++row) {
        QModelIndex index = topLeft.sibling(row, topLeft.column());

        char* itemPtr = newBytes.data();
        itemPtr += m_itemStride * row;

        writeDataForIndex(index, m_mappings.count(), itemPtr);
    } // of rows changed iteration

    m_buffer->setData(newBytes);
}

void QItemModelBuffer::onModelReset()
{
    if (!m_buffer)
        return;

    QByteArray b = computeBufferData();
    m_buffer->setData(b);
}

QByteArray QItemModelBuffer::computeBufferData()
{
    int rowCount = m_model->rowCount(m_rootIndex);

    int mappingCount = m_mappings.count();
    QByteArray newData;
    newData.resize(m_itemStride * rowCount);
    char* bufferPtr = newData.data();

    for (int row=0; row<rowCount; ++row) {
        writeDataForIndex(m_model->index(row, 0, m_rootIndex), mappingCount, bufferPtr);
        bufferPtr += m_itemStride;
    } // of rows iteration

    return newData;
}

void QItemModelBuffer::writeDataForIndex(const QModelIndex& index, int mappingCount, char* bufferPtr)
{
    char* fieldPtr = bufferPtr;
    for (int m=0; m<mappingCount; ++m) {
        const RoleMapping& mapping(m_mappings.at(m));
        QVariant v = m_model->data(index, mapping.cachedRole);
        variantToBytes(fieldPtr, v, mapping.type);

        fieldPtr += mapping.byteSize;
    } // of mappings iteration
}

bool QItemModelBuffer::validateRoles()
{
    Q_ASSERT(m_model);

    QHash<int, QByteArray> roles(m_model->roleNames());
    // create a lookup that's the the way round we need
    QHash<QByteArray, int> inverseRoles;
    foreach (int r, roles.keys())
        inverseRoles[roles.value(r)] = r;

    for (int m=0; m<m_mappings.count(); ++m) {
        QByteArray rnm(m_mappings.at(m).roleName);
        if (!inverseRoles.contains(rnm)) {
            qWarning() << "unknown role:" << rnm;
            return false;
        }

        m_mappings[m].cachedRole = inverseRoles[rnm];
    } // of mappings iteration

    return true;
}

QItemModelBuffer::RoleMapping::RoleMapping(QByteArray rnm, QString nm, int ty) :
    roleName(rnm),
    cachedRole(-1),
    attribute(nm),
    type(ty)
{
    byteSize = Render::QGraphicsContext::byteSizeFromType(ty);
}

} // namespace Qt3D

QT_END_NAMESPACE
