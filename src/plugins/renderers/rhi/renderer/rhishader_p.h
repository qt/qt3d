// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_RHI_GLSHADER_P_H
#define QT3DRENDER_RENDER_RHI_GLSHADER_P_H

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
#include <QtGui/private/qshader_p.h>
#include <rhi/qrhi.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace Rhi {

class Q_AUTOTEST_EXPORT RHIShader
{
public:

    struct UBO_Member
    {
        int nameId;
        QShaderDescription::BlockVariable blockVariable;
        std::vector<UBO_Member> structMembers;
    };

    struct UBO_Block
    {
        ShaderUniformBlock block;
        std::vector<UBO_Member> members;
    };

    RHIShader();

    bool isLoaded() const { return m_isLoaded; }
    void setLoaded(bool loaded) { m_isLoaded = loaded; }

    void setFragOutputs(QHash<QString, int> fragOutputs);
    const QHash<QString, int> &fragOutputs() const;

    inline const std::vector<int> &uniformsNamesIds() const { return m_uniformsNamesIds; }
    inline const std::vector<int> &standardUniformNameIds() const { return m_standardUniformNamesIds; }
    inline const std::vector<int> &uniformBlockNamesIds() const { return m_uniformBlockNamesIds; }
    inline const std::vector<int> &storageBlockNamesIds() const { return m_shaderStorageBlockNamesIds; }
    inline const std::vector<int> &attributeNamesIds() const { return m_attributeNamesIds; }

    const std::vector<QString> &uniformsNames() const;
    const std::vector<QString> &attributesNames() const;
    const std::vector<QString> &uniformBlockNames() const;
    const std::vector<QString> &storageBlockNames() const;
    const std::vector<QString> &samplerNames() const;
    const std::vector<QString> &imagesNames() const;

    inline const std::vector<ShaderUniform> &uniforms() const { return m_uniforms; }
    inline const std::vector<ShaderAttribute> &attributes() const { return m_attributes; }
    inline const std::vector<ShaderUniformBlock> &uniformBlocks() const { return m_uniformBlocks; }
    inline const std::vector<ShaderStorageBlock> &storageBlocks() const { return m_shaderStorageBlocks; }
    inline const std::vector<ShaderAttribute> &samplers() const { return m_samplers; }
    inline const std::vector<ShaderAttribute> &images() const { return m_images; }

    QHash<QString, ShaderUniform> activeUniformsForUniformBlock(int blockIndex) const;

    ShaderUniformBlock uniformBlockForBlockIndex(int blockNameId) const noexcept;
    ShaderUniformBlock uniformBlockForBlockNameId(int blockIndex) const noexcept;
    ShaderUniformBlock uniformBlockForBlockName(const QString &blockName) const noexcept;

    ShaderUniformBlock uniformBlockForInstanceName(const QString &instanceName) const noexcept;
    ShaderUniformBlock uniformBlockForInstanceNameId(int instanceNameId) const noexcept;

    ShaderStorageBlock storageBlockForBlockIndex(int blockIndex) const noexcept;
    ShaderStorageBlock storageBlockForBlockNameId(int blockNameId) const noexcept;
    ShaderStorageBlock storageBlockForBlockName(const QString &blockName) const noexcept;

    enum ParameterKind { Uniform, UBO, SSBO, Struct };
    ParameterKind categorizeVariable(int nameId) const noexcept;

    bool hasUniform(int nameId) const noexcept;
    bool hasActiveVariables() const noexcept;

    void setShaderCode(std::vector<QByteArray> shaderCode);
    const std::vector<QByteArray> &shaderCode() const;

    const QShader &shaderStage(QShader::Stage stage) const noexcept { return m_stages[stage]; }
    const std::vector<UBO_Block> &uboBlocks() const { return m_uboBlocks; }

    const QSet<QString> &unqualifiedUniformNames() const noexcept
    {
        return m_unqualifiedUniformNames;
    }

    void introspect();

private:
    bool m_isLoaded;
    QShader m_stages[6];

    std::vector<QString> m_uniformsNames;
    std::vector<int> m_uniformsNamesIds;
    std::vector<int> m_standardUniformNamesIds;
    std::vector<ShaderUniform> m_uniforms;

    std::vector<QString> m_attributesNames;
    std::vector<int> m_attributeNamesIds;
    std::vector<ShaderAttribute> m_attributes;

    std::vector<QString> m_uniformBlockNames;
    std::vector<int> m_uniformBlockNamesIds;
    std::vector<ShaderUniformBlock> m_uniformBlocks;
    QHash<int, QHash<QString, ShaderUniform>> m_uniformBlockIndexToShaderUniforms;
    QSet<QString> m_unqualifiedUniformNames;

    std::vector<QString> m_shaderStorageBlockNames;
    std::vector<int> m_shaderStorageBlockNamesIds;
    std::vector<ShaderStorageBlock> m_shaderStorageBlocks;

    std::vector<QString> m_samplerNames;
    std::vector<int> m_samplerIds;
    std::vector<ShaderAttribute> m_samplers;

    std::vector<QString> m_imageNames;
    std::vector<int> m_imageIds;
    std::vector<ShaderAttribute> m_images;

    std::vector<QString> m_structNames;
    std::vector<int> m_structNamesIds;

    QHash<QString, int> m_fragOutputs;
    std::vector<QByteArray> m_shaderCode;

    // Private so that only SubmissionContext can call it
    friend class SubmissionContext;
    void initializeAttributes(std::vector<Qt3DRender::Render::Rhi::ShaderAttribute> attributesDescription);
    void initializeUniformBlocks(std::vector<ShaderUniformBlock> uniformBlockDescription);
    void initializeShaderStorageBlocks(std::vector<ShaderStorageBlock> shaderStorageBlockDescription);
    void initializeSamplers(std::vector<ShaderAttribute> samplerDescription);
    void initializeImages(std::vector<ShaderAttribute> imageDescription);
    void recordAllUniforms(UBO_Member &uboMember, QString parentName);

    std::vector<UBO_Block> m_uboBlocks;

    mutable QMutex m_mutex;
    QMetaObject::Connection m_contextConnection;
};

} // Rhi

} // Render

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RHI_GLSHADER_P_H
