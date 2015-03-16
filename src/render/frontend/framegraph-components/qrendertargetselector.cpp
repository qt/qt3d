/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qrendertargetselector.h"
#include "qrendertargetselector_p.h"
#include <Qt3DRenderer/qrendertarget.h>
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DRenderer/private/qrenderpass_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QRenderTargetSelectorPrivate
    \internal
*/
QRenderTargetSelectorPrivate::QRenderTargetSelectorPrivate(QRenderTargetSelector *qq)
    : QFrameGraphNodePrivate(qq)
    , m_target(Q_NULLPTR)
{
}

void QRenderTargetSelector::copy(const QNode *ref)
{
    QFrameGraphNode::copy(ref);

    const QRenderTargetSelector *other = static_cast<const QRenderTargetSelector*>(ref);

    if (other->d_func()->m_target)
        setTarget(qobject_cast<QRenderTarget *>(QNode::clone(other->d_func()->m_target)));
}

QRenderTargetSelector::QRenderTargetSelector(QNode *parent)
    : QFrameGraphNode(*new QRenderTargetSelectorPrivate(this), parent)
{
}

void QRenderTargetSelector::setTarget(QRenderTarget *target)
{
    Q_D(QRenderTargetSelector);
    if (d->m_target != target) {
        d->m_target = target;

        // For inline declaration cases
        if (!target->parent())
            target->setParent(this);
        emit targetChanged();
    }
}

QRenderTarget *QRenderTargetSelector::target() const
{
    Q_D(const QRenderTargetSelector);
    return d->m_target;
}

/*! \internal */
QRenderTargetSelector::QRenderTargetSelector(QRenderTargetSelectorPrivate &dd, QNode *parent)
    : QFrameGraphNode(dd, parent)
{
}

} // Qt3D

QT_END_NAMESPACE
