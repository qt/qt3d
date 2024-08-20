// Copyright (C) 2019 Ford Motor Company
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qsubtreeenabler_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender
{

/*!
    \class Qt3DRender::QSubtreeEnabler
    \inmodule Qt3DRender
    \brief Enables or disables entire subtrees of framegraph nodes.
    \since 5.14

    While QFrameGraphNodes can be individually enabled and disabled via the
    \c enabled property, this can become tedious when an entire path
    needs to be turned on or off. QSubtreeEnabler is a convenience node
    that makes this use case trivial, allowing all of its children to be
    controlled by a single switch.

    QSubtreeEnabler is enabled by default.
*/

/*!
    \qmltype SubtreeEnabler
    \inqmlmodule Qt3D.Render
    \since 5.14
    \nativetype Qt3DRender::QSubtreeEnabler
    \inherits FrameGraphNode
    \brief Enables or disables entire subtrees of frame graph nodes.

    While FrameGraphNodes can be individually enabled and disabled via the
    \c enabled property, this can become tedious when an entire path
    needs to be turned on or off. SubtreeEnabler is a convenience node
    that makes this use case trivial, allowing all of its children to be
    controlled by a single switch.

    For example, the following simplified frame graph includes a subtree for
    debug rendering that can easily be enabled only when debugging.

    \qml
    RenderSurfaceSelector {
        ClearBuffers {
            Viewport {
                CameraSelector {}
            }
        }

        SubtreeEnabler {
            enabled: showDebugView
            Viewport {
                CameraSelector {
                    RenderPassFilter {}
                }
            }
        }
    }
    \endqml

    SubtreeEnabler is enabled by default.
 */

QSubtreeEnabler::QSubtreeEnabler(Qt3DCore::QNode *parent)
    : QFrameGraphNode(*new QSubtreeEnablerPrivate, parent)
{
}

QSubtreeEnabler::~QSubtreeEnabler()
{
}

/*!
    \enum Qt3DRender::QSubtreeEnabler::Enablement

    Specifies whether subtree enablement is persistent or transient.

    \value Persistent
    The value of enabled is persistent. This is the default.

    \value SingleShot
    The value of enabled will last for a single frame and then be reset to false.
    This might be used for a subtree drawing to an FBO, for example, to only update
    the FBO when the relevant portions of the scene changed.
*/

/*!
    \qmlproperty enumeration Qt3D.Render::SubtreeEnabler::enablement
    Controls whether subtree enablement is persistent or transient.

    \value Persistent
    The value of enabled is persistent. This is the default.

    \value SingleShot
    The value of enabled will last for a single frame and then be reset to false.
    This might be used for a subtree drawing to an FBO, for example, to only update
    the FBO when the relevant portions of the scene changed.
*/

/*!
    \property Qt3DRender::QSubtreeEnabler::enablement
    Controls whether subtree enablement is persistent or transient.
*/
QSubtreeEnabler::Enablement QSubtreeEnabler::enablement() const
{
    Q_D(const QSubtreeEnabler);
    return d->m_enablement;
}

void QSubtreeEnabler::setEnablement(QSubtreeEnabler::Enablement enablement)
{
    Q_D(QSubtreeEnabler);
    if (d->m_enablement == enablement)
        return;
    d->m_enablement = enablement;
    emit enablementChanged(d->m_enablement);
}

/*!
    \qmlmethod void Qt3D.Render::SubtreeEnabler::requestUpdate()
    Requests that the subtree be enabled.

    A conveninence method intended to be used with \c SingleShot enablement.
 */

/*!
    Requests that the subtree be enabled.

    A convenience method intended to be used with \c SingleShot enablement.
 */
void QSubtreeEnabler::requestUpdate()
{
    setEnabled(true);
}

} //Qt3DRender

QT_END_NAMESPACE

#include "moc_qsubtreeenabler.cpp"
