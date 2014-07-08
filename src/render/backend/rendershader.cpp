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
#include <QOpenGLContext>
#include <QOpenGLShaderProgram>
#include <Qt3DRenderer/QGraphicsContext>
#include <qshaderprogram.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

RenderShader::RenderShader() :
    m_program(Q_NULLPTR),
    m_peer(Q_NULLPTR)
{
}

void RenderShader::setPeer(QShaderProgram *peer)
{
    Q_ASSERT(peer);
    m_peer = peer;
}

/*!
 * Must be called with a valid, current QOpenGLContext
 */
QOpenGLShaderProgram *RenderShader::getOrCreateProgram()
{
    if (!m_program) {
        // waiting for shader source to be loaded
        if (!m_peer->isLoaded()) {

            // FIXME - should not be done in the backend!
            m_peer->load();
            //return NULL;
        }

        m_program = createProgram();
        if (!m_program)
            m_program = createDefaultProgram();
    }

    return m_program;
}

QString RenderShader::name() const
{
    return m_peer->objectName();
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
    bool ok = p->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                         m_peer->vertexSourceCode());
    if (!ok) {
        qWarning() << "bad vertex source:" << m_program->log();
        return NULL;
    }

    ok = p->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                    m_peer->fragmentSourceCode());
    if (!ok) {
        qWarning() << "bad fragment source:" << m_program->log();
        return NULL;
    }

    ok = p->link();
    if (!ok) {
        qWarning() << "program failed to link:" << m_program->log();
        return NULL;
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

QStringList RenderShader::uniformsNames() const
{
    return m_uniforms.keys();
}

QStringList RenderShader::attributesNames() const
{
    return m_attributes.keys();
}

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
