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

#include <Qt3DRender/private/effect_p.h>
#include <Qt3DRender/private/renderer_p.h>
#include <Qt3DRender/qeffect.h>
#include <Qt3DRender/qparameter.h>

#include <Qt3DCore/qscenepropertychange.h>

#include <QVariant>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

Effect::Effect()
    : QBackendNode()
{
}

Effect::~Effect()
{
    cleanup();
}

void Effect::cleanup()
{
}

void Effect::updateFromPeer(Qt3DCore::QNode *peer)
{
    QEffect *effect = static_cast<QEffect *>(peer);

    m_techniques.clear();
    m_parameterPack.clear();

    Q_FOREACH (QTechnique *t, effect->techniques())
        appendRenderTechnique(t->id());

    Q_FOREACH (QParameter *p, effect->parameters())
        m_parameterPack.appendParameter(p->id());
}

void Effect::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
    QVariant propertyValue = propertyChange->value();
    switch (e->type()) {

    case NodeAdded:
        if (propertyChange->propertyName() == QByteArrayLiteral("technique"))
            appendRenderTechnique(propertyValue.value<QNodeId>());
        else if (propertyChange->propertyName() == QByteArrayLiteral("parameter"))
            m_parameterPack.appendParameter(propertyValue.value<QNodeId>());
        break;

    case NodeRemoved:
        if (propertyChange->propertyName() == QByteArrayLiteral("technique"))
            m_techniques.removeOne(propertyValue.value<QNodeId>());
        else if (propertyChange->propertyName() == QByteArrayLiteral("parameter"))
            m_parameterPack.removeParameter(propertyValue.value<QNodeId>());
        break;

    default :
        break;
    }
}

void Effect::appendRenderTechnique(const Qt3DCore::QNodeId &technique)
{
    if (!m_techniques.contains(technique))
        m_techniques.append(technique);
}

QList<Qt3DCore::QNodeId> Effect::techniques() const
{
    return m_techniques;
}

QList<Qt3DCore::QNodeId> Effect::parameters() const
{
    return m_parameterPack.parameters();
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
