// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "shaderbuilder_p.h"

#include <Qt3DRender/private/qshaderprogrambuilder_p.h>
#include <Qt3DRender/qshaderprogram.h>
#include <Qt3DRender/private/qshaderprogram_p.h>
#include <Qt3DCore/private/qurlhelper_p.h>

#include <Qt3DRender/private/qshaderformat_p.h>
#include <Qt3DRender/private/qshadergraphloader_p.h>
#include <Qt3DRender/private/qshadergenerator_p.h>
#include <Qt3DRender/private/qshadernodesloader_p.h>
#include <Qt3DRender/private/renderlogging_p.h>

#include <QFile>
#include <QFileInfo>
#include <QUrl>
#include <QCryptographicHash>
#include <QDateTime>
#include <QStandardPaths>
#include <QDir>

static void initResources()
{
#ifdef QT_STATIC
    Q_INIT_RESOURCE(materialsystem);
#endif
}

QT_BEGIN_NAMESPACE

class GlobalShaderPrototypes
{
public:
    GlobalShaderPrototypes()
    {
        initResources();
        setPrototypesFile(QStringLiteral(":/prototypes/default.json"));
    }

    QString prototypesFile() const
    {
        return m_fileName;
    }

    void setPrototypesFile(const QString &fileName)
    {
        m_fileName = fileName;
        load();
    }

    QHash<QString, Qt3DRender::QShaderNode> prototypes() const
    {
        return m_prototypes;
    }

private:
    void load()
    {
        QFile file(m_fileName);
        if (!file.open(QFile::ReadOnly)) {
            qWarning() << "Couldn't open file:" << m_fileName;
            return;
        }

        Qt3DRender::QShaderNodesLoader loader;
        loader.setDevice(&file);
        loader.load();
        m_prototypes = loader.nodes();
    }

    QString m_fileName;
    QHash<QString, Qt3DRender::QShaderNode> m_prototypes;
};

Q_GLOBAL_STATIC(GlobalShaderPrototypes, qt3dGlobalShaderPrototypes)

namespace Qt3DRender {
namespace Render {

using namespace Qt3DCore;

QString ShaderBuilder::getPrototypesFile()
{
    return qt3dGlobalShaderPrototypes->prototypesFile();
}

void ShaderBuilder::setPrototypesFile(const QString &file)
{
    qt3dGlobalShaderPrototypes->setPrototypesFile(file);
}

QStringList ShaderBuilder::getPrototypeNames()
{
    return qt3dGlobalShaderPrototypes->prototypes().keys();
}

ShaderBuilder::ShaderBuilder()
    : BackendNode(ReadWrite)
{
}

ShaderBuilder::~ShaderBuilder()
{
}

void ShaderBuilder::cleanup()
{
    m_shaderProgramId = Qt3DCore::QNodeId();
    m_enabledLayers.clear();
    m_graphs.clear();
    m_dirtyTypes.clear();
    m_pendingUpdates.clear();
    QBackendNode::setEnabled(false);
}

Qt3DCore::QNodeId ShaderBuilder::shaderProgramId() const
{
    return m_shaderProgramId;
}

QStringList ShaderBuilder::enabledLayers() const
{
    return m_enabledLayers;
}


void ShaderBuilder::setEnabledLayers(const QStringList &layers)
{
    if (m_enabledLayers == layers)
        return;

    m_enabledLayers = layers;

    for (auto it = m_graphs.cbegin(); it != m_graphs.cend(); ++it) {
        if (!it.value().isEmpty())
            m_dirtyTypes.insert(it.key());
    }
}

GraphicsApiFilterData ShaderBuilder::graphicsApi() const
{
    return m_graphicsApi;
}

void ShaderBuilder::setGraphicsApi(const GraphicsApiFilterData &graphicsApi)
{
    if (m_graphicsApi == graphicsApi)
        return;

    m_graphicsApi = graphicsApi;
    for (auto it = m_graphs.cbegin(); it != m_graphs.cend(); ++it) {
        if (!it.value().isEmpty())
            m_dirtyTypes.insert(it.key());
    }
}

QUrl ShaderBuilder::shaderGraph(QShaderProgram::ShaderType type) const
{
    return m_graphs.value(type);
}

void ShaderBuilder::setShaderGraph(QShaderProgram::ShaderType type, const QUrl &url)
{
    if (url != m_graphs.value(type)) {
        m_graphs.insert(type, url);
        m_dirtyTypes.insert(type);
    }
}

QByteArray ShaderBuilder::shaderCode(QShaderProgram::ShaderType type) const
{
    return m_codes.value(type);
}

bool ShaderBuilder::isShaderCodeDirty(QShaderProgram::ShaderType type) const
{
    return m_dirtyTypes.contains(type);
}

void ShaderBuilder::generateCode(QShaderProgram::ShaderType type)
{
    const auto graphPath = Qt3DCore::QUrlHelper::urlToLocalFileOrQrc(shaderGraph(type));
    QFile file(graphPath);
    if (!file.open(QFile::ReadOnly)) {
        qWarning() << "Couldn't open file:" << graphPath;
        return;
    }

    auto updateShaderCodeAndClearDirty = [&] (const QByteArray &shaderCode) {
        m_codes.insert(type, shaderCode);
        m_dirtyTypes.remove(type);
        m_pendingUpdates.push_back({ peerId(),
                                     type,
                                     m_codes.value(type) });
    };

    const QByteArray cacheKey = hashKeyForShaderGraph(type);
    const bool forceRegenerate = qEnvironmentVariableIsSet("QT3D_REBUILD_SHADER_CACHE");
    const bool useCache = !qEnvironmentVariableIsSet("QT3D_DISABLE_SHADER_CACHE") && !forceRegenerate;
    const QByteArray userProvidedPath = qgetenv("QT3D_WRITABLE_CACHE_PATH");
    const QString cachedFilterPath = QDir(userProvidedPath.isEmpty() ?
                                              QStandardPaths::writableLocation(QStandardPaths::TempLocation)
                                            : QString::fromUtf8(userProvidedPath)).absoluteFilePath(QString::fromUtf8(cacheKey) + QLatin1String(".qt3d"));
    QFile cachedShaderFile(cachedFilterPath);

    // Check our runtime cache to see if we have already loaded the shader previously
    if (useCache) {
        // We check if we already have generated a shader previously for the
        // given type, the given graph, the given API and the current set of layer
        // If that's the case it's faster to load the pre generated shader file

        if (m_renderer && m_renderer->containsGeneratedShaderGraph(cacheKey)) {
            qCDebug(ShaderCache) << "Using runtime cache for shader graph with key" << cacheKey;
            updateShaderCodeAndClearDirty(m_renderer->cachedGeneratedShaderGraph(cacheKey));
            return;
        }

        // else check if a cachedShader file exists
        if (cachedShaderFile.exists()) {
            if (!cachedShaderFile.open(QFile::ReadOnly)) {
                qCWarning(ShaderCache) << "Couldn't open cached shader file:" << graphPath;
                // Too bad, we have to generate the shader below
            } else {
                // Use cached shader
                qCDebug(ShaderCache) << "Using cached shader file" << cachedFilterPath;
                const QByteArray shaderCode = cachedShaderFile.readAll();
                updateShaderCodeAndClearDirty(shaderCode);

                // Record to runtime cache
                if (m_renderer) {
                    qCDebug(ShaderCache) << "Insert shader " << cacheKey << "into runtime cache";
                    m_renderer->insertGeneratedShaderGraph(cacheKey, shaderCode);
                }
                return;
            }
        }
    }

    // Generate Shader and Cache the result for subsequent uses
    auto graphLoader = QShaderGraphLoader();
    graphLoader.setPrototypes(qt3dGlobalShaderPrototypes->prototypes());
    graphLoader.setDevice(&file);
    graphLoader.load();

    if (graphLoader.status() == QShaderGraphLoader::Error)
        return;

    const auto graph = graphLoader.graph();

    auto format = QShaderFormat();
    format.setApi(m_graphicsApi.m_api == QGraphicsApiFilter::OpenGLES ? QShaderFormat::OpenGLES
                : m_graphicsApi.m_api == QGraphicsApiFilter::Vulkan ? QShaderFormat::VulkanFlavoredGLSL
                : m_graphicsApi.m_api == QGraphicsApiFilter::RHI ? QShaderFormat::RHI
                : m_graphicsApi.m_profile == QGraphicsApiFilter::CoreProfile ? QShaderFormat::OpenGLCoreProfile
                : m_graphicsApi.m_profile == QGraphicsApiFilter::CompatibilityProfile ? QShaderFormat::OpenGLCompatibilityProfile
                : QShaderFormat::OpenGLNoProfile);
    format.setVersion(QVersionNumber(m_graphicsApi.m_major, m_graphicsApi.m_minor));
    format.setExtensions(m_graphicsApi.m_extensions);
    format.setVendor(m_graphicsApi.m_vendor);

    auto generator = QShaderGenerator();
    generator.format = format;
    generator.graph = graph;

    const auto code = generator.createShaderCode(m_enabledLayers);
    const auto deincludified = QShaderProgramPrivate::deincludify(code, graphPath + QStringLiteral(".glsl"));

    updateShaderCodeAndClearDirty(deincludified);

    // Record to runtime cache
    if (useCache || forceRegenerate) {
        if (m_renderer) {
            qCDebug(ShaderCache) << "Insert shader " << cacheKey << "into runtime cache";
            m_renderer->insertGeneratedShaderGraph(cacheKey, deincludified);
        }

        // Record to file cache
        if (cachedShaderFile.open(QFile::WriteOnly)) {
            cachedShaderFile.write(deincludified);
            qCDebug(ShaderCache) << "Saving cached shader file" << cachedFilterPath;
        } else {
            qCWarning(ShaderCache) << "Unable to write cached shader file";
        }
    }
}

void ShaderBuilder::syncFromFrontEnd(const QNode *frontEnd, bool firstTime)
{
    const QShaderProgramBuilder *node = qobject_cast<const QShaderProgramBuilder *>(frontEnd);
    if (!node)
        return;

    const bool oldEnabled = isEnabled();
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);

    if (oldEnabled != isEnabled()) {
        markDirty(AbstractRenderer::ShadersDirty);
    }

    const Qt3DCore::QNodeId shaderProgramId = Qt3DCore::qIdForNode(node->shaderProgram());
    if (shaderProgramId != m_shaderProgramId) {
        m_shaderProgramId = shaderProgramId;
        markDirty(AbstractRenderer::ShadersDirty);
    }

    if (node->enabledLayers() != m_enabledLayers) {
        setEnabledLayers(node->enabledLayers());
        markDirty(AbstractRenderer::ShadersDirty);
    }

    static const QVarLengthArray<std::pair<QShaderProgram::ShaderType, QUrl (QShaderProgramBuilder::*)() const>, 6> shaderTypesToGetters {
        {QShaderProgram::Vertex, &QShaderProgramBuilder::vertexShaderGraph},
        {QShaderProgram::TessellationControl, &QShaderProgramBuilder::tessellationControlShaderGraph},
        {QShaderProgram::TessellationEvaluation, &QShaderProgramBuilder::tessellationEvaluationShaderGraph},
        {QShaderProgram::Geometry, &QShaderProgramBuilder::geometryShaderGraph},
        {QShaderProgram::Fragment, &QShaderProgramBuilder::fragmentShaderGraph},
        {QShaderProgram::Compute, &QShaderProgramBuilder::computeShaderGraph},
    };

    for (auto it = shaderTypesToGetters.cbegin(), end = shaderTypesToGetters.cend(); it != end; ++it) {
        const QUrl url = (node->*(it->second))();
        if (url != m_graphs.value(it->first)) {
            setShaderGraph(it->first, url);
            markDirty(AbstractRenderer::ShadersDirty);
        }
    }
}

QByteArray ShaderBuilder::hashKeyForShaderGraph(QShaderProgram::ShaderType type) const
{
    const auto graphPath = Qt3DCore::QUrlHelper::urlToLocalFileOrQrc(shaderGraph(type));
    QFile file(graphPath);
    if (!file.exists()) {
        qWarning() << graphPath << "doesn't exist";
        return {};
    }

    QCryptographicHash hashBuilder(QCryptographicHash::Sha1);
    // Add graphPath
    hashBuilder.addData(graphPath.toUtf8());
    // Get TimeStamp and Graph file size
    QFileInfo info(graphPath);
    const QString fileInfo = QString::fromUtf8("%1_%2")
            .arg(info.lastModified().toSecsSinceEpoch())
            .arg(info.size());
    hashBuilder.addData(fileInfo.toUtf8());

    // Add Layers
    for (const QString &layer : m_enabledLayers)
        hashBuilder.addData(layer.toUtf8());

    // Add GraphicsInfo
    const QString graphicsInfo = QString::fromUtf8("API: %1 Profile: %2 Major: %3 Minor: %4")
            .arg(int(m_graphicsApi.m_api))
            .arg(int(m_graphicsApi.m_profile))
            .arg(int(m_graphicsApi.m_major))
            .arg(int(m_graphicsApi.m_minor));
    hashBuilder.addData(graphicsInfo.toUtf8());

    // Add Shader Type
    hashBuilder.addData(QString::number(type).toUtf8());

    return hashBuilder.result().toHex();
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
