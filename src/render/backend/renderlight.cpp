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

#include "renderlight_p.h"
#include "qabstractlight.h"
#include <Qt3DCore/qscenepropertychange.h>
#include <QVariant>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

RenderLight::RenderLight()
    : QBackendNode()
{
}

RenderLight::~RenderLight()
{
    cleanup();
}

void RenderLight::cleanup()
{
}

void RenderLight::updateFromPeer(QNode *peer)
{
    QAbstractLight *light = static_cast<QAbstractLight *>(peer);

    m_lightProperties = light->lightProperties();
    // Properties common to all lights
    m_lightProperties[QStringLiteral("color")] = light->color();
    m_lightProperties[QStringLiteral("intensity")] = light->intensity();
    m_lightUniformName = light->lightUniformName();
    m_lightBlockName = light->lightBlockName();
}

QHash<QString, QVariant> RenderLight::lightProperties() const
{
    return m_lightProperties;
}

void RenderLight::sceneChangeEvent(const QSceneChangePtr &e)
{
    if (e->type() == NodeUpdated) {
        QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        m_lightProperties[QString::fromUtf8(propertyChange->propertyName())] = propertyChange->value();
    }
}

QString RenderLight::lightBlockName() const
{
    return m_lightBlockName;
}

QString RenderLight::lightUniformName() const
{
    return m_lightUniformName;
}

} // Qt3D

} // Render

QT_END_NAMESPACE
