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

#ifndef QT3DRENDER_RENDER_QFRAMEGRAPHNODECREATEDCHANGE_P_H
#define QT3DRENDER_RENDER_QFRAMEGRAPHNODECREATEDCHANGE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DCore/private/qnodecreatedchange_p.h>
#include <Qt3DRender/qframegraphnodecreatedchange.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QFrameGraphNode;

class QFrameGraphNodeCreatedChangeBasePrivate : public Qt3DCore::QNodeCreatedChangeBasePrivate
{
public:
    QFrameGraphNodeCreatedChangeBasePrivate(const QFrameGraphNode *node);

    Qt3DCore::QNodeId m_parentFrameGraphNodeId;
    Qt3DCore::QNodeIdVector m_childFrameGraphNodeIds;

    static QFrameGraphNodeCreatedChangeBasePrivate *get(QFrameGraphNodeCreatedChangeBase *change)
    {
        return change->d_func();
    }
    static const QFrameGraphNodeCreatedChangeBasePrivate *get(const QFrameGraphNodeCreatedChangeBase *change)
    {
        return change->d_func();
    }
};

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_QFRAMEGRAPHNODECREATEDCHANGE_P_H
