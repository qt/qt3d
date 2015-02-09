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

#include "renderparameter_p.h"
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DRenderer/qparameter.h>
#include <Qt3DRenderer/qtexture.h>
#include <Qt3DRenderer/qshaderdata.h>

#include <Qt3DRenderer/private/managers_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

RenderParameter::RenderParameter()
    : QBackendNode()
    , m_shaderDataManager(Q_NULLPTR)
    , m_textureManager(Q_NULLPTR)
{
}

void RenderParameter::updateFromPeer(QNode *mat)
{
    QParameter *param = static_cast<QParameter *>(mat);
    m_name = param->name();
    m_value = toBackendValue(param->value());
}

void RenderParameter::sceneChangeEvent(const QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);

    if (e->type() == NodeUpdated) {
        if (propertyChange->propertyName() == QByteArrayLiteral("name"))
            m_name = propertyChange->value().toString();
        else if (propertyChange->propertyName() == QByteArrayLiteral("value"))
            m_value = toBackendValue(propertyChange->value());
    }
}

QString RenderParameter::name() const
{
    return m_name;
}

QVariant RenderParameter::value() const
{
    return m_value;
}

void RenderParameter::setShaderDataManager(ShaderDataManager *shaderDataManager)
{
    m_shaderDataManager = shaderDataManager;
}

void RenderParameter::setTextureManager(TextureManager *textureManager)
{
    m_textureManager = textureManager;
}

QVariant RenderParameter::toBackendValue(const QVariant &value)
{
    QNode *node = value.value<QNode *>();

    if (node == Q_NULLPTR) {
        return value;
    } else if (qobject_cast<QAbstractTextureProvider*>(node)) {
        return QVariant::fromValue(static_cast<RenderTexture*>(createBackendNode(node)));
    } else if (qobject_cast<QShaderData*>(node)) {
        return QVariant::fromValue(static_cast<RenderShaderData*>(createBackendNode(node)));
    } else {
        qFatal("Texture and ShaderData are the only types of Node allowed as parameters");
        return QVariant();
    }
}

} // Render

} // Qt3D

QT_END_NAMESPACE
