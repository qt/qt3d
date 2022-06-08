// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "shader_p.h"
#include "renderlogging_p.h"

#include <QFile>
#include <QOpenGLContext>
#include <QOpenGLShaderProgram>
#include <QMutexLocker>
#include <qshaderprogram.h>
#include <Qt3DRender/private/attachmentpack_p.h>
#include <Qt3DRender/private/qshaderprogram_p.h>
#include <Qt3DRender/private/stringtoint_p.h>
#include <Qt3DRender/private/managers_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {
const int Shader::modelMatrixNameId = StringToInt::lookupId(QLatin1String("modelMatrix"));
const int Shader::viewMatrixNameId = StringToInt::lookupId(QLatin1String("viewMatrix"));
const int Shader::projectionMatrixNameId = StringToInt::lookupId(QLatin1String("projectionMatrix"));
const int Shader::modelViewMatrixNameId = StringToInt::lookupId(QLatin1String("modelView"));
const int Shader::viewProjectionMatrixNameId = StringToInt::lookupId(QLatin1String("viewProjectionMatrix"));
const int Shader::modelViewProjectionNameId = StringToInt::lookupId(QLatin1String("modelViewProjection"));
const int Shader::mvpNameId = StringToInt::lookupId(QLatin1String("mvp"));
const int Shader::inverseModelMatrixNameId = StringToInt::lookupId(QLatin1String("inverseModelMatrix"));
const int Shader::inverseViewMatrixNameId = StringToInt::lookupId(QLatin1String("inverseViewMatrix"));
const int Shader::inverseProjectionMatrixNameId = StringToInt::lookupId(QLatin1String("inverseProjectionMatrix"));
const int Shader::inverseModelViewNameId = StringToInt::lookupId(QLatin1String("inverseModelView"));
const int Shader::inverseViewProjectionMatrixNameId = StringToInt::lookupId(QLatin1String("inverseViewProjectionMatrix"));
const int Shader::inverseModelViewProjectionNameId = StringToInt::lookupId(QLatin1String("inverseModelViewProjection"));
const int Shader::modelNormalMatrixNameId = StringToInt::lookupId(QLatin1String("modelNormalMatrix"));
const int Shader::modelViewNormalNameId = StringToInt::lookupId(QLatin1String("modelViewNormal"));
const int Shader::viewportMatrixNameId = StringToInt::lookupId(QLatin1String("viewportMatrix"));
const int Shader::inverseViewportMatrixNameId = StringToInt::lookupId(QLatin1String("inverseViewportMatrix"));
const int Shader::textureTransformMatrixNameId = StringToInt::lookupId(QLatin1String("textureTransformMatrix"));
const int Shader::aspectRatioNameId = StringToInt::lookupId(QLatin1String("aspectRatio"));
const int Shader::exposureNameId = StringToInt::lookupId(QLatin1String("exposure"));
const int Shader::gammaNameId = StringToInt::lookupId(QLatin1String("gamma"));
const int Shader::timeNameId = StringToInt::lookupId(QLatin1String("time"));
const int Shader::eyePositionNameId = StringToInt::lookupId(QLatin1String("eyePosition"));
const int Shader::skinningPaletteNameId = StringToInt::lookupId(QLatin1String("skinningPalette[0]"));
const int Shader::yUpInFBOId = StringToInt::lookupId(QLatin1String("yUpInFBO"));
const int Shader::yUpInNDCId = StringToInt::lookupId(QLatin1String("yUpInNDC"));

Shader::Shader()
    : BackendNode(ReadWrite)
    , m_requiresFrontendSync(false)
    , m_status(QShaderProgram::NotReady)
    , m_format(QShaderProgram::GLSL)
    , m_dirty(false)
{
    m_shaderCode.resize(static_cast<int>(QShaderProgram::Compute) + 1);
}

Shader::~Shader()
{
}

void Shader::cleanup()
{
    QBackendNode::setEnabled(false);
    m_status = QShaderProgram::NotReady;
    m_format = QShaderProgram::GLSL;
    m_log.clear();
    m_requiresFrontendSync = false;
    m_dirty = false;
}

void Shader::syncFromFrontEnd(const QNode *frontEnd, bool firstTime)
{
    const QShaderProgram *node = qobject_cast<const QShaderProgram *>(frontEnd);
    if (!node)
        return;

    BackendNode::syncFromFrontEnd(frontEnd, firstTime);

    if (firstTime)
        for (int i = QShaderProgram::Vertex; i <= QShaderProgram::Compute; ++i)
            m_shaderCode[i].clear();

    for (int i = QShaderProgram::Vertex; i <= QShaderProgram::Compute; ++i) {
        const QShaderProgram::ShaderType shaderType = static_cast<QShaderProgram::ShaderType>(i);
        const QByteArray code = node->shaderCode(shaderType);
        if (code != m_shaderCode[shaderType])
            setShaderCode(shaderType, code);
    }
    setFormat(node->format());
}

void Shader::setShaderCode(QShaderProgram::ShaderType type, const QByteArray &code)
{
    if (code == m_shaderCode[type])
        return;

    m_shaderCode[type] = code;
    m_requiresFrontendSync = true;
    m_dirty = true;
    setStatus(QShaderProgram::NotReady);
    markDirty(AbstractRenderer::ShadersDirty);
}

void Shader::setFormat(QShaderProgram::Format format)
{
    if (format == m_format)
        return;
    m_format = format;
    m_dirty = true;
    setStatus(QShaderProgram::NotReady);
    markDirty(AbstractRenderer::ShadersDirty);
}

const std::vector<QByteArray> &Shader::shaderCode() const
{
    return m_shaderCode;
}

/*!
   \internal
   Initializes this Shader's state relating to attributes, global block uniforms and
   and named uniform blocks by copying these details from \a other.
*/
void Shader::initializeFromReference(const Shader &other)
{
    m_status = other.m_status;
    m_log = other.m_log;
    m_requiresFrontendSync = true;
    setStatus(other.status());
    setLog(other.log());
}

// Called by renderer plugin when loading a GL Shader plugins
void Shader::requestCacheRebuild()
{
    markDirty(AbstractRenderer::MaterialDirty);
}

void Shader::setLog(const QString &log)
{
    m_log = log;
    m_requiresFrontendSync = true;
}

void Shader::setStatus(QShaderProgram::Status status)
{
    m_status = status;
    m_requiresFrontendSync = true;
}

ShaderFunctor::ShaderFunctor(AbstractRenderer *renderer, ShaderManager *manager)
    : m_renderer(renderer)
    , m_shaderManager(manager)
{
}

QBackendNode *ShaderFunctor::create(QNodeId id) const
{
    Shader *backend = m_shaderManager->getOrCreateResource(id);
    // Remove from the list of ids to destroy in case we were added to it
    m_shaderManager->removeShaderIdFromIdsToCleanup(id);
    backend->setRenderer(m_renderer);
    return backend;
}

QBackendNode *ShaderFunctor::get(QNodeId id) const
{
    // If we are marked for destruction, return nullptr so that
    // if we were to be recreated, create would be called again
    if (m_shaderManager->hasShaderIdToCleanup(id))
        return nullptr;
    return m_shaderManager->lookupResource(id);
}

void ShaderFunctor::destroy(QNodeId id) const
{
    // We only add ourselves to the dirty list
    // The actual removal needs to be performed after we have
    // destroyed the associated APIShader in the RenderThread
    if (m_shaderManager->lookupResource(id))
        m_shaderManager->addShaderIdToCleanup(id);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
