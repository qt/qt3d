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

#include <Qt3DRenderer/private/rendereffect_p.h>
#include <Qt3DRenderer/private/renderer_p.h>
#include <Qt3DRenderer/private/renderscenebuilder_p.h>
#include <Qt3DRenderer/private/techniquemanager_p.h>
#include <Qt3DRenderer/qeffect.h>
#include <Qt3DRenderer/qparameter.h>
#include <Qt3DRenderer/rendereraspect.h>

#include <Qt3DCore/qaspectmanager.h>
#include <Qt3DCore/qchangearbiter.h>
#include <Qt3DCore/qscenepropertychange.h>

#include <QVariant>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

RenderEffect::RenderEffect()
    : m_renderer(Q_NULLPTR)
{
}

RenderEffect::~RenderEffect()
{
    cleanup();
}

void RenderEffect::cleanup()
{
    if (m_renderer != Q_NULLPTR && !m_effectUuid.isNull())
        m_renderer->rendererAspect()->aspectManager()->changeArbiter()->unregisterObserver(this, m_effectUuid);
}

void RenderEffect::setPeer(QEffect *effect)
{
    QUuid peerUuid;
    if (effect != Q_NULLPTR)
        peerUuid = effect->uuid();
    if (peerUuid != m_effectUuid) {
        QChangeArbiter *arbiter = m_renderer->rendererAspect()->aspectManager()->changeArbiter();
        if (!m_effectUuid.isNull()) {
            arbiter->unregisterObserver(this, m_effectUuid);
            m_techniques.clear();
            m_parameterPack.clear();
        }
        m_effectUuid = peerUuid;
        if (!m_effectUuid.isNull()) {
            arbiter->registerObserver(this, m_effectUuid, NodeAdded|NodeRemoved|ComponentUpdated);

            Q_FOREACH (QTechnique *t, effect->techniques())
                appendRenderTechnique(t);

            Q_FOREACH (QParameter *p, effect->parameters())
                m_parameterPack.appendParameter(p);
        }
    }
}

void RenderEffect::setRenderer(Renderer *renderer)
{
    m_renderer = renderer;
    m_parameterPack.setRenderer(m_renderer);
}

void RenderEffect::sceneChangeEvent(const QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
    QVariant propertyValue = propertyChange->value();
    switch (e->type()) {

    case NodeAdded:
        if (propertyChange->propertyName() == QByteArrayLiteral("technique")) {
            appendRenderTechnique(propertyValue.value<QTechnique *>());
        }
        // We don't need a RenderParameter as we store them in a QHash[QString name] = QVariant value
        else if (propertyChange->propertyName() == QByteArrayLiteral("parameter")) {
            m_parameterPack.appendParameter(propertyValue.value<QParameter*>());
        }
        break;

    case NodeRemoved:
        if (propertyChange->propertyName() == QByteArrayLiteral("technique")) {
            QUuid techniqueUuid = propertyValue.value<QUuid>();
            m_techniques.removeOne(techniqueUuid);
        }
        else if (propertyChange->propertyName() == QByteArrayLiteral("parameter")) {
            m_parameterPack.removeParameter(propertyValue.value<QParameter*>());
        }
        break;

    default :
        break;
    }
}

void RenderEffect::appendRenderTechnique(QTechnique *technique)
{
    if (!technique)
        return ;
    if (!m_techniques.contains(technique->uuid()))
        m_techniques.append(technique->uuid());
}

const QHash<QString, QVariant> RenderEffect::parameters() const
{
    return m_parameterPack.namedValues();
}

QUuid RenderEffect::effectUuid() const
{
    return m_effectUuid;
}

QList<QUuid> RenderEffect::techniques() const
{
    return m_techniques;
}

} // Render

} // Qt3D

QT_END_NAMESPACE
