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

#include "rendermaterial_p.h"
#include "qgraphicscontext_p.h"
#include "rendertechnique_p.h"
#include "rendertextureprovider_p.h"
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
    , m_textureProvider(Q_NULLPTR)
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
    if (mat->effect() != Q_NULLPTR)
        m_effectUuid = mat->effect()->uuid();
    Q_FOREACH (QParameter *p, mat->parameters())
        m_parameterPack.appendParameter(p->uuid());
}

void RenderMaterial::sceneChangeEvent(const QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);

    switch (e->type()) {
    case NodeUpdated: {
        // Check for effect change
        break;
    }
        // Check for shader parameter
    case NodeAdded: {
        if (propertyChange->propertyName() == QByteArrayLiteral("parameter"))
            m_parameterPack.appendParameter(propertyChange->value().value<QNodeUuid>());
        else if (propertyChange->propertyName() == QByteArrayLiteral("effect"))
                m_effectUuid = propertyChange->value().value<QNodeUuid>();
        break;
    }
    case NodeRemoved: {
        if (propertyChange->propertyName() == QByteArrayLiteral("parameter"))
            m_parameterPack.removeParameter(propertyChange->value().value<QNodeUuid>());
        else if (propertyChange->propertyName() == QByteArrayLiteral("effect"))
            m_effectUuid = QNodeUuid();
        break;
    }

    default:
        break;
    }
}

QList<QNodeUuid> RenderMaterial::parameters() const
{
    return m_parameterPack.parameters();
}

QNodeUuid RenderMaterial::effect() const
{
    return m_effectUuid;
}

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
