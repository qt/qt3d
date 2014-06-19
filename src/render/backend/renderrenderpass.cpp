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

#include "renderrenderpass.h"
#include "renderer.h"
#include "rendereraspect.h"
#include <Qt3DCore/qaspectmanager.h>
#include <Qt3DCore/qchangearbiter.h>
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DCore/qabstractshader.h>
#include <Qt3DRenderer/parametermapper.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

RenderRenderPass::RenderRenderPass()
    : m_renderer(Q_NULLPTR)
    , m_peer(Q_NULLPTR)
    , m_shader(Q_NULLPTR)
{
}

void RenderRenderPass::setRenderer(Renderer *renderer)
{
    m_renderer = renderer;
}

void RenderRenderPass::setPeer(RenderPass *peer)
{
    if (m_peer != peer) {
        QChangeArbiter *arbiter = m_renderer->rendererAspect()->aspectManager()->changeArbiter();
        if (m_peer)
            arbiter->unregisterObserver(this, m_peer);
        m_peer = peer;
        m_shader = Q_NULLPTR;
        if (m_peer) {
            arbiter->registerObserver(this, m_peer, ComponentUpdated);
            m_shader = m_peer->shaderProgram();
            // TO DO -> Have backend classes for Bindings and Parameters so that we can easily monitor for updates
            m_bindings = m_peer->bindings();
        }
    }
}

void RenderRenderPass::sceneChangeEvent(const QSceneChangePtr &e)
{
    if (e->m_type == ComponentUpdated) {
        QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        if (propertyChange->m_propertyName == QByteArrayLiteral("shaderProgram"))
            ;// TO DO: to be completed
    }
}

QAbstractShader *RenderRenderPass::shaderProgram() const
{
    return m_shader;
}

QList<ParameterMapper *> RenderRenderPass::bindings() const
{
    return m_bindings;
}

} // Render

} // Qt3D

QT_END_NAMESPACE
