/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qclipplane.h"
#include <Qt3DRenderer/private/qrenderstate_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QClipPlanePrivate : public QRenderStatePrivate
{
public:
    QClipPlanePrivate()
        : QRenderStatePrivate(QRenderState::ClipPlane)
        , m_plane(0)
    {}

    int m_plane;
};

/*!
    \class Qt3D::QClipPlane
    \inmodule Qt3DRenderer
    \since 5.5
    \brief Enables an additional OpenGL clipping plane that can be in shaders
    using gl_ClipDistance

    By default, OpenGL supports up to 8 additional clipping planes.
    Qt3D::QClipPlane allows to enable one of these additional planes. These
    planes can then be manipulated in the shaders using gl_ClipDistance[i]
    where i varies between 0 and 8.
 */

/*!
    \qmltype ClipPlane
    \instantiates Qt3D::QClipPlane
    \inherits RenderState
    \inqmlmodule Qt3D.Renderer
    \since 5.5
    \brief Enables an additional OpenGL clipping plane that can be in shaders
    using gl_ClipDistance

    By default, OpenGL supports up to 8 additional clipping planes. ClipPlane
    allows to enable one of these additional planes. These planes can then be
    manipulated in the shaders using gl_ClipDistance[i] where i varies between
    0 and 8.
*/

QClipPlane::QClipPlane(QNode *parent)
    : QRenderState(*new QClipPlanePrivate(), parent)
{
}

QClipPlane::~QClipPlane()
{
    QNode::cleanup();
}

/*!
 * Returns the index of the clip plane.
 * \note usually between 0-7
 */
int QClipPlane::plane() const
{
    Q_D(const QClipPlane);
    return d->m_plane;
}

/*!
 * Sets the index of the clip plane to \a plane.
 * \note above 7, support is not garanteed
 */
void QClipPlane::setPlane(int plane)
{
    Q_D(QClipPlane);
    if (plane != d->m_plane) {
        d->m_plane = plane;
        Q_EMIT planeChanged();
    }
}

void QClipPlane::copy(const QNode *ref)
{
    QRenderState::copy(ref);
    const QClipPlane *refClip = static_cast<const QClipPlane *>(ref);
    d_func()->m_plane = refClip->plane();
}

} // Qt3D

QT_END_NAMESPACE
