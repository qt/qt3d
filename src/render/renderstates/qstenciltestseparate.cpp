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
        , m_comparisonMask(0)
        , m_referenceValue(0)
        , m_stencilFunction(QStencilTestSeparate::Never)
    {
    }

    Q_DECLARE_PUBLIC(QStencilTestSeparate)
    QStencilTestSeparate::StencilFaceMode m_face;
    uint m_comparisonMask;
    int m_referenceValue;
    QStencilTestSeparate::StencilFunction m_stencilFunction;
};


QStencilTestSeparate::QStencilTestSeparate(QStencilTestSeparate::StencilFaceMode face, QObject *parent)
    : QObject(*new QStencilTestSeparatePrivate(face), parent)
{
}

QStencilTestSeparate::~QStencilTestSeparate()
{
}

uint QStencilTestSeparate::comparisonMask() const
{
    Q_D(const QStencilTestSeparate);
    return d->m_comparisonMask;
}

void QStencilTestSeparate::setComparisonMask(uint comparisonMask)
{
    Q_D(QStencilTestSeparate);
    if (d->m_comparisonMask != comparisonMask) {
        d->m_comparisonMask = comparisonMask;
        emit comparisonMaskChanged(comparisonMask);
    }
}

int QStencilTestSeparate::referenceValue() const
{
    Q_D(const QStencilTestSeparate);
    return d->m_referenceValue;
}

void QStencilTestSeparate::setReferenceValue(int referenceValue)
{
    Q_D(QStencilTestSeparate);
    if (d->m_referenceValue != referenceValue) {
        d->m_referenceValue = referenceValue;
        emit referenceValueChanged(referenceValue);
    }
}

QStencilTestSeparate::StencilFunction QStencilTestSeparate::stencilFunction() const
{
    Q_D(const QStencilTestSeparate);
    return d->m_stencilFunction;
}

void QStencilTestSeparate::setStencilFunction(QStencilTestSeparate::StencilFunction stencilFunction)
{
    Q_D(QStencilTestSeparate);
    if (d->m_stencilFunction != stencilFunction) {
        d->m_stencilFunction = stencilFunction;
        emit stencilFunctionChanged(stencilFunction);
    }
}

QStencilTestSeparate::StencilFaceMode QStencilTestSeparate::faceMode() const
{
    Q_D(const QStencilTestSeparate);
    return d->m_face;
}

} // namespace Qt3DRender

QT_END_NAMESPACE
