// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qshaderprogrambuilder.h"
#include "qshaderprogrambuilder_p.h"
#include "qshaderprogram.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QUrl>

/*!
    \class Qt3DRender::QShaderProgramBuilder
    \inmodule Qt3DRender
    \brief Generates a Shader Program content from loaded graphs.
    \inherits Qt3DCore::QNode
    \since 5.10

    A shader program builder consists of several different shader graphs
    used to generate shader code.

    A cache of generated shader code is maintained. Generated shaders are by
    defaults saved in
    QStandardPaths::writableLocation(QStandardPaths::TempLocation)). This path
    can be overridden by setting environment variable QT3D_WRITABLE_CACHE_PATH
    to a valid writable path.

    The use of the cache can be disabled by setting environment variable
    QT3D_DISABLE_SHADER_CACHE.

    In most cases, changes made to a graph are detected by Qt 3D and a new
    cache entry will be generated. One case were this will not happen is when
    code snippets included by a graphs are changed. To work around that,
    clearing the cache directory or setting environment variable
    QT3D_REBUILD_SHADER_CACHE can be used to force shader code to be generated
    again.
*/

/*!
    \qmltype ShaderProgramBuilder
    \nativetype Qt3DRender::QShaderProgramBuilder
    \inqmlmodule Qt3D.Render
    \brief Generates a Shader Program content from loaded graphs.
    \since 5.10

    A shader program builder consists of several different shader graphs
    used to generate shader code.

    A cache of generated shader code is maintained. Generated shaders are by
    defaults saved in
    QStandardPaths::writableLocation(QStandardPaths::TempLocation)). This path
    can be overridden by setting environment variable QT3D_WRITABLE_CACHE_PATH
    to a valid writable path.

    The use of the cache can be disabled by setting environment variable
    QT3D_DISABLE_SHADER_CACHE.

    In most cases, changes made to a graph are detected by Qt 3D and a new
    cache entry will be generated. One case were this will not happen is when
    code snippets included by a graphs are changed. To work around that,
    clearing the cache directory or setting environment variable
    QT3D_REBUILD_SHADER_CACHE can be used to force shader code to be generated
    again.
*/

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QShaderProgramBuilderPrivate::QShaderProgramBuilderPrivate()
    : QNodePrivate(),
      m_shaderProgram(nullptr)
{
}

void QShaderProgramBuilderPrivate::setShaderCode(const QByteArray &code, QShaderProgram::ShaderType type)
{
    Q_Q(QShaderProgramBuilder);
    const bool blocked = q->blockNotifications(true);

    switch (type) {
    case QShaderProgram::Vertex: {
        m_vertexShaderCode = code;
        emit q->vertexShaderCodeChanged(m_vertexShaderCode);
        break;
    }
    case QShaderProgram::Fragment:{
        m_fragmentShaderCode = code;
        emit q->fragmentShaderCodeChanged(m_fragmentShaderCode);
        break;
    }
    case QShaderProgram::Geometry: {
        m_geometryShaderCode = code;
        emit q->geometryShaderCodeChanged(m_geometryShaderCode);
        break;
    }
    case QShaderProgram::Compute: {
        m_computeShaderCode = code;
        emit q->computeShaderCodeChanged(m_computeShaderCode);
        break;
    }
    case QShaderProgram::TessellationControl: {
        m_tessControlShaderCode = code;
        emit q->tessellationControlShaderCodeChanged(m_tessControlShaderCode);
        break;
    }
    case QShaderProgram::TessellationEvaluation: {
        m_tessEvalShaderCode = code;
        emit q->tessellationEvaluationShaderCodeChanged(m_tessEvalShaderCode);
        break;
    }
    }

    q->blockNotifications(blocked);
}

QShaderProgramBuilder::QShaderProgramBuilder(QNode *parent)
    : QNode(*new QShaderProgramBuilderPrivate, parent)
{
}

QShaderProgramBuilder::~QShaderProgramBuilder()
{
}

/*! \internal */
QShaderProgramBuilder::QShaderProgramBuilder(QShaderProgramBuilderPrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

/*!
    \qmlproperty string ShaderProgramBuilder::shaderProgram

    Holds the shader program on which this builder generates code.
*/
/*!
    \property Qt3DRender::QShaderProgramBuilder::shaderProgram

    Holds the shader program on which this builder generates code.
*/
void QShaderProgramBuilder::setShaderProgram(QShaderProgram *program)
{
    Q_D(QShaderProgramBuilder);
    if (program != d->m_shaderProgram) {

        if (d->m_shaderProgram)
            d->unregisterDestructionHelper(d->m_shaderProgram);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, it gets destroyed as well
        if (program && !program->parent())
            program->setParent(this);
        d->m_shaderProgram = program;

        // Ensures proper bookkeeping
        if (d->m_shaderProgram)
            d->registerDestructionHelper(d->m_shaderProgram, &QShaderProgramBuilder::setShaderProgram, d->m_shaderProgram);

        emit shaderProgramChanged(program);
    }
}

QShaderProgram *QShaderProgramBuilder::shaderProgram() const
{
    Q_D(const QShaderProgramBuilder);
    return d->m_shaderProgram;
}

/*!
    \qmlproperty stringlist ShaderProgramBuilder::enabledLayers

    Holds the list of layers this builder will activate on the shader graphs
    during code generation.
*/
/*!
    \property Qt3DRender::QShaderProgramBuilder::enabledLayers

    Holds the list of layers this builder will activate on the shader graphs
    during code generation.
*/
void QShaderProgramBuilder::setEnabledLayers(const QStringList &layers)
{
    Q_D(QShaderProgramBuilder);
    if (layers != d->m_enabledLayers) {
        d->m_enabledLayers = layers;
        emit enabledLayersChanged(layers);
    }
}

QStringList QShaderProgramBuilder::enabledLayers() const
{
    Q_D(const QShaderProgramBuilder);
    return d->m_enabledLayers;
}

/*!
    \qmlproperty string ShaderProgram::vertexShaderGraph

    Holds the URL to the vertex shader graph used by this shader program builder.
*/
/*!
    \property Qt3DRender::QShaderProgramBuilder::vertexShaderGraph

    Holds the URL to the vertex shader graph used by this shader program builder.
*/
void QShaderProgramBuilder::setVertexShaderGraph(const QUrl &vertexShaderGraph)
{
    Q_D(QShaderProgramBuilder);
    if (vertexShaderGraph != d->m_vertexShaderGraph) {
        d->m_vertexShaderGraph = vertexShaderGraph;
        emit vertexShaderGraphChanged(vertexShaderGraph);
    }
}

QUrl QShaderProgramBuilder::vertexShaderGraph() const
{
    Q_D(const QShaderProgramBuilder);
    return d->m_vertexShaderGraph;
}

/*!
    \qmlproperty string ShaderProgram::tessellationControlShaderGraph

    Holds the URL to the tesselation control shader graph used by this shader program builder.
*/
/*!
    \property Qt3DRender::QShaderProgramBuilder::tessellationControlShaderGraph

    Holds the URL to the tesselation control shader graph used by this shader program builder.
*/
void QShaderProgramBuilder::setTessellationControlShaderGraph(const QUrl &tessellationControlShaderGraph)
{
    Q_D(QShaderProgramBuilder);
    if (tessellationControlShaderGraph != d->m_tessControlShaderGraph) {
        d->m_tessControlShaderGraph = tessellationControlShaderGraph;
        emit tessellationControlShaderGraphChanged(tessellationControlShaderGraph);
    }
}

QUrl QShaderProgramBuilder::tessellationControlShaderGraph() const
{
    Q_D(const QShaderProgramBuilder);
    return d->m_tessControlShaderGraph;
}

/*!
    \qmlproperty string ShaderProgram::tessellationEvaluationShaderGraph

    Holds the URL to the tesselation evaluation shader graph used by this shader program builder.
*/
/*!
    \property Qt3DRender::QShaderProgramBuilder::tessellationEvaluationShaderGraph

    Holds the URL to the tesselation evaluation shader graph used by this shader program builder.
*/
void QShaderProgramBuilder::setTessellationEvaluationShaderGraph(const QUrl &tessellationEvaluationShaderGraph)
{
    Q_D(QShaderProgramBuilder);
    if (tessellationEvaluationShaderGraph != d->m_tessEvalShaderGraph) {
        d->m_tessEvalShaderGraph = tessellationEvaluationShaderGraph;
        emit tessellationEvaluationShaderGraphChanged(tessellationEvaluationShaderGraph);
    }
}

QUrl QShaderProgramBuilder::tessellationEvaluationShaderGraph() const
{
    Q_D(const QShaderProgramBuilder);
    return d->m_tessEvalShaderGraph;
}

/*!
    \qmlproperty string ShaderProgram::geometryShaderGraph

    Holds the URL to the geometry shader graph used by this shader program builder.
*/
/*!
    \property Qt3DRender::QShaderProgramBuilder::geometryShaderGraph

    Holds the URL to the geometry shader graph used by this shader program builder.
*/
void QShaderProgramBuilder::setGeometryShaderGraph(const QUrl &geometryShaderGraph)
{
    Q_D(QShaderProgramBuilder);
    if (geometryShaderGraph != d->m_geometryShaderGraph) {
        d->m_geometryShaderGraph = geometryShaderGraph;
        emit geometryShaderGraphChanged(geometryShaderGraph);
    }
}

QUrl QShaderProgramBuilder::geometryShaderGraph() const
{
    Q_D(const QShaderProgramBuilder);
    return d->m_geometryShaderGraph;
}

/*!
    \qmlproperty string ShaderProgram::fragmentShaderGraph

    Holds the URL to the fragment shader graph used by this shader program builder.
*/
/*!
    \property Qt3DRender::QShaderProgramBuilder::fragmentShaderGraph

    Holds the URL to the fragment shader graph used by this shader program builder.
*/
void QShaderProgramBuilder::setFragmentShaderGraph(const QUrl &fragmentShaderGraph)
{
    Q_D(QShaderProgramBuilder);
    if (fragmentShaderGraph != d->m_fragmentShaderGraph) {
        d->m_fragmentShaderGraph = fragmentShaderGraph;
        emit fragmentShaderGraphChanged(fragmentShaderGraph);
    }
}

QUrl QShaderProgramBuilder::fragmentShaderGraph() const
{
    Q_D(const QShaderProgramBuilder);
    return d->m_fragmentShaderGraph;
}

/*!
    \qmlproperty string ShaderProgram::computeShaderGraph

    Holds the URL to the compute shader graph used by this shader program builder.
*/
/*!
    \property Qt3DRender::QShaderProgramBuilder::computeShaderGraph

    Holds the URL to the compute shader graph used by this shader program builder.
*/
void QShaderProgramBuilder::setComputeShaderGraph(const QUrl &computeShaderGraph)
{
    Q_D(QShaderProgramBuilder);
    if (computeShaderGraph != d->m_computeShaderGraph) {
        d->m_computeShaderGraph = computeShaderGraph;
        emit computeShaderGraphChanged(computeShaderGraph);
    }
}

QUrl QShaderProgramBuilder::computeShaderGraph() const
{
    Q_D(const QShaderProgramBuilder);
    return d->m_computeShaderGraph;
}

/*!
    \qmlproperty string ShaderProgramBuilder::vertexShaderCode

    Holds the generated vertex shader code
    \since 2.13
*/
/*!
    \property Qt3DRender::QShaderProgramBuilder::vertexShaderCode

    Holds the generate vertex shader code.
    \since 5.13
*/
QByteArray QShaderProgramBuilder::vertexShaderCode() const
{
    Q_D(const QShaderProgramBuilder);
    return d->m_vertexShaderCode;
}

/*!
    \qmlproperty string ShaderProgramBuilder::tessellationControlShaderCode

    Holds the generated tessellation control shader code
    \since 2.13
*/
/*!
    \property Qt3DRender::QShaderProgramBuilder::tessellationControlShaderCode

    Holds the generate tessellation control shader code.
    \since 5.13
*/
QByteArray QShaderProgramBuilder::tessellationControlShaderCode() const
{
    Q_D(const QShaderProgramBuilder);
    return d->m_tessControlShaderCode;
}

/*!
    \qmlproperty string ShaderProgramBuilder::tessellationEvaluationShaderCode

    Holds the generated tessellation evaluation shader code
    \since 2.13
*/
/*!
    \property Qt3DRender::QShaderProgramBuilder::tessellationEvaluationShaderCode

    Holds the generate tessellation evaluation shader code.
    \since 5.13
*/
QByteArray QShaderProgramBuilder::tessellationEvaluationShaderCode() const
{
    Q_D(const QShaderProgramBuilder);
    return d->m_tessEvalShaderCode;
}

/*!
    \qmlproperty string ShaderProgramBuilder::geometryShaderCode

    Holds the generated geometry shader code
    \since 2.13
*/
/*!
    \property Qt3DRender::QShaderProgramBuilder::geometryShaderCode

    Holds the generate geometry shader code.
    \since 5.13
*/
QByteArray QShaderProgramBuilder::geometryShaderCode() const
{
    Q_D(const QShaderProgramBuilder);
    return d->m_geometryShaderCode;
}

/*!
    \qmlproperty string ShaderProgramBuilder::fragmentShaderCode

    Holds the generated fragment shader code
    \since 2.13
*/
/*!
    \property Qt3DRender::QShaderProgramBuilder::fragmentShaderCode

    Holds the generate fragment shader code.
    \since 5.13
*/
QByteArray QShaderProgramBuilder::fragmentShaderCode() const
{
    Q_D(const QShaderProgramBuilder);
    return d->m_fragmentShaderCode;
}

/*!
    \qmlproperty string ShaderProgramBuilder::computeShaderCode

    Holds the generated compute shader code
    \since 2.13
*/
/*!
    \property Qt3DRender::QShaderProgramBuilder::computeShaderCode

    Holds the generate compute shader code.
    \since 5.13
*/
QByteArray QShaderProgramBuilder::computeShaderCode() const
{
    Q_D(const QShaderProgramBuilder);
    return d->m_computeShaderCode;
}

} // of namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qshaderprogrambuilder.cpp"
