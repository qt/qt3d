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

#include "rendermaterial.h"

#include "rendereraspect.h"
#include "qgraphicscontext.h"
#include "rendertechnique.h"
#include "rendertextureprovider.h"
#include "qparameter.h"
#include "renderer.h"
#include <qtechnique.h> // for Parameter
#include <qmaterial.h>
#include <Qt3DCore/qaspectmanager.h>
#include <Qt3DCore/qscenepropertychange.h>

#include <QOpenGLShaderProgram>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

RenderMaterial::RenderMaterial()
    : m_renderer(Q_NULLPTR)
    , m_peer(Q_NULLPTR)
    , m_textureProvider(Q_NULLPTR)
{
}

RenderMaterial::~RenderMaterial()
{
    if (m_renderer != Q_NULLPTR && m_peer != Q_NULLPTR)
        m_renderer->rendererAspect()->aspectManager()->changeArbiter()->unregisterObserver(this, m_peer);
}

void RenderMaterial::setPeer(QMaterial *mat)
{
    if (m_peer != mat) {
        QChangeArbiter *arbiter = m_renderer->rendererAspect()->aspectManager()->changeArbiter();
        if (m_peer)
            arbiter->unregisterObserver(this, m_peer);
        m_peer = mat;
        // Register for changes
        if (m_peer) {
            arbiter->registerObserver(this, m_peer, ComponentUpdated);
            m_parameterPack.clear();
            Q_FOREACH (QParameter *p, m_peer->parameters()) {
                m_parameterPack.appendParameter(p);
            }
        }
    }
}

void RenderMaterial::setRenderer(Renderer *renderer)
{
    m_renderer = renderer;
    m_parameterPack.setRenderer(m_renderer);
}

//void RenderMaterial::setTextureParameter(QString paramName, RenderTexturePtr tex)
//{
//    Q_ASSERT(tex);
//    Parameter* param = m_technique->parameterByName(paramName);
//    if (!param)
//        return;

//    Q_ASSERT(param->isTextureType());

//    QStringList names(m_technique->glslNamesForUniformParameter(paramName));
//    Q_ASSERT(names.count() == (int) m_technique->passCount());

//    for (unsigned int p=0; p<m_technique->passCount(); ++p) {
//        if (names.at(p).isEmpty())
//            continue; // not set in this pass

//        m_packs.at(p)->setTexture(names.at(p), tex);
//    } // of pass iteration
//}

void RenderMaterial::sceneChangeEvent(const QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);

    switch (e->type()) {
    case ComponentUpdated: {
        // Check for effect change
        if (propertyChange->propertyName() == QByteArrayLiteral("effect")) {

        }
        else {
        }
        break;
    }
    // Check for shader parameter
    case ComponentAdded: {
        QParameter *param = Q_NULLPTR;
        if (propertyChange->propertyName() == QByteArrayLiteral("parameter") &&
                (param = propertyChange->value().value<QParameter*>()) != Q_NULLPTR)
            m_parameterPack.appendParameter(param);
        break;
    }
    case ComponentRemoved: {
        QParameter *param = Q_NULLPTR;
        if (propertyChange->propertyName() == QByteArrayLiteral("parameter") &&
                (param = propertyChange->value().value<QParameter*>()) != Q_NULLPTR)
            m_parameterPack.removeParameter(param);
        break;
    }

    default:
        break;
    }
}

const QHash<QString, QVariant> RenderMaterial::parameters() const
{
    return m_parameterPack.namedValues();
}

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
