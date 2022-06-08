// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "instancebuffer.h"

#include <QtGui/qvector3d.h>

static const int rowCount = 20;
static const int colCount = 20;
static const int maxInstanceCount = rowCount * colCount;

InstanceBuffer::InstanceBuffer(Qt3DCore::QNode *parent)
    : Qt3DCore::QBuffer(parent)
    , m_instanceCount(maxInstanceCount)
{
    // Create some per instance data - position of each instance
    QByteArray ba;
    ba.resize(maxInstanceCount * sizeof(QVector3D));
    QVector3D *posData = reinterpret_cast<QVector3D *>(ba.data());
    for (int j = 0; j < rowCount; ++j) {
        const float z = float(j);
        for (int i = 0; i < colCount; ++i) {
            const float x = float(i);
            const QVector3D pos(x, 0.0f, z);
            *posData = pos;
            ++posData;
        }
    }

    // Put the data into the buffer
    setData(ba);
}

int InstanceBuffer::instanceCount() const
{
    return m_instanceCount;
}

void InstanceBuffer::setInstanceCount(int instanceCount)
{
    if (m_instanceCount == instanceCount)
        return;

    m_instanceCount = instanceCount;
    emit instanceCountChanged(instanceCount);
}

