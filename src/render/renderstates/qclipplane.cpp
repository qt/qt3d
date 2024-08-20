// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qclipplane.h"
#include "qclipplane_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DRender::QClipPlane
    \inmodule Qt3DRender
    \since 5.5
    \brief Enables an additional OpenGL clipping plane that can be in shaders
    using gl_ClipDistance.

    By default, OpenGL supports up to 8 additional clipping planes.
    Qt3DCore::QClipPlane allows to enable one of these additional planes. These
    planes can then be manipulated in the shaders using gl_ClipDistance[i]
    where i varies between 0 and 7. The underlying implementation may support more
    than 8 clip planes, but it is not guaranteed.
 */

/*!
    \qmltype ClipPlane
    \nativetype Qt3DRender::QClipPlane
    \inherits RenderState
    \inqmlmodule Qt3D.Render
    \since 5.5
    \brief Enables an additional OpenGL clipping plane that can be in shaders
    using gl_ClipDistance.

    By default, OpenGL supports up to 8 additional clipping planes. ClipPlane
    allows to enable one of these additional planes. These planes can then be
    manipulated in the shaders using gl_ClipDistance[i] where i varies between
    0 and 7. The underlying implementation may support more than 8 clip planes,
    but it is not guaranteed.
*/

/*!
    \qmlproperty int ClipPlane::planeIndex
    Holds the index of the plane.
    \note Usually between 0-7.
*/

/*!
    \qmlproperty vector3d ClipPlane::normal
    Holds the normal of the plane.
*/

/*!
    \qmlproperty real ClipPlane::distance
    Holds the distance of the plane from the world origin.
*/


/*!
    \property Qt3DRender::QClipPlane::planeIndex
    Holds the index of the plane.
    \note Usually between 0-7.
*/

/*!
    \property Qt3DRender::QClipPlane::normal
    Holds the normal of the plane.
*/

/*!
    \property Qt3DRender::QClipPlane::distance
    Holds the distance of the plane from the world origin.
*/


QClipPlane::QClipPlane(QNode *parent)
    : QRenderState(*new QClipPlanePrivate(), parent)
{
}

/*! \internal */
QClipPlane::~QClipPlane()
{
}

int QClipPlane::planeIndex() const
{
    Q_D(const QClipPlane);
    return d->m_planeIndex;
}

QVector3D QClipPlane::normal() const
{
    Q_D(const QClipPlane);
    return d->m_normal;
}

float QClipPlane::distance() const
{
    Q_D(const QClipPlane);
    return d->m_distance;
}

void QClipPlane::setPlaneIndex(int planeIndex)
{
    Q_D(QClipPlane);
    if (planeIndex != d->m_planeIndex) {
        d->m_planeIndex = planeIndex;
        Q_EMIT planeIndexChanged(planeIndex);
    }
}

void QClipPlane::setNormal(QVector3D normal)
{
    Q_D(QClipPlane);
    if (normal != d->m_normal) {
        d->m_normal = normal;
        Q_EMIT normalChanged(normal);
    }
}

void QClipPlane::setDistance(float distance)
{
    Q_D(QClipPlane);
    if (distance != d->m_distance) {
        d->m_distance = distance;
        Q_EMIT distanceChanged(distance);
    }
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qclipplane.cpp"
