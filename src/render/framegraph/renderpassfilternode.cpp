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

#include "renderpassfilternode_p.h"
#include "qannotation.h"
#include "qrenderpassfilter.h"
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DRender/qparameter.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

RenderPassFilter::RenderPassFilter()
    : FrameGraphNode(FrameGraphNode::RenderPassFilter)
{
}

void RenderPassFilter::updateFromPeer(Qt3DCore::QNode *peer)
{
    QRenderPassFilter *filter = static_cast<QRenderPassFilter *>(peer);
    m_filters.clear();
    m_parameterPack.clear();
    setEnabled(filter->isEnabled());
    Q_FOREACH (QAnnotation *criterion, filter->includes())
        appendFilter(criterion->id());
    Q_FOREACH (QParameter *p, filter->parameters())
        m_parameterPack.appendParameter(p->id());
}

QList<Qt3DCore::QNodeId> RenderPassFilter::filters() const
{
    return m_filters;
}

void RenderPassFilter::appendFilter(const Qt3DCore::QNodeId &criterionId)
{
    if (!m_filters.contains(criterionId))
        m_filters.append(criterionId);
}

void RenderPassFilter::removeFilter(const Qt3DCore::QNodeId &criterionId)
{
    m_filters.removeOne(criterionId);
}

QList<Qt3DCore::QNodeId> RenderPassFilter::parameters() const
{
    return m_parameterPack.parameters();
}

void RenderPassFilter::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);

    switch (e->type()) {
    case NodeUpdated: {
        if (propertyChange->propertyName() == QByteArrayLiteral("enabled"))
            setEnabled(propertyChange->value().toBool());
    }
        break;

    case NodeAdded: {
        if (propertyChange->propertyName() == QByteArrayLiteral("include"))
            appendFilter(propertyChange->value().value<QNodeId>());
        else if (propertyChange->propertyName() == QByteArrayLiteral("parameter"))
            m_parameterPack.appendParameter(propertyChange->value().value<QNodeId>());
    }
        break;
    case NodeRemoved: {
        if (propertyChange->propertyName() == QByteArrayLiteral("include"))
            removeFilter(propertyChange->value().value<QNodeId>());
        else if (propertyChange->propertyName() == QByteArrayLiteral("parameter"))
            m_parameterPack.removeParameter(propertyChange->value().value<QNodeId>());
    }
        break;
    default:
        break;
    }
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
