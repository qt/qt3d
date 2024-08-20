// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qrenderpass.h"
#include "qrenderpass_p.h"
#include "qparameter.h"
#include "qfilterkey.h"
#include "qrenderstate.h"
#include <Qt3DCore/private/qnode_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {

QRenderPassPrivate::QRenderPassPrivate()
    : QNodePrivate()
    , m_shader(nullptr)
{
}
/*!
    \qmltype RenderPass
    \nativetype Qt3DRender::QRenderPass
    \inqmlmodule Qt3D.Render
    \inherits Node
    \since 5.7
    \brief Encapsulates a Render Pass.

    A RenderPass specifies a single rendering pass - an instance of shader program
    execution - used by Technique. A Render pass consists of a ShaderProgram and
    a list of FilterKey objects, a list of RenderState objects and a list
    of \l Parameter objects.

    RenderPass executes the ShaderProgram using the given RenderState and
    Parameter nodes when at least one of FilterKey nodes being referenced
    matches any of the FilterKey nodes in RenderPassFilter or when no
    RenderPassFilter is present in the FrameGraph.

    If the RenderPass defines a Parameter, it will be overridden by a Parameter
    with the same name if it exists in any of the Technique, Effect, Material,
    TechniqueFilter, RenderPassFilter associated with the pass at runtime. This
    still can be useful to define sane default values.

    At render time, for each leaf node of the FrameGraph a base render state is
    recorded by accumulating states defined by all RenderStateSet nodes in the
    FrameGraph branch. Each RenderPass can overload this base render state by
    specifying its own RenderState nodes.

    \qml
    Technique {
        filterKeys: [
            FilterKey { name: "renderingStyle"; value: "forward" }
        ]
        graphicsApiFilter: {
            api: GraphicsApiFilter.OpenGL
            profile: GraphicsApiFilter.CoreProfile
            majorVersion: 3
            minorVersion: 1
        }
        renderPasses: [
            RenderPass {
                id: pass
                shaderProgram: ShaderProgram {
                    // ...
                }
                parameters: [
                    Parameter { name: "color"; value: "red" }
                ]
                renderStates: [
                    DepthTest {}
                ]
            }
        ]
    }
    \endqml

    \sa RenderPassFilter, FilterKey, Parameter, RenderState, Effect, Technique
 */

/*!
    \class Qt3DRender::QRenderPass
    \inmodule Qt3DRender
    \since 5.7
    \brief Encapsulates a Render Pass.

    A Qt3DRender::QRenderPass specifies a single rendering pass - an instance of shader
    program execution - used by Qt3DRender::QTechnique. Render pass consists
    of a Qt3DRender::QShaderProgram and a list of Qt3DRender::QFilterKey objects,
    a list of Qt3DRender::QRenderState objects and a list of Qt3DRender::QParameter objects.

    QRenderPass executes the QShaderProgram using the given QRenderState and
    QParameter nodes when at least one of QFilterKey nodes being referenced
    matches any of the QFilterKey nodes in QRenderPassFilter or when no
    QFilterKey nodes are specified and no QRenderPassFilter is present in the
    FrameGraph.

    If the QRenderPass defines a QParameter, it will be overridden by a
    QParameter with the same name if it exists in any of the QTechnique,
    QEffect, QMaterial, QTechniqueFilter, QRenderPassFilter associated with the
    pass at runtime. This still can be useful to define sane default values.

    At render time, for each leaf node of the FrameGraph a base render state is
    recorded by accumulating states defined by all QRenderStateSet nodes in the
    FrameGraph branch. Each QRenderPass can overload this base render state by
    specifying its own QRenderState nodes.

    \code
    // Create the render passes
    QRenderPass *pass = new QRenderPass();

    // Create shader program
    QShaderProgram *glShader = new QShaderProgram();

    // Set the shader on the render pass
    pass->setShaderProgram(glShader);

    // Create a FilterKey
    QFilterKey *filterKey = new QFilterKey();
    filterKey->setName(QStringLiteral("name"));
    fitlerKey->setValue(QStringLiteral("zFillPass"));

    // Add the FilterKey to the pass
    pass->addFilterKey(filterKey);

    // Create a QParameter
    QParameter *colorParameter = new QParameter(QStringLiteral("color"), QColor::fromRgbF(0.0f, 0.0f, 1.0f, 1.0f));

    // Add parameter to pass
    pass->addParameter(colorParameter);

    // Create a QRenderState
    QDepthTest *depthTest = new QDepthTest();

    // Add the render state to the pass
    pass->addRenderState(depthTest);
    \endcode

    \sa QRenderPassFilter, QFilterKey, QParameter, QRenderState, QEffect, QTechnique
 */
/*!
    \typedef Qt3DRender::ParameterList
    \relates Qt3DRender::QRenderPass

    A list of Qt3DRender::QParameter parameters.
*/
/*!
    \qmlproperty ShaderProgram Qt3D.Render::RenderPass::shaderProgram
    Holds the shader program to be used for this render pass.
*/
/*!
    \qmlproperty list<FilterKey> Qt3D.Render::RenderPass::filterKeys
    Holds the filter keys enabling the use of this render pass.
*/
/*!
    \qmlproperty list<RenderState> Qt3D.Render::RenderPass::renderStates
    Holds the render states used by the render pass.
*/
/*!
    \qmlproperty list<Parameter> Qt3D.Render::RenderPass::parameters
    Holds the shader parameter values used by the render pass.
*/

/*!
    \property Qt3DRender::QRenderPass::shaderProgram
    Specifies the shader program to be used for this render pass.
 */

/*!
  \fn Qt3DRender::QRenderPass::QRenderPass(Qt3DCore::QNode *parent)
  Constructs a new QRenderPass with the specified \a parent.
 */
QRenderPass::QRenderPass(QNode *parent)
    : QNode(*new QRenderPassPrivate, parent)
{
}

/*! \internal */
QRenderPass::~QRenderPass()
{
}

/*! \internal */
QRenderPass::QRenderPass(QRenderPassPrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

void QRenderPass::setShaderProgram(QShaderProgram *shaderProgram)
{
    Q_D(QRenderPass);
    if (d->m_shader != shaderProgram) {

        if (d->m_shader)
            d->unregisterDestructionHelper(d->m_shader);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, it gets destroyed as well
        if (shaderProgram && !shaderProgram->parent())
            shaderProgram->setParent(this);

        d->m_shader = shaderProgram;

        // Ensures proper bookkeeping
        if (d->m_shader)
            d->registerDestructionHelper(d->m_shader, &QRenderPass::setShaderProgram, d->m_shader);

        emit shaderProgramChanged(shaderProgram);
    }
}

QShaderProgram *QRenderPass::shaderProgram() const
{
    Q_D(const QRenderPass);
    return d->m_shader;
}

/*!
    Adds \a filterKey to the Qt3DRender::QRenderPass local filter keys.
 */
void QRenderPass::addFilterKey(QFilterKey *filterKey)
{
    Q_ASSERT(filterKey);
    Q_D(QRenderPass);
    if (!d->m_filterKeyList.contains(filterKey)) {
        d->m_filterKeyList.append(filterKey);

        // Ensures proper bookkeeping
        d->registerDestructionHelper(filterKey, &QRenderPass::removeFilterKey, d->m_filterKeyList);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, it gets destroyed as well
        if (!filterKey->parent())
            filterKey->setParent(this);

        d->update();
    }
}

/*!
    Removes \a filterKey from the Qt3DRender::QRenderPass local filter keys.
 */
void QRenderPass::removeFilterKey(QFilterKey *filterKey)
{
    Q_ASSERT(filterKey);
    Q_D(QRenderPass);
    if (!d->m_filterKeyList.removeOne(filterKey))
        return;
    d->update();
    // Remove bookkeeping connection
    d->unregisterDestructionHelper(filterKey);
}

/*!
    Returns the list of Qt3DRender::QFilterKey key objects making up the filter keys
    of the Qt3DRender::QRenderPass.
 */
QList<QFilterKey *> QRenderPass::filterKeys() const
{
    Q_D(const QRenderPass);
    return d->m_filterKeyList;
}

/*!
    Adds a render \a state to the rendering pass. That implies that
    when the pass is executed at render time, the globally set render state will
    be modified by the states defined locally by the Qt3DRender::QRenderPass.

    \note not defining any Qt3DRender::QRenderState in a pass will result in the pass using
    the globally set render state for a given FrameGraph branch execution path.
 */
void QRenderPass::addRenderState(QRenderState *state)
{
    Q_ASSERT(state);
    Q_D(QRenderPass);
    if (!d->m_renderStates.contains(state)) {
        d->m_renderStates.append(state);

        // Ensures proper bookkeeping
        d->registerDestructionHelper(state, &QRenderPass::removeRenderState, d->m_renderStates);

        if (!state->parent())
            state->setParent(this);

        d->update();
    }
}

/*!
    Removes \a state from the Qt3DRender::QRenderPass local render state.
 */
void QRenderPass::removeRenderState(QRenderState *state)
{
    Q_ASSERT(state);
    Q_D(QRenderPass);
    if (!d->m_renderStates.removeOne(state))
        return;
    d->update();
    // Remove bookkeeping connection
    d->unregisterDestructionHelper(state);
}

/*!
    Returns the list of Qt3DRender::QRenderState state objects making up the render
    state of the Qt3DRender::QRenderPass.
 */
QList<QRenderState *> QRenderPass::renderStates() const
{
    Q_D(const QRenderPass);
    return d->m_renderStates;
}

/*!
    Add \a parameter to the render pass' parameters.
 */
void QRenderPass::addParameter(QParameter *parameter)
{
    Q_ASSERT(parameter);
    Q_D(QRenderPass);
    if (!d->m_parameters.contains(parameter)) {
        d->m_parameters.append(parameter);

        // Ensures proper bookkeeping
        d->registerDestructionHelper(parameter, &QRenderPass::removeParameter, d->m_parameters);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, the child parameters get destroyed as well
        if (!parameter->parent())
            parameter->setParent(this);

        d->update();
    }
}

/*!
    Remove \a parameter from the render pass' parameters.
 */
void QRenderPass::removeParameter(QParameter *parameter)
{
    Q_ASSERT(parameter);
    Q_D(QRenderPass);
    if (!d->m_parameters.removeOne(parameter))
        return;
    d->update();
    // Remove bookkeeping connection
    d->unregisterDestructionHelper(parameter);
}

/*!
    Returns a vector of the render pass' current parameters
 */
ParameterList QRenderPass::parameters() const
{
    Q_D(const QRenderPass);
    return d->m_parameters;
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qrenderpass.cpp"
