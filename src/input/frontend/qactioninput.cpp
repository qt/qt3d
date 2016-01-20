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
****************************************************************************/

#include "qactioninput.h"
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DInput/qabstractphysicaldevice.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QActionInputPrivate : public Qt3DCore::QNodePrivate
{
public:
    QActionInputPrivate()
        : Qt3DCore::QNodePrivate()
        , m_sourceDevice(Q_NULLPTR)
    {}

    QVariantList m_keys;
    QAbstractPhysicalDevice *m_sourceDevice;
};

/*!
 * \qmltype ActionInput
 * \instantiates Qt3DInput::QActionInput
 * \inqmlmodule Qt3D.Input
 * \since 5.5
 * \TODO
 *
 */

/*!
 * \class Qt3DInput::QActionInput
 * \inmodule Qt3DInput
 * \since 5.5
 * \TODO
 *
 */

QActionInput::QActionInput(Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(*new QActionInputPrivate(), parent)
{
}

QActionInput::~QActionInput()
{
    QNode::cleanup();
}

QVariantList QActionInput::keys() const
{
    Q_D(const QActionInput);
    return d->m_keys;
}

void QActionInput::setSourceDevice(QAbstractPhysicalDevice *sourceDevice)
{
    Q_D(QActionInput);
    if (d->m_sourceDevice != sourceDevice) {

        // Check and set parent if needed
        // to force creation in the backend
        if (sourceDevice && !sourceDevice->parent())
            sourceDevice->setParent(this);

        d->m_sourceDevice = sourceDevice;
        emit sourceDeviceChanged(sourceDevice);
    }
}

QAbstractPhysicalDevice *QActionInput::sourceDevice() const
{
    Q_D(const QActionInput);
    return d->m_sourceDevice;
}

void QActionInput::setKeys(const QVariantList &keys)
{
    Q_D(QActionInput);
    if (d->m_keys != keys) {
        d->m_keys = keys;
        emit keysChanged(keys);
    }
}

void QActionInput::copy(const Qt3DCore::QNode *ref)
{
    QNode::copy(ref);
    const QActionInput *input = static_cast<const QActionInput *>(ref);
    d_func()->m_sourceDevice = qobject_cast<QAbstractPhysicalDevice *>(QNode::clone(input->d_func()->m_sourceDevice));
    d_func()->m_keys = input->d_func()->m_keys;
}

} // Qt3DInput

QT_END_NAMESPACE
