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

#include "qviewport.h"
#include "qviewport_p.h"

#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

QViewportPrivate::QViewportPrivate(QViewport *qq) :
    QFrameGraphItemPrivate(qq)
{
}

void QViewport::copy(const QNode *ref)
{
    QFrameGraphItem::copy(ref);
    const QViewport *viewport = static_cast<const QViewport*>(ref);
    d_func()->m_rect = viewport->d_func()->m_rect;
    Q_FOREACH (QFrameGraphItem *fgChild, viewport->d_func()->m_fgChildren)
        appendFrameGraphItem(qobject_cast<QFrameGraphItem *>(QNodePrivate::get(fgChild)->clone()));
}

QViewport::QViewport(QNode *parent)
    : QFrameGraphItem(*new QViewportPrivate(this), parent)
{
}

QViewport::QViewport(QViewportPrivate &dd, QNode *parent)
    : QFrameGraphItem(dd, parent)
{
}

QRectF QViewport::rect() const
{
    Q_D(const QViewport);
    return d->m_rect;
}

void QViewport::setRect(const QRectF &rect)
{
    Q_D(QViewport);
    if (rect != d->m_rect) {
        d->m_rect = rect;
        emit rectChanged();
    }
}

QColor QViewport::clearColor() const
{
    Q_D(const QViewport);
    return d->m_clearColor;
}

void QViewport::setClearColor(const QColor &color)
{
    Q_D(QViewport);
    if (color != d->m_clearColor) {
        d->m_clearColor = color;
        emit clearColorChanged();
    }
}

} // Qt3D

QT_END_NAMESPACE
