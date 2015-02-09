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

#include "quick3drendertarget.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

namespace Quick {

Quick3DRenderTarget::Quick3DRenderTarget(QObject * parent)
    : QObject(parent)
{
}

QQmlListProperty<QRenderAttachment> Quick3DRenderTarget::qmlAttachments()
{
    return QQmlListProperty<QRenderAttachment>(this, 0,
                                               &Quick3DRenderTarget::appendRenderAttachment,
                                               &Quick3DRenderTarget::renderAttachmentCount,
                                               &Quick3DRenderTarget::renderAttachmentAt,
                                               &Quick3DRenderTarget::clearRenderAttachments);
}

void Quick3DRenderTarget::appendRenderAttachment(QQmlListProperty<QRenderAttachment> *list, QRenderAttachment *attachment)
{
    Quick3DRenderTarget *rT = qobject_cast<Quick3DRenderTarget *>(list->object);
    if (rT)
        rT->parentRenderTarget()->addAttachment(attachment);
}

QRenderAttachment *Quick3DRenderTarget::renderAttachmentAt(QQmlListProperty<QRenderAttachment> *list, int index)
{
    Quick3DRenderTarget *rT = qobject_cast<Quick3DRenderTarget *>(list->object);
    if (rT)
        return rT->parentRenderTarget()->attachments().at(index);
    return Q_NULLPTR;
}

int Quick3DRenderTarget::renderAttachmentCount(QQmlListProperty<QRenderAttachment> *list)
{
    Quick3DRenderTarget *rT = qobject_cast<Quick3DRenderTarget *>(list->object);
    if (rT)
        return rT->parentRenderTarget()->attachments().count();
    return -1;
}

void Quick3DRenderTarget::clearRenderAttachments(QQmlListProperty<QRenderAttachment> *list)
{
    Quick3DRenderTarget *rT = qobject_cast<Quick3DRenderTarget *>(list->object);
    if (rT) {
        Q_FOREACH (QRenderAttachment *att, rT->parentRenderTarget()->attachments())
            rT->parentRenderTarget()->removeAttachment(att);
    }
}

} // Quick

} // Render

} // Qt3D

QT_END_NAMESPACE
