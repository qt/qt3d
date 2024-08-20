// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qfrontface.h"
#include "qfrontface_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DRender::QFrontFace
    \brief The QFrontFace class defines front and back facing polygons.
    \since 5.7
    \ingroup renderstates
    \inmodule Qt3DRender

    A Qt3DRender::QFrontFace sets the winding direction of the front facing polygons.

    \sa QCullFace
 */

/*!
    \qmltype FrontFace
    \brief The FrontFace type defines front and back facing polygons.
    \since 5.7
    \ingroup renderstates
    \inqmlmodule Qt3D.Render
    \inherits RenderState
    \nativetype Qt3DRender::QFrontFace

    A FrontFace sets the winding direction of the front facing polygons.

    \sa CullFace
 */

/*!
    \enum Qt3DRender::QFrontFace::WindingDirection

    This enumeration specifies the winding direction values.
    \value ClockWise Clockwise polygons are front facing.
    \value CounterClockWise Counter clockwise polygons are front facing.
*/

/*!
    \qmlproperty enumeration FrontFace::direction
    Holds the winding direction of the front facing polygons. Default is FrontFace.Clockwise.
*/

/*!
    \property Qt3DRender::QFrontFace::direction
    Holds the winding direction of the front facing polygons. Default is Clockwise.
*/

/*!
    The constructor creates a new QFrontFace::QFrontFace instance with the
    specified \a parent
 */
QFrontFace::QFrontFace(QNode *parent)
    : QRenderState(*new QFrontFacePrivate, parent)
{
}

/*! \internal */
QFrontFace::~QFrontFace()
{
}

QFrontFace::WindingDirection QFrontFace::direction() const
{
    Q_D(const QFrontFace);
    return d->m_direction;
}

void QFrontFace::setDirection(QFrontFace::WindingDirection direction)
{
    Q_D(QFrontFace);
    if (d->m_direction != direction) {
        d->m_direction = direction;
        emit directionChanged(direction);
    }
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qfrontface.cpp"
