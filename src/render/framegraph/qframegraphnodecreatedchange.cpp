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

#include "qframegraphnodecreatedchange.h"
#include "qframegraphnodecreatedchange_p.h"

#include <Qt3DRender/qframegraphnode.h>
#include <Qt3DRender/private/qframegraphnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QFrameGraphNodeCreatedChangeBasePrivate::QFrameGraphNodeCreatedChangeBasePrivate(const QFrameGraphNode *node)
    : Qt3DCore::QNodeCreatedChangeBasePrivate(node)
    , m_parentFrameGraphNodeId(Qt3DCore::qIdForNode(node->parentFrameGraphNode()))
    , m_childFrameGraphNodeIds(Qt3DCore::qIdsForNodes(QFrameGraphNodePrivate::get(node)->childFrameGraphNodes()))
{
}

/*!
    \class Qt3DRender::QFrameGraphNodeCreatedChangeBase
    \inmodule Qt3DRender

    \brief A base class for changes in the FrameGraphNode.
*/
QFrameGraphNodeCreatedChangeBase::QFrameGraphNodeCreatedChangeBase(const QFrameGraphNode *node)
    : Qt3DCore::QNodeCreatedChangeBase(*new QFrameGraphNodeCreatedChangeBasePrivate(node), node)
{
}

/*! \internal */
QFrameGraphNodeCreatedChangeBase::~QFrameGraphNodeCreatedChangeBase()
{
}

Qt3DCore::QNodeId QFrameGraphNodeCreatedChangeBase::parentFrameGraphNodeId() const
{
    Q_D(const QFrameGraphNodeCreatedChangeBase);
    return d->m_parentFrameGraphNodeId;
}

} // Qt3DRender

QT_END_NAMESPACE
