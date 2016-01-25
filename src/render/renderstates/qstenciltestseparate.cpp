/****************************************************************************
**
** Copyright (C) 2015 Paul Lemire
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

#include "qstenciltestseparate.h"
#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QStencilTestSeparatePrivate : public QObjectPrivate
{
public:
    QStencilTestSeparatePrivate(QStencilTestSeparate::StencilFaceMode face)
        : QObjectPrivate()
        , m_face(face)
        , m_mask(0)
        , m_ref(0)
        , m_func(QStencilTestSeparate::Never)
    {
    }

    Q_DECLARE_PUBLIC(QStencilTestSeparate)
    QStencilTestSeparate::StencilFaceMode m_face;
    uint m_mask;
    int m_ref;
    QStencilTestSeparate::StencilFunc m_func;
};


QStencilTestSeparate::QStencilTestSeparate(QStencilTestSeparate::StencilFaceMode face, QObject *parent)
    : QObject(*new QStencilTestSeparatePrivate(face), parent)
{
}

QStencilTestSeparate::~QStencilTestSeparate()
{
}

uint QStencilTestSeparate::mask() const
{
    Q_D(const QStencilTestSeparate);
    return d->m_mask;
}

void QStencilTestSeparate::setMask(uint mask)
{
    Q_D(QStencilTestSeparate);
    if (d->m_mask != mask) {
        d->m_mask = mask;
        emit maskChanged(mask);
    }
}

int QStencilTestSeparate::ref() const
{
    Q_D(const QStencilTestSeparate);
    return d->m_ref;
}

void QStencilTestSeparate::setRef(int ref)
{
    Q_D(QStencilTestSeparate);
    if (d->m_ref != ref) {
        d->m_ref = ref;
        emit refChanged(ref);
    }
}

QStencilTestSeparate::StencilFunc QStencilTestSeparate::func() const
{
    Q_D(const QStencilTestSeparate);
    return d->m_func;
}

void QStencilTestSeparate::setFunc(QStencilTestSeparate::StencilFunc func)
{
    Q_D(QStencilTestSeparate);
    if (d->m_func != func) {
        d->m_func = func;
        emit funcChanged(func);
    }
}

QStencilTestSeparate::StencilFaceMode QStencilTestSeparate::faceMode() const
{
    Q_D(const QStencilTestSeparate);
    return d->m_face;
}

} // namespace Qt3DRender

QT_END_NAMESPACE
