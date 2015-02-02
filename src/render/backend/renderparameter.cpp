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

#include "renderparameter_p.h"
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DRenderer/qparameter.h>

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
    m_value = param->value();
}

void RenderParameter::sceneChangeEvent(const QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);

    if (e->type() == NodeUpdated) {
        if (propertyChange->propertyName() == QByteArrayLiteral("name"))
            m_name = propertyChange->value().toString();
        else if (propertyChange->propertyName() == QByteArrayLiteral("value"))
            m_value = propertyChange->value();
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

RenderParameterFunctor::RenderParameterFunctor(ParameterManager *parameterManager, ShaderDataManager *shaderDataManager, TextureManager *textureManager)
    : m_parameterManager(parameterManager)
    , m_shaderDataManager(shaderDataManager)
    , m_textureManager(textureManager)
{
}

QBackendNode *RenderParameterFunctor::create(QNode *frontend) const
{
    HParameter parameterNodeHandle = m_parameterManager->getOrAcquireHandle(frontend->id());
    RenderParameter *parameter = m_parameterManager->data(parameterNodeHandle);
    parameter->setShaderDataManager(m_shaderDataManager);
    parameter->setTextureManager(m_textureManager);
    parameter->setPeer(frontend);
    return parameter;
}

QBackendNode *RenderParameterFunctor::get(QNode *frontend) const
{
    return m_parameterManager->lookupResource(frontend->id());
}

void RenderParameterFunctor::destroy(QNode *frontend) const
{
    m_parameterManager->releaseResource(frontend->id());
}

} // Render

} // Qt3D

QT_END_NAMESPACE
