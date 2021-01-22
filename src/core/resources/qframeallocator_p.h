/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#ifndef QT3DCORE_QFRAMEALLOCATOR_P_H
#define QT3DCORE_QFRAMEALLOCATOR_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#ifdef QFRAMEALLOCATOR_DEBUG
#include <valgrind/valgrind.h>
#include <valgrind/memcheck.h>
#endif

#include <QtCore/QDebug>
#include <QtCore/QScopedPointer>
#include <QtCore/QVector>

#include <Qt3DCore/private/qt3dcore_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QFrameAllocatorPrivate;

class Q_3DCORE_PRIVATE_EXPORT QFrameAllocator
{
public:
    explicit QFrameAllocator(uint maxObjectSize, uint alignment = 16, uint pageSize = 128);
    ~QFrameAllocator();

    template<typename T>
    T* allocate()
    {
        void* ptr = allocateRawMemory(sizeof(T));
        new (ptr) T(); // Don't forget to call the constructor of the object using the placement new operator
        return static_cast<T*>(ptr);
    }

    template<typename T>
    void deallocate(T *ptr)
    {
        ptr->~T(); // Call destructor
        deallocateRawMemory(ptr, sizeof(T));
    }

    void* allocateRawMemory(size_t size);

    void deallocateRawMemory(void *ptr, size_t size);

    void clear();
    void trim();
    uint maxObjectSize() const;
    uint totalChunkCount() const;
    int allocatorPoolSize() const;
    bool isEmpty() const;

private:
    Q_DECLARE_PRIVATE(QFrameAllocator)
    const QScopedPointer<QFrameAllocatorPrivate> d_ptr;
};

} // Qt3D

QT_END_NAMESPACE

#endif // QFRAMEALLOCATOR_P_H
