/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3DRENDER_RENDER_SHADER_H
#define QT3DRENDER_RENDER_SHADER_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QMutex>
#include <QVector>
#include <Qt3DRender/private/quniformvalue_p.h>
#include <Qt3DRender/private/shadervariables_p.h>
#include <Qt3DCore/qbackendnode.h>

QT_BEGIN_NAMESPACE

class QOpenGLShaderProgram;

namespace Qt3DRender {

class QShaderProgram;

namespace Render {

class ShaderManager;
class AttachmentPack;

typedef uint ProgramDNA;

class Q_AUTOTEST_EXPORT Shader : public Qt3DCore::QBackendNode
{
public:
    Shader();
    ~Shader();

    void cleanup();

    void updateFromPeer(Qt3DCore::QNode *peer) Q_DECL_OVERRIDE;
    void updateUniforms(GraphicsContext *ctx, const ShaderParameterPack &pack);
    void setFragOutputs(const QHash<QString, int> &fragOutputs);

    inline QVector<int> uniformsNamesIds() const Q_DECL_NOEXCEPT { return m_uniformsNamesIds; }
    inline QVector<int> uniformBlockNamesIds() const Q_DECL_NOEXCEPT { return m_uniformBlockNamesIds; }
    inline QVector<int> storageBlockNamesIds() const Q_DECL_NOEXCEPT { return m_shaderStorageBlockNamesIds; }

    QVector<QString> uniformsNames() const;
    QVector<QString> attributesNames() const;
    QVector<QString> uniformBlockNames() const;
    QVector<QString> storageBlockNames() const;
    QVector<QByteArray> shaderCode() const;

    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e) Q_DECL_OVERRIDE;
    bool isLoaded() const Q_DECL_NOEXCEPT { return m_isLoaded; }
    ProgramDNA dna() const Q_DECL_NOEXCEPT { return m_dna; }

    inline QVector<ShaderUniform> uniforms() const Q_DECL_NOEXCEPT { return m_uniforms; }
    inline QVector<ShaderAttribute> attributes() const Q_DECL_NOEXCEPT { return m_attributes; }
    inline QVector<ShaderUniformBlock> uniformBlocks() const Q_DECL_NOEXCEPT { return m_uniformBlocks; }
    inline QVector<ShaderStorageBlock> storageBlocks() const Q_DECL_NOEXCEPT { return m_shaderStorageBlocks; }

    QHash<QString, ShaderUniform> activeUniformsForUniformBlock(int blockIndex) const;

    ShaderUniformBlock uniformBlockForBlockIndex(int blockNameId);
    ShaderUniformBlock uniformBlockForBlockNameId(int blockIndex);
    ShaderUniformBlock uniformBlockForBlockName(const QString &blockName);

    ShaderStorageBlock storageBlockForBlockIndex(int blockIndex);
    ShaderStorageBlock storageBlockForBlockNameId(int blockNameId);
    ShaderStorageBlock storageBlockForBlockName(const QString &blockName);

private:
    QOpenGLShaderProgram *m_program;

    QOpenGLShaderProgram *createProgram(GraphicsContext *context);
    QOpenGLShaderProgram *createDefaultProgram();

    QVector<QString> m_uniformsNames;
    QVector<int> m_uniformsNamesIds;
    QVector<ShaderUniform> m_uniforms;

    QVector<QString> m_attributesNames;
    QVector<ShaderAttribute> m_attributes;

    QVector<QString> m_uniformBlockNames;
    QVector<int> m_uniformBlockNamesIds;
    QVector<ShaderUniformBlock> m_uniformBlocks;
    QHash<int, QHash<QString, ShaderUniform> > m_uniformBlockIndexToShaderUniforms;

    QVector<QString> m_shaderStorageBlockNames;
    QVector<int> m_shaderStorageBlockNamesIds;
    QVector<ShaderStorageBlock> m_shaderStorageBlocks;

    QHash<QString, int> m_fragOutputs;

    QVector<QByteArray> m_shaderCode;

    bool m_isLoaded;
    ProgramDNA m_dna;
    QMutex m_mutex;

    void updateDNA();

    // Private so that only GraphicContext can call it
    void initializeUniforms(const QVector<ShaderUniform> &uniformsDescription);
    void initializeAttributes(const QVector<ShaderAttribute> &attributesDescription);
    void initializeUniformBlocks(const QVector<ShaderUniformBlock> &uniformBlockDescription);
    void initializeShaderStorageBlocks(const QVector<ShaderStorageBlock> &shaderStorageBlockDescription);

    void initialize(const Shader &other);

    QOpenGLShaderProgram *getOrCreateProgram(GraphicsContext *ctx);
    friend class GraphicsContext;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_SHADER_H
