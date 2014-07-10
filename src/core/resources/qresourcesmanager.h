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



template <typename T, int INDEXBITS>
class ArrayAllocatingPolicy
{
public:
    ArrayAllocatingPolicy()
        : m_resourcesEntries(1 << INDEXBITS)
    {
        m_resourcesEntries.resize(1 << INDEXBITS);
        for (int i = 0; i < m_resourcesEntries.size(); i++)
            m_freeEntryIndices << i;
    }

    T* allocateResource()
    {
        int idx = m_freeEntryIndices.takeFirst();
        T* newT = m_resourcesEntries.begin() + idx;
        m_resourcesToIndices[newT] = idx;
        return newT;
    }

    void releaseResource(T *r)
    {
        if (m_resourcesToIndices.contains(r)) {
            int idx = m_resourcesToIndices.take(r);
            m_resourcesEntries[idx] = T();
            m_freeEntryIndices.append(idx);
        }
    }

    void reset()
    {
        m_resourcesToIndices.clear();
        m_resourcesEntries.clear();
        m_resourcesEntries.resize(1 << INDEXBITS);
        for (int i = 0; i < m_resourcesEntries.size(); i++)
            m_freeEntryIndices << i;
    }

private:
    QVector<T> m_resourcesEntries;
    QList<int> m_freeEntryIndices;
    QHash<T*, int> m_resourcesToIndices;

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
        m_resourcesEntries << T();
        int idx = m_resourcesEntries.size() - 1;
        T* newT = &m_resourcesEntries.last();
        m_resourcesToIndices[newT] = idx;
        return newT;
    }

    void releaseResource(T *r)
    {
        if (m_resourcesToIndices.contains(r)) {
            m_resourcesEntries.removeAt(m_resourcesToIndices[r]);
        }
    }

    void reset()
    {
        m_resourcesEntries.clear();
    }

private:
    QList<T> m_resourcesEntries;
    QHash<T*, int> m_resourcesToIndices;
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
        m_maxResourcesEntries(1 << INDEXBITS)
    {
    }

    ~QResourcesManager()
    {}

    QHandle<T, INDEXBITS> acquire()
    {
        typename LockingPolicy<QResourcesManager>::WriteLocker(this);
        QHandle<T, INDEXBITS> handle = m_handleManager.acquire(AllocatingPolicy<T, INDEXBITS>::allocateResource());
        return handle;
    }

    T* data(const QHandle<T, INDEXBITS> &handle)
    {
        typename LockingPolicy<QResourcesManager>::ReadLocker(this);
        T* data = m_handleManager.data(handle);
        return data;
    }

    void release(const QHandle<T, INDEXBITS> &handle)
    {
        typename LockingPolicy<QResourcesManager>::WriteLocker(this);
        m_handleToResourceMapper.remove(m_handleToResourceMapper.key(handle));
        T *val = m_handleManager.data(handle);
        AllocatingPolicy<T, INDEXBITS>::releaseResource(val);
        m_handleManager.release(handle);
    }

    void reset()
    {
        typename LockingPolicy<QResourcesManager>::WriteLocker(this);
        m_handleManager.reset();
        AllocatingPolicy<T, INDEXBITS>::reset();
    }

    bool contains(const C &id)
    {
        typename LockingPolicy<QResourcesManager>::ReadLocker(this);
        return m_handleToResourceMapper.contains(id);
    }

    QHandle<T, INDEXBITS> getOrAcquireHandle(const C &id)
    {
        if (!contains(id)) {
            typename LockingPolicy<QResourcesManager>::WriteLocker(this);
            if (!m_handleToResourceMapper.contains(id))
                m_handleToResourceMapper[id] = acquire();
        }
        typename LockingPolicy<QResourcesManager>::ReadLocker(this);
        return m_handleToResourceMapper[id];
    }

    QHandle<T, INDEXBITS> lookupHandle(const C &id)
    {
        typename LockingPolicy<QResourcesManager>::ReadLocker(this);
        QHandle<T, INDEXBITS> handle;
        if (m_handleToResourceMapper.contains(id))
            handle = m_handleToResourceMapper[id];
        return handle;
    }

    T *lookupResource(const C &id)
    {
        T *ret = Q_NULLPTR;
        typename LockingPolicy<QResourcesManager>::ReadLocker(this);
        if (m_handleToResourceMapper.contains(id))
            ret = data(m_handleToResourceMapper[id]);
        return ret;
    }

    T *getOrCreateResource(const C &id)
    {
        typename LockingPolicy<QResourcesManager>::ReadLocker readLock(this);
        if (!m_handleToResourceMapper.contains(id)) {
            readLock.unlock();
            typename LockingPolicy<QResourcesManager>::WriteLocker(this);
            if (!m_handleToResourceMapper.contains(id))
                m_handleToResourceMapper[id] = acquire();
            return m_handleManager.data(m_handleToResourceMapper[id]);
        }
        else {
            return m_handleManager.data(m_handleToResourceMapper[id]);
        }
    }

    void releaseResource(const C &id)
    {
        typename LockingPolicy<QResourcesManager>::WriteLocker(this);
        if (m_handleToResourceMapper.contains(id))
            release(m_handleToResourceMapper[id]);
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
