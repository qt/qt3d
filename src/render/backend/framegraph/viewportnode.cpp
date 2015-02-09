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

#include "viewportnode_p.h"
#include <Qt3DRenderer/qviewport.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

ViewportNode::ViewportNode()
    : FrameGraphNode(FrameGraphNode::Viewport)
    , m_xMin(0.0f)
    , m_yMin(0.0f)
    , m_xMax(1.0f)
    , m_yMax(1.0f)
{
}

void ViewportNode::updateFromPeer(QNode *peer)
{
    QViewport *viewport = static_cast<QViewport *>(peer);
    setXMin(viewport->rect().x());
    setXMax(viewport->rect().width());
    setYMin(viewport->rect().y());
    setYMax(viewport->rect().height());
    m_clearColor = viewport->clearColor();
    setEnabled(viewport->isEnabled());
}

float ViewportNode::xMin() const
{
    return m_xMin;
}

void ViewportNode::setXMin(float xMin)
{
    m_xMin = xMin;
}
float ViewportNode::yMin() const
{
    return m_yMin;
}

void ViewportNode::setYMin(float yMin)
{
    m_yMin = yMin;
}
float ViewportNode::xMax() const
{
    return m_xMax;
}

void ViewportNode::setXMax(float xMax)
{
    m_xMax = xMax;
}
float ViewportNode::yMax() const
{
    return m_yMax;
}

void ViewportNode::setYMax(float yMax)
{
    m_yMax = yMax;
}

QColor ViewportNode::clearColor() const
{
    return m_clearColor;
}

void ViewportNode::sceneChangeEvent(const QSceneChangePtr &e)
{
    if (e->type() == NodeUpdated) {
        QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        if (propertyChange->propertyName() == QByteArrayLiteral("rect")) {
            QRectF rect = propertyChange->value().value<QRectF>();
            setXMin(rect.x());
            setYMin(rect.y());
            setXMax(rect.width());
            setYMax(rect.height());
        }
        else if (propertyChange->propertyName() == QByteArrayLiteral("clearColor")) {
            m_clearColor = propertyChange->value().value<QColor>();
        } else if (propertyChange->propertyName() == QByteArrayLiteral("enabled")) {
            setEnabled(propertyChange->value().toBool());
        }
    }
}

QRectF computeViewport(const QRectF &childViewport, const ViewportNode *parentViewport)
{
    QRectF vp(parentViewport->xMin(),
              parentViewport->yMin(),
              parentViewport->xMax(),
              parentViewport->yMax());

    if (childViewport.isEmpty()) {
        return vp;
    } else {
        return QRectF(vp.x() + vp.width() * childViewport.x(),
                      vp.y() + vp.height() * childViewport.y(),
                      vp.width() * childViewport.width(),
                      vp.height() * childViewport.height());
    }
}

} // Render
} // Qt3D

QT_END_NAMESPACE
