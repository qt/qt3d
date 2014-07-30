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

#include "rendershader.h"

#include <QDebug>
#include <QFile>
#include <QOpenGLContext>
#include <QOpenGLShaderProgram>
#include <qshaderprogram.h>
#include <Qt3DRenderer/QGraphicsContext>
#include <Qt3DRenderer/renderer.h>
#include <Qt3DRenderer/rendereraspect.h>
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DCore/qaspectmanager.h>
#include <Qt3DCore/qchangearbiter.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

RenderShader::RenderShader() :
    m_program(Q_NULLPTR),
    m_renderer(Q_NULLPTR),
    m_isLoaded(false)
{
}

RenderShader::~RenderShader()
{
    cleanup();
}

void RenderShader::cleanup()
{
    m_isLoaded = false;
}

void RenderShader::setPeer(QShaderProgram *peer)
{
    Q_ASSERT(peer);

    QUuid peerUuid = peer->uuid();

    if (peerUuid != m_shaderUuid) {
        QChangeArbiter *arbiter = m_renderer->rendererAspect()->aspectManager()->changeArbiter();

        if (!m_shaderUuid.isNull()) {
            arbiter->unregisterObserver(this, m_shaderUuid);
            m_shaderUuid = QUuid();
            m_vertexSourceCode.clear();
            m_fragmentSourceCode.clear();
        }
        m_isLoaded = false;
        m_shaderUuid = peerUuid;
        arbiter->registerObserver(this, m_shaderUuid, NodeUpdated);
        m_vertexSourceCode = peer->vertexSourceCode();
        m_vertexSourceFile = peer->vertexSourceFile();
        m_fragmentSourceCode = peer->fragmentSourceCode();
        m_fragmentSourceFile = peer->fragmentSourceFile();
    }
}

void RenderShader::setRenderer(Renderer *renderer)
{
    m_renderer = renderer;
}

QStringList RenderShader::uniformsNames() const
{
    return m_uniforms.keys();
}

QStringList RenderShader::attributesNames() const
{
    return m_attributes.keys();
}

QUuid RenderShader::shaderUuid() const
{
    return m_shaderUuid;
}

void RenderShader::sceneChangeEvent(const QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = e.staticCast<QScenePropertyChange>();
    QVariant propertyValue = propertyChange->value();

    if (e->type() == NodeUpdated) {
        if (propertyChange->propertyName() == QByteArrayLiteral("vertexSourceCode")) {
            m_vertexSourceCode = propertyValue.toByteArray();
            m_isLoaded = false;
        }
        else if (propertyChange->propertyName() == QByteArrayLiteral("fragmentSourceCode")) {
            m_fragmentSourceCode = propertyValue.toByteArray();
            m_isLoaded = false;
        }
        else if (propertyChange->propertyName() == QByteArrayLiteral("vertexSourceFile")) {
            m_vertexSourceFile = propertyValue.toString();
            m_isLoaded = false;
        }
        else if (propertyChange->propertyName() == QByteArrayLiteral("fragmentSourceFile")) {
            m_fragmentSourceFile = propertyValue.toString();
            m_isLoaded = false;
        }
    }
}

/*!
 * Must be called with a valid, current QOpenGLContext
 */
QOpenGLShaderProgram *RenderShader::getOrCreateProgram()
{
    if (!m_isLoaded) {
        delete m_program;
        m_program = createProgram();
        if (!m_program)
            m_program = createDefaultProgram();
        m_isLoaded = true;
    }
    return m_program;
}

void RenderShader::updateUniforms(const QUniformPack &pack)
{
    const QHash<QString, const QUniformValue* > &values = pack.uniforms();
    Q_FOREACH (const QString &uniformName, values.keys()) {
        if (m_uniforms.contains(uniformName)) {
            values[uniformName]->apply(m_program, m_uniforms[uniformName], uniformName);
        }
    }
}

QOpenGLShaderProgram* RenderShader::createProgram()
{
    Q_ASSERT(QOpenGLContext::currentContext());
    // scoped pointer so early-returns delete automatically
    QScopedPointer<QOpenGLShaderProgram> p(new QOpenGLShaderProgram);

    if (!m_fragmentSourceFile.isEmpty()) {
        QFile f(m_fragmentSourceFile);
        if (!f.exists()) {
            qWarning() << "couldn't find shader source file:" << m_fragmentSourceFile;
        } else {
            f.open(QIODevice::ReadOnly);
            m_fragmentSourceCode = f.readAll();
            f.close();
        }
    }
    if (!m_vertexSourceFile.isEmpty()) {
        QFile vs(m_vertexSourceFile);
        if (!vs.exists()) {
            qWarning() << "couldn't find shader source file:" << m_vertexSourceFile;
        } else {
            vs.open(QIODevice::ReadOnly);
            m_vertexSourceCode = vs.readAll();
            vs.close();
        }
    }

    bool ok = p->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                         m_vertexSourceCode);
    if (!ok) {
        qWarning() << "bad vertex source:" << p->log();
        return Q_NULLPTR;
    }

    ok = p->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                    m_fragmentSourceCode);
    if (!ok) {
        qWarning() << "bad fragment source:" << p->log();
        return Q_NULLPTR;
    }

    ok = p->link();
    if (!ok) {
        qWarning() << "program failed to link:" << p->log();
        return Q_NULLPTR;
    }
    // take from scoped-pointer so it doesn't get deleted
    return p.take();
}

QOpenGLShaderProgram* RenderShader::createDefaultProgram()
{
    QOpenGLShaderProgram* p = new QOpenGLShaderProgram;
    p->addShaderFromSourceCode(QOpenGLShader::Vertex,
                               "");

    p->addShaderFromSourceCode(QOpenGLShader::Fragment,
                               "");

    p->link();

    return p;
}

void RenderShader::initializeUniforms(const QVector<QPair<QString, int> > &uniformsNamesAndLocations)
{
    for (int i = 0; i < uniformsNamesAndLocations.size(); i++)
        m_uniforms[uniformsNamesAndLocations[i].first] = uniformsNamesAndLocations[i].second;
}

void RenderShader::initializeAttributes(const QVector<QPair<QString, int> > &attributesNameAndLocation)
{
    for (int i = 0; i < attributesNameAndLocation.size(); i++)
        m_attributes[attributesNameAndLocation[i].first] = attributesNameAndLocation[i].second;
}

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
