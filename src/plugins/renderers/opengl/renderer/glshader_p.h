/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DRENDER_RENDER_OPENGL_GLSHADER_P_H
#define QT3DRENDER_RENDER_OPENGL_GLSHADER_P_H

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


#include <shadervariables_p.h>
#include <shaderparameterpack_p.h>
#include <Qt3DRender/qshaderprogram.h>
#include <QMutex>

#ifdef QT_BUILD_INTERNAL
    class tst_BenchShaderParameterPack;
#endif

QT_BEGIN_NAMESPACE

class QOpenGLShaderProgram;

namespace Qt3DRender {

namespace Render {

namespace OpenGL {

class Q_AUTOTEST_EXPORT GLShader
{
public:
    GLShader();
    ~GLShader();

    void setGraphicsContext(GraphicsContext *context);
    GraphicsContext *graphicsContext();

    bool isLoaded() const { return m_isLoaded; }
    void setLoaded(bool loaded) { m_isLoaded = loaded; }

    void prepareUniforms(ShaderParameterPack &pack);

    void setFragOutputs(const QHash<QString, int> &fragOutputs);
    const QHash<QString, int> fragOutputs() const;

    inline const std::vector<int> &uniformsNamesIds() const { return m_uniformsNamesIds; }
    inline const std::vector<int> &lightUniformsNamesIds() const { return m_lightUniformsNamesIds; }
    inline const std::vector<int> &standardUniformNameIds() const { return m_standardUniformNamesIds; }
    inline const std::vector<int> &uniformBlockNamesIds() const { return m_uniformBlockNamesIds; }
    inline const std::vector<int> &storageBlockNamesIds() const { return m_shaderStorageBlockNamesIds; }
    inline const std::vector<int> &attributeNamesIds() const { return m_attributeNamesIds; }

    const std::vector<QString> &uniformsNames() const;
    const std::vector<QString> &attributesNames() const;
    const std::vector<QString> &uniformBlockNames() const;
    const std::vector<QString> &storageBlockNames() const;

    inline const std::vector<ShaderUniform> &uniforms() const { return m_uniforms; }
    inline const std::vector<ShaderAttribute> &attributes() const { return m_attributes; }
    inline const std::vector<ShaderUniformBlock> &uniformBlocks() const { return m_uniformBlocks; }
    inline const std::vector<ShaderStorageBlock> &storageBlocks() const { return m_shaderStorageBlocks; }

    QHash<QString, ShaderUniform> activeUniformsForUniformBlock(int blockIndex) const;

    ShaderUniformBlock uniformBlockForBlockIndex(int blockNameId) const noexcept;
    ShaderUniformBlock uniformBlockForBlockNameId(int blockIndex) const noexcept;
    ShaderUniformBlock uniformBlockForBlockName(const QString &blockName) const noexcept;

    ShaderStorageBlock storageBlockForBlockIndex(int blockIndex) const noexcept;
    ShaderStorageBlock storageBlockForBlockNameId(int blockNameId) const noexcept;
    ShaderStorageBlock storageBlockForBlockName(const QString &blockName) const noexcept;

    enum ParameterKind {
        Uniform,
        UBO,
        SSBO,
        Struct
    };
    ParameterKind categorizeVariable(int nameId) const noexcept;

    bool hasUniform(int nameId) const noexcept;
    inline bool hasActiveVariables() const noexcept { return m_hasActiveVariables; }
    inline int parameterPackSize() const noexcept { return m_parameterPackSize; }

    QOpenGLShaderProgram *shaderProgram() { return &m_shader; }

    void setShaderCode(const std::vector<QByteArray> shaderCode) { m_shaderCode = shaderCode; }
    const std::vector<QByteArray> &shaderCode() const;

private:
    bool m_isLoaded;
    QOpenGLShaderProgram m_shader;
    GraphicsContext *m_graphicsContext;

    std::vector<QString> m_uniformsNames;
    std::vector<int> m_uniformsNamesIds;
    std::vector<int> m_lightUniformsNamesIds;
    std::vector<int> m_standardUniformNamesIds;
    std::vector<ShaderUniform> m_uniforms;

    std::vector<QString> m_attributesNames;
    std::vector<int> m_attributeNamesIds;
    std::vector<ShaderAttribute> m_attributes;

    std::vector<QString> m_uniformBlockNames;
    std::vector<int> m_uniformBlockNamesIds;
    std::vector<ShaderUniformBlock> m_uniformBlocks;
    QHash<int, QHash<QString, ShaderUniform> > m_uniformBlockIndexToShaderUniforms;

    std::vector<QString> m_shaderStorageBlockNames;
    std::vector<int> m_shaderStorageBlockNamesIds;
    std::vector<ShaderStorageBlock> m_shaderStorageBlocks;

    QHash<QString, int> m_fragOutputs;
    std::vector<QByteArray> m_shaderCode;

    int m_parameterPackSize;
    int m_hasActiveVariables;

    // Private so that only GraphicContext can call it
    void initializeUniforms(const std::vector<ShaderUniform> &uniformsDescription);
    void initializeAttributes(const std::vector<ShaderAttribute> &attributesDescription);
    void initializeUniformBlocks(const std::vector<ShaderUniformBlock> &uniformBlockDescription);
    void initializeShaderStorageBlocks(const std::vector<ShaderStorageBlock> &shaderStorageBlockDescription);

    friend class GraphicsContext;
#ifdef QT_BUILD_INTERNAL
    friend class ::tst_BenchShaderParameterPack;
#endif

    mutable QMutex m_mutex;
    QMetaObject::Connection m_contextConnection;
};

} // OpenGL

} // Render

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_OPENGL_GLSHADER_P_H
