// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qcolormask.h"
#include "qcolormask_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DRender::QColorMask
    \inmodule Qt3DRender
    \since 5.7
    \brief Allows specifying which color components should be written to the
    currently bound frame buffer.

    By default, the property for each color component (red, green, blue, alpha)
    is set to \c true which means they will be written to the frame buffer.
    Setting any of the color component to \c false will prevent it from being
    written into the frame buffer.
 */

/*!
    \qmltype ColorMask
    \inqmlmodule Qt3D.Render
    \since 5.7
    \inherits RenderState
    \nativetype Qt3DRender::QColorMask
    \brief Allows specifying which color components should be written to the
    currently bound frame buffer.

    By default, the property for each color component (red, green, blue, alpha)
    is set to \c true which means they will be written to the frame buffer.
    Setting any of the color component to \c false will prevent it from being
    written into the frame buffer.
 */

/*!
    \qmlproperty bool ColorMask::redMasked
    Holds whether red color component should be written to the frame buffer.
*/

/*!
    \qmlproperty bool ColorMask::greenMasked
    Holds whether green color component should be written to the frame buffer.
*/

/*!
    \qmlproperty bool ColorMask::blueMasked
    Holds whether blue color component should be written to the frame buffer.
*/

/*!
    \qmlproperty bool ColorMask::alphaMasked
    Holds whether alpha component should be written to the frame buffer.
*/


/*!
    Constructs a new Qt3DCore::QColorMask instance with \a parent as parent.
 */
QColorMask::QColorMask(QNode *parent)
    : QRenderState(*new QColorMaskPrivate, parent)
{
}

/*! \internal */
QColorMask::~QColorMask()
{
}

bool QColorMask::isRedMasked() const
{
    Q_D(const QColorMask);
    return d->m_redMasked;
}

bool QColorMask::isGreenMasked() const
{
    Q_D(const QColorMask);
    return d->m_greenMasked;
}

bool QColorMask::isBlueMasked() const
{
    Q_D(const QColorMask);
    return d->m_blueMasked;
}

bool QColorMask::isAlphaMasked() const
{
    Q_D(const QColorMask);
    return d->m_alphaMasked;
}

/*!
    \property Qt3DRender::QColorMask::redMasked
    Holds whether the red color component should be written to the frame buffer.
 */
void QColorMask::setRedMasked(bool redMasked)
{
    Q_D(QColorMask);
    if (redMasked != d->m_redMasked) {
        d->m_redMasked = redMasked;
        emit redMaskedChanged(redMasked);
    }
}

/*!
    \property Qt3DRender::QColorMask::greenMasked
    Holds whether the green color component should be written to the frame buffer.
 */
void QColorMask::setGreenMasked(bool greenMasked)
{
    Q_D(QColorMask);
    if (greenMasked != d->m_greenMasked) {
        d->m_greenMasked = greenMasked;
        emit greenMaskedChanged(greenMasked);
    }
}

/*!
    \property Qt3DRender::QColorMask::blueMasked
    Holds whether the blue color component should be written to the frame buffer.
 */
void QColorMask::setBlueMasked(bool blueMasked)
{
    Q_D(QColorMask);
    if (blueMasked != d->m_blueMasked) {
        d->m_blueMasked = blueMasked;
        emit blueMaskedChanged(blueMasked);
    }
}

/*!
    \property Qt3DRender::QColorMask::alphaMasked
    Holds whether the alphaMasked component should be written to the frame buffer.
 */
void QColorMask::setAlphaMasked(bool alphaMasked)
{
    Q_D(QColorMask);
    if (alphaMasked != d->m_alphaMasked) {
        d->m_alphaMasked = alphaMasked;
        emit alphaMaskedChanged(alphaMasked);
    }
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qcolormask.cpp"

