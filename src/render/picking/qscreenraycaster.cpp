// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qscreenraycaster.h"
#include "qabstractraycaster_p.h"
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/private/qcomponent_p.h>
#include <Qt3DCore/private/qscene_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DRender::QScreenRayCaster
    \brief Performe ray casting test based on screen coordinates.
    \inmodule Qt3DRender
    \since 5.11
    \inherits QAbstractRayCaster

    QScreenRayCaster can be used to perform ray casting tests by specifying coordinates in
    screen space, which will be used to construct an actual 3D ray between the near and
    far planes.

    \sa QRayCaster, QNoPicking
*/
/*!
    \qmltype ScreenRayCaster
    \brief Performe ray casting test based on screen coordinates.
    \inqmlmodule Qt3D.Render
    \since 5.11
    \nativetype Qt3DRender::QScreenRayCaster

    ScreenRayCaster can be used to perform ray casting tests by specifying coordinates in
    screen space, which will be used to construct an actual 3D ray between the near and
    far planes.

    \sa RayCaster, NoPicking
*/

/*!
    \property Qt3DRender::QScreenRayCaster::position

    Holds the screen space position used to compute the actual 3D ray for intersection tests.

    Note: the coordinates will be used for every available render surface as long as they are
    in the valid range.
*/
/*!
    \qmlproperty point Qt3D.Render::ScreenRayCaster::position

    Holds the length of the 3D ray.

    \note The coordinates will be used for every available render surface as long as they are
    in the valid range.
*/
QScreenRayCaster::QScreenRayCaster(Qt3DCore::QNode *parent)
    : QAbstractRayCaster(parent)
{
    QAbstractRayCasterPrivate::get(this)->m_rayCasterType = QAbstractRayCasterPrivate::ScreenScapeRayCaster;
}

/*! \internal */
QScreenRayCaster::QScreenRayCaster(QAbstractRayCasterPrivate &dd, Qt3DCore::QNode *parent)
    : QAbstractRayCaster(dd, parent)
{
    QAbstractRayCasterPrivate::get(this)->m_rayCasterType = QAbstractRayCasterPrivate::ScreenScapeRayCaster;
}

/*! \internal */
QScreenRayCaster::~QScreenRayCaster()
{
}

QPoint QScreenRayCaster::position() const
{
    auto d = QAbstractRayCasterPrivate::get(this);
    return d->m_position;
}

void QScreenRayCaster::setPosition(const QPoint &position)
{
    auto d = QAbstractRayCasterPrivate::get(this);
    if (d->m_position != position) {
        d->m_position = position;
        emit positionChanged(d->m_position);
    }
}

/*!
  Convenience method to enable the component and trigger tests using the current coordinate value.
*/
void QScreenRayCaster::trigger()
{
    setEnabled(true);
}

/*!
  Convenience method to set the coordinate value \a position and enable the component to trigger tests.
*/
void QScreenRayCaster::trigger(const QPoint &position)
{
    setPosition(position);
    setEnabled(true);
}

QAbstractRayCaster::Hits QScreenRayCaster::pick(const QPoint &position)
{
    setPosition(position);

    auto d = QAbstractRayCasterPrivate::get(this);
    return d->pick();
}

} // Qt3DRender

QT_END_NAMESPACE

#include "moc_qscreenraycaster.cpp"
