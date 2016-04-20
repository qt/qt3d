/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qscenepropertychange.h"
#include "qscenepropertychange_p.h"
#include <Qt3DCore/private/qframeallocator_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QFrameAllocator *QScenePropertyChangePrivate::m_allocator = new QFrameAllocator(128, sizeof(QScenePropertyChange), sizeof(QScenePropertyChangePrivate) * 2);
QMutex QScenePropertyChangePrivate::m_mutex;

QScenePropertyChangePrivate::QScenePropertyChangePrivate()
    : QSceneChangePrivate()
{
}

QScenePropertyChangePrivate::~QScenePropertyChangePrivate()
{

}

void *QScenePropertyChangePrivate::operator new(size_t size)
{
    QMutexLocker locker(&QScenePropertyChangePrivate::m_mutex);
    return QScenePropertyChangePrivate::m_allocator->allocateRawMemory(size);
}

void QScenePropertyChangePrivate::operator delete(void *ptr, size_t size)
{
    QMutexLocker locker(&QScenePropertyChangePrivate::m_mutex);
    QScenePropertyChangePrivate::m_allocator->deallocateRawMemory(ptr, size);
}

/*!
 * \class Qt3DCore::QScenePropertyChange
 * \inmodule Qt3DCore
 *
 * TODO
 */

/*!
 * \typedef Qt3DCore::QScenePropertyChangePtr
 * \relates Qt3DCore::QScenePropertyChange
 *
 * A shared pointer for QScenePropertyChange.
 */

/*!
 * Constructs a new QScenePropertyChange with \a type, \a observableType, \a subjectId, and
 * \a priority.
 */
QScenePropertyChange::QScenePropertyChange(ChangeFlag type, ObservableType observableType,
                                           QNodeId subjectId, QSceneChange::Priority priority)
    : QSceneChange(*new QScenePropertyChangePrivate, type, observableType, subjectId, priority)
{
}

/*!
 * \internal
 */
QScenePropertyChange::QScenePropertyChange(QScenePropertyChangePrivate &dd)
    : QSceneChange(dd)
{
}

/*!
 * \internal
 */
QScenePropertyChange::QScenePropertyChange(QScenePropertyChangePrivate &dd, ChangeFlag type, ObservableType observableType, QNodeId subjectId, QSceneChange::Priority priority)
    : QSceneChange(dd, type, observableType, subjectId, priority)
{
}

QScenePropertyChange::~QScenePropertyChange()
{
}

/*!
 * \return name of the property.
 */
const char *QScenePropertyChange::propertyName() const
{
    Q_D(const QScenePropertyChange);
    return d->m_propertyName;
}

/*!
 * \return property value.
 */
QVariant QScenePropertyChange::value() const
{
    Q_D(const QScenePropertyChange);
    return d->m_value;
}

/*!
 * Sets the property change \a name.
 */
void QScenePropertyChange::setPropertyName(const char *name)
{
    Q_D(QScenePropertyChange);
    d->m_propertyName = name;
}

/*!
 * Set the property change \a value.
 */
void QScenePropertyChange::setValue(const QVariant &value)
{
    Q_D(QScenePropertyChange);
    d->m_value = value;
}

/*!
 * \return new scene property change of size \a n.
 */
void *QScenePropertyChange::operator new(size_t n)
{
    QMutexLocker locker(&QScenePropertyChangePrivate::m_mutex);
    return QScenePropertyChangePrivate::m_allocator->allocateRawMemory(n);
}

/*!
 * Deletes \a size block from scene property change starting from \a ptr.
 */
void QScenePropertyChange::operator delete(void *ptr, size_t size)
{
    QMutexLocker locker(&QScenePropertyChangePrivate::m_mutex);
    QScenePropertyChangePrivate::m_allocator->deallocateRawMemory(ptr, size);
}

} // Qt3D

QT_END_NAMESPACE
