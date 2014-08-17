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

#include "qscenepropertychange.h"
#include "qscenepropertychange_p.h"
#include <Qt3DCore/qframeallocator.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

QFrameAllocator *QScenePropertyChangePrivate::m_allocator = new QFrameAllocator(sizeof(QScenePropertyChangePrivate), sizeof(QScenePropertyChange), sizeof(QScenePropertyChangePrivate) * 2);
QMutex QScenePropertyChangePrivate::m_mutex;

QScenePropertyChangePrivate::QScenePropertyChangePrivate(QScenePropertyChange *qq)
    : QSceneChangePrivate(qq)
{
}

QScenePropertyChangePrivate::~QScenePropertyChangePrivate()
{

}

void *QScenePropertyChangePrivate::operator new(size_t)
{
    QMutexLocker locker(&QScenePropertyChangePrivate::m_mutex);
    return QScenePropertyChangePrivate::m_allocator->allocateRawMemory<QScenePropertyChangePrivate>();
}

void QScenePropertyChangePrivate::operator delete(void *ptr)
{
    QMutexLocker locker(&QScenePropertyChangePrivate::m_mutex);
    QScenePropertyChangePrivate::m_allocator->deallocateRawMemory<QScenePropertyChangePrivate>(static_cast<QScenePropertyChangePrivate *>(ptr));
}

QScenePropertyChange::QScenePropertyChange(ChangeFlag type, QObservableInterface *subject, QSceneChange::Priority priority)
    : QSceneChange(*new QScenePropertyChangePrivate(this), type, subject, priority)
{
}

QScenePropertyChange::QScenePropertyChange(ChangeFlag type, QNode *node, QSceneChange::Priority priority)
    : QSceneChange(*new QScenePropertyChangePrivate(this), type, node, priority)
{
}

QScenePropertyChange::~QScenePropertyChange()
{
}

QScenePropertyChange::QScenePropertyChange(QScenePropertyChangePrivate &dd, ChangeFlag type, QObservableInterface *subject, QSceneChange::Priority priority)
    : QSceneChange(dd, type, subject, priority)
{
}

QScenePropertyChange::QScenePropertyChange(QScenePropertyChangePrivate &dd, ChangeFlag type, QNode *node, QSceneChange::Priority priority)
    : QSceneChange(dd, type, node, priority)
{
}

QByteArray QScenePropertyChange::propertyName() const
{
    Q_D(const QScenePropertyChange);
    return d->m_propertyName;
}

QVariant QScenePropertyChange::value() const
{
    Q_D(const QScenePropertyChange);
    return d->m_value;
}

void QScenePropertyChange::setPropertyName(const QByteArray &name)
{
    Q_D(QScenePropertyChange);
    d->m_propertyName = name;
}

void QScenePropertyChange::setValue(const QVariant &value)
{
    Q_D(QScenePropertyChange);
    d->m_value = value;
}

void *QScenePropertyChange::operator new(size_t)
{
    QMutexLocker locker(&QScenePropertyChangePrivate::m_mutex);
    return QScenePropertyChangePrivate::m_allocator->allocateRawMemory<QScenePropertyChange>();
}

void QScenePropertyChange::operator delete(void *ptr)
{
    QMutexLocker locker(&QScenePropertyChangePrivate::m_mutex);
    QScenePropertyChangePrivate::m_allocator->deallocateRawMemory<QScenePropertyChange>(static_cast<QScenePropertyChange *>(ptr));
}

} // Qt3D

QT_END_NAMESPACE
