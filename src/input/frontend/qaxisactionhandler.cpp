/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qaxisactionhandler.h"
#include "qaxisactionhandler_p.h"

#include <Qt3DInput/qlogicaldevice.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

QAxisActionHandlerPrivate::QAxisActionHandlerPrivate()
    : Qt3DCore::QComponentPrivate()
    , m_logicalDevice(Q_NULLPTR)
{
}

QAxisActionHandler::QAxisActionHandler(Qt3DCore::QNode *parent)
    : Qt3DCore::QComponent(*new QAxisActionHandlerPrivate, parent)
{
}

QAxisActionHandler::QAxisActionHandler(QAxisActionHandlerPrivate &dd, Qt3DCore::QNode *parent)
    : Qt3DCore::QComponent(dd, parent)
{

}

QAxisActionHandler::~QAxisActionHandler()
{
    QNode::cleanup();
}

Qt3DInput::QLogicalDevice *QAxisActionHandler::logicalDevice() const
{
    Q_D(const QAxisActionHandler);
    return d->m_logicalDevice;
}

void QAxisActionHandler::setLogicalDevice(Qt3DInput::QLogicalDevice *logicalDevice)
{
    Q_D(QAxisActionHandler);
    if (d->m_logicalDevice == logicalDevice)
        return;

    d->m_logicalDevice = logicalDevice;
    emit logicalDeviceChanged(logicalDevice);
}

void QAxisActionHandler::copy(const QNode *ref)
{
    QComponent::copy(ref);
    const QAxisActionHandler *component = static_cast<const QAxisActionHandler *>(ref);
    d_func()->m_logicalDevice = qobject_cast<QLogicalDevice *>(QNode::clone(component->d_func()->m_logicalDevice));
}

QT_END_NAMESPACE

} // namespace Qt3DInput

