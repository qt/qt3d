/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
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
****************************************************************************/

#ifndef QT3DRENDER_RENDER_QFRAMEGRAPHNODECREATEDCHANGE_H
#define QT3DRENDER_RENDER_QFRAMEGRAPHNODECREATEDCHANGE_H

#include <Qt3DCore/qnodecreatedchange.h>
#include <Qt3DRender/qt3drender_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QFrameGraphNode;
class QFrameGraphNodeCreatedChangeBasePrivate;

class Q_3DRENDERSHARED_EXPORT QFrameGraphNodeCreatedChangeBase : public Qt3DCore::QNodeCreatedChangeBase
{
public:
    explicit QFrameGraphNodeCreatedChangeBase(const QFrameGraphNode *node);
    ~QFrameGraphNodeCreatedChangeBase();

    Qt3DCore::QNodeId parentFrameGraphNodeId() const;

private:
    Q_DECLARE_PRIVATE(QFrameGraphNodeCreatedChangeBase)
};

typedef QSharedPointer<QFrameGraphNodeCreatedChangeBase> QFrameGraphNodeCreatedChangeBasePtr;

template<typename T>
class QFrameGraphNodeCreatedChange : public QFrameGraphNodeCreatedChangeBase
{
public:
    explicit QFrameGraphNodeCreatedChange(const QFrameGraphNode *node)
        : QFrameGraphNodeCreatedChangeBase(node)
        , data()
    {
    }

    T data;
};

template<typename T>
using QFrameGraphNodeCreatedChangePtr = QSharedPointer<QFrameGraphNodeCreatedChange<T>>;

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_QFRAMEGRAPHNODECREATEDCHANGE_H
