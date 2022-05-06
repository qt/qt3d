/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

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
