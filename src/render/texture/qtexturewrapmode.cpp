/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#include "qtexturewrapmode.h"
#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QTextureWrapModePrivate : public QObjectPrivate
{
public:
    QTextureWrapModePrivate()
        : QObjectPrivate()
        , m_x(QTextureWrapMode::ClampToEdge)
        , m_y(QTextureWrapMode::ClampToEdge)
        , m_z(QTextureWrapMode::ClampToEdge)
    {
    }

    Q_DECLARE_PUBLIC(QTextureWrapMode)
    QTextureWrapMode::WrapMode m_x;
    QTextureWrapMode::WrapMode m_y;
    QTextureWrapMode::WrapMode m_z;
};

/*!
    \class Qt3DRender::QTextureWrapMode
    \inmodule Qt3DRender
    \since 5.5

    \brief Defines the wrap mode a Qt3DRender::QAbstractTexture
    should apply to a texture.
 */

QTextureWrapMode::QTextureWrapMode(WrapMode wrapMode, QObject *parent)
    : QObject(*new QTextureWrapModePrivate, parent)
{
    d_func()->m_x = wrapMode;
    d_func()->m_y = wrapMode;
    d_func()->m_z = wrapMode;
}

/*!
    Contrusts a new Qt3DRender::QTextureWrapMode instance with the wrap mode to apply to
    each dimension \a x, \a y \a z of the texture and \a parent as parent.
 */
QTextureWrapMode::QTextureWrapMode(WrapMode x,WrapMode y, WrapMode z, QObject *parent)
    : QObject(*new QTextureWrapModePrivate, parent)
{
    d_func()->m_x = x;
    d_func()->m_y = y;
    d_func()->m_z = z;
}

/*! \internal */
QTextureWrapMode::~QTextureWrapMode()
{
}

/*!
    Sets the wrap mode of the x dimension to \a x.
 */
void QTextureWrapMode::setX(WrapMode x)
{
    Q_D(QTextureWrapMode);
    if (d->m_x != x) {
        d->m_x = x;
        emit xChanged(x);
    }
}

/*!
    \property QTextureWrapMode::x

    Returns the wrap mode of the x dimension.
 */
QTextureWrapMode::WrapMode QTextureWrapMode::x() const
{
    Q_D(const QTextureWrapMode);
    return d->m_x;
}

/*!
    \enum Qt3DRender::QTextureWrapMode::WrapMode

    Specifies the type of text wrapping.
    Possible values:

    \value Repeat
    \value MirroredRepeat
    \value ClampToEdge
    \value ClampToBorder
*/


/*!
    Sets the wrap mode of the y dimension to \a y.
    \note this is not available on 1D textures.
 */
void QTextureWrapMode::setY(WrapMode y)
{
    Q_D(QTextureWrapMode);
    if (d->m_y != y) {
        d->m_y = y;
        emit yChanged(y);
    }
}

/*!
    \property QTextureWrapMode::y

    Returns the wrap mode of the y dimension.
 */
QTextureWrapMode::WrapMode QTextureWrapMode::y() const
{
    Q_D(const QTextureWrapMode);
    return d->m_y;
}

/*!
    Sets the wrap mode of the z dimension to \a z.
    \note this is only available on 3D textures.
 */
void QTextureWrapMode::setZ(WrapMode z)
{
    Q_D(QTextureWrapMode);
    if (d->m_z != z) {
        d->m_z = z;
        emit zChanged(z);
    }
}

/*!
    \property QTextureWrapMode::z

    Returns the wrap mode of the z dimension.
 */
QTextureWrapMode::WrapMode QTextureWrapMode::z() const
{
    Q_D(const QTextureWrapMode);
    return d->m_z;
}

} // namespace Qt3DRender

QT_END_NAMESPACE
