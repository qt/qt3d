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

#include <shaderprogram.h>

namespace Qt3D
{

RenderShader::RenderShader(ShaderProgram *peer) :
    m_program(NULL),
    m_peer(peer)
{
    Q_ASSERT(peer);
}

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

void RenderShader::setStandardUniform(Parameter::StandardUniform su, QString name)
{
    Q_ASSERT(su != Parameter::None);
    Q_ASSERT(!name.isEmpty());
    int index = static_cast<int>(su);
    if (index >= m_standardUniformNames.size())
        m_standardUniformNames.resize(index + 1);
    m_standardUniformNames[index] = name;
}

QString RenderShader::name() const
{
    return m_peer->objectName();
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

    m_standardUniformLocations.resize(m_standardUniformNames.size());
    for (int i=0; i<m_standardUniformNames.size(); ++i) {
        QString n = m_standardUniformNames.at(i);
        if (n.isEmpty()) {
            m_standardUniformLocations[i] = -1;
            continue;
        }

        m_standardUniformLocations[i] = p->uniformLocation(n);
       // qDebug() << "resolved location" << m_standardUniformLocations[i] <<
       //             "for uniform" << n;
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

} // of namespace Qt3D
