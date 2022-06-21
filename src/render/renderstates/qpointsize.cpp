// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qpointsize.h"
#include "qpointsize_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DRender::QPointSize
    \inmodule Qt3DRender
    \since 5.7
    \brief Specifies the size of rasterized points. May either be set statically
    or by shader programs.

    When the sizeMode property is set to SizeMode::Fixed, the value is set
    using glPointSize(), if available. When using SizeMode::Programmable,
    gl_PointSize must be set within shader programs, the value provided to this
    RenderState is ignored in that case.
 */

/*!
    \qmltype PointSize
    \since 5.7
    \inherits RenderState
    \instantiates Qt3DRender::QPointSize
    \inqmlmodule Qt3D.Render

    \brief Specifies the size of rasterized points. May either be set statically
    or by shader programs.

    When the sizeMode property is set to SizeMode::Fixed, the value is set
    using glPointSize(), if available. When using SizeMode::Programmable,
    gl_PointSize must be set within shader programs, the value provided to this
    RenderState is ignored in that case.
 */

/*!
    \enum Qt3DRender::QPointSize::SizeMode

    This enumeration specifies values for the size mode.
    \value Fixed The point size is by the QPointSize::value.
    \value Programmable The point size value must be set in shader
*/
/*!
    \qmlproperty real PointSize::value
    Specifies the point size value to be used.
*/

/*!
    \qmlproperty enumeration PointSize::sizeMode
    Specifies the sizeMode to be used.
*/

/*!
    \property  QPointSize::value
    Specifies the point size value to be used.
*/

/*!
    \property QPointSize::sizeMode
    Specifies the sizeMode to be used.
*/

QPointSize::QPointSize(Qt3DCore::QNode *parent)
    : QRenderState(*new QPointSizePrivate(SizeMode::Programmable, 0.f), parent)
{
}

/*! \internal */
QPointSize::~QPointSize()
{
}

QPointSize::SizeMode QPointSize::sizeMode() const
{
    Q_D(const QPointSize);
    return d->m_sizeMode;
}

float QPointSize::value() const
{
    Q_D(const QPointSize);
    return d->m_value;
}

void QPointSize::setSizeMode(SizeMode sizeMode)
{
    Q_D(QPointSize);
    d->m_sizeMode = sizeMode;
    emit sizeModeChanged(sizeMode);
}

void QPointSize::setValue(float size)
{
    Q_D(QPointSize);
    d->m_value = size;
    emit valueChanged(size);
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qpointsize.cpp"

