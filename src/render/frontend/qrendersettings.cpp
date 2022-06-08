// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qrendersettings.h"
#include "qrendersettings_p.h"
#include "qrendercapabilities.h"
#include "qframegraphnode.h"
#include "qrendersurfaceselector.h"
#include "qrendersurfaceselector_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
   \class Qt3DRender::QRenderSettings
   \brief The QRenderSettings class holds settings related to rendering process and host the active
   \l{Qt 3D Render Framegraph}{FrameGraph}.
   \since 5.7
   \inmodule Qt3DRender
   \inherits Qt3DCore::QComponent

   The QRenderSettings component must be set as a component of the scene root entity. It specifies
   render policy and picking settings, as well as hosts the active
   \l{Qt 3D Render Framegraph}{FrameGraph}.
 */

/*!
   \qmltype RenderSettings
   \brief The RenderSettings type holds settings related to rendering process and host the active
   \l{Qt 3D Render Framegraph}{FrameGraph}.
   \since 5.7
   \inqmlmodule Qt3D.Render
   \instantiates Qt3DRender::QRenderSettings

   The RenderSettings component must be set as a component of the scene root entity. It specifies
   render policy and picking settings, as well as hosts the active
   \l{Qt 3D Render Framegraph}{FrameGraph}.
 */


/*! \internal */
QRenderSettingsPrivate::QRenderSettingsPrivate()
    : Qt3DCore::QComponentPrivate()
    , m_activeFrameGraph(nullptr)
    , m_renderPolicy(QRenderSettings::Always)
{
}

/*! \internal */
void QRenderSettingsPrivate::init()
{
    Q_Q(QRenderSettings);
    QObject::connect(&m_pickingSettings, SIGNAL(pickMethodChanged(QPickingSettings::PickMethod)),
                     q, SLOT(_q_onPickingMethodChanged(QPickingSettings::PickMethod)));
    QObject::connect(&m_pickingSettings, SIGNAL(pickResultModeChanged(QPickingSettings::PickResultMode)),
                     q, SLOT(_q_onPickResultModeChanged(QPickingSettings::PickResultMode)));
    QObject::connect(&m_pickingSettings, SIGNAL(faceOrientationPickingModeChanged(QPickingSettings::FaceOrientationPickingMode)),
                     q, SLOT(_q_onFaceOrientationPickingModeChanged(QPickingSettings::FaceOrientationPickingMode)));
    QObject::connect(&m_pickingSettings, SIGNAL(worldSpaceToleranceChanged(float)),
                     q, SLOT(_q_onWorldSpaceToleranceChanged(float)));
}

void QRenderSettingsPrivate::invalidateFrame()
{
    update();
}

/*! \internal */
void QRenderSettingsPrivate::_q_onPickingMethodChanged(QPickingSettings::PickMethod pickMethod)
{
    notifyPropertyChange("pickMethod", pickMethod);
}

/*! \internal */
void QRenderSettingsPrivate::_q_onPickResultModeChanged(QPickingSettings::PickResultMode pickResultMode)
{
    notifyPropertyChange("pickResultMode", pickResultMode);
}

/*! \internal */
void QRenderSettingsPrivate::_q_onFaceOrientationPickingModeChanged(QPickingSettings::FaceOrientationPickingMode faceOrientationPickingMode)
{
    notifyPropertyChange("faceOrientationPickingMode", faceOrientationPickingMode);
}

/*! \internal */
void QRenderSettingsPrivate::_q_onWorldSpaceToleranceChanged(float worldSpaceTolerance)
{
    notifyPropertyChange("pickWorldSpaceTolerance", worldSpaceTolerance);
}

QRenderSettings::QRenderSettings(Qt3DCore::QNode *parent)
    : QRenderSettings(*new QRenderSettingsPrivate, parent) {}

/*! \internal */
QRenderSettings::QRenderSettings(QRenderSettingsPrivate &dd, Qt3DCore::QNode *parent)
    : Qt3DCore::QComponent(dd, parent)
{
    Q_D(QRenderSettings);
    d->init();
}

/*! \internal */
QRenderSettings::~QRenderSettings()
{
}

/*!
    \qmlproperty RenderCapabilities RenderSettings::renderCapabilities

    Holds the details of the supported rendering engine

    \readonly
    \since 5.15
*/
/*!
    \property QRenderSettings::renderCapabilities

    Holds the details of the supported rendering engine

    \readonly
    \since 5.15
*/
QRenderCapabilities *QRenderSettings::renderCapabilities()
{
    Q_D(QRenderSettings);
    return &(d->m_renderCapabilities);
}

/*!
    \qmlproperty PickingSettings RenderSettings::pickingSettings

    Holds the current pick settings for the \l{Qt 3D Render Framegraph}{FrameGraph}.

    \readonly
*/
/*!
    \property QRenderSettings::pickingSettings

    Holds the current pick settings for the \l{Qt 3D Render Framegraph}{FrameGraph}.

    \readonly
*/
QPickingSettings *QRenderSettings::pickingSettings()
{
    Q_D(QRenderSettings);
    return &(d->m_pickingSettings);
}

/*!
    \qmlproperty FrameGraphNode RenderSettings::activeFrameGraph

    Holds the currently active \l{Qt 3D Render Framegraph}{FrameGraph}.
*/
/*!
    \property QRenderSettings::activeFrameGraph

    Holds the currently active \l{Qt 3D Render Framegraph}{FrameGraph}.
*/
QFrameGraphNode *QRenderSettings::activeFrameGraph() const
{
    Q_D(const QRenderSettings);
    return d->m_activeFrameGraph;
}


/*!
    \enum QRenderSettings::RenderPolicy

    The render policy.

    \value OnDemand The \l{Qt 3D Render Framegraph}{FrameGraph} is rendered only when something
    changes.
    \value Always The \l{Qt 3D Render Framegraph}{FrameGraph} is rendered continuously, even if
    nothing has changed.
*/

/*!
    \qmlproperty enumeration RenderSettings::renderPolicy

    Holds the current render policy.

    \list
        \li RenderSettings.OnDemand
        \li RenderSettings.Always
    \endlist

    \sa Qt3DRender::QRenderSettings::RenderPolicy
*/
/*!
    \property QRenderSettings::renderPolicy

    Holds the current render policy.
*/
QRenderSettings::RenderPolicy QRenderSettings::renderPolicy() const
{
    Q_D(const QRenderSettings);
    return d->m_renderPolicy;
}

void QRenderSettings::setActiveFrameGraph(QFrameGraphNode *activeFrameGraph)
{
    Q_D(QRenderSettings);
    if (d->m_activeFrameGraph == activeFrameGraph)
        return;

    // if the old frame graph had a SurfaceSelector, use the given surface for the new framegraph, too.
    if (d->m_activeFrameGraph && activeFrameGraph) {
        Qt3DRender::QRenderSurfaceSelector *oldSurfaceSelector = Qt3DRender::QRenderSurfaceSelectorPrivate::find(d->m_activeFrameGraph);
        Qt3DRender::QRenderSurfaceSelector *newSurfaceSelector = Qt3DRender::QRenderSurfaceSelectorPrivate::find(activeFrameGraph);
        if (oldSurfaceSelector && newSurfaceSelector && oldSurfaceSelector->surface()) {
            newSurfaceSelector->setExternalRenderTargetSize(oldSurfaceSelector->externalRenderTargetSize());
            newSurfaceSelector->setSurfacePixelRatio(oldSurfaceSelector->surfacePixelRatio());
            newSurfaceSelector->setSurface(oldSurfaceSelector->surface());
        }
    }

    if (d->m_activeFrameGraph)
        d->unregisterDestructionHelper(d->m_activeFrameGraph);

    if (activeFrameGraph != nullptr && !activeFrameGraph->parent())
        activeFrameGraph->setParent(this);

    d->m_activeFrameGraph = activeFrameGraph;

    // Ensures proper bookkeeping
    if (d->m_activeFrameGraph)
        d->registerDestructionHelper(d->m_activeFrameGraph, &QRenderSettings::setActiveFrameGraph, d->m_activeFrameGraph);

    emit activeFrameGraphChanged(activeFrameGraph);
}

void QRenderSettings::setRenderPolicy(QRenderSettings::RenderPolicy renderPolicy)
{
    Q_D(QRenderSettings);
    if (d->m_renderPolicy == renderPolicy)
        return;

    d->m_renderPolicy = renderPolicy;
    emit renderPolicyChanged(renderPolicy);
}

} // namespace Qt3Drender

QT_END_NAMESPACE

#include "moc_qrendersettings.cpp"
