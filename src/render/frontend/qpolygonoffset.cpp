/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qpolygonoffset.h"
#include <private/qrenderstate_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QPolygonOffsetPrivate : public QRenderStatePrivate
{
public:
    QPolygonOffsetPrivate(QPolygonOffset *qq)
        : QRenderStatePrivate(qq)
        , m_factor(0)
        , m_units(0)
    {
    }

    float m_factor;
    float m_units;

    Q_DECLARE_PUBLIC(QPolygonOffset)
};

QPolygonOffset::QPolygonOffset(QNode *parent)
    : QRenderState(*new QPolygonOffsetPrivate(this), parent)
{
}

float QPolygonOffset::factor() const
{
    Q_D(const QPolygonOffset);
    return d->m_factor;
}

void QPolygonOffset::setFactor(float factor)
{
    Q_D(QPolygonOffset);
    if (d->m_factor != factor) {
        d->m_factor = factor;
        emit factorChanged(d->m_factor);
    }
}

float QPolygonOffset::units() const
{
    Q_D(const QPolygonOffset);
    return d->m_units;
}

void QPolygonOffset::setUnits(float units)
{
    Q_D(QPolygonOffset);
    if (d->m_units != units) {
        d->m_units = units;
        emit unitsChanged(d->m_units);
    }
}

void QPolygonOffset::copy(const QNode *ref)
{
    QRenderState::copy(ref);
    const QPolygonOffset *refState = static_cast<const QPolygonOffset *>(ref);
    d_func()->m_factor = refState->d_func()->m_factor;
    d_func()->m_units = refState->d_func()->m_units;
}

} // Qt3D

QT_END_NAMESPACE
