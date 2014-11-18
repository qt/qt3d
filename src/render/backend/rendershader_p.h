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

#ifndef QT3D_RENDER_RENDERSHADER_H
#define QT3D_RENDER_RENDERSHADER_H

#include <QVector>
#include <Qt3DRenderer/private/quniformvalue_p.h>
#include <Qt3DRenderer/private/shadervariables_p.h>
#include <Qt3DCore/qbackendnode.h>

QT_BEGIN_NAMESPACE

class QOpenGLShaderProgram;

namespace Qt3D {

class QShaderProgram;

namespace Render {

class ShaderManager;
class AttachmentPack;

class RenderShader : public QBackendNode
{
public:
    RenderShader();
    ~RenderShader();

    void cleanup();

    void updateFromPeer(QNode *peer);
    void updateUniforms(QGraphicsContext *ctx, const QUniformPack &pack);
    void setFragOutputs(const QHash<QString, int> &fragOutputs);

    QVector<QString> uniformsNames() const;
    QVector<QString> attributesNames() const;
    QVector<QString> uniformBlockNames() const;

    void sceneChangeEvent(const QSceneChangePtr &e) Q_DECL_OVERRIDE;
    bool isLoaded() const;

    QVector<ShaderUniform> uniforms() const;
    QVector<ShaderAttribute> attributes() const;
    QVector<ShaderUniformBlock> uniformBlocks() const;

private:
    QOpenGLShaderProgram *m_program;

    QOpenGLShaderProgram *createProgram(QGraphicsContext *context);
    QOpenGLShaderProgram *createDefaultProgram();

    QVector<QString> m_uniformsNames;
    QVector<ShaderUniform> m_uniforms;

    QVector<QString> m_attributesNames;
    QVector<ShaderAttribute> m_attributes;

    QVector<QString> m_uniformBlockNames;
    QVector<ShaderUniformBlock> m_uniformBlocks;

    QHash<QString, int> m_fragOutputs;

    QVector<QByteArray> m_shaderCode;

    bool m_isLoaded;

    // Private so that only GraphicContext can call it
    void initializeUniforms(const QVector<ShaderUniform> &uniformsDescription);
    void initializeAttributes(const QVector<ShaderAttribute> &attributesDescription);
    void initializeUniformBlocks(const QVector<ShaderUniformBlock> &uniformBlockDescription);
    QOpenGLShaderProgram *getOrCreateProgram(QGraphicsContext *ctx);
    friend class QGraphicsContext;
};

} // Render
} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_RENDERSHADER_H
