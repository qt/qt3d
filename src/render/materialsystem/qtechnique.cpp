// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qtechnique.h"
#include "qtechnique_p.h"
#include "qparameter.h"
#include "qgraphicsapifilter.h"

QT_BEGIN_NAMESPACE


namespace Qt3DRender {

using namespace Qt3DCore;

QTechniquePrivate::QTechniquePrivate()
    : QNodePrivate()
{
}

QTechniquePrivate::~QTechniquePrivate()
{
}

/*!
    \qmltype Technique
    \nativetype Qt3DRender::QTechnique
    \inqmlmodule Qt3D.Render
    \inherits Qt3D.Core::Node
    \since 5.7
    \brief Encapsulates a Technique.

    A Technique specifies a set of RenderPass objects, FilterKey objects,
    Parameter objects and a GraphicsApiFilter, which together define a
    rendering technique the given graphics API can render. The filter keys are
    used by TechniqueFilter to select specific techniques at specific parts of
    the FrameGraph. A Parameter defined on a Technique overrides parameter
    (of the same name) defined in RenderPass, but are overridden by
    parameter in RenderPassFilter, TechniqueFilter, Material and Effect.

    When creating an Effect that targets several versions of a graphics API, it
    is useful to create several Technique nodes each with a graphicsApiFilter
    set to match one of the targeted versions. At runtime, the Qt3D renderer
    will select the most appropriate Technique based on which graphics API
    versions are supported and (if specified) the FilterKey nodes that satisfy
    a given TechniqueFilter in the FrameGraph.

    \note When using OpenGL as the graphics API for rendering, Qt3D relies on
    the QSurfaceFormat returned by QSurfaceFormat::defaultFormat() at runtime
    to decide what is the most appropriate GL version available. If you need to
    customize the QSurfaceFormat, do not forget to apply it with
    QSurfaceFormat::setDefaultFormat(). Setting the QSurfaceFormat on the view
    will likely have no effect on Qt3D related rendering.

    \note Technique node can not be disabled.

    \qml
    Technique {
        id: gl3Technique
        parameters: [
            Parameter { name: "color"; value: "orange" }
        ]
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
                id: firstPass
                shaderProgram: ShaderProgram {
                    // ...
                }
            },
            RenderPass {
                id: secondPass
                shaderProgram: ShaderProgram {
                    // ...
                }
            }
        ]
    }
    \endqml

    \sa Effect, RenderPass, TechniqueFilter
 */

/*!
    \class Qt3DRender::QTechnique
    \inmodule Qt3DRender
    \inherits Node
    \since 5.7
    \brief Encapsulates a Technique.

    A Qt3DRender::QTechnique specifies a set of Qt3DRender::QRenderPass
    objects, Qt3DRender::QFilterKey objects, Qt3DRender::QParameter objects and
    a Qt3DRender::QGraphicsApiFilter, which together define a rendering
    technique the given graphics API can render. The filter keys are used by
    Qt3DRender::QTechniqueFilter to select specific techniques at specific
    parts of the FrameGraph. A QParameter defined on a QTechnique overrides parameter
    (of the same name) defined in QRenderPass, but are overridden by
    parameter in QRenderPassFilter, QTechniqueFilter, QMaterial and QEffect.

    When creating an QEffect that targets several versions of a graphics API,
    it is useful to create several QTechnique nodes each with a
    graphicsApiFilter set to match one of the targeted GL versions. At runtime,
    the Qt3D renderer will select the most appropriate QTechnique based on
    which graphics API versions are supported and (if specified) the QFilterKey
    nodes that satisfy a given QTechniqueFilter in the FrameGraph.

    \note When using OpenGL as the graphics API for rendering, Qt3D relies on
    the QSurfaceFormat returned by QSurfaceFormat::defaultFormat() at runtime
    to decide what is the most appropriate GL version available. If you need to
    customize the QSurfaceFormat, do not forget to apply it with
    QSurfaceFormat::setDefaultFormat(). Setting the QSurfaceFormat on the view
    will likely have no effect on Qt3D related rendering.

    \note QTechnique node can not be disabled.

    \code
    QTechnique *gl3Technique = new QTechnique();

    // Create the render passes
    QRenderPass *firstPass = new QRenderPass();
    QRenderPass *secondPass = new QRenderPass();

    // Add the passes to the technique
    gl3Technique->addRenderPass(firstPass);
    gl3Technique->addRenderPass(secondPass);

    // Set the targeted GL version for the technique
    gl3Technique->graphicsApiFilter()->setApi(QGraphicsApiFilter::OpenGL);
    gl3Technique->graphicsApiFilter()->setMajorVersion(3);
    gl3Technique->graphicsApiFilter()->setMinorVersion(1);
    gl3Technique->graphicsApiFilter()->setProfile(QGraphicsApiFilter::CoreProfile);

    // Create a FilterKey
    QFilterKey *filterKey = new QFilterKey();
    filterKey->setName(QStringLiteral("name"));
    fitlerKey->setValue(QStringLiteral("zFillPass"));

    // Add the FilterKey to the Technique
    gl3Technique->addFilterKey(filterKey);

    // Create a QParameter
    QParameter *colorParameter = new QParameter(QStringLiteral("color"), QColor::fromRgbF(0.0f, 0.0f, 1.0f, 1.0f));

    // Add parameter to technique
    gl3Technique->addParameter(colorParameter);
    \endcode

    \sa QEffect, QRenderPass, QTechniqueFilter
 */

/*!
    \qmlproperty GraphicsApiFilter Qt3D.Render::Technique::graphicsApiFilter
    Specifies the graphics API filter being used
*/
/*!
    \qmlproperty list<FilterKey> Qt3D.Render::Technique::filterKeys
    Specifies the list of filter keys enabling this technique
*/
/*!
    \qmlproperty list<RenderPass> Qt3D.Render::Technique::renderPasses
    Specifies the render passes used by the tehcnique
*/
/*!
    \qmlproperty list<Parameter> Qt3D.Render::Technique::parameters
    Specifies the parameters used by the technique
*/
/*!
    \property Qt3DRender::QTechnique::graphicsApiFilter
    Specifies the graphics API filter being used
 */

QTechnique::QTechnique(QNode *parent)
    : QNode(*new QTechniquePrivate, parent)
{
    Q_D(QTechnique);
    QObject::connect(&d->m_graphicsApiFilter, SIGNAL(graphicsApiFilterChanged()), this, SLOT(_q_graphicsApiFilterChanged()));
}

/*! \internal */
QTechnique::~QTechnique()
{
}

/*! \internal */
QTechnique::QTechnique(QTechniquePrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
    Q_D(QTechnique);
    QObject::connect(&d->m_graphicsApiFilter, SIGNAL(graphicsApiFilterChanged()), this, SLOT(_q_graphicsApiFilterChanged()));
}

/*! \internal */
void QTechniquePrivate::_q_graphicsApiFilterChanged()
{
    update();
}

/*!
    Add \a filterKey to the Qt3DRender::QTechnique local filter keys.
 */
void QTechnique::addFilterKey(QFilterKey *filterKey)
{
    Q_ASSERT(filterKey);
    Q_D(QTechnique);
    if (!d->m_filterKeys.contains(filterKey)) {
        d->m_filterKeys.append(filterKey);

        // Ensures proper bookkeeping
        d->registerDestructionHelper(filterKey, &QTechnique::removeFilterKey, d->m_filterKeys);

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
    Removes \a filterKey from the Qt3DRender::QTechnique local filter keys.
 */
void QTechnique::removeFilterKey(QFilterKey *filterKey)
{
    Q_ASSERT(filterKey);
    Q_D(QTechnique);
    if (!d->m_filterKeys.removeOne(filterKey))
        return;
    d->update();
    // Remove bookkeeping connection
    d->unregisterDestructionHelper(filterKey);
}

/*!
    Returns the list of Qt3DCore::QFilterKey key objects making up the filter keys
    of the Qt3DRender::QTechnique.
 */
QList<QFilterKey *> QTechnique::filterKeys() const
{
    Q_D(const QTechnique);
    return d->m_filterKeys;
}

/*!
    Add \a parameter to the technique's parameters.
 */
void QTechnique::addParameter(QParameter *parameter)
{
    Q_ASSERT(parameter);
    Q_D(QTechnique);
    if (!d->m_parameters.contains(parameter)) {
        d->m_parameters.append(parameter);

        // Ensures proper bookkeeping
        d->registerDestructionHelper(parameter, &QTechnique::removeParameter, d->m_parameters);

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
    Remove \a parameter from the technique's parameters.
 */
void QTechnique::removeParameter(QParameter *parameter)
{
    Q_ASSERT(parameter);
    Q_D(QTechnique);
    if (!d->m_parameters.removeOne(parameter))
        return;
    d->update();
    // Remove bookkeeping connection
    d->unregisterDestructionHelper(parameter);
}

/*!
    Appends a \a pass to the technique.
 */
void QTechnique::addRenderPass(QRenderPass *pass)
{
    Q_ASSERT(pass);
    Q_D(QTechnique);
    if (!d->m_renderPasses.contains(pass)) {
        d->m_renderPasses.append(pass);

        // Ensures proper bookkeeping
        d->registerDestructionHelper(pass, &QTechnique::removeRenderPass, d->m_renderPasses);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, it gets destroyed as well
        if (!pass->parent())
            pass->setParent(this);

        d->update();
    }
}

/*!
    Removes a \a pass from the technique.
 */
void QTechnique::removeRenderPass(QRenderPass *pass)
{
    Q_ASSERT(pass);
    Q_D(QTechnique);
    if (!d->m_renderPasses.removeOne(pass))
        return;
    d->update();
    // Remove bookkeeping connection
    d->unregisterDestructionHelper(pass);
}

/*!
    Returns the list of render passes contained in the technique.
 */
QList<QRenderPass *> QTechnique::renderPasses() const
{
    Q_D(const QTechnique);
    return d->m_renderPasses;
}

/*!
    Returns a vector of the techniques current parameters
 */
QList<QParameter *> QTechnique::parameters() const
{
    Q_D(const QTechnique);
    return d->m_parameters;
}

QGraphicsApiFilter *QTechnique::graphicsApiFilter()
{
    Q_D(QTechnique);
    return &d->m_graphicsApiFilter;
}

const QGraphicsApiFilter *QTechnique::graphicsApiFilter() const
{
    Q_D(const QTechnique);
    return &d->m_graphicsApiFilter;
}

} // of namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qtechnique.cpp"
