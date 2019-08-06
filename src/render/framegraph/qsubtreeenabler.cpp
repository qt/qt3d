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

#include <Qt3DRender/qsubtreeenabler.h>
#include <Qt3DRender/private/qframegraphnode_p.h>

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
    : QFrameGraphNode(*new QFrameGraphNodePrivate, parent)
{
}

QSubtreeEnabler::~QSubtreeEnabler()
{
}

} //Qt3DRender

QT_END_NAMESPACE
