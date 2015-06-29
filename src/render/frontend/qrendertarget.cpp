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

#include "qrendertarget.h"
#include "qrendertarget_p.h"
#include "qrenderattachment.h"
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QRenderTargetPrivate
    \internal
*/
QRenderTargetPrivate::QRenderTargetPrivate()
    : QComponentPrivate()
{
}

void QRenderTarget::copy(const QNode *ref)
{
    QNode::copy(ref);
    const QRenderTarget *other = static_cast<const QRenderTarget*>(ref);
    Q_FOREACH (QRenderAttachment *attachment, other->d_func()->m_attachments)
        addAttachment(qobject_cast<QRenderAttachment *>(QNode::clone(attachment)));
}

QRenderTarget::QRenderTarget(QNode *parent)
    : QComponent(*new QRenderTargetPrivate, parent)
{
}

QRenderTarget::~QRenderTarget()
{
    QNode::cleanup();
}

/*! \internal */
QRenderTarget::QRenderTarget(QRenderTargetPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{
}

void QRenderTarget::addAttachment(QRenderAttachment *attachment)
{
    Q_D(QRenderTarget);
    if (!d->m_attachments.contains(attachment)) {
        d->m_attachments.append(attachment);

        if (!attachment->parent())
            attachment->setParent(this);

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeAdded, QSceneChange::Node, id()));
            change->setPropertyName("attachment");
            change->setValue(QVariant::fromValue(attachment->id()));
            d->notifyObservers(change);
        }
    }
}

void QRenderTarget::removeAttachment(QRenderAttachment *attachment)
{
    Q_D(QRenderTarget);

    if (d->m_changeArbiter != Q_NULLPTR) {
        QScenePropertyChangePtr change(new QScenePropertyChange(NodeRemoved, QSceneChange::Node, id()));
        change->setPropertyName("attachment");
        change->setValue(QVariant::fromValue(attachment->id()));
        d->notifyObservers(change);
    }
    d->m_attachments.removeOne(attachment);
}

QList<QRenderAttachment *> QRenderTarget::attachments() const
{
    Q_D(const QRenderTarget);
    return d->m_attachments;
}

} // Qt3D

QT_END_NAMESPACE
