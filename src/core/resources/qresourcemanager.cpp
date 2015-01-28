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

/*!
    \class Qt3D::QResourceManager
    \inmodule Qt3DCore
    \since 5.5

    \brief The QResourceManager allocates memory for resources that can be referenced by a QHandle.

    Using a QHandleManager for handle management, the QResourceManager's responsibility is
    to provide memory for resources and to offer ways to interact with the resource through
    the QHandle.

    Using the QHandle obtained when acquiring a resource, the resource can be retrieved and
    released when no longer needed.

    Internally, memory can be reorganized for best performance while being transparent to the user.

    The memory allocation scheme and locking policies can be customized by providing template
    parameters. The defaults are ArrayAllocationPolicy and NonLockingPolicy respectively.
*/

/*!
    \class Qt3D::ArrayAllocatingPolicy
    \inmodule Qt3DCore
    \since 5.5

    \brief Allocates memory in a contiguous space trying to minimize fragmentation and cache misses.

    Once the maximum number of entities is reached, no more allocations can be made until some resources are
    released

    \sa QResourceManager
    \sa ListAllocatingPolicy
*/

/*!
    \class Qt3D::ListAllocatingPolicy
    \inmodule Qt3DCore
    \since 5.5

    \brief Allocates resources in a list.

    It is best to use it when you don't need to iterate over an entire set of resources, in which
    case ArrayAllocatingPolicy is faster. It can store a non predefined amount of resources, though
    there might not be enough handles at some point, depending on the INDEXBITS used.
    It's main use case is to manage resources that are accessed in an independent manner from other
    resources of the same type.

    \sa QResourceManager
    \sa ArrayAllocatingPolicy
*/

/*!
    \class Qt3D::ObjectLevelLockingPolicy
    \inmodule Qt3DCore
    \since 5.5

    \brief Provides locking access to a resource through the use of a QReadWriteLock.

    This policy should be used in a QResourceManager when multiple threads may access the manager for
    read and write operations at the same time.

    It provides two convenience classes WriteLocker and ReadLocker that behave like QReadLocker and QWriteLocker.
*/
