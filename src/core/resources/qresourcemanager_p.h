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

#ifndef QT3DCORE_QABSTRACTRESOURCESMANAGER_H
#define QT3DCORE_QABSTRACTRESOURCESMANAGER_H

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

#include <Qt3DCore/qt3dcore_global.h>
#include <QtCore/QHash>
#include <QtCore/QMutex>
#include <QtCore/QReadLocker>
#include <QtCore/QReadWriteLock>
#include <QtCore/QtGlobal>
#include <limits>

#include <Qt3DCore/private/qhandle_p.h>

// Silence complaints about unreferenced local variables in
// ArrayAllocatingPolicy::deallocateBuckets() when the compiler
// inlines the call to the dtor and it is empty. Default warning
// setting re-enabled at bottom of this file
#if defined(Q_CC_MSVC)
#pragma warning(disable : 4189)
#endif

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

template <class Host>
struct NonLockingPolicy
{
    struct ReadLocker
    {
        ReadLocker(const NonLockingPolicy*) {}
        void unlock() {}
        void relock() {}
    };

    struct WriteLocker
    {
        WriteLocker(const NonLockingPolicy*) {}
        void unlock() {}
        void relock() {}
    };

    struct Locker
    {
        Locker(const NonLockingPolicy*) {}
        void unlock() {}
        void relock() {}
    };
};

template <class Host>
class ObjectLevelLockingPolicy
{
public :
    ObjectLevelLockingPolicy()
    {}

    class ReadLocker
    {
    public:
        ReadLocker(const ObjectLevelLockingPolicy *host)
            : m_locker(&host->m_readWritelock)
        { }

        void unlock()
        {
            m_locker.unlock();
        }

        void relock()
        {
            m_locker.relock();
        }

    private:
        QReadLocker m_locker;
    };

    class WriteLocker
    {
    public:
        WriteLocker(const ObjectLevelLockingPolicy *host)
            : m_locker(&host->m_readWritelock)
        { }

        void unlock()
        {
            m_locker.unlock();
        }

        void relock()
        {
            m_locker.relock();
        }

    private:
        QWriteLocker m_locker;
    };

    class Locker
    {
    public:
        Locker(const ObjectLevelLockingPolicy *host)
            : m_locker(&host->m_lock)
        { }

        void unlock()
        {
            m_locker.unlock();
        }

        void relock()
        {
            m_locker.relock();
        }

    private:
        QMutexLocker m_locker;
    };

private:
    friend class Locker;
    friend class ReadLocker;
    friend class WriteLocker;
    mutable QReadWriteLock m_readWritelock;
    mutable QMutex m_lock;
};

template <typename T>
struct QResourceInfo
{
    enum
    {
        needsCleanup = false
    };
};

template <>
struct QResourceInfo<void>
{
    enum
    {
        needsCleanup = false
    };
};

enum
{
    Q_REQUIRES_CLEANUP = 0
};

#define Q_DECLARE_RESOURCE_INFO(TYPE, FLAGS) \
    namespace Qt3DCore { \
    template<> \
    struct QResourceInfo<TYPE > \
{ \
    enum \
{ \
    needsCleanup = ((FLAGS & Q_REQUIRES_CLEANUP) == 0) \
}; \
}; \
} // namespace Qt3DCore

template <int v>
struct Int2Type
{
    enum
    {
        value = v
    };
};

template <typename T, uint INDEXBITS = 16>
class ArrayAllocatingPolicy
{
public:
    typedef QHandle<T, INDEXBITS> Handle;
    ArrayAllocatingPolicy()
    {
        m_freeList.resize(MaxSize);
        for (int i = 0; i < MaxSize; i++)
            m_freeList[i] = MaxSize - (i + 1);
    }

    ~ArrayAllocatingPolicy()
    {
        deallocateBuckets();
    }

    Handle allocateResource()
    {
        Q_ASSERT(!m_freeList.isEmpty());
        int idx = m_freeList.takeLast();
        int bucketIdx = idx / BucketSize;
        int localIdx = idx % BucketSize;
        Q_ASSERT(bucketIdx <= m_numBuckets);
        if (bucketIdx == m_numBuckets) {
            m_bucketDataPtrs[bucketIdx] = static_cast<T*>(malloc(sizeof(T) * BucketSize));
            m_counters[bucketIdx] = static_cast<short *>(malloc(sizeof(short) * BucketSize));
            // ### memset is only needed as long as we also use this for primitive types (see FrameGraphManager)
            // ### remove once this is fixed, add a static_assert on T instead
            memset((void *)m_bucketDataPtrs[bucketIdx], 0, sizeof(T) * BucketSize);
            memset(m_counters[bucketIdx], 0, sizeof(short) * BucketSize);
            ++m_numBuckets;
        }

        Q_ASSERT(idx <= m_numConstructed);
        if (idx == m_numConstructed) {
            new (m_bucketDataPtrs[bucketIdx] + localIdx) T;
            ++m_numConstructed;
        }
        Q_STATIC_ASSERT(Handle::MaxCounter < USHRT_MAX);
        Q_ASSERT(m_counters[bucketIdx][localIdx] <= 0);
        m_counters[bucketIdx][localIdx] *= -1;
        ++m_counters[bucketIdx][localIdx];
        if (m_counters[bucketIdx][localIdx] >= Handle::MaxCounter)
            m_counters[bucketIdx][localIdx] = 1;

        return Handle(idx, m_counters[bucketIdx][localIdx]);
    }

    void releaseResource(Handle h)
    {
        int idx = h.index();
        int bucketIdx = idx / BucketSize;
        int localIdx = idx % BucketSize;

        Q_ASSERT(h.counter() == static_cast<quint32>(m_counters[bucketIdx][localIdx]));
        T *r = m_bucketDataPtrs[bucketIdx] + localIdx;

        m_freeList.append(idx);
        m_counters[bucketIdx][localIdx] *= -1;
        performCleanup(r, Int2Type<QResourceInfo<T>::needsCleanup>());
    }

    T *data(Handle h/*, bool *ok = 0*/) {
        int bucketIdx = h.index() / BucketSize;
        int localIdx = h.index() % BucketSize;

        if (h.counter() != static_cast<quint32>(m_counters[bucketIdx][localIdx])) {
            return nullptr;
        }
        return m_bucketDataPtrs[bucketIdx] + localIdx;
    }

    void for_each(std::function<void(T*)> f) {
        for (int idx = 0; idx < m_numConstructed; ++idx) {
            int bucketIdx = idx / BucketSize;
            int localIdx = idx % BucketSize;
            T * t = m_bucketDataPtrs[bucketIdx] + localIdx;
            f(t);
        }
    }

private:
    Q_DISABLE_COPY(ArrayAllocatingPolicy)

    enum {
        MaxSize = (1 << INDEXBITS),
        // use at most 1024 items per bucket, or put all items into a single
        // bucket if MaxSize is small enough
        BucketSize = (1 << (INDEXBITS < 10 ? INDEXBITS : 10))
    };

    void deallocateBuckets()
    {
        while (m_numConstructed > 0) {
            --m_numConstructed;
            int bucketIdx = m_numConstructed / BucketSize;
            int localIdx = m_numConstructed % BucketSize;
            (m_bucketDataPtrs[bucketIdx] + localIdx)->~T();
        }

        while (m_numBuckets > 0) {
            --m_numBuckets;
            free(m_bucketDataPtrs[m_numBuckets]);
            free(m_counters[m_numBuckets]);
        }
    }

    T* m_bucketDataPtrs[MaxSize / BucketSize];
    short *m_counters[MaxSize / BucketSize];
    QVector<int> m_freeList;
    int m_numBuckets = 0;
    int m_numConstructed = 0;

    void performCleanup(T *r, Int2Type<true>)
    {
        r->cleanup();
    }

    void performCleanup(T *, Int2Type<false>)
    {}

};

#ifndef QT_NO_DEBUG_STREAM
template <typename ValueType, typename KeyType, uint INDEXBITS,
          template <class> class LockingPolicy
          >
class QResourceManager;

template <typename ValueType, typename KeyType, uint INDEXBITS = 16,
          template <class> class LockingPolicy = NonLockingPolicy
          >
QDebug operator<<(QDebug dbg, const QResourceManager<ValueType, KeyType, INDEXBITS, LockingPolicy> &manager);
#endif

template <typename ValueType, typename KeyType, uint INDEXBITS = 16,
          template <class> class LockingPolicy = NonLockingPolicy
          >
class QResourceManager
        : public ArrayAllocatingPolicy<ValueType, INDEXBITS>
        , public LockingPolicy< QResourceManager<ValueType, KeyType, INDEXBITS, LockingPolicy> >
{
public:
    typedef ArrayAllocatingPolicy<ValueType, INDEXBITS> Allocator;
    typedef QHandle<ValueType, INDEXBITS> Handle;

    QResourceManager() :
        Allocator(),
        m_maxSize((1 << INDEXBITS) - 1)
    {
    }

    ~QResourceManager()
    {}

    Handle acquire()
    {
        typename LockingPolicy<QResourceManager>::WriteLocker lock(this);
        Handle handle = Allocator::allocateResource();
        m_activeHandles.push_back(handle);
        return handle;
    }

    ValueType* data(const Handle &handle)
    {
        typename LockingPolicy<QResourceManager>::ReadLocker lock(this);
        return Allocator::data(handle);
    }

    void release(const Handle &handle)
    {
        typename LockingPolicy<QResourceManager>::WriteLocker lock(this);
        releaseLocked(handle);
    }

    bool contains(const KeyType &id) const
    {
        typename LockingPolicy<QResourceManager>::ReadLocker lock(this);
        return m_keyToHandleMap.contains(id);
    }

    Handle getOrAcquireHandle(const KeyType &id)
    {
        typename LockingPolicy<QResourceManager>::ReadLocker lock(this);
        Handle handle = m_keyToHandleMap.value(id);
        if (handle.isNull()) {
            lock.unlock();
            typename LockingPolicy<QResourceManager>::WriteLocker writeLock(this);
            // Test that the handle hasn't been set (in the meantime between the read unlock and the write lock)
            Handle &handleToSet = m_keyToHandleMap[id];
            if (handleToSet.isNull()) {
                handleToSet = Allocator::allocateResource();
                m_activeHandles.push_back(handleToSet);
            }
            return handleToSet;
        }
        return handle;
    }

    Handle lookupHandle(const KeyType &id)
    {
        typename LockingPolicy<QResourceManager>::ReadLocker lock(this);
        return m_keyToHandleMap.value(id);
    }

    ValueType *lookupResource(const KeyType &id)
    {
        ValueType* ret = nullptr;
        {
            typename LockingPolicy<QResourceManager>::ReadLocker lock(this);
            Handle handle = m_keyToHandleMap.value(id);
            if (!handle.isNull())
                ret = Allocator::data(handle);
        }
        return ret;
    }

    ValueType *getOrCreateResource(const KeyType &id)
    {
        const Handle handle = getOrAcquireHandle(id);
        typename LockingPolicy<QResourceManager>::ReadLocker lock(this);
        return Allocator::data(handle);
    }

    void releaseResource(const KeyType &id)
    {
        typename LockingPolicy<QResourceManager>::WriteLocker lock(this);
        Handle handle = m_keyToHandleMap.take(id);
        if (!handle.isNull())
            releaseLocked(handle);
    }

    int maximumSize() const { return m_maxSize; }

    int count() const Q_DECL_NOEXCEPT { return m_activeHandles.size(); }

    inline QVector<Handle > activeHandles() const Q_DECL_NOEXCEPT { return m_activeHandles; }

protected:
    QHash<KeyType, Handle > m_keyToHandleMap;
    QVector<Handle > m_activeHandles;
    const int m_maxSize;

private:
    void releaseLocked(const Handle &handle)
    {
        m_activeHandles.removeOne(handle);
        Allocator::releaseResource(handle);
    }

    friend QDebug operator<< <>(QDebug dbg, const QResourceManager<ValueType, KeyType, INDEXBITS, LockingPolicy> &manager);
};

#ifndef QT_NO_DEBUG_STREAM
template <typename ValueType, typename KeyType, uint INDEXBITS,
          template <class> class LockingPolicy
          >
QDebug operator<<(QDebug dbg, const QResourceManager<ValueType, KeyType, INDEXBITS, LockingPolicy> &manager)
{
    QDebugStateSaver saver(dbg);
    dbg << "Contains" << manager.count() << "items" << "of a maximum" << manager.maximumSize() << endl;

    dbg << "Key to Handle Map:" << endl;
    const auto end = manager.m_keyToHandleMap.cend();
    for (auto it = manager.m_keyToHandleMap.cbegin(); it != end; ++it)
        dbg << "QNodeId =" << it.key() << "Handle =" << it.value() << endl;

//    dbg << "Resources:" << endl;
//    dbg << manager.m_handleManager;
    return dbg;
}
#endif

}// Qt3D

QT_END_NAMESPACE

#if defined(Q_CC_MSVC)
#pragma warning(default : 4189)
#endif

#endif // QT3DCORE_QABSTRACTRESOURCESMANAGER_H
