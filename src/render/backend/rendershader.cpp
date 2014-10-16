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

#include "rendershader_p.h"

#include <QDebug>
#include <QFile>
#include <QOpenGLContext>
#include <QOpenGLShaderProgram>
#include <qshaderprogram.h>
#include <Qt3DRenderer/private/qgraphicscontext_p.h>
#include <Qt3DRenderer/private/attachmentpack_p.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

RenderShader::RenderShader()
    : QBackendNode()
    , m_program(Q_NULLPTR)
    , m_isLoaded(false)
{
    m_shaderCode.resize(static_cast<int>(QShaderProgram::Compute) + 1);
}

RenderShader::~RenderShader()
{
    cleanup();
}

void RenderShader::cleanup()
{
    m_isLoaded = false;
}

void RenderShader::updateFromPeer(QNode *peer)
{
    QShaderProgram *shader = static_cast<QShaderProgram *>(peer);

    for (int i = QShaderProgram::Vertex; i <= QShaderProgram::Compute; ++i) {
        m_shaderCode[i].clear();
    }

    m_isLoaded = false;

    for (int i = QShaderProgram::Vertex; i <= QShaderProgram::Compute; ++i) {
        QShaderProgram::ShaderType type = static_cast<const QShaderProgram::ShaderType>(i);
        m_shaderCode[i] = shader->shaderCode(type);
    }
}

QStringList RenderShader::uniformsNames() const
{
    return m_uniforms.keys();
}

QStringList RenderShader::attributesNames() const
{
    return m_attributes.keys();
}

void RenderShader::sceneChangeEvent(const QSceneChangePtr &e)
{
    if (e->type() == NodeUpdated) {
        QScenePropertyChangePtr propertyChange = e.staticCast<QScenePropertyChange>();
        QVariant propertyValue = propertyChange->value();

        if (propertyChange->propertyName() == QByteArrayLiteral("vertexSourceCode")) {
            m_shaderCode[QShaderProgram::Vertex] = propertyValue.toByteArray();
            m_isLoaded = false;
        } else if (propertyChange->propertyName() == QByteArrayLiteral("fragmentSourceCode")) {
            m_shaderCode[QShaderProgram::Fragment] = propertyValue.toByteArray();
            m_isLoaded = false;
        } else if (propertyChange->propertyName() == QByteArrayLiteral("tessellationControlSourceCode")) {
            m_shaderCode[QShaderProgram::TessellationControl] = propertyValue.toByteArray();
            m_isLoaded = false;
        } else if (propertyChange->propertyName() == QByteArrayLiteral("tessellationEvaluationSourceCode")) {
            m_shaderCode[QShaderProgram::TessellationEvaluation] = propertyValue.toByteArray();
            m_isLoaded = false;
        } else if (propertyChange->propertyName() == QByteArrayLiteral("geometrySourceCode")) {
            m_shaderCode[QShaderProgram::Geometry] = propertyValue.toByteArray();
            m_isLoaded = false;
        } else if (propertyChange->propertyName() == QByteArrayLiteral("computeSourceCode")) {
            m_shaderCode[QShaderProgram::Compute] = propertyValue.toByteArray();
            m_isLoaded = false;
        }
    }
}

bool RenderShader::isLoaded() const
{
    return m_isLoaded;
}

/*!
 * Must be called with a valid, current QOpenGLContext
 */
QOpenGLShaderProgram *RenderShader::getOrCreateProgram(QGraphicsContext *ctx)
{
    if (!m_isLoaded) {
        delete m_program;
        m_program = createProgram(ctx);
        if (!m_program)
            m_program = createDefaultProgram();
        m_isLoaded = true;
    }
    return m_program;
}

void RenderShader::updateUniforms(QGraphicsContext *ctx, const QUniformPack &pack)
{
    const QHash<QString, const QUniformValue* > &values = pack.uniforms();
    Q_FOREACH (const QString &uniformName, values.keys()) {
        if (m_uniforms.contains(uniformName)) {
            values.value(uniformName)->apply(ctx, m_uniforms.value(uniformName));
        }
    }
}

void RenderShader::setFragOutputs(const QHash<QString, int> &fragOutputs)
{
    m_fragOutputs = fragOutputs;
}

static QOpenGLShader::ShaderType shaderType(QShaderProgram::ShaderType type)
{
    switch (type) {
    case QShaderProgram::Vertex: return QOpenGLShader::Vertex;
    case QShaderProgram::TessellationControl: return QOpenGLShader::TessellationControl;
    case QShaderProgram::TessellationEvaluation: return QOpenGLShader::TessellationEvaluation;
    case QShaderProgram::Geometry: return QOpenGLShader::Geometry;
    case QShaderProgram::Fragment: return QOpenGLShader::Fragment;
    case QShaderProgram::Compute: return QOpenGLShader::Compute;
    default: Q_UNREACHABLE();
    }
}

QOpenGLShaderProgram* RenderShader::createProgram(QGraphicsContext *context)
{
    Q_ASSERT(QOpenGLContext::currentContext());
    // Scoped pointer so early-returns delete automatically
    QScopedPointer<QOpenGLShaderProgram> p(new QOpenGLShaderProgram);

    for (int i = QShaderProgram::Vertex; i <= QShaderProgram::Compute; ++i) {
        QShaderProgram::ShaderType type = static_cast<const QShaderProgram::ShaderType>(i);
        // Compile shaders
        if (!m_shaderCode[type].isEmpty()) {
            if (!p->addShaderFromSourceCode(shaderType(type), m_shaderCode[type]))
                qWarning() << "Failed to compile shader:" << p->log();
        }
    }

    // Call glBindFragDataLocation and link the program
    context->bindFragOutputs(p->programId(), m_fragOutputs);
    if (!p->link()) {
        qWarning() << "Failed to link shader program:" << p->log();
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

void RenderShader::initializeUniforms(const QVector<ShaderUniform> &uniformsDescription)
{
    for (int i = 0; i < uniformsDescription.size(); i++)
        m_uniforms.insert(uniformsDescription[i].m_name, uniformsDescription[i]);
}

void RenderShader::initializeAttributes(const QVector<ShaderAttribute> &attributesDescription)
{
    for (int i = 0; i < attributesDescription.size(); i++)
        m_attributes.insert(attributesDescription[i].m_name, attributesDescription[i]);
}

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
