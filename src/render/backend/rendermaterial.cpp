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
#include "parameter.h"
#include <technique.h> // for Parameter
#include <material.h>

#include <Qt3DCore/qaspectmanager.h>
#include <Qt3DCore/qscenepropertychange.h>

#include <QOpenGLShaderProgram>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

RenderMaterial::RenderMaterial()
    : m_rendererAspect(Q_NULLPTR)
    , m_peer(Q_NULLPTR)
    , m_textureProvider(Q_NULLPTR)
{
}

RenderMaterial::~RenderMaterial()
{
}

void RenderMaterial::setPeer(Material *mat)
{
    if (m_peer != mat) {
        QChangeArbiter *arbiter = m_rendererAspect->aspectManager()->changeArbiter();
        if (m_peer)
            arbiter->unregisterObserver(this, m_peer);
        m_peer = mat;
        // Register for changes
        if (m_peer) {
            arbiter->registerObserver(this, m_peer, ComponentUpdated);

            m_parameters.clear();
            m_parameterPack.clear();
            Q_FOREACH (Parameter *p, m_peer->parameters()) {
                m_parameters[p->name()] = p;
                m_parameterPack.appendParameter(p);
            }
        }
    }
}

void RenderMaterial::setRendererAspect(RendererAspect *rendererAspect)
{
    m_rendererAspect = rendererAspect;
    m_parameterPack.setRendererAspect(m_rendererAspect);
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

void RenderMaterial::syncParametersFromPeer()
{
    // To be replace by sceneChangeEvent further on
    Q_ASSERT(m_peer);
    Q_FOREACH (Parameter *param, m_peer->parameters()) {
        if (!param->value().isValid()) {
            qWarning() << Q_FUNC_INFO << "bad value:" << param->value() << "for parameter" << param->name();
            continue;
        }
//        setParameter(param->name(), param->value());
    }

// FIXME - should do this on demand, otherwise we're re-uploading
// for no good reason
//    foreach (QString nm, m_peer->textureValues().keys()) {
//        Texture* t = m_peer->textureValues().value(nm);
//        setTextureParameter(nm, m_textureProvider->get(t));
//    }
}

void RenderMaterial::sceneChangeEvent(const QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);

    switch (e->m_type) {
    case ComponentUpdated: {
        // Check for shader parameter
        // Check for effect change
//        QVariant propertyValue = propertyChange->m_value;
        if (propertyChange->m_propertyName == QByteArrayLiteral("effect")) {

        }
        else {
        }
        break;
    }
    case ComponentAdded: {
        Parameter *param = Q_NULLPTR;
        if (propertyChange->m_propertyName == QByteArrayLiteral("parameter") &&
                (param = propertyChange->m_value.value<Parameter*>()) != Q_NULLPTR)
            m_parameterPack.appendParameter(param);
        break;
    }
    case ComponentRemoved: {
        Parameter *param = Q_NULLPTR;
        if (propertyChange->m_propertyName == QByteArrayLiteral("parameter") &&
                (param = propertyChange->m_value.value<Parameter*>()) != Q_NULLPTR)
            m_parameterPack.removeParameter(param);
        break;
    }

    default:
        break;
    }
}

QHash<QString, Parameter *> RenderMaterial::parameters() const
{
    return m_parameters;
}

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
