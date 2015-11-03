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

#include "technique_p.h"

#include <Qt3DRender/qrenderpass.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qtechnique.h>
#include <Qt3DRender/qgraphicsapifilter.h>
#include <Qt3DRender/private/renderer_p.h>
#include <Qt3DRender/private/annotation_p.h>
#include <Qt3DRender/private/shader_p.h>
#include <Qt3DCore/private/qchangearbiter_p.h>
#include <Qt3DCore/qscenepropertychange.h>

#include <QDebug>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

Technique::Technique()
    : QBackendNode()
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
}

void Technique::updateFromPeer(Qt3DCore::QNode *peer)
{
    m_parameterPack.clear();
    m_renderPasses.clear();
    m_annotationList.clear();

    if (m_graphicsApiFilter == Q_NULLPTR)
        m_graphicsApiFilter = new QGraphicsApiFilter();

    QTechnique *technique = static_cast<QTechnique *>(peer);

    if (technique != Q_NULLPTR) {
        Q_FOREACH (QParameter *p, technique->parameters())
            m_parameterPack.appendParameter(p->id());
        Q_FOREACH (QRenderPass *rPass, technique->renderPasses())
            appendRenderPass(rPass->id());
        Q_FOREACH (QAnnotation *annotation, technique->annotations())
            appendAnnotation(annotation->id());

        // Copy GraphicsApiFilter info from frontend GraphicsApiFilter
        QGraphicsApiFilter *peerFilter = technique->graphicsApiFilter();
        m_graphicsApiFilter->copy(*peerFilter);
    }
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
        else if (propertyChange->propertyName() == QByteArrayLiteral("annotation")) {
            appendAnnotation(propertyChange->value().value<QNodeId>());
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
        else if (propertyChange->propertyName() == QByteArrayLiteral("annotation")) {
            removeAnnotation(propertyChange->value().value<QNodeId>());
        }
        break;
    }

    default:
        break;
    }
}

QList<Qt3DCore::QNodeId> Technique::parameters() const
{
    return m_parameterPack.parameters();
}

void Technique::appendRenderPass(const Qt3DCore::QNodeId &renderPassId)
{
    if (!m_renderPasses.contains(renderPassId))
        m_renderPasses.append(renderPassId);
}

void Technique::removeRenderPass(const Qt3DCore::QNodeId &renderPassId)
{
    m_renderPasses.removeOne(renderPassId);
}

QList<Qt3DCore::QNodeId> Technique::annotations() const
{
    return m_annotationList;
}

QList<Qt3DCore::QNodeId> Technique::renderPasses() const
{
    return m_renderPasses;
}

QGraphicsApiFilter *Technique::graphicsApiFilter() const
{
    return m_graphicsApiFilter;
}

void Technique::appendAnnotation(const Qt3DCore::QNodeId &criterionId)
{
    if (!m_annotationList.contains(criterionId))
        m_annotationList.append(criterionId);
}

void Technique::removeAnnotation(const Qt3DCore::QNodeId &criterionId)
{
    m_annotationList.removeOne(criterionId);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
