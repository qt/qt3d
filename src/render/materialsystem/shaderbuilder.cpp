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

#include "shaderbuilder_p.h"

#include <Qt3DCore/qpropertyupdatedchange.h>

#include <Qt3DRender/private/qshaderprogrambuilder_p.h>
#include <Qt3DRender/qshaderprogram.h>
#include <Qt3DRender/private/qshaderprogram_p.h>
#include <Qt3DRender/private/qurlhelper_p.h>

#include <Qt3DRender/private/qshaderformat_p.h>
#include <Qt3DRender/private/qshadergraphloader_p.h>
#include <Qt3DRender/private/qshadergenerator_p.h>
#include <Qt3DRender/private/qshadernodesloader_p.h>

#include <QFile>
#include <QFileInfo>
#include <QUrl>

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

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

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
    const auto graphPath = QUrlHelper::urlToLocalFileOrQrc(shaderGraph(type));
    QFile file(graphPath);
    if (!file.open(QFile::ReadOnly)) {
        qWarning() << "Couldn't open file:" << graphPath;
        return;
    }

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
    m_codes.insert(type, deincludified);
    m_dirtyTypes.remove(type);

    m_pendingUpdates.push_back({ peerId(),
                                 type,
                                 m_codes.value(type) });
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

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
