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
******************************************************************************/

/* !\internal
    \class Qt3DCore::QResourceManager
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
    parameters. The defaults are ArrayAllocatingPolicy and NonLockingPolicy respectively.
*/

/* !\internal
    \class Qt3DCore::ArrayAllocatingPolicy
    \inmodule Qt3DCore
    \since 5.5

    \brief Allocates memory in a contiguous space trying to minimize fragmentation and cache misses.

    Once the maximum number of entities is reached, no more allocations can be made until some resources are
    released

    \sa QResourceManager
*/

/* !\internal
    \class Qt3DCore::ObjectLevelLockingPolicy
    \inmodule Qt3DCore
    \since 5.5

    \brief Provides locking access to a resource through the use of a QReadWriteLock.

    This policy should be used in a QResourceManager when multiple threads may access the manager for
    read and write operations at the same time.

    It provides two convenience classes WriteLocker and ReadLocker that behave like QReadLocker and QWriteLocker.
*/

#include "qresourcemanager_p.h"
#include <QtCore/private/qsimd_p.h>
#include <Qt3DCore/private/qt3dcore-config_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

void *AlignedAllocator::allocate(uint size)
{
#if QT_CONFIG(qt3d_simd_avx2) && defined(__AVX2__) && defined(QT_COMPILER_SUPPORTS_AVX2)
    return _mm_malloc(size, 32);
#elif QT_CONFIG(qt3d_simd_sse2) && defined(__SSE2__) && defined(QT_COMPILER_SUPPORTS_SSE2)
    return _mm_malloc(size, 16);
#else
    return malloc(size);
#endif
}

void AlignedAllocator::release(void *p)
{
#if QT_CONFIG(qt3d_simd_avx2) && defined(__AVX2__) && defined(QT_COMPILER_SUPPORTS_AVX2)
    _mm_free(p);
#elif QT_CONFIG(qt3d_simd_sse2) && defined(__SSE2__) && defined(QT_COMPILER_SUPPORTS_SSE2)
    _mm_free(p);
#else
    free(p);
#endif
}

} // Qt3DCore

QT_END_NAMESPACE
