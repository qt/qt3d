/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "rendernode.h"
#include "rendereraspect.h"

#include <transform.h>

#include <qaspectmanager.h>
#include <sphere.h>
#include <entity.h>

#include <QMatrix4x4>
#include <QString>
#include <QDebug>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

// TODO: Create custom allocators for each of the matrices and
// bounding volumes to that they end up in contiguous arrays.

RenderNode::RenderNode()
    : m_rendererAspect(Q_NULLPTR)
    , m_transform(Q_NULLPTR)
    , m_parent(Q_NULLPTR)
    , m_localTransform(new QMatrix4x4)
    , m_worldTransform(new QMatrix4x4)
    , m_localBoundingVolume(new Qt3D::Sphere)
    , m_worldBoundingVolume(new Qt3D::Sphere)
    , m_frontEndPeer(0)
{

}

void RenderNode::setParent(RenderNode *parent)
{
    m_parent = parent;
    if (parent)
        parent->m_children.append(this);
}

void RenderNode::setRendererAspect(RendererAspect *rendererAspect)
{
    m_rendererAspect = rendererAspect;
}

void RenderNode::setTransform(Transform *transform)
{
    m_transform = transform;

    // Register for changes
    QChangeArbiter *arbiter = m_rendererAspect->aspectManager()->changeArbiter();
    arbiter->registerObserver(this, m_transform, LocalTransform);
}

void RenderNode::sceneChangeEvent(const QSceneChangePtr &e)
{
    switch (e->m_type) {
    case LocalTransform: {
        QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        *m_localTransform = propertyChange->m_value.value<QMatrix4x4>();
        break;
    }

    default:
        break;
    }
}

void RenderNode::dump() const
{
    static int depth = 0;
    QString indent(2 * depth++, QChar::fromLatin1(' '));
    qDebug() << indent + m_frontEndPeer->objectName();
    foreach (const RenderNode *child, m_children)
        child->dump();
    --depth;
}

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
