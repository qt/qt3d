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

#include "rendermaterial_p.h"
#include "qgraphicscontext_p.h"
#include "rendertechnique_p.h"
#include "rendereffect_p.h"
#include "qparameter.h"
#include "qtechnique.h"
#include "qmaterial.h"
#include "qeffect.h"

#include <Qt3DCore/qscenepropertychange.h>
#include <QOpenGLShaderProgram>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

RenderMaterial::RenderMaterial()
    : QBackendNode()
    , m_enabled(true)
{
}

RenderMaterial::~RenderMaterial()
{
    cleanup();
}

void RenderMaterial::cleanup()
{
    m_parameterPack.clear();
}

void RenderMaterial::updateFromPeer(QNode *node)
{
    QMaterial *mat = static_cast<QMaterial *>(node);
    m_parameterPack.clear();
    m_enabled = mat->isEnabled();
    if (mat->effect() != Q_NULLPTR)
        m_effectUuid = mat->effect()->id();
    Q_FOREACH (QParameter *p, mat->parameters())
        m_parameterPack.appendParameter(p->id());
}

void RenderMaterial::sceneChangeEvent(const QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);

    switch (e->type()) {
    case NodeUpdated: {
        if (propertyChange->propertyName() == QByteArrayLiteral("enabled"))
            m_enabled = propertyChange->value().toBool();
        break;
    }
        // Check for shader parameter
    case NodeAdded: {
        if (propertyChange->propertyName() == QByteArrayLiteral("parameter"))
            m_parameterPack.appendParameter(propertyChange->value().value<QNodeId>());
        else if (propertyChange->propertyName() == QByteArrayLiteral("effect"))
            m_effectUuid = propertyChange->value().value<QNodeId>();
        break;
    }
    case NodeRemoved: {
        if (propertyChange->propertyName() == QByteArrayLiteral("parameter"))
            m_parameterPack.removeParameter(propertyChange->value().value<QNodeId>());
        else if (propertyChange->propertyName() == QByteArrayLiteral("effect"))
            m_effectUuid = QNodeId();
        break;
    }

    default:
        break;
    }
}

QList<QNodeId> RenderMaterial::parameters() const
{
    return m_parameterPack.parameters();
}

QNodeId RenderMaterial::effect() const
{
    return m_effectUuid;
}

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
