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
#include <Qt3DCore/qscenepropertychange.h>

#include <QDebug>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

Technique::Technique()
    : BackendNode()
    , m_graphicsApiFilter(Q_NULLPTR)
{
}

Technique::~Technique()
{
    cleanup();
}

void Technique::cleanup()
{
    if (m_graphicsApiFilter)
        delete m_graphicsApiFilter;
    m_graphicsApiFilter = Q_NULLPTR;
    m_parameterPack.clear();
    m_renderPasses.clear();
    m_filterKeyList.clear();
}

void Technique::updateFromPeer(Qt3DCore::QNode *peer)
{
    m_parameterPack.clear();
    m_renderPasses.clear();
    m_filterKeyList.clear();

    if (m_graphicsApiFilter == Q_NULLPTR)
        m_graphicsApiFilter = new QGraphicsApiFilter();

    QTechnique *technique = static_cast<QTechnique *>(peer);

    if (technique != Q_NULLPTR) {
        Q_FOREACH (QParameter *p, technique->parameters())
            m_parameterPack.appendParameter(p->id());
        Q_FOREACH (QRenderPass *rPass, technique->renderPasses())
            appendRenderPass(rPass->id());
        Q_FOREACH (QFilterKey *filterKey, technique->filterKeys())
            appendFilterKey(filterKey->id());

        // Copy GraphicsApiFilter info from frontend GraphicsApiFilter
        QGraphicsApiFilter *peerFilter = technique->graphicsApiFilter();
        m_graphicsApiFilter->copy(*peerFilter);
    }
}

void Technique::initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change)
{
    const auto typedChange = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<QTechniqueData>>(change);
    const auto &data = typedChange->data;

    // TODO: Replace this with a value backend type
    if (m_graphicsApiFilter == Q_NULLPTR)
        m_graphicsApiFilter = new QGraphicsApiFilter();
    m_graphicsApiFilter->setApi(data.api);
    m_graphicsApiFilter->setProfile(data.profile);
    m_graphicsApiFilter->setMajorVersion(data.majorVersion);
    m_graphicsApiFilter->setMinorVersion(data.minorVersion);
    m_graphicsApiFilter->setExtensions(data.extensions);
    m_graphicsApiFilter->setVendor(data.vendor);

    m_filterKeyList = data.filterKeyIds;
    m_parameterPack.setParameters(data.parameterIds);
    m_renderPasses = data.renderPassIds;
}

void Technique::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
    switch (e->type()) {

    case NodeUpdated: {
        if (propertyChange->propertyName() == QByteArrayLiteral("graphicsApiFilter")) {
            QGraphicsApiFilter *filter = propertyChange->value().value<QGraphicsApiFilter *>();
            if (filter != Q_NULLPTR) {
                m_graphicsApiFilter->copy(*filter);
                delete filter;
            }
        }
        break;
    }

    case NodeAdded: {
        if (propertyChange->propertyName() == QByteArrayLiteral("pass")) {
            appendRenderPass(propertyChange->value().value<QNodeId>());
        }
        else if (propertyChange->propertyName() == QByteArrayLiteral("parameter")) {
            m_parameterPack.appendParameter(propertyChange->value().value<QNodeId>());
        }
        else if (propertyChange->propertyName() == QByteArrayLiteral("filterKeys")) {
            appendFilterKey(propertyChange->value().value<QNodeId>());
        }
        break;
    }

    case NodeRemoved: {
        if (propertyChange->propertyName() == QByteArrayLiteral("pass")) {
            removeRenderPass(propertyChange->value().value<QNodeId>());
        }
        else if (propertyChange->propertyName() == QByteArrayLiteral("parameter")) {
            m_parameterPack.removeParameter(propertyChange->value().value<QNodeId>());
        }
        else if (propertyChange->propertyName() == QByteArrayLiteral("filterKeys")) {
            removeFilterKey(propertyChange->value().value<QNodeId>());
        }
        break;
    }

    default:
        break;
    }
    markDirty(AbstractRenderer::AllDirty);
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

QVector<Qt3DCore::QNodeId> Technique::      filterKeys() const
{
    return m_filterKeyList;
}

QVector<Qt3DCore::QNodeId> Technique::renderPasses() const
{
    return m_renderPasses;
}

QGraphicsApiFilter *Technique::graphicsApiFilter() const
{
    return m_graphicsApiFilter;
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
