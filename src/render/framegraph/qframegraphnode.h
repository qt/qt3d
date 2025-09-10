// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QFRAMEGRAPHNODE_H
#define QT3DRENDER_QFRAMEGRAPHNODE_H

#include <Qt3DCore/qnode.h>
#include <Qt3DRender/qt3drender_global.h>
#include <QtCore/QObject>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QFrameGraphNodePrivate;

class Q_3DRENDERSHARED_EXPORT QFrameGraphNode : public Qt3DCore::QNode
{
    Q_OBJECT
public:
    explicit QFrameGraphNode(Qt3DCore::QNode *parent = nullptr);
    ~QFrameGraphNode();

    QFrameGraphNode *parentFrameGraphNode() const;

protected:
    explicit QFrameGraphNode(QFrameGraphNodePrivate &dd, Qt3DCore::QNode *parent = nullptr);

private Q_SLOTS:
    void onParentChanged(QObject *);

private:
    Q_DECLARE_PRIVATE(QFrameGraphNode)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QFRAMEGRAPHNODE_H
