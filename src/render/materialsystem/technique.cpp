/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "technique_p.h"

#include <Qt3DRender/qrenderpass.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qtechnique.h>
#include <Qt3DRender/qgraphicsapifilter.h>
#include <Qt3DRender/private/renderer_p.h>
#include <Qt3DRender/private/filterkey_p.h>
#include <Qt3DRender/private/qtechnique_p.h>
#include <Qt3DRender/private/shader_p.h>
#include <Qt3DCore/private/qchangearbiter_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DCore/qpropertynodeaddedchange.h>
#include <Qt3DCore/qpropertynoderemovedchange.h>

#include <QDebug>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

Technique::Technique()
    : BackendNode()
{
}

Technique::~Technique()
{
    cleanup();
}

void Technique::cleanup()
{
    QBackendNode::setEnabled(false);
    m_parameterPack.clear();
    m_renderPasses.clear();
    m_filterKeyList.clear();
}

void Technique::initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change)
{
    const auto typedChange = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<QTechniqueData>>(change);
    const QTechniqueData &data = typedChange->data;

    m_graphicsApiFilterData = data.graphicsApiFilterData;
    m_filterKeyList = data.filterKeyIds;
    m_parameterPack.setParameters(data.parameterIds);
    m_renderPasses = data.renderPassIds;
}

void Technique::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    switch (e->type()) {
    case PropertyUpdated: {
        const auto change = qSharedPointerCast<QPropertyUpdatedChange>(e);
        if (change->propertyName() == QByteArrayLiteral("graphicsApiFilterData")) {
            GraphicsApiFilterData filterData = change->value().value<GraphicsApiFilterData>();
            m_graphicsApiFilterData = filterData;
        }
        break;
    }

    case PropertyValueAdded: {
        const auto change = qSharedPointerCast<QPropertyNodeAddedChange>(e);
        if (change->propertyName() == QByteArrayLiteral("pass"))
            appendRenderPass(change->addedNodeId());
        else if (change->propertyName() == QByteArrayLiteral("parameter"))
            m_parameterPack.appendParameter(change->addedNodeId());
        else if (change->propertyName() == QByteArrayLiteral("filterKeys"))
            appendFilterKey(change->addedNodeId());
        break;
    }

    case PropertyValueRemoved: {
        const auto change = qSharedPointerCast<QPropertyNodeRemovedChange>(e);
        if (change->propertyName() == QByteArrayLiteral("pass"))
            removeRenderPass(change->removedNodeId());
        else if (change->propertyName() == QByteArrayLiteral("parameter"))
            m_parameterPack.removeParameter(change->removedNodeId());
        else if (change->propertyName() == QByteArrayLiteral("filterKeys"))
            removeFilterKey(change->removedNodeId());
        break;
    }

    default:
        break;
    }
    markDirty(AbstractRenderer::AllDirty);
    BackendNode::sceneChangeEvent(e);
}

QVector<Qt3DCore::QNodeId> Technique::parameters() const
{
    return m_parameterPack.parameters();
}

void Technique::appendRenderPass(Qt3DCore::QNodeId renderPassId)
{
    if (!m_renderPasses.contains(renderPassId))
        m_renderPasses.append(renderPassId);
}

void Technique::removeRenderPass(Qt3DCore::QNodeId renderPassId)
{
    m_renderPasses.removeOne(renderPassId);
}

QVector<Qt3DCore::QNodeId> Technique::filterKeys() const
{
    return m_filterKeyList;
}

QVector<Qt3DCore::QNodeId> Technique::renderPasses() const
{
    return m_renderPasses;
}

const GraphicsApiFilterData *Technique::graphicsApiFilter() const
{
    return &m_graphicsApiFilterData;
}

void Technique::appendFilterKey(Qt3DCore::QNodeId criterionId)
{
    if (!m_filterKeyList.contains(criterionId))
        m_filterKeyList.append(criterionId);
}

void Technique::removeFilterKey(Qt3DCore::QNodeId criterionId)
{
    m_filterKeyList.removeOne(criterionId);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
