// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qmaterial.h"
#include "qmaterial_p.h"
#include "qtexture.h"
#include "qeffect.h"
#include <Qt3DRender/private/renderlogging_p.h>
#include "qparameter.h"

/*!
    \qmltype Material
    \instantiates Qt3DRender::QMaterial
    \inqmlmodule Qt3D.Render
    \brief Non-creatable abstract base for materials.
    \since 5.5

    Material provides a way to specify the rendering of an \l{Qt3D.Core::Entity}{entity}.
    Any aspect can define its own subtype of Material so that a
    Material can be used to describe a visual element; for example, the way
    sound should reflect off an element, the temperature of a surface,
    and so on.

    In itself, a Material doesn't do anything. It's only when it references an
    Effect node that a Material becomes useful.

    In practice, it often happens that a single Effect is being referenced by
    several Material components. This allows to only create the effect,
    techniques, passes and shaders once while allowing to specify the material
    by adding Parameter instances.

    A Parameter defined on a Material overrides parameter (of the same
    name) defined in a Effect, Technique and RenderPass, but are overridden by parameters in
    RenderPassFilter and TechniqueFilter.

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

    Material {
        id: material1
        parameters: [
            Parameter { name: "color"; value: "green" }
        ]
    }

    Material {
        id: material2
        parameters: [
            Parameter { name: "color"; value: "white" }
        ]
    }
    \endcode

    \sa Effect, Technique, Parameter
*/

/*!
    \class Qt3DRender::QMaterial
    \inmodule Qt3DRender
    \inherits Qt3DCore::QComponent
    \brief Provides an abstract class that should be the base of all material component classes
    in a scene.
    \since 5.5

    QMaterial provides a way to specify the rendering of an \l{Qt3DCore::QEntity}{entity}.
    Any aspect can define its own subclass of QMaterial so that a
    Material can be used to describe a visual element; for example, the way
    sound should reflect off an element, the temperature of a surface,
    and so on.

    In itself, a QMaterial doesn't do anything. It's only when it references a
    QEffect node that a QMaterial becomes useful.

    In practice, it often happens that a single QEffect is being referenced by
    several QMaterial components. This allows to only create the effect,
    techniques, passes and shaders once while allowing to specify the material
    by adding QParameter instances.

    A QParameter defined on a QMaterial overrides parameter (of the same
    name) defined in a QEffect, QTechnique and QRenderPass, but are overridden by parameter in
    QRenderPassFilter and QTechniqueFilter.

    \code
    QMaterial *material1 = new QMaterial();
    QMaterial *material2 = new QMaterial();

    // Create effect, technique, render pass and shader
    QEffect *effect = new QEffect();
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

    // Set the effect on the materials
    material1->setEffect(effect);
    material2->setEffect(effect);

    // Set different parameters on the materials
    const QString parameterName = QStringLiteral("color");
    material1->addParameter(new QParameter(parameterName, QColor::fromRgbF(0.0f, 1.0f, 0.0f, 1.0f)));
    material2->addParameter(new QParameter(parameterName, QColor::fromRgbF(1.0f, 1.0f, 1.0f, 1.0f)));

    \endcode

    \sa QEffect, QTechnique, QParameter
*/

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {

QMaterialPrivate::QMaterialPrivate()
    : QComponentPrivate()
    , m_effect(nullptr)
{
}

QMaterialPrivate::~QMaterialPrivate()
{
}

QMaterial::QMaterial(QNode *parent)
    : QComponent(*new QMaterialPrivate, parent)
{
}

QMaterial::~QMaterial()
{
}

/*! \internal */
QMaterial::QMaterial(QMaterialPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{
}

/*!
    \qmlproperty list<Parameter> Material::parameters

    Holds the list of parameters used by the material.
*/

/*!
    \qmlproperty Effect Material::effect

    Specifies the effect to be used with the material.
*/
/*!
    \property QMaterial::effect

    Specifies the effect to be used with the material.
 */
void QMaterial::setEffect(QEffect *effect)
{
    Q_D(QMaterial);
    if (effect != d->m_effect) {

        if (d->m_effect)
            d->unregisterDestructionHelper(d->m_effect);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, it gets destroyed as well
        if (effect && !effect->parent())
            effect->setParent(this);
        d->m_effect = effect;

        // Ensures proper bookkeeping
        if (d->m_effect)
            d->registerDestructionHelper(d->m_effect, &QMaterial::setEffect, d->m_effect);

        emit effectChanged(effect);
    }
}

QEffect *QMaterial::effect() const
{
    Q_D(const QMaterial);
    return d->m_effect;
}

/*!
 * Add a \a parameter to the material's parameters.
 */
void QMaterial::addParameter(QParameter *parameter)
{
    Q_ASSERT(parameter);
    Q_D(QMaterial);
    if (!d->m_parameters.contains(parameter)) {
        d->m_parameters.append(parameter);

        // Ensures proper bookkeeping
        d->registerDestructionHelper(parameter, &QMaterial::removeParameter, d->m_parameters);

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
 * Remove a \a parameter from the material's parameters.
 */
void QMaterial::removeParameter(QParameter *parameter)
{
    Q_ASSERT(parameter);
    Q_D(QMaterial);
    if (!d->m_parameters.removeOne(parameter))
        return;
    d->update();
}

/*!
 * Returns a vector of the material's current parameters
 */
QList<QParameter *> QMaterial::parameters() const
{
    Q_D(const QMaterial);
    return d->m_parameters;
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qmaterial.cpp"
