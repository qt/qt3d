/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
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

#include "qscissortest.h"
#include "qrenderstate_p.h"
#include <private/qnode_p.h>
#include <Qt3DCore/qscenepropertychange.h>


QT_BEGIN_NAMESPACE

namespace Qt3D {

class QScissorTestPrivate : public QRenderStatePrivate
{
public:
    QScissorTestPrivate(QScissorTest *qq)
        : QRenderStatePrivate(qq)
        , m_left(0)
        , m_bottom(0)
        , m_width(0)
        , m_height(0)
    {
    }

    void copy(const QNodePrivate *ref) Q_DECL_OVERRIDE;

    Q_DECLARE_PUBLIC(QScissorTest)
    int m_left;
    int m_bottom;
    int m_width;
    int m_height;
};

QScissorTest::QScissorTest(QNode *parent)
    : QRenderState(*new QScissorTestPrivate(this), parent)
{
}

void QScissorTestPrivate::copy(const QNodePrivate *ref)
{
    QRenderStatePrivate::copy(ref);
    const QScissorTestPrivate *refState = static_cast<const QScissorTestPrivate *>(ref);
    m_left = refState->m_left;
    m_bottom = refState->m_bottom;
    m_width = refState->m_width;
    m_height = refState->m_height;
}

int QScissorTest::left() const
{
    Q_D(const QScissorTest);
    return d->m_left;
}

void QScissorTest::setLeft(int left)
{
    Q_D(QScissorTest);
    if (d->m_left != left) {
        d->m_left = left;
        emit leftChanged();
    }
}

int QScissorTest::bottom() const
{
    Q_D(const QScissorTest);
    return d->m_bottom;
}

void QScissorTest::setBottom(int bottom)
{
    Q_D(QScissorTest);
    if (d->m_bottom != bottom) {
        d->m_bottom = bottom;
        emit bottomChanged();
    }
}

int QScissorTest::width() const
{
    Q_D(const QScissorTest);
    return d->m_width;
}

void QScissorTest::setWidth(int width)
{
    Q_D(QScissorTest);
    if (d->m_width != width) {
        d->m_width = width;
        emit widthChanged();
    }
}

int QScissorTest::height() const
{
    Q_D(const QScissorTest);
    return d->m_height;
}

void QScissorTest::setHeight(int height)
{
    Q_D(QScissorTest);
    if (d->m_height != height) {
        d->m_height = height;
        emit heightChanged();
    }
}

} // Qt3D

QT_END_NAMESPACE
