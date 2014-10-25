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

#ifndef QT3D_QABSTRACTRESOURCESMANAGER_H
#define QT3D_QABSTRACTRESOURCESMANAGER_H

#include <QtGlobal>
#include <QReadWriteLock>
#include <QHash>
#include <Qt3DCore/qt3dcore_global.h>
#include <Qt3DCore/qhandle.h>
#include <Qt3DCore/qhandlemanager.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

template <class Host>
struct NonLockingPolicy
{
    void lockForRead() {}
    void lockForWrite() {}
    void unlock() {}

    struct WriteLocker
    {
        WriteLocker(NonLockingPolicy*) {}
        void unlock() {}
    };

    struct ReadLocker
    {
        ReadLocker(NonLockingPolicy*) {}
        void unlock() {}
    };
};

template <class Host>
class ObjectLevelLockingPolicy
{
public :

    ObjectLevelLockingPolicy()
        : m_lock(QReadWriteLock::Recursive)
    {}

    void lockForRead()    { m_lock.lockForRead(); }
    void lockForWrite()   { m_lock.lockForWrite(); }
    void unlock() { m_lock.unlock(); }

    class WriteLocker
    {
    public:
        WriteLocker(ObjectLevelLockingPolicy *host)
            : m_host(host)
            , m_locked(true)
        { m_host->lockForWrite(); }

        ~WriteLocker() { unlock(); }
        void unlock()
        {
            if (m_locked) {
                m_host->unlock();
                m_locked = false;
            }
        }

    private:
        ObjectLevelLockingPolicy *m_host;
        bool m_locked;
    };

    class ReadLocker
    {
    public:
        ReadLocker(ObjectLevelLockingPolicy *host)
            : m_host(host)
            , m_locked(true)
        { m_host->lockForRead(); }

        ~ReadLocker() { unlock(); }
        void unlock()
        {
            if (m_locked) {
                m_host->unlock();
                m_locked = false;
            }
        }

    private:
        ObjectLevelLockingPolicy *m_host;
        bool m_locked;
    };

private:
    QReadWriteLock m_lock;
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
    template<> \
    struct QResourceInfo<TYPE > \
{ \
    enum \
{ \
    needsCleanup = ((FLAGS & Q_REQUIRES_CLEANUP) == 0) \
}; \
}

template <int v>
struct Int2Type
{
    enum
    {
        value = v
    };
};

template <typename T, int INDEXBITS>
class ArrayAllocatingPolicy
{
public:
    ArrayAllocatingPolicy()
    {
        reset();
    }

    T* allocateResource()
    {
        Q_ASSERT(!m_freeList.isEmpty());
        int idx = m_freeList.last();
        m_freeList.pop_back();
        int bucketIdx = idx / BucketSize;
        int localIdx = idx % BucketSize;
        Q_ASSERT(bucketIdx <= m_buckets.size());
        if (bucketIdx == m_buckets.size()) {
            m_buckets.append(Bucket(BucketSize));
            m_bucketDataPtrs[bucketIdx] = m_buckets.last().data();
        }
        return m_bucketDataPtrs[bucketIdx] + localIdx;
    }

    void releaseResource(T *r)
    {
        // search linearly over buckets to find the index of the resource
        // and put it into the free list
        const int numBuckets = m_buckets.size();
        for (int bucketIdx = 0; bucketIdx < numBuckets; ++bucketIdx) {
            const T* firstItem = m_bucketDataPtrs[bucketIdx];
            if (firstItem > r || r > firstItem + BucketSize) {
                // resource is not in this bucket when its pointer address
                // is outside the address range spanned by the addresses of
                // the first and last items in a bucket
                continue;
            }

            // now we found the bucket we can reconstruct the global index
            // and put it back into the free list
            const int localIdx = static_cast<int>(r - firstItem);
            const int idx = bucketIdx * BucketSize + localIdx;
            m_freeList.append(idx);
            performCleanup(r, Int2Type<QResourceInfo<T>::needsCleanup>());
            // release the actual resource by overwriting it with a default
            // constructed item which can then be reused later
            *r = T();
            break;
        }
    }

    void reset()
    {
        m_buckets.clear();
        m_freeList.resize(MaxSize);
        for (int i = 0; i < MaxSize; i++)
            m_freeList[i] = MaxSize - (i + 1);
    }

private:

    enum {
        MaxSize = (1 << INDEXBITS),
        // use at most 1024 items per bucket, or put all items into a single
        // bucket if MaxSize is small enough
        BucketSize = (1 << (INDEXBITS < 10 ? INDEXBITS : 10))
    };

    typedef QVector<T> Bucket;
    QList<Bucket> m_buckets;
    // optimization: quick access to bucket data pointers
    // this improves the performance of the releaseResource benchmarks
    // and reduces their runtime by a factor 2
    T* m_bucketDataPtrs[MaxSize / BucketSize];
    QVector<int> m_freeList;

    void performCleanup(T *r, Int2Type<true>)
    {
        r->cleanup();
    }

    void performCleanup(T *, Int2Type<false>)
    {}

};

template <typename T, int INDEXBITS>
class ArrayPreallocationPolicy
{
public:
    ArrayPreallocationPolicy()
    {
        reset();
    }

    T* allocateResource()
    {
        Q_ASSERT(!m_freeList.isEmpty());
        int idx = m_freeList.last();
        m_freeList.pop_back();
        return m_bucket.data() + idx;
    }

    void releaseResource(T *r)
    {
        Q_ASSERT(m_bucket.data() <= r && r < m_bucket.data() + MaxSize);
        int idx = r - m_bucket.data();
        m_freeList.append(idx);
        performCleanup(r, Int2Type<QResourceInfo<T>::needsCleanup>());
        *r = T();
    }

    void reset()
    {
        m_bucket.clear();
        m_bucket.resize(MaxSize);
        m_freeList.resize(MaxSize);
        for (int i = 0; i < MaxSize; i++)
            m_freeList[i] = MaxSize - (i + 1);
    }

private:
    enum {
      MaxSize = 1 << INDEXBITS
    };

    QVector<T> m_bucket;
    QVector<int> m_freeList;

    void performCleanup(T *r, Int2Type<true>)
    {
        r->cleanup();
    }

    void performCleanup(T *, Int2Type<false>)
    {}

};

template <typename T, int INDEXBITS>
class ListAllocatingPolicy
{
public:
    ListAllocatingPolicy()
    {
    }

    T* allocateResource()
    {
        int idx;
        T *newT;
        if (!m_freeEntries.isEmpty()) {
            idx = m_freeEntries.takeFirst();
            m_resourcesEntries[idx] = T();
            newT = &m_resourcesEntries[idx];
        }
        else {
            m_resourcesEntries.append(T());
            idx = m_resourcesEntries.size() - 1;
            newT = &m_resourcesEntries.last();
        }
        // If elements are added to the list, we're not sure the address
        // that was returned here for previous elements stays valid
        m_resourcesToIndices[newT] = idx;
        return newT;
    }

    void releaseResource(T *r)
    {
        if (m_resourcesToIndices.contains(r)) {
            performCleanup(r, Int2Type<QResourceInfo<T>::needsCleanup>());
            m_freeEntries.append(m_resourcesToIndices.take(r));
        }
    }

    void reset()
    {
        m_resourcesEntries.clear();
        m_resourcesToIndices.clear();
    }

private:
    QList<T> m_resourcesEntries;
    QHash<T*, int> m_resourcesToIndices;
    QList<int> m_freeEntries;

    void performCleanup(T *r, Int2Type<true>)
    {
        r->cleanup();
    }

    void performCleanup(T *, Int2Type<false>)
    {}
};

template <typename T, typename C, int INDEXBITS = 16,
          template <typename, int> class AllocatingPolicy = ArrayAllocatingPolicy,
          template <class> class LockingPolicy = NonLockingPolicy
          >
class QResourcesManager
        : public AllocatingPolicy<T, INDEXBITS>
        , public LockingPolicy< QResourcesManager<T, C, INDEXBITS, AllocatingPolicy, LockingPolicy> >
{
public:
    QResourcesManager() :
        AllocatingPolicy<T, INDEXBITS>(),
        m_maxResourcesEntries((1 << INDEXBITS) - 1)
    {
    }

    ~QResourcesManager()
    {}

    QHandle<T, INDEXBITS> acquire()
    {
        typename LockingPolicy<QResourcesManager>::WriteLocker writeLock(this);
        QHandle<T, INDEXBITS> handle = m_handleManager.acquire(AllocatingPolicy<T, INDEXBITS>::allocateResource());
        return handle;
    }

    T* data(const QHandle<T, INDEXBITS> &handle)
    {
        typename LockingPolicy<QResourcesManager>::ReadLocker readLock(this);
        T* d = m_handleManager.data(handle);
        return d;
    }

    void release(const QHandle<T, INDEXBITS> &handle)
    {
        typename LockingPolicy<QResourcesManager>::WriteLocker writeLock(this);
        T *val = m_handleManager.data(handle);
        m_handleManager.release(handle);
        AllocatingPolicy<T, INDEXBITS>::releaseResource(val);
    }

    void reset()
    {
        typename LockingPolicy<QResourcesManager>::WriteLocker writeLock(this);
        m_handleManager.reset();
        AllocatingPolicy<T, INDEXBITS>::reset();
    }

    bool contains(const C &id)
    {
        typename LockingPolicy<QResourcesManager>::ReadLocker readLock(this);
        return m_handleToResourceMapper.contains(id);
    }

    QHandle<T, INDEXBITS> getOrAcquireHandle(const C &id)
    {
        if (!contains(id)) {
            typename LockingPolicy<QResourcesManager>::WriteLocker writeLock(this);
            if (!m_handleToResourceMapper.contains(id))
                m_handleToResourceMapper[id] = acquire();
        }
        typename LockingPolicy<QResourcesManager>::ReadLocker readLock(this);
        return m_handleToResourceMapper[id];
    }

    QHandle<T, INDEXBITS> lookupHandle(const C &id)
    {
        typename LockingPolicy<QResourcesManager>::ReadLocker readLock(this);
        QHandle<T, INDEXBITS> handle;
        if (m_handleToResourceMapper.contains(id))
            handle = m_handleToResourceMapper[id];
        return handle;
    }

    T *lookupResource(const C &id)
    {
        T *ret = Q_NULLPTR;
        typename LockingPolicy<QResourcesManager>::ReadLocker readLock(this);
        if (m_handleToResourceMapper.contains(id))
            ret = data(m_handleToResourceMapper[id]);
        return ret;
    }

    T *getOrCreateResource(const C &id)
    {
        typename LockingPolicy<QResourcesManager>::ReadLocker readLock(this);
        if (!m_handleToResourceMapper.contains(id)) {
            readLock.unlock();
            typename LockingPolicy<QResourcesManager>::WriteLocker writeLock(this);
            if (!m_handleToResourceMapper.contains(id))
                m_handleToResourceMapper[id] = acquire();
        }
        return m_handleManager.data(m_handleToResourceMapper[id]);
    }

    void releaseResource(const C &id)
    {
        typename LockingPolicy<QResourcesManager>::WriteLocker writeLock(this);
        if (m_handleToResourceMapper.contains(id))
            release(m_handleToResourceMapper.take(id));
    }

    int maxResourcesEntries() const { return m_maxResourcesEntries; }

protected:
    QHandleManager<T, INDEXBITS> m_handleManager;
    QHash<C, QHandle<T, INDEXBITS> > m_handleToResourceMapper;
    int m_maxResourcesEntries;

};

}// Qt3D

QT_END_NAMESPACE

#endif // QT3D_QABSTRACTRESOURCESMANAGER_H
