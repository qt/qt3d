// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qrenderstateset.h"
#include "qrenderstateset_p.h"

#include <Qt3DRender/qrenderstate.h>

QT_BEGIN_NAMESPACE


namespace Qt3DRender {

using namespace Qt3DCore;

QRenderStateSetPrivate::QRenderStateSetPrivate()
    : QFrameGraphNodePrivate()
{
}

/*!
    \class Qt3DRender::QRenderStateSet
    \inmodule Qt3DRender
    \since 5.5
    \brief The QRenderStateSet \l {QFrameGraphNode}{FrameGraph} node offers a way of
    specifying a set of QRenderState objects to be applied during the execution
    of a framegraph branch.

    States set on a QRenderStateSet are set globally, contrary to the per-material
    states that can be set on a QRenderPass. By default, an empty
    QRenderStateSet will result in all render states being disabled when
    executed. Adding a QRenderState state explicitly enables that render
    state at runtime.

    The RenderStateSet is enabled when added to the active frame graph:

    \code
    // using namespace Qt3DRender;

    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity();

    QRenderSettings *renderSettings = new QRenderSettings();

    QViewport *viewport = new QViewport();
    QCameraSelector *cameraSelector = new QCameraSelector(viewport);

    QClearBuffers *clearBuffers = new QClearBuffers(cameraSelector);
    clearBuffers->setBuffers(QClearBuffers::ColorDepthBuffer);

    QRenderStateSet *renderStateSet = new QRenderStateSet(cameraSelector);
    QCullFace *cullFace = new QCullFace(renderStateSet);
    cullFace->setMode(QCullFace::Front);
    renderStateSet->addRenderState(cullFace);

    renderSettings->setActiveFrameGraph(viewport);

    rootEntity->addComponent(renderSettings);
    \endcode

    \sa QRenderState, QRenderPass
 */

/*!
    \qmltype RenderStateSet
    \inqmlmodule Qt3D.Render
    \since 5.5
    \inherits FrameGraphNode
    \nativetype Qt3DRender::QRenderStateSet
    \brief The RenderStateSet \l {FrameGraphNode}{FrameGraph} node offers a way of
    specifying a set of RenderState objects to be applied during the execution
    of a framegraph branch.

    States set on a RenderStateSet are set globally, contrary to the per-material
    states that can be set on a RenderPass. By default, an empty
    RenderStateSet will result in all render states being disabled when
    executed. Adding a RenderState state explicitly enables that render
    state at runtime.

    The RenderStateSet is enabled when added to the active frame graph:

    \qml
    import Qt3D.Core 2.0
    import Qt3D.Render 2.0
    import Qt3D.Extras 2.0

    Entity {
        id: rootNode
        components: [
            RenderSettings {
                activeFrameGraph: RenderSurfaceSelector {
                    ClearBuffers {
                        buffers : ClearBuffers.ColorDepthBuffer

                        CameraSelector {
                            camera: Camera {
                                position: Qt.vector3d(10, 0, 0)
                                viewCenter: Qt.vector3d(0, 0, 0)
                            }
                            RenderStateSet {
                                renderStates: [
                                    CullFace { mode: CullFace.Back }
                                ]
                            }
                        }
                    }
                }
            }
        ]

        Entity {
            id: sphereEntity
            components: [
                GeometryRenderer { view: SphereMesh {} },
                PhongMaterial {}
            ]
        }
    }
    \endqml

    \sa RenderState, RenderPass
 */

/*!
    \qmlproperty list<RenderState> RenderStateSet::renderStates
    Holds the list of RenderState objects used by the RenderStateSet.
*/

QRenderStateSet::QRenderStateSet(QNode *parent)
    : QFrameGraphNode(*new QRenderStateSetPrivate, parent)
{
}

/*! \internal */
QRenderStateSet::~QRenderStateSet()
{
}

/*!
    Adds a new QRenderState \a state to the QRenderStateSet instance.

    \note Not setting any QRenderState state on a QRenderStateSet instance
    implies all the render states will be disabled at render time.
 */
void QRenderStateSet::addRenderState(QRenderState *state)
{
    Q_ASSERT(state);
    Q_D(QRenderStateSet);

    if (!d->m_renderStates.contains(state)) {
        d->m_renderStates.append(state);

        // Ensures proper bookkeeping
        d->registerDestructionHelper(state, &QRenderStateSet::removeRenderState, d->m_renderStates);

        if (!state->parent())
            state->setParent(this);

        d->update();
    }
}

/*!
    Removes the QRenderState \a state from the QRenderStateSet instance.
 */
void QRenderStateSet::removeRenderState(QRenderState *state)
{
    Q_ASSERT(state);
    Q_D(QRenderStateSet);

    if (!d->m_renderStates.removeOne(state))
        return;
    d->update();
    // Remove bookkeeping connection
    d->unregisterDestructionHelper(state);
}

/*!
    Returns the list of QRenderState objects that compose the QRenderStateSet instance.
 */
QList<QRenderState *> QRenderStateSet::renderStates() const
{
    Q_D(const QRenderStateSet);
    return d->m_renderStates;
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qrenderstateset.cpp"
