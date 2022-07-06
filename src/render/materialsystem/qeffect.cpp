// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qeffect.h"
#include "qeffect_p.h"
#include "qtechnique.h"
#include "qparameter.h"

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {

QEffectPrivate::QEffectPrivate()
    : QNodePrivate()
{
}

/*!
    \class Qt3DRender::QEffect
    \inmodule Qt3DRender
    \inherits Qt3DCore::QNode
    \since 5.7
    \brief The base class for effects in a Qt 3D scene.

    The QEffect class combines a set of techniques and parameters used by those techniques to
    produce a rendering effect for a material.

    An QEffect instance should be shared among several QMaterial instances when possible.

    \note QEffect node can not be disabled.

    \code
    QEffect *effect = new QEffect();

    // Create technique, render pass and shader
    QTechnique *gl3Technique = new QTechnique();
    QRenderPass *gl3Pass = new QRenderPass();
    QShaderProgram *glShader = new QShaderProgram();

    // Set the shader on the render pass
    gl3Pass->setShaderProgram(glShader);

    // Add the pass to the technique
    gl3Technique->addRenderPass(gl3Pass);

    // Set the targeted GL version for the technique
    gl3Technique->graphicsApiFilter()->setApi(QGraphicsApiFilter::OpenGL);
    gl3Technique->graphicsApiFilter()->setMajorVersion(3);
    gl3Technique->graphicsApiFilter()->setMinorVersion(1);
    gl3Technique->graphicsApiFilter()->setProfile(QGraphicsApiFilter::CoreProfile);

    // Add the technique to the effect
    effect->addTechnique(gl3Technique);
    \endcode

    A QParameter defined on a QEffect overrides parameter (of the same
    name) defined in QTechnique and QRenderPass, but are overridden by parameter in
    QRenderPassFilter, QTechniqueFilter and QMaterial.

    \sa QMaterial, QTechnique, QParameter
*/

/*!
    \qmltype Effect
    \instantiates Qt3DRender::QEffect
    \inherits Node
    \inqmlmodule Qt3D.Render
    \since 5.7
    \brief The base class for effects in a Qt 3D scene.

    The Effect type combines a set of techniques and parameters used by those techniques to
    produce a rendering effect for a material.

    An Effect instance should be shared among several Material instances when possible.

    A Parameter defined on a Effect overrides parameter (of the same
    name) defined in Technique and RenderPass, but are overridden by parameter in
    RenderPassFilter, TechniqueFilter and Material.

    \note Effect node can not be disabled.

    \code
    Effect {
        id: effect

        techniques: [
            Technique {
                id: gl3Technique
                graphicsApiFilter {
                    api: GraphicsApiFilter.OpenGL
                    profile: GraphicsApiFilter.CoreProfile
                    majorVersion: 3
                    minorVersion: 1
                }
                renderPasses: [
                    RenderPass {
                        id: gl3Pass
                        shaderProgram: ShaderProgram {
                            ...
                        }
                    }
                ]
            }
        ]
    }
    \endcode

    \sa Material, Technique, Parameter
*/

QEffect::QEffect(QNode *parent)
    : QNode(*new QEffectPrivate, parent)
{
}

QEffect::~QEffect()
{
}

/*! \internal */
QEffect::QEffect(QEffectPrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

/*!
    \qmlproperty list<Technique> Effect::techniques

    Holds the list of techniques used by this effect.
*/
/*!
    \qmlproperty list<Parameter> Effect::parameters

    Holds the list of parameters used by this effect.
    A parameter is used to set a corresponding uniform value in the shader used by this effect.
*/

/*!
 * Adds \a parameter to the effect. It sends an update to the backend.
 * The \a parameter will be used to set a corresponding uniform value in the shader used
 * by this effect.
 */
void QEffect::addParameter(QParameter *parameter)
{
    Q_D(QEffect);
    if (parameter && !d->m_parameters.contains(parameter)) {
        d->m_parameters.append(parameter);

        // Ensures proper bookkeeping
        d->registerDestructionHelper(parameter, &QEffect::removeParameter, d->m_parameters);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, it gets destroyed as well
        if (!parameter->parent())
            parameter->setParent(this);

        d->update();
    }
}

/*!
 * Removes a parameter \a parameter from the effect.
 */
void QEffect::removeParameter(QParameter *parameter)
{
    Q_D(QEffect);

    if (!d->m_parameters.removeOne(parameter))
        return;
    // Remove bookkeeping connection
    d->unregisterDestructionHelper(parameter);
    d->update();
}

/*!
 * Returns the list of parameters used by the effect.
 */
QList<QParameter *> QEffect::parameters() const
{
    Q_D(const QEffect);
    return d->m_parameters;
}

/*!
 * Adds a new technique \a t to the effect. It sends an update to the backend.
 */
void QEffect::addTechnique(QTechnique *t)
{
    Q_ASSERT(t);
    Q_D(QEffect);
    if (t && !d->m_techniques.contains(t)) {
        d->m_techniques.append(t);

        // Ensures proper bookkeeping
        d->registerDestructionHelper(t, &QEffect::removeTechnique, d->m_techniques);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, tit gets destroyed as well
        if (!t->parent())
            t->setParent(this);

        d->update();
    }
}

/*!
 * Removes a technique \a t from the effect.
 */
void QEffect::removeTechnique(QTechnique *t)
{
    Q_D(QEffect);
    if (!d->m_techniques.removeOne(t))
        return;
    d->update();
    // Remove bookkeeping connection
    d->unregisterDestructionHelper(t);
}

/*!
 * Returns the list of techniques used by the effect.
 */
QList<QTechnique *> QEffect::techniques() const
{
    Q_D(const QEffect);
    return d->m_techniques;
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qeffect.cpp"
