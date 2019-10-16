/****************************************************************************
**
** Copyright (C) 2019 Ford Motor Company
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

#include "qsubtreeenabler_p.h"
#include <Qt3DRender/qframegraphnodecreatedchange.h>

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
    \instantiates Qt3DRender::QSubtreeEnabler
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
    \enum QSubtreeEnabler::Enablement

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

Qt3DCore::QNodeCreatedChangeBasePtr QSubtreeEnabler::createNodeCreationChange() const
{
    auto creationChange = QFrameGraphNodeCreatedChangePtr<QSubtreeEnablerData>::create(this);
    auto &data = creationChange->data;
    Q_D(const QSubtreeEnabler);
    data.enablement = d->m_enablement;
    return creationChange;
}

} //Qt3DRender

QT_END_NAMESPACE
