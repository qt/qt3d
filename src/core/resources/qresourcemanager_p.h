// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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

#include <QtCore/QHash>
#include <QtCore/QMutex>
#include <QtCore/QReadLocker>
#include <QtCore/QReadWriteLock>
#include <QtCore/QtGlobal>
#include <limits>

#include <Qt3DCore/private/qhandle_p.h>
#include <Qt3DCore/private/qt3dcore_global_p.h>

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
        QMutexLocker<QMutex> m_locker;
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
    static const  bool needsCleanup = false;
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
    static const  bool needsCleanup = (FLAGS & Q_REQUIRES_CLEANUP) == 0;\
}; \
} // namespace Qt3DCore

template<typename T>
class QHandleData : public QHandle<T>::Data
{
public:
    T data;
};

template<typename T>
inline T *QHandle<T>::operator->() const { return (d && counter == d->counter) ? &static_cast<QHandleData<T> *>(d)->data : nullptr; }
template<typename T>
inline T *QHandle<T>::data() const { return (d && counter == d->counter) ? &static_cast<QHandleData<T> *>(d)->data : nullptr; }


class Q_3DCORE_PRIVATE_EXPORT AlignedAllocator
{
public:
    static void *allocate(uint size);
    static void release(void *p);
};

template <typename T>
class ArrayAllocatingPolicy
{
public:
    typedef QHandleData<T> HandleData;
    typedef QHandle<T> Handle;
    ArrayAllocatingPolicy()
    {
    }

    ~ArrayAllocatingPolicy()
    {
        m_activeHandles.clear();
        deallocateBuckets();
    }

    Handle allocateResource()
    {
        if (!freeList)
            allocateBucket();
        typename Handle::Data *d = freeList;
        freeList = freeList->nextFree;
        d->counter = allocCounter;
        allocCounter += 2; // ensure this will never clash with a pointer in nextFree by keeping the lowest bit set
        Handle handle(d);
        m_activeHandles.push_back(handle);
        return handle;
    }

    void releaseResource(const Handle &handle)
    {
        m_activeHandles.erase(std::remove(m_activeHandles.begin(), m_activeHandles.end(), handle), m_activeHandles.end());
        typename Handle::Data *d = handle.data_ptr();
        d->nextFree = freeList;
        freeList = d;
        performCleanup(&static_cast<QHandleData<T> *>(d)->data, std::integral_constant<bool, QResourceInfo<T>::needsCleanup>{});
    }

    T *data(Handle h)
    {
        return h.operator->();
    }

    void for_each(std::function<void(T*)> f)
    {
        Bucket *b = firstBucket;
        while (b) {
            for (int idx = 0; idx < Bucket::NumEntries; ++idx) {
                T *t = &b->data[idx].data;
                f(t);
            }
            b = b->header.next;
        }
    }

    int count() const { return int(m_activeHandles.size()); }
    const std::vector<Handle> &activeHandles() const { return m_activeHandles; }

private:
    Q_DISABLE_COPY(ArrayAllocatingPolicy)
    struct Bucket
    {
        struct Header
        {
            Bucket *next;
        } header;
        enum {
            Size = 4096,
            NumEntries = (Size - sizeof(Header)) / sizeof(HandleData)
        };
        HandleData data[NumEntries];
    };

    Bucket *firstBucket = nullptr;
    std::vector<Handle> m_activeHandles;
    typename Handle::Data *freeList = nullptr;
    int allocCounter = 1;

    void allocateBucket()
    {
        // no free handle, allocate a new
        // allocate aligned memory
        Bucket *b = static_cast<Bucket *>(AlignedAllocator::allocate(sizeof(Bucket)));

        // placement new
        new (b) Bucket;

        b->header.next = firstBucket;
        firstBucket = b;
        for (int i = 0; i < Bucket::NumEntries - 1; ++i) {
            b->data[i].nextFree = &b->data[i + 1];
        }
        b->data[Bucket::NumEntries - 1].nextFree = nullptr;
        freeList = &b->data[0];
    }

    void deallocateBuckets()
    {
        Bucket *b = firstBucket;
        while (b) {
            Bucket *n = b->header.next;
            // Call dtor explicitly
            b->~Bucket();
            // Release aligned memory
            AlignedAllocator::release(b);
            b = n;
        }
    }

    template<typename Q = T>
    void performCleanup(Q *r, std::integral_constant<bool, true>)
    {
        r->cleanup();
    }

    template<typename Q = T>
    void performCleanup(Q *, std::integral_constant<bool, false>)
    {
    }

};

#ifndef QT_NO_DEBUG_STREAM
template <typename ValueType, typename KeyType,
          template <class> class LockingPolicy
          >
class QResourceManager;

template <typename ValueType, typename KeyType,
          template <class> class LockingPolicy = NonLockingPolicy
          >
QDebug operator<<(QDebug dbg, const QResourceManager<ValueType, KeyType, LockingPolicy> &manager);
#endif

template <typename ValueType, typename KeyType,
          template <class> class LockingPolicy = NonLockingPolicy
          >
class QResourceManager
        : public ArrayAllocatingPolicy<ValueType>
        , public LockingPolicy< QResourceManager<ValueType, KeyType, LockingPolicy> >
{
public:
    typedef ArrayAllocatingPolicy<ValueType> Allocator;
    typedef QHandle<ValueType> Handle;

    QResourceManager() :
        Allocator()
    {
    }

    ~QResourceManager()
    {}

    Handle acquire()
    {
        typename LockingPolicy<QResourceManager>::WriteLocker lock(this);
        return Allocator::allocateResource();
    }

    ValueType* data(const Handle &handle)
    {
        return handle.operator->();
    }

    void release(const Handle &handle)
    {
        typename LockingPolicy<QResourceManager>::WriteLocker lock(this);
        Allocator::releaseResource(handle);
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
        return handle.operator->();
    }

    void releaseResource(const KeyType &id)
    {
        typename LockingPolicy<QResourceManager>::WriteLocker lock(this);
        Handle handle = m_keyToHandleMap.take(id);
        if (!handle.isNull())
            Allocator::releaseResource(handle);
    }

    // Releases all resources referenced by a key
    // Resources allocated manually with just a handle aren't releases
    void releaseAllResources()
    {
        typename LockingPolicy<QResourceManager>::WriteLocker lock(this);
        // Make a copy as releaseResource removes the entry in m_activeHanldes
        const std::vector<Handle> activeHandles = Allocator::activeHandles();
        for (const Handle &h : activeHandles)
            Allocator::releaseResource(h);
        // Clear Key to Handle Map
        m_keyToHandleMap.clear();
    }

protected:
    QHash<KeyType, Handle > m_keyToHandleMap;

private:
    friend QDebug operator<< <>(QDebug dbg, const QResourceManager<ValueType, KeyType, LockingPolicy> &manager);
};

#ifndef QT_NO_DEBUG_STREAM
template <typename ValueType, typename KeyType,
          template <class> class LockingPolicy
          >
QDebug operator<<(QDebug dbg, const QResourceManager<ValueType, KeyType, LockingPolicy> &manager)
{
    QDebugStateSaver saver(dbg);
    dbg << "Contains" << manager.count() << "items" << Qt::endl;

    dbg << "Key to Handle Map:" << Qt::endl;
    const auto end = manager.m_keyToHandleMap.cend();
    for (auto it = manager.m_keyToHandleMap.cbegin(); it != end; ++it)
        dbg << "QNodeId =" << it.key() << "Handle =" << it.value() << Qt::endl;

//    dbg << "Resources:" << Qt::endl;
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
