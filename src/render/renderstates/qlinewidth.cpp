// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qlinewidth.h"
#include "qlinewidth_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DRender::QLineWidth
    \inmodule Qt3DRender
    \since 5.10
    \brief Specifies the width of rasterized lines.
 */

/*!
    \qmltype LineWidth
    \since 5.10
    \inherits RenderState
    \instantiates Qt3DRender::QLineWidth
    \inqmlmodule Qt3D.Render

    \brief Specifies the width of rasterized lines.
 */

/*!
    \qmlproperty real LineWidth::value
    Specifies the width value to be used.
*/

/*!
    \property  QLineWidth::value
    Specifies the width value to be used.
*/

QLineWidth::QLineWidth(Qt3DCore::QNode *parent)
    : QRenderState(*new QLineWidthPrivate(1.0f), parent)
{
}

QLineWidth::~QLineWidth()
{
}

float QLineWidth::value() const
{
    Q_D(const QLineWidth);
    return d->m_value;
}

void QLineWidth::setValue(float width)
{
    Q_D(QLineWidth);
    d->m_value = width;
    emit valueChanged(width);
}

bool QLineWidth::smooth() const
{
    Q_D(const QLineWidth);
    return d->m_smooth;
}

void QLineWidth::setSmooth(bool enabled)
{
    Q_D(QLineWidth);
    if (d->m_smooth != enabled) {
        d->m_smooth = enabled;
        emit smoothChanged(enabled);
    }
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qlinewidth.cpp"
