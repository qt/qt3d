// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qrenderpassfilter.h"
#include "qrenderpassfilter_p.h"

#include <Qt3DRender/qfilterkey.h>
#include <Qt3DRender/qparameter.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {

/*!
    \class Qt3DRender::QRenderPassFilter
    \inmodule Qt3DRender
    \since 5.7
    \brief Provides storage for vectors of Filter Keys and Parameters.

    A Qt3DRender::QRenderPassFilter FrameGraph node is used to select which
    Qt3DRender::QRenderPass objects are selected for drawing. QRenderPassFilter
    specifies a list of Qt3DRender::QFilterKey objects and Qt3DRender::QParameter objects.
    When QRenderPassFilter is present in the FrameGraph, only the QRenderPass objects,
    whose Qt3DRender::QFilterKey objects match the keys in QRenderPassFilter are
    selected for rendering. If no QRenderPassFilter is present, then all QRenderPass
    objects are selected for rendering. The parameters in the list can be used
    to set values for shader parameters. The parameters in QRenderPassFilter
    override parameters in QTechniqueFilter, QMaterial, QEffect, QTechnique and QRenderPass.
*/

/*!
    \qmltype RenderPassFilter
    \inqmlmodule Qt3D.Render
    \since 5.7
    \instantiates Qt3DRender::QRenderPassFilter
    \inherits FrameGraphNode
    \brief Provides storage for vectors of Filter Keys and Parameters.

    A RenderPassFilter FrameGraph node is used to select which RenderPass
    objects are selected for drawing. When RenderPassFilter is present in the FrameGraph,
    only the RenderPass objects, whose FilterKey objects match the keys
    in RenderPassFilter are selected for rendering. If no RenderPassFilter is present,
    then all RenderPass objects are selected for rendering. RenderPassFilter
    specifies a list of Parameter objects. The parameters in the list can be used
    to set values for shader parameters. The parameters in RenderPassFilter
    override parameters in TechniqueFilter, Material, Effect, Technique and RenderPass.
*/

/*!
    \qmlproperty list<FilterKey> RenderPassFilter::matchAny
    Holds the list of filterkeys used by the RenderPassFilter
*/
/*!
    \qmlproperty list<Parameter> RenderPassFilter::parameters
    Holds the list of parameters used by the RenderPassFilter
*/


/*!
    The constructor creates an instance with the specified \a parent.
 */
QRenderPassFilter::QRenderPassFilter(QNode *parent)
    : QFrameGraphNode(*new QRenderPassFilterPrivate, parent)
{}

/*! \internal */
QRenderPassFilter::~QRenderPassFilter()
{
}

/*! \internal */
QRenderPassFilter::QRenderPassFilter(QRenderPassFilterPrivate &dd, QNode *parent)
    : QFrameGraphNode(dd, parent)
{
}

/*!
    Returns a vector of the current keys for the filter.
 */
QList<QFilterKey *> QRenderPassFilter::matchAny() const
{
    Q_D(const QRenderPassFilter);
    return d->m_matchList;
}

/*!
    Add the \a filterKey to the match vector.
 */
void QRenderPassFilter::addMatch(QFilterKey *filterKey)
{
    Q_ASSERT(filterKey);
    Q_D(QRenderPassFilter);
    if (!d->m_matchList.contains(filterKey)) {
        d->m_matchList.append(filterKey);

        // Ensures proper bookkeeping
        d->registerDestructionHelper(filterKey, &QRenderPassFilter::removeMatch, d->m_matchList);

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
    Remove the \a filterKey from the match vector.
 */
void QRenderPassFilter::removeMatch(QFilterKey *filterKey)
{
    Q_ASSERT(filterKey);
    Q_D(QRenderPassFilter);

    if (!d->m_matchList.removeOne(filterKey))
        return;
    d->update();
    // Remove bookkeeping connection
    d->unregisterDestructionHelper(filterKey);
}

/*!
    Add the given \a parameter to the parameter vector.
 */
void QRenderPassFilter::addParameter(QParameter *parameter)
{
    Q_ASSERT(parameter);
    Q_D(QRenderPassFilter);
    if (!d->m_parameters.contains(parameter)) {
        d->m_parameters.append(parameter);

        // Ensures proper bookkeeping
        d->registerDestructionHelper(parameter, &QRenderPassFilter::removeParameter, d->m_parameters);

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
    Remove the given \a parameter from the parameter vector.
 */
void QRenderPassFilter::removeParameter(QParameter *parameter)
{
    Q_ASSERT(parameter);
    Q_D(QRenderPassFilter);

    if (!d->m_parameters.removeOne(parameter))
        return;
    d->update();
    // Remove bookkeeping connection
    d->unregisterDestructionHelper(parameter);
}

/*!
    Returns the current vector of parameters.
 */
QList<QParameter *> QRenderPassFilter::parameters() const
{
    Q_D(const QRenderPassFilter);
    return d->m_parameters;
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qrenderpassfilter.cpp"
