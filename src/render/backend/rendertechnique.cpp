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

#include "rendertechnique.h"
#include "rendershader.h"

#include <renderpass.h>
#include <technique.h>
#include "rendereraspect.h"
#include "renderer.h"
#include <Qt3DCore/qaspectmanager.h>
#include <Qt3DCore/qchangearbiter.h>
#include <Qt3DCore/qscenepropertychange.h>

#include <QDebug>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

RenderTechnique::RenderTechnique() :
    m_renderer(Q_NULLPTR),
    m_peer(Q_NULLPTR),
    m_passCount(0)
{
}

void RenderTechnique::setRenderer(Renderer *renderer)
{
    m_renderer = renderer;
}

void RenderTechnique::setPeer(Technique *peer)
{
    if (m_peer != peer) {
        QChangeArbiter *arbiter = m_renderer->rendererAspect()->aspectManager()->changeArbiter();
        arbiter->unregisterObserver(this, m_peer);
        m_peer = peer;
        // TO DO: Remove those variables
        m_passCount = peer->renderPasses().count();
        m_passBin.resize(m_passCount);
        m_passShader.resize(m_passCount);
        if (m_peer)
            arbiter->registerObserver(this, m_peer);
    }
}

Technique *RenderTechnique::peer() const
{
    return m_peer;
}

unsigned int RenderTechnique::passCount() const
{
    return m_passCount;
}

void RenderTechnique::setShaderForPass(unsigned int passIndex, RenderShader *s)
{
    Q_ASSERT(passIndex < passCount());
    m_passShader[passIndex] = s;

    // bind standard uniform names

    RenderPass* pass =  qobject_cast<RenderPass*>(m_peer->renderPasses().at(passIndex));
    if (pass) {
        foreach (Parameter* p, m_peer->parameters()) {
            if (!p->isStandardUniform())
                continue;

            QString glslName = pass->glslNameForParameter(p->name());
            if (glslName.isEmpty())
                continue;

            s->setStandardUniform(p->standardUniform(), glslName);
        } // of parameter iteration
    }
}

RenderShader *RenderTechnique::shaderForPass(unsigned int pass)
{
    Q_ASSERT(pass < passCount());
    return  m_passShader.at(pass);
}

void RenderTechnique::setBinForPass(unsigned int pass, RenderBin *bin)
{
    Q_ASSERT(pass < passCount());
    m_passBin[pass] = bin;
}

RenderBin *RenderTechnique::binForPass(unsigned int pass) const
{
    return m_passBin.at(pass);
}

DrawStateSet *RenderTechnique::stateSetForPass(unsigned int pass) const
{
    return qobject_cast<RenderPass*>(m_peer->renderPasses().at(pass))->stateSet();
}

QString RenderTechnique::glslNameForMeshAttribute(unsigned int passIndex, QString meshAttributeName)
{
    // find the parameter
    foreach (Parameter* p, m_peer->parameters()) {
        if (p->meshAttributeName() != meshAttributeName) {
            continue;
        }

        Q_ASSERT((int) passIndex < m_peer->renderPasses().count());
        RenderPass* pass =  qobject_cast<RenderPass*>(m_peer->renderPasses().at(passIndex));

        return pass->glslNameForParameter(p->name());
    } // of parameter iteration

    // mesh attribute is not referenced in this technique
    return QString();
}

Parameter *RenderTechnique::parameterByName(QString name) const
{
    foreach (Parameter* p, m_peer->parameters()) {
        if (p->name() == name)
            return p;
    }

    qWarning() << Q_FUNC_INFO << "couldn't find parameter:" << name
               << "in technique" << m_peer->objectName();
    return NULL;
}

void RenderTechnique::sceneChangeEvent(const QSceneChangePtr &e)
{
    switch (e->m_type) {

    case ComponentAdded: {
        QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        if (propertyChange->m_propertyName == QByteArrayLiteral("pass"))
            ;
        // This will be filled when we have a proper backend RenderPass class
    }
        break;

    case ComponentRemoved: {
        QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        if (propertyChange->m_propertyName == QByteArrayLiteral("pass"))
            ;
        // See above
    }
        break;

    default:
        break;
    }
}

QStringList RenderTechnique::glslNamesForUniformParameter(QString pName) const
{
    QStringList result;
    Parameter* param = parameterByName(pName);
    if (!param)
        return result;

    for (unsigned int p=0; p<m_passCount;++p) {
        RenderPass* pass =  qobject_cast<RenderPass*>(m_peer->renderPasses().at(p));
        // will return an empty QString if the pass doesn't use the parameter,
        // but that's what we want so no probem.
        QString glslName = pass->glslNameForParameter(pName);
        result.append(glslName);
    }

    return result;
}

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
