// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef INSTANCEBUFFER_H
#define INSTANCEBUFFER_H

#include <Qt3DCore/QBuffer>

class InstanceBuffer : public Qt3DCore::QBuffer
{
    Q_OBJECT
    Q_PROPERTY(int instanceCount READ instanceCount WRITE setInstanceCount NOTIFY instanceCountChanged)

public:
    InstanceBuffer(Qt3DCore::QNode *parent = 0);

    int instanceCount() const;

public slots:
    void setInstanceCount(int instanceCount);

signals:
    void instanceCountChanged(int instanceCount);

private:
    int m_instanceCount;
};

#endif // INSTANCEBUFFER_H
